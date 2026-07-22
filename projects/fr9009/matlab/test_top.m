close all;
clear all;

%% data source select: "file" | "serial"
dataSource = "serial";

%% serial action select: "download" | "capture" | "fh_enable" | "fh_disable"
serialAction = "fh_enable";

%% FH (Frequency Hopping) action: "enable" | "disable"
fhAction = "enable";

%% download waveform source: "file" | "generate"
downloadWaveformSource = "generate";

%% shared capture length config from C side
CAP_LENGTH_MAX = 16384;
CAP_LENGTH_READ = CAP_LENGTH_MAX / 4;
expectedIqPairs = CAP_LENGTH_READ * 2;  % one loop prints rx1 and rx2
cfg_fs_bb = 245.76e6; % default for file mode; serial mode overrides this per waveform

%% tx waveform select must match the Zynq command and plot sample rate
%% "491M" -> tone_lut_iq_491M / W0 / 491.52e6
%% "245M" -> tone_lut_iq_245M / W1 / 245.76e6
txWaveformMode = "491M"; % Should be 500M as ref clock is 125M instead of 122.88M

%% tx source config: "dds" | "ddr" | "dc"
txSourceCfg.source = "dc";

% source="dds" uses freq0/freq1 (Hz), script computes pinc0/pinc1 automatically
% Formula: dds_pinc = round(freqHz / fsHz * 2^30)
% Example at fs=245.76 MHz:
% 5M -> 0x14D5555, 10M -> 0x29AAAAA, 12.288M -> 0x3333333, 20M -> 0x5355555, 30.72M -> 0x8000000
txSourceCfg.dds.freq0Hz = 10e6;
txSourceCfg.dds.freq1Hz = 20e6;
txSourceCfg.dds.poff0 = uint32(0);
txSourceCfg.dds.poff1 = uint32(0);

% source="dc" uses these two parameters
txSourceCfg.dc.const0 = uint32(hex2dec('11223344'));
txSourceCfg.dc.const1 = uint32(hex2dec('55667788'));

switch lower(dataSource)
case "file"
    %% read capture data from saved txt file
    fileCfg.path = '.\readdata_rx2.txt';
    signal = read_signal_from_file(fileCfg.path);
    rx1 = signal(1:2:end);
    rx2 = signal(2:2:end);

case "serial"
    %% serial mode: waveform download and capture are independent actions
    serialCfg.port = "COM7";          % modify by your local setup
    serialCfg.baudRate = 115200;
    serialCfg.downloadCommand = uint8('D');
    serialCfg.configCommand = uint8('C');
    serialCfg.captureCommand = uint8('G');
    serialCfg.fhEnableCommand = uint8('J');
    serialCfg.fhDisableCommand = uint8('K');
    serialCfg.totalTimeoutSec = 25;      % full frame timeout
    serialCfg.idleTimeoutSec = 2;        % inter-line timeout
    serialCfg.pollIntervalSec = 0.005;
    serialCfg.waitCapMarker = true;      % wait "cap_buffer=" before parsing
    serialCfg.downloadAckTimeoutSec = 8; % download ack timeout
    serialCfg.configAckTimeoutSec = 5;   % config ack timeout
    serialCfg.maxWaveWords = CAP_LENGTH_MAX;

    waveformCfg.source = lower(downloadWaveformSource);
    waveformCfg.path = '.\br3109_waveform_hex_15M.txt';
    waveformCfg.gen.toneFreqHz = 20e6;
    waveformCfg.gen.ampFS = 0.1;
    waveformCfg.gen.phaseRad = 0;
    waveformCfg.gen.numSamples = CAP_LENGTH_MAX;
    waveformCfg.gen.roundingMode = "round";
    txSourceCfg.dds.fsHz = 250e6; % according to FPGA DDS config

    switch txWaveformMode
        case "491M"
            cfg_fs_bb = 500e6; % 491.52e6
            waveformCfg.gen.fsHz = 500e6; % 491.52e6
        case "245M"
            cfg_fs_bb = 250e6;%245.76e6;
            waveformCfg.gen.fsHz = 250e6;%245.76e6;
        otherwise
            error("Unsupported txWaveformMode: %s", txWaveformMode);
    end

    if lower(txSourceCfg.source) == "dds"
        txSourceCfg.dds.pinc0 = calc_dds_pinc_word(txSourceCfg.dds.freq0Hz, txSourceCfg.dds.fsHz);
        txSourceCfg.dds.pinc1 = calc_dds_pinc_word(txSourceCfg.dds.freq1Hz, txSourceCfg.dds.fsHz);
    end

    switch lower(serialAction)
        case "download"
            configure_tx_source_over_serial(serialCfg, txSourceCfg, true);
            if lower(txSourceCfg.source) == "ddr"
                download_waveform_to_serial(serialCfg, waveformCfg);
            else
                fprintf("Tx source %s configured. Waveform payload download skipped (DDR only).\n", lower(txSourceCfg.source));
            end
            return;
        case "capture"
            configure_tx_source_over_serial(serialCfg, txSourceCfg, false);
            signal = read_signal_from_serial_capture(serialCfg, expectedIqPairs);
            rx1 = signal(1:2:end);
            rx2 = signal(2:2:end);
        case "fh_enable"
            send_fh_command_over_serial(serialCfg, 'J');
            return;
        case "fh_disable"
            send_fh_command_over_serial(serialCfg, 'K');
            return;
        otherwise
            error("Unsupported serialAction: %s", serialAction);
    end

otherwise
    error("Unsupported dataSource: %s", dataSource);
end

plot_channel_overview(rx1, cfg_fs_bb, "rx1");
plot_channel_overview(rx2, cfg_fs_bb, "rx2");

function signal = read_signal_from_file(filePath)
    readdata = readtable(filePath);
    raw = table2array(readdata(:, 1:2));

    signal_i = to_signed_16(raw(:, 1));
    signal_q = to_signed_16(raw(:, 2));
    signal = (signal_i + 1i * signal_q).';
end

function signal = read_signal_from_serial_capture(cfg, expectedIqPairs)
    if exist('serialportfind', 'file') || exist('serialportfind', 'builtin')
        existing = serialportfind("Port", cfg.port);
        if ~isempty(existing)
            delete(existing);
        end
    end

    s = serialport(cfg.port, cfg.baudRate, "Timeout", 1);
    cleanupObj = onCleanup(@() clear("s")); %#ok<NASGU>

    configureTerminator(s, "LF");
    flush(s);
    write(s, cfg.captureCommand, "uint8");

    vals = zeros(expectedIqPairs, 2);
    got = 0;
    capMarkerSeen = ~cfg.waitCapMarker;
    tTotal = tic;
    tIdle = tic;

    while got < expectedIqPairs
        if toc(tTotal) > cfg.totalTimeoutSec
            error("Serial total timeout: got %d/%d IQ pairs.", got, expectedIqPairs);
        end

        if got > 0 && toc(tIdle) > cfg.idleTimeoutSec
            error("Serial idle timeout after %d IQ pairs.", got);
        end

        if s.NumBytesAvailable == 0
            pause(cfg.pollIntervalSec);
            continue;
        end

        line = strtrim(readline(s));
        tIdle = tic;

        if ~capMarkerSeen
            if contains(line, "cap_buffer=")
                capMarkerSeen = true;
            end
            continue;
        end

        m = regexp(line, '^\s*(-?\d+)\s+(-?\d+)\s*$', 'tokens', 'once');
        if isempty(m)
            continue;
        end

        got = got + 1;
        vals(got, 1) = str2double(m{1});
        vals(got, 2) = str2double(m{2});
    end

    signal = vals(:,1).' + 1i * vals(:,2).';
end

function configure_tx_source_over_serial(cfg, txSourceCfg, ~)
    payload = build_tx_source_payload(txSourceCfg);

    if exist('serialportfind', 'file') || exist('serialportfind', 'builtin')
        existing = serialportfind("Port", cfg.port);
        if ~isempty(existing)
            delete(existing);
        end
    end

    s = serialport(cfg.port, cfg.baudRate, "Timeout", 1);
    cleanupObj = onCleanup(@() clear("s")); %#ok<NASGU>

    configureTerminator(s, "LF");
    flush(s);

    write(s, cfg.configCommand, "uint8");
    write(s, payload, "uint8");

    tStart = tic;
    lastLine = "";
    ackOk = false;
    unsupportedCmd = false;
    while toc(tStart) < cfg.configAckTimeoutSec
        if s.NumBytesAvailable == 0
            pause(cfg.pollIntervalSec);
            continue;
        end

        lastLine = strtrim(readline(s));
        if contains(lastLine, "tx_config_ok")
            ackOk = true;
            break;
        end

        if contains(lastLine, "tx_config_err_unsupported")
            unsupportedCmd = true;
            break;
        end

        if contains(lastLine, "tx_config_err")
            error("Tx source config failed: %s", lastLine);
        end
    end

    if unsupportedCmd
        fprintf("Tx source config command unsupported by current firmware, continue without runtime tx source update.\n");
        return;
    end

    if ~ackOk
        error("Tx source config ack timeout. Last response: %s", lastLine);
    end

end

function send_fh_command_over_serial(cfg, cmdByte)
    if exist('serialportfind', 'file') || exist('serialportfind', 'builtin')
        existing = serialportfind("Port", cfg.port);
        if ~isempty(existing)
            delete(existing);
        end
    end

    s = serialport(cfg.port, cfg.baudRate, "Timeout", 1);
    cleanupObj = onCleanup(@() clear("s")); %#ok<NASGU>

    configureTerminator(s, "LF");
    flush(s);
    write(s, uint8(cmdByte), "uint8");

    if cmdByte == 'J'
        fprintf("FH enable command sent.\n");
    else
        fprintf("FH disable command sent.\n");
    end
end

function payload = build_tx_source_payload(txSourceCfg)
    switch lower(txSourceCfg.source)
        case "dds"
            srcSel = uint8(0);
            payload = [
                srcSel, ...
                typecast(uint32(txSourceCfg.dds.pinc0), "uint8"), ...
                typecast(uint32(txSourceCfg.dds.poff0), "uint8"), ...
                typecast(uint32(txSourceCfg.dds.pinc1), "uint8"), ...
                typecast(uint32(txSourceCfg.dds.poff1), "uint8")
            ];
        case "ddr"
            payload = uint8(1);
        case "dc"
            srcSel = uint8(2);
            payload = [
                srcSel, ...
                typecast(uint32(txSourceCfg.dc.const0), "uint8"), ...
                typecast(uint32(txSourceCfg.dc.const1), "uint8")
            ];
        otherwise
            error("Unsupported tx source: %s", txSourceCfg.source);
    end
end

function download_waveform_to_serial(cfg, waveformCfg)
    switch lower(waveformCfg.source)
        case "file"
            words = read_waveform_words_from_file(waveformCfg.path);
            sourceLabel = waveformCfg.path;
        case "generate"
            words = generate_waveform_words(waveformCfg.gen);
            sourceLabel = sprintf("generated_tone_%gMHz", waveformCfg.gen.toneFreqHz / 1e6);
        otherwise
            error("Unsupported waveform source: %s", waveformCfg.source);
    end

    wordCount = numel(words);

    if wordCount == 0
        error("Waveform source has no samples: %s", sourceLabel);
    end
    if wordCount > cfg.maxWaveWords
        error("Waveform length %d exceeds max %d words.", wordCount, cfg.maxWaveWords);
    end

    if exist('serialportfind', 'file') || exist('serialportfind', 'builtin')
        existing = serialportfind("Port", cfg.port);
        if ~isempty(existing)
            delete(existing);
        end
    end

    s = serialport(cfg.port, cfg.baudRate, "Timeout", 1);
    cleanupObj = onCleanup(@() clear("s")); %#ok<NASGU>

    configureTerminator(s, "LF");
    flush(s);

    write(s, cfg.downloadCommand, "uint8");
    write(s, typecast(uint32(wordCount), "uint8"), "uint8");
    write(s, typecast(uint32(words), "uint8"), "uint8");

    tStart = tic;
    lastLine = "";
    ackOk = false;
    while toc(tStart) < cfg.downloadAckTimeoutSec
        if s.NumBytesAvailable == 0
            pause(cfg.pollIntervalSec);
            continue;
        end
        lastLine = strtrim(readline(s));
        if contains(lastLine, "waveform_download_ok")
            ackOk = true;
            break;
        end
    end

    if ~ackOk
        error("Waveform download ack timeout. Last response: %s", lastLine);
    end

    fprintf("Waveform download done: %d words from %s\n", wordCount, sourceLabel);
end

function words = read_waveform_words_from_file(filePath)
    txt = fileread(filePath);
    tokens = regexp(txt, '0x([0-9A-Fa-f]{1,8})', 'tokens');

    if isempty(tokens)
        words = uint32([]);
        return;
    end

    words = zeros(1, numel(tokens), 'uint32');
    for k = 1:numel(tokens)
        words(k) = uint32(hex2dec(tokens{k}{1}));
    end
end

function words = generate_waveform_words(cfg)
    n = (0:cfg.numSamples-1).';
    tone = cfg.ampFS .* exp(1j * (2*pi*(cfg.toneFreqHz/cfg.fsHz).*n + cfg.phaseRad));

    fullScale = 32767;
    iQ = quantize_to_int16_local(real(tone), fullScale, cfg.roundingMode);
    qQ = quantize_to_int16_local(imag(tone), fullScale, cfg.roundingMode);

    iU16 = typecast(iQ, 'uint16');
    qU16 = typecast(qQ, 'uint16');
    words = bitor(bitshift(uint32(qU16), 16), uint32(iU16));
end

function xQ = quantize_to_int16_local(x, fullScale, roundingMode)
    xScaled = x * fullScale;
    switch roundingMode
        case "round"
            xRounded = round(xScaled);
        case "floor"
            xRounded = floor(xScaled);
        case "ceil"
            xRounded = ceil(xScaled);
        case "fix"
            xRounded = fix(xScaled);
        otherwise
            error("Invalid rounding mode: %s", roundingMode);
    end

    xClipped = max(min(xRounded, fullScale), -fullScale-1);
    xQ = int16(xClipped);
end

function pinc = calc_dds_pinc_word(freqHz, fsHz)
    ddsPhaseWidth = 30;
    if freqHz < 0 || freqHz >= fsHz
        error("DDS frequency %.6g Hz out of range [0, %.6g) Hz.", freqHz, fsHz);
    end

    pinc = uint32(round((freqHz / fsHz) * (2^ddsPhaseWidth)));
end

function y = to_signed_16(x)
    y = double(x);
    mask = y >= 2^15;
    y(mask) = y(mask) - 2^16;
end

function plot_channel_overview(rx, fsHz, rxName)
    n = numel(rx);
    t = (0:n-1) / fsHz;
    timeI = real(rx) / 2^16;
    timeQ = imag(rx) / 2^16;

    spectrum = fftshift(fft(rx));
    freqAxisHz = ((-floor(n/2)):(ceil(n/2)-1)) * (fsHz / n);
    mag = abs(spectrum);
    magDb = 20 * log10(max(mag / max(mag), eps));
    [~, peakIdx] = max(mag);
    peakFreqHz = freqAxisHz(peakIdx);
    peakMagDb = magDb(peakIdx);

    figure('Name', char(rxName), 'NumberTitle', 'off');

    subplot(2,1,1);
    plot(t * 1e6, timeI, 'b', 'LineWidth', 1.0);
    hold on;
    plot(t * 1e6, timeQ, 'r', 'LineWidth', 1.0);
    hold off;
    grid on;
    xlabel('Time (us)');
    ylabel('Amplitude (FS)');
    title(sprintf('%s Time Domain', rxName));
    legend('I', 'Q');

    subplot(2,1,2);
    plot(freqAxisHz / 1e6, magDb, 'k', 'LineWidth', 1.0);
    grid on;
    ylim([-100, 10]);
    xlabel('Frequency (MHz)');
    ylabel('Magnitude (dBFS, normalized)');
    title(sprintf('%s Frequency Domain', rxName));
    hold on;
    plot(peakFreqHz / 1e6, peakMagDb, 'ro', 'MarkerSize', 8, 'LineWidth', 1.5);
    text(peakFreqHz / 1e6, peakMagDb, sprintf('  Peak: %.3f MHz', peakFreqHz / 1e6), ...
        'Color', 'r', 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'left');
    hold off;
end
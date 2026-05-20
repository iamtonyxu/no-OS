% FR9009 host script:
% 1) optional capture readback (file/serial)
% 2) waveform download over UART with READY handshake

clear all; 
close all; 
clc;

% ========== Configuration ==========
serialCfg.port = "COM3";           % Serial port (adjust as needed)
serialCfg.baudRate = 115200;       % UART baud rate
serialCfg.readyTimeoutSec = 10;    % Time to wait for READY
serialCfg.commandArmDelaySec = 1.1; % Target polls commands every 1s after READY

runCfg.mode = "download_waveform";   % 'config_src_sel', 'download_waveform', 'capture', or 'plot_file'
txCfg.srcSel = uint8(1);           % 0: fpga dds; 1: ddr; 2: const_data

capCfg.command = uint8('G');       % Capture command
capCfg.waitCapMarker = true;       % Wait for 'cap_buffer=' marker
capCfg.totalTimeoutSec = 10;       % Total timeout for capture
capCfg.idleTimeoutSec = 1;         % Idle timeout between samples
capCfg.pollIntervalSec = 0.001;    % Poll interval
capCfg.expectedIqPairs = 8192;     % CAP_LENGTH_MAX/2 with current firmware defaults
capCfg.saveToFile = true;
capCfg.outFile = fullfile(pwd, ['fr9009_capture_iq_' char(datetime('now', 'Format', 'MMdd_HHmm')) '.txt']);

wfCfg.numSamples = 16384;          % must be <= TX_BUF_LEN (board-side LUT words)
wfCfg.Ftone_Hz = 12e6;           % Single-tone frequency in Hz
wfCfg.ampFS = 0.5;                % Single-tone amplitude, full-scale ratio 0..1
wfCfg.plotSpectrum = false;
wfCfg.saveHexFile = true;
wfCfg.outFile = fullfile(pwd, 'fr9009_tone_out.txt');

% JESD Mode: 0 -> 491.52M (L=4,M=4,S=2), 1 -> 245.76M (L=2,M=4,S=1)
jesdMode = 0;

% ========== Main Script Flow ==========
switch runCfg.mode
    case "plot_file"
        % Load from file
        signal = read_signal_from_file("fr9009_capture_iq.txt");
        cfg_fs_bb = 491.52e6; % or 245.76e6 depending on mode
        plot_capture(signal, cfg_fs_bb);
    case {"config_src_sel", "download_waveform", "capture"}
        % Open serial and ensure cleanup on exit (normal or exception)
        s = open_serial(serialCfg);
        cleanup = onCleanup(@() close_serial(s));
        
        try
            fprintf("Waiting for READY...\n");
            wait_for_ready(s, serialCfg.readyTimeoutSec);
            fprintf("Received READY, board is initialized.\n");

            switch runCfg.mode
                case "config_src_sel"
                    fprintf("Configuring tx source...\n");
                    configure_tx_source(s, txCfg.srcSel);
                    fprintf("TX source configuration complete.\n");

                case "download_waveform"
                    fprintf("Generating and downloading waveform...\n");
                    % Generate waveform
                    [iqWords, wfCfgOut] = fr9009_waveform_generate(wfCfg);
                    % Determine sample rate based on JESD mode
                    if jesdMode == 0
                        fsHz = 491.52e6;
                    else
                        fsHz = 245.76e6;
                    end
                    % Download to board
                    download_waveform_to_board(s, iqWords, fsHz, serialCfg);
                    fprintf("Waveform download complete.\n");

                case "capture"
                    fprintf("Capturing ADC data...\n");
                    if jesdMode == 0
                        cfg_fs_bb = 491.52e6;
                    else
                        cfg_fs_bb = 245.76e6;
                    end
                    signal = read_signal_from_serial(s, capCfg, capCfg.expectedIqPairs);
                    if capCfg.saveToFile
                        save_signal_to_file(capCfg.outFile, signal);
                    end
                    plot_capture(signal, cfg_fs_bb);
            end
        catch ME
            fprintf(2, "Error during serial operation: %s\n", ME.message);
            close_serial(s);
            rethrow(ME);
        end
    otherwise
        error("Invalid runCfg.mode='%s'.", runCfg.mode);
end


function configure_tx_source(s, srcSel)
    % Configure firmware tx source selector: 0 DDS, 1 DDR, 2 CONST.
    if ~isscalar(srcSel) || ~ismember(double(srcSel), [0, 1, 2])
        error('Invalid srcSel=%g (must be 0, 1, or 2).', double(srcSel));
    end

    configureTerminator(s, "LF");
    flush(s);
    write(s, uint8('S'), "uint8");
    write(s, uint8(srcSel), "uint8");

    reply = read_reply_lines(s, 2.0, {'SRC:OK', 'SRC:ERR'});
    fprintf('TX source reply:\n%s\n', reply);

    if ~contains(reply, 'SRC:OK')
        if contains(reply, 'SRC:ERR')
            error('TX source configuration failed: %s', reply);
        else
            error('TX source configuration failed: no response.');
        end
    end
end


function download_waveform_to_board(s, iqWords, fsHz, serialCfg)
    % Wrapper: download waveform to board.
    % iqWords: uint32 array where [31:16]=Q, [15:0]=I
    % fsHz: sample rate (491.52e6 or 245.76e6)
    download_waveform_to_board_inner(s, iqWords, fsHz, serialCfg);
end

function download_waveform_to_board_inner(s, iqWords, fsHz, serialCfg)
    % Map sample rate to firmware rate selector.
    if fsHz > 300e6
        rateSel = uint8(0); % 491.52M
    else
        rateSel = uint8(1); % 245.76M
    end

    sampleCount = numel(iqWords);
    if sampleCount <= 0 || sampleCount > 65535
        error('Invalid sampleCount=%d (must be 1..65535).', sampleCount);
    end

    % Same checksum rule as firmware: uint32 sum modulo 2^32.
    checksum = uint32(mod(sum(uint64(iqWords)), 2^32));

    % Protocol per README_UART_PROTOCOL.md section 5.1:
    % Step 1: Send 'W' as command byte to target
    configureTerminator(s, "LF");
    flush(s);
    write(s, uint8('W'), "uint8");

    % Target checks RX only after its 1s READY heartbeat delay.
    pause(serialCfg.commandArmDelaySec);
    
    % Step 2: Send frame: F09 + rateSel + sampleCount(u16) + payload + checksum(u32)
    % All in little-endian format
    magic = uint8(['F', '0', '9']);          % Magic bytes: 0x46 0x30 0x39
    sampleCountU16 = uint16(sampleCount);
    header = uint8([magic, rateSel, reshape(typecast(sampleCountU16, 'uint8'), 1, [])]);
    payload = reshape(typecast(uint32(iqWords), 'uint8'), 1, []);
    tail = reshape(typecast(uint32(checksum), 'uint8'), 1, []);
    frame = uint8([header, payload, tail]);
    
    write(s, frame, 'uint8');

    % Step 3: Wait for target response
    replyTimeoutSec = max(2.0, (numel(frame) * 10.0 / double(serialCfg.baudRate)) + 2.0);
    reply = read_reply_lines(s, replyTimeoutSec, {'WF:OK', 'WF:ERR'});
    fprintf('Board reply:\n%s\n', reply);

    % Check for success (target sends "WF:OK rate=... samples=... checksum=...")
    if ~contains(reply, 'WF:OK')
        if contains(reply, 'WF:ERR')
            error('Waveform download failed: %s', reply);
        else
            error('Waveform download failed: no response.');
        end
    end

    fprintf('Download done: Fs=%.2f MHz, samples=%d, checksum=0x%08X\n', ...
            fsHz / 1e6, sampleCount, checksum);
end

function text = read_reply_lines(s, timeoutSec, stopTokens)
    % Read ASCII response until one stop token is seen or timeout.
    t0 = tic;
    lines = strings(0, 1);

    while toc(t0) < timeoutSec
        if s.NumBytesAvailable == 0
            pause(0.01);
            continue;
        end

        line = strtrim(readline(s));
        if strlength(line) == 0
            continue;
        end

        lines(end + 1, 1) = line; %#ok<AGROW>
        if any(contains(line, string(stopTokens)))
            break;
        end
    end

    if isempty(lines)
        text = '';
    else
        text = join(lines, newline);
        text = char(text);
    end
end

function s = open_serial(serialCfg)
    % Re-open a clean serial object to avoid stale handles.
    % Try to close any existing serial ports on the same COM port
    try
        existing = instrfind('Port', serialCfg.port);
        if ~isempty(existing)
            fclose(existing);
            delete(existing);
        end
    catch
        % Ignore if no existing ports found
    end

    s = serialport(serialCfg.port, serialCfg.baudRate, "Timeout", 1);
    % Configure line terminator for text communication
    configureTerminator(s, "LF");
end

function close_serial(s)
    % Close and cleanup serial port.
    if ~isempty(s) && isvalid(s)
        try
            if isa(s, "serialport")
                configureCallback(s, "off");
                flush(s);
                delete(s);
            else
                fclose(s);
                delete(s);
            end
            fprintf("Serial port closed.\n");
        catch ME
            fprintf(2, "Warning: Failed to close serial port: %s\n", ME.message);
        end
    end
end

function plot_capture(signal, cfg_fs_bb)
    % signal packs two channels interleaved: rx1, rx2, rx1, rx2, ...
    rx1 = signal(1:2:end);
    rx2 = signal(2:2:end);

    figure;
    subplot(2,1,1);
    plot(real(rx1) / 2^16);
    title("real(rx1) in time-domain");
    subplot(2,1,2);
    plot(imag(rx1) / 2^16);
    title("imag(rx1) in time-domain");

    figure;
    subplot(2,1,1);
    plot(real(rx2) / 2^16);
    title("real(rx2) in time-domain");
    subplot(2,1,2);
    plot(imag(rx2) / 2^16);
    title("imag(rx2) in time-domain");

    nBits = 16;
    nHarmonics = 1;
    useHann = true;
    PlotFFT(rx1, nHarmonics, nBits, useHann, cfg_fs_bb);
    PlotFFT(rx2, nHarmonics, nBits, useHann, cfg_fs_bb);
end

function signal = read_signal_from_file(filePath)
    % Text file format: two integer columns = I Q
    readdata = readtable(filePath);
    raw = table2array(readdata(:, 1:2));

    signal_i = to_signed_16(raw(:, 1));
    signal_q = to_signed_16(raw(:, 2));
    signal = (signal_i + 1i * signal_q).';
end

function save_signal_to_file(filePath, signal)
    % Text file format: two integer columns = I Q
    fileID = fopen(filePath, 'w');
    if fileID < 0
        error('Cannot open capture output file: %s', filePath);
    end

    cleanupObj = onCleanup(@() fclose(fileID)); %#ok<NASGU>
    signal_i = round(real(signal(:)));
    signal_q = round(imag(signal(:)));

    for idx = 1:numel(signal_i)
        fprintf(fileID, '%d %d\n', signal_i(idx), signal_q(idx));
    end

    fprintf('Capture saved to file: %s\n', filePath);
end


function signal = read_signal_from_serial(s, capCfg, expectedIqPairs)
    % Wrapper: read signal from serial port.
    signal = read_signal_from_serial_inner(s, capCfg, expectedIqPairs);
end

function signal = read_signal_from_serial_inner(s, capCfg, expectedIqPairs)
    % Trigger board capture command per protocol section 7.
    % Host sends 'G' command byte, then target prints:
    %   cap_buffer=
    %   <I> <Q>
    %   <I> <Q>
    %   ...
    configureTerminator(s, "LF");
    flush(s);
    
    % Send 'G' command
    write(s, uint8('G'), "uint8");
    
    vals = zeros(expectedIqPairs, 2);
    got = 0;
    capMarkerSeen = ~capCfg.waitCapMarker;
    tTotal = tic;
    tIdle = tic;

    while true
        if got >= expectedIqPairs
            break;
        end

        if toc(tTotal) > capCfg.totalTimeoutSec
            error("Serial total timeout: got %d/%d IQ pairs.", got, expectedIqPairs);
        end

        if got > 0 && toc(tIdle) > capCfg.idleTimeoutSec
            break;
        end

        if s.NumBytesAvailable == 0
            pause(capCfg.pollIntervalSec);
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

        % Parse "I Q" text lines according to protocol section 7
        m = regexp(line, '^\s*(-?\d+)\s+(-?\d+)\s*$', 'tokens', 'once');
        if isempty(m)
            continue;
        end

        got = got + 1;
        if got > size(vals, 1)
            vals(end + 1024, :) = 0; %#ok<AGROW>
        end
        vals(got, 1) = str2double(m{1});  % I component
        vals(got, 2) = str2double(m{2});  % Q component
    end

    if got == 0
        error("No IQ pairs received after cap_buffer marker.");
    end

    vals = vals(1:got, :);
    fprintf("Captured IQ pairs: %d\n", got);

    % Reconstruct complex signal: I + j*Q
    signal = vals(:,1).' + 1i * vals(:,2).';
end
function wait_for_ready(s, timeoutSec)
    % Firmware sends "READY" every 1s after init; host must wait for it.
    % Use readline() to get complete lines terminated by LF
    t0 = tic;
    
    while toc(t0) < timeoutSec
        try
            if s.NumBytesAvailable > 0
                line = strtrim(readline(s));
                if strlength(line) > 0
                    fprintf("[RX] %s\n", line);  % Debug output to see what we receive
                    if contains(line, "READY")
                        fprintf("READY signal received successfully.\n");
                        return;
                    end
                end
            end
        catch
            % readline timeout or error, continue trying
        end
        pause(0.05);  % Wait 50ms between checks
    end
    error('No READY from device within %.1f seconds.', timeoutSec);
end

function y = to_signed_16(x)
    y = double(x);
    mask = y >= 2^15;
    y(mask) = y(mask) - 2^16;
end

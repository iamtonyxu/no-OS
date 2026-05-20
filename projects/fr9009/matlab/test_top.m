close all;
clear all;

%% data source select: "file" | "serial"
dataSource = "serial";

%% shared capture length config from C side
CAP_LENGTH_MAX = 16384;
CAP_LENGTH_READ = CAP_LENGTH_MAX / 4;
expectedIqPairs = CAP_LENGTH_READ * 2;  % one loop prints rx1 and rx2

switch lower(dataSource)
case "file"
    %% read capture data from saved txt file
    fileCfg.path = '.\readdata_rx2.txt';
    signal = read_signal_from_file(fileCfg.path);
    rx1 = signal(1:2:end);
    rx2 = signal(2:2:end);

case "serial"
    %% read capture data from serial in real-time
    serialCfg.port = "COM3";          % modify by your local setup
    serialCfg.baudRate = 115200;
    serialCfg.command = uint8('G');
    serialCfg.totalTimeoutSec = 25;    % full frame timeout
    serialCfg.idleTimeoutSec = 2;      % inter-line timeout
    serialCfg.pollIntervalSec = 0.005;
    serialCfg.waitCapMarker = true;    % wait "cap_buffer=" before parsing

    signal = read_signal_from_serial(serialCfg, expectedIqPairs);
    rx1 = signal(1:2:end);
    rx2 = signal(2:2:end);

otherwise
    error("Unsupported dataSource: %s", dataSource);
end

cfg_fs_bb = 245.76e6; % 245.76e6， 491.52e6?
plot_channel_overview(rx1, cfg_fs_bb, "rx1");
plot_channel_overview(rx2, cfg_fs_bb, "rx2");

function signal = read_signal_from_file(filePath)
    readdata = readtable(filePath);
    raw = table2array(readdata(:, 1:2));

    signal_i = to_signed_16(raw(:, 1));
    signal_q = to_signed_16(raw(:, 2));
    signal = (signal_i + 1i * signal_q).';
end

function signal = read_signal_from_serial(cfg, expectedIqPairs)
    if exist('serialportfind', 'file') || exist('serialportfind', 'builtin')
        existing = serialportfind("Port", cfg.port);
        if ~isempty(existing)
            delete(existing);
        end
    elseif exist('instrfind', 'file') || exist('instrfind', 'builtin')
        existing = instrfind('Port', char(cfg.port));
        if ~isempty(existing)
            fclose(existing);
            delete(existing);
        end
    end

    s = serialport(cfg.port, cfg.baudRate, "Timeout", 1);
    cleanupObj = onCleanup(@() clear("s")); %#ok<NASGU>

    configureTerminator(s, "LF");
    flush(s);
    write(s, cfg.command, "uint8");

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

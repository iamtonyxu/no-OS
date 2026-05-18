close all;
clear all;

%% data source select: "file" | "serial"
dataSource = "file";

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

figure;
subplot(2,1,1);
plot(real(rx1)/2^16);title("real(rx1) in time-domain");
subplot(2,1,2);
plot(imag(rx1)/2^16);title("imag(rx1) in time-domain");

figure;
subplot(2,1,1);
plot(real(rx2)/2^16);title("real(rx2) in time-domain");
subplot(2,1,2);
plot(imag(rx2)/2^16);title("imag(rx2) in time-domain");

%% FFT analysis
nBits = 16;
nHarmonics = 1;
useHann = true;%wrong if useHann = true!
cfg_fs_bb = 245.76e6;
PlotFFT(rx1, nHarmonics, nBits, useHann, cfg_fs_bb);
PlotFFT(rx2, nHarmonics, nBits, useHann, cfg_fs_bb);

function signal = read_signal_from_file(filePath)
    readdata = readtable(filePath);
    raw = table2array(readdata(:, 1:2));

    signal_i = to_signed_16(raw(:, 1));
    signal_q = to_signed_16(raw(:, 2));
    signal = (signal_i + 1i * signal_q).';
end

function signal = read_signal_from_serial(cfg, expectedIqPairs)
    existing = serialportfind("Port", cfg.port);
    if ~isempty(existing)
        clear existing;
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

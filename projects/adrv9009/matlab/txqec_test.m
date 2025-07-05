close all;
clear all;
clc;

%% Test Configuration
offline_sim = 1;
enable_pathdelay_est = 1;
debug_info = 1;
serial_port = "COM3";

% signal generate
Fs = 122.88e6;
Fc = 15e6;%3e6, 15e6
L = 16384; % 4096, 16384
t = 1/Fs*(0:L-1);
Amp = 0.4; % amplitude of single tone
cap_size = L;

% txqec stuff
INITIAL_PHASE_VALUE = 512;
INITIAL_GAIN_VALUE = 0x4000;

%% generate single tone waveform
if offline_sim
    phi = 2/180*pi; % phase error
    gain = 0.80; % gain error

    tx = Amp * exp(1i*2*pi*Fc*t); 
    %rxAligned = tx; % no qec error
    rxAligned = Amp * (cos(2*pi*Fc*t) + gain * 1j*sin(2*pi*Fc*t + phi));
else
    tx = Amp * exp(1i*2*pi*Fc*t);
    plot_signal_in_freq_domain(tx, Fs, L);
end

%% online test: download waveform and read capture
if offline_sim == 0
    % power on adrv9009 and wait the calibration is done
    % monitor tx is transfering waveform
    
    if enable_pathdelay_est == 1
        % download wideband signal
        tx_wb = load('LTE20_122P88_N11BackOff.txt');
        tx_wb = (tx_wb(1:L, 1) + 1j*tx_wb(1:L,2))./2^15; tx_wb = tx_wb.';
        plot_signal_in_freq_domain(tx_wb, Fs, L);
        download_waveform(serial_port, tx_wb);
        pause(1);
    
        % capture data to calculate path delay between tx and orx
        rx_wb = read_capture(serial_port, cap_size, 8);
    
        % path delay estimation
        [intDelay, fracDelay, rx_wb_aligned, m] = CalDelayPhase(tx_wb, rx_wb);
    end

    % disable txqec tracking cal
    enable_mask = get_tracking_cal_mask(serial_port);
    enable_mask = bitand(enable_mask, uint32(0x3BF)); % disable TX1 QEC Track
    set_tracking_cal_mask(serial_port, enable_mask);

    % read calibrated phase/gain/group delay and save
    %[phase, gain, gd] = get_txqec_phase_gain_gd(serial_port);

    % send tx test tone (refer)
    download_waveform(serial_port, tx);
    pause(1);
 
    % read orx capture (bench mark)
    %rx_good = read_capture(serial_port, cap_size, 8);
    
    % reset phase/gain
    
    % read orx capture (without txqec)
    rx = read_capture(serial_port, cap_size, 8);
    plot_signal_in_freq_domain(rx, Fs, cap_size);

end

%% channel estimation and compute phase/gain adjustment
update = 0;
for batch_cnt = 1:10
    fprintf("batch = %d\n", batch_cnt);
    %sync tx and orx
    if offline_sim
        % do nothing
        % assume tx and rx are aligned while offline simulation
    else
        [intDelay, fracDelay, rxAligned, m] = CalDelayPhase(tx, rx);
        if debug_info
            figure;
            plot(abs(tx)); hold on
            plot(abs(rxAligned), '-.');
        end
    end

    % read and accumulate data from correlator
    u = real(tx); v = imag(tx);
    z = real(rxAligned); y = imag(rxAligned);
    
    out.uu = u * u';
    out.vv = v * v';
    out.uv = u * v';
    out.zu = z * u';
    out.yu = y * u';
    out.zv = z * v';
    out.yv = y * v';
    
    % estimate gain and phase parameters 
    det = out.uu * out.vv - out.uv * out.uv;
    
    % check if not nearly singular 
    % (signal too small or u nearly identical to v)
    if det >= 1e6
        a = (out.zu * out.vv - out.zv * out.uv) / det;
        b = (out.yu * out.vv - out.yv * out.uv) / det;
        c = (-out.zu * out.uv + out.zv * out.uu) / det;
        d = (-out.yu * out.uv + out.yv * out.uu) / det;
    else
        a = 0; b = 0; c = 0; d = 0;
    end
    
    precision = (out.uu + out.vv) * (a * a + b * b);
    
    % Check if the precision in our calculation is 'good' enough
    % Compute the quadrature error correction adjustment
    if precision > 30
        gain_correction = sqrt((a*a + b*b) / (c*c + d*d)) - 1;
        phase_correction =  -(a*c + b*d) / (a*d - b*c);

        fprintf("gain_est = %.3f, phase_est = %.3f\n", ...
            1/(gain_correction+1), -phase_correction);
    
        % Scale to code words
        TXQEC_GAIN_SCALE = 15400;
        txqec_adj_phase_scale_factor = 1;
        lo_ghz = 2.4; % GHz
        adj.gain  = gain_correction * TXQEC_GAIN_SCALE;
        adj.phase = (phase_correction * txqec_adj_phase_scale_factor) / lo_ghz;
        update = 1;
        disp("compute qec adjustment - done!");
        break;
    else
        adj.gain = INITIAL_PHASE_VALUE;
        adj.phase = INITIAL_GAIN_VALUE;
    end
end

%% program qec correction hw if update is true
if update
    % write qec phase and gain
    % set_txqec_phase_gain_gd(serial_port, phase, gain, gd);
end

%% correction
if offline_sim
    rr = cos(2*pi*Fc*t) + ...
        (gain_correction+1) * gain * 1j*sin(2*pi*Fc*t + phi + phase_correction);
end

if debug_info
    plot_signal_in_freq_domain([rxAligned; rr], Fs, cap_size);
end

%% helper functions



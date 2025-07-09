close all;
clear all;
clc;

%% Test Configuration
offline_sim = 0;
enable_pathdelay_est = 0;
skip_download_waveform = 0;
debug_info = 1;
serial_port = "COM3";

% signal generate
Fs = 245.76e6;
Fc = 15e6;%3e6, 15e6
L = 32768; % HW Waveform RAM is 32768 depth
t = 1/Fs*(0:L-1);
Amp = 0.4; % amplitude of single tone
cap_size = 4096; % HW CAP RAM is 4096 depth

% txqec stuff
INITIAL_PHASE_VALUE = 512;
INITIAL_GAIN_VALUE = 0x4000;
TXQEC_GAIN_SCALE = 15400;
TXQEC_PHASE_SCALE = 20372;
PARAM_ADJ_SCALE = 0.5;

%% generate single tone waveform
if offline_sim
    phi = 2/180*pi; % phase error
    gain = 0.80; % gain error

    tu_aligned = Amp * exp(1i*2*pi*Fc*t); 
    %rx_aligned = tu; % no qec error
    rx_aligned = Amp * (cos(2*pi*Fc*t) + gain * 1j*sin(2*pi*Fc*t + phi));
else
    if 1
        tu = Amp * exp(1i*2*pi*Fc*t);
        plot_signal_in_freq_domain(tu, Fs, L, "generated tone");
    else
        Fc1 = 15e6; % start frequency
        Fc2 = 18e6; % end frequency
        freq_scale = Fs/1e3;
        fo = Fc1*1e3/Fs;
        f1 = Fc2*1e3/Fs;
        t = 0:1/1e3:(L-1)/1e3;
        tu = Amp*chirp(t,fo,t(end),f1,'linear',0,'complex').';
        plot_signal_in_freq_domain(tu, Fs, L, "generated chirp signal");
    end
end

%% online test: download waveform and read capture
if offline_sim == 0
    % power on adrv9009 and wait the calibration is done
    % monitor tu is transfering waveform

    disp("bypass dpd actuator");
    ADDR_ACT_OUT_SEL = 0x10;
    DPD_ENABLE = 1; DPD_BYPASS = 2;
    dpd_register_write(serial_port, ADDR_ACT_OUT_SEL, DPD_BYPASS);

    if enable_pathdelay_est == 1
        % download wideband signal
        tx_wb = load('LTE20_245P76_N11BackOff_32K.txt');
        tx_wb = (tx_wb(1:L, 1) + 1j*tx_wb(1:L,2))./2^15;
        plot_signal_in_freq_domain(tx_wb.', Fs, L, "generate wideband signal");
        download_waveform(serial_port, tx_wb);
        pause(1);
    
        % capture data to calculate path delay between tu and orx
        [cap_rx_wb, cap_tu_wb] = read_capture(serial_port, cap_size, 5);
        plot_signal_in_freq_domain([cap_rx_wb;cap_tu_wb], Fs, L, "capture signals");

        % path delay estimation
        [intDelay, fracDelay, rx_wb_aligned, m] = CalDelayPhase(cap_tu_wb, cap_rx_wb);
    else
        intDelay = -321+5;
        fracDelay = 40;
    end

    % disable txqec tracking cal
    % NOTE: It's not allowed to disable track cals during radio ON.
    enable_mask = get_tracking_cal_mask(serial_port);
    %enable_mask = bitand(enable_mask, uint32(0x3BB)); % disable TX1/ORX1 QEC Track
    %set_tracking_cal_mask(serial_port, enable_mask);

    % read calibrated phase/gain/group delay and save
    [good_phase, good_gain, good_gd] = get_txqec_phase_gain_gd(serial_port);

    % send tu test tone (refer)
    if ~skip_download_waveform
        download_waveform(serial_port, tu);
        pause(3);
    end

    % read orx capture (bench mark)
    [cap_rx_good, cap_tu_good] = read_capture(serial_port, cap_size, 5); % dummy capture
    [cap_rx_good, cap_tu_good] = read_capture(serial_port, cap_size, 5);
    plot_signal_in_freq_domain([cap_tu_good;cap_rx_good], Fs, cap_size, "capture signals after txqec");

    cap_rx_good = std(cap_tu_good)/std(cap_rx_good).*cap_rx_good;
    [tu_aligned, rx_aligned] = adjust_delay(cap_tu_good, cap_rx_good, intDelay, fracDelay);
    
    rr = 1:3700;
    rx_aligned = rx_aligned(rr);
    tu_aligned = tu_aligned(rr);
    figure;
    plot(imag(tu_aligned),'.-'); hold on
    plot(imag(rx_aligned),'.--');

    % reset phase/gain
    if 1
        set_txqec_phase_gain_gd(serial_port, INITIAL_GAIN_VALUE, INITIAL_PHASE_VALUE, [0,0]);
    else
        set_txqec_phase_gain_gd(serial_port, good_gain, good_phase, good_gd);
    end

    % read orx capture (without txqec)
    [cap_rx, cap_tu] = read_capture(serial_port, cap_size, 8);
    plot_signal_in_freq_domain([cap_tu; cap_rx], Fs, cap_size, "captures without txqec");

    cap_rx = std(cap_tu)/std(cap_rx).*cap_rx;
    [tu_aligned, rx_aligned] = adjust_delay(cap_tu, cap_rx, intDelay, fracDelay);
    
    rr = 1:3700;
    rx_aligned = rx_aligned(rr);
    tu_aligned = tu_aligned(rr);
    figure;
    plot(real(tu_aligned),'.-'); hold on
    plot(real(rx_aligned),'.--');

end

%% channel estimation and compute phase/gain adjustment
updates = 0;
txqec.gain = INITIAL_GAIN_VALUE;
txqec.phase = INITIAL_PHASE_VALUE;
txqec.gd = zeros(1,2);

%% iterations
for batch_cnt = 1:10
    fprintf("batch = %d\n", batch_cnt);
    %sync tu and orx
    if offline_sim
        % do nothing
        % assume tu and rx are aligned while offline simulation
    else
        sign = (-1)^(batch_cnt+1);
        tu = Amp * exp(1i*2*pi*sign*Fc*t);
        download_waveform(serial_port, tu);
        pause(3);

        [cap_rx, cap_tu] = read_capture(serial_port, cap_size, 8);
        plot_signal_in_freq_domain([cap_tu; cap_rx], Fs, cap_size, sprintf("updates=%d",updates-1));
        
        cap_rx = std(cap_tu) / std(cap_rx) * cap_rx;
        [tu_aligned, rx_aligned] = adjust_delay(cap_tu, cap_rx, intDelay, fracDelay);
        if debug_info
            rr = 1:3700;
            rx_aligned = rx_aligned(rr);
            tu_aligned = tu_aligned(rr);
            figure;
            plot(real(tu_aligned),'.-'); hold on
            plot(real(rx_aligned),'.--');
        end
    end

    % read and accumulate data from correlator
    u = real(tu_aligned); v = imag(tu_aligned);
    z = real(rx_aligned); y = imag(rx_aligned);
    
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
    if det >= 1e4 %1e6
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
        txqec_adj_phase_scale_factor = TXQEC_PHASE_SCALE * 0.5;
        lo_ghz = 3.5; % GHz
        adj.gain  = gain_correction * TXQEC_GAIN_SCALE;
        adj.phase = (phase_correction * txqec_adj_phase_scale_factor) / lo_ghz;

        % Only adjust by a fraction of the correction to avoid overshooting
        gain_adjust  = adj.gain * PARAM_ADJ_SCALE;
        phase_adjust = adj.phase * PARAM_ADJ_SCALE;
  
        txqec.gain = round(txqec.gain + gain_adjust)
        txqec.phase = round(txqec.phase - phase_adjust)

        % program txqec hw
        if offline_sim == 0
            set_txqec_phase_gain_gd(serial_port, txqec.gain, txqec.phase, txqec.gd);
        end
        updates = updates + 1;
        fprintf("txqec updates = %d\n", updates);
    else
        adj.gain = 0;
        adj.phase = 0;
    end
end

%% program qec correction hw if update is true
if updates > 0
    % write qec phase and gain
    % set_txqec_phase_gain_gd(serial_port, phase, gain, gd);
end

%% correction
if offline_sim
    rr = cos(2*pi*Fc*t) + ...
        (gain_correction+1) * gain * 1j*sin(2*pi*Fc*t + phi + phase_correction);
end

if debug_info
    plot_signal_in_freq_domain([rx_aligned; rr], Fs, cap_size, "before vs after txqec");
end

%% helper functions



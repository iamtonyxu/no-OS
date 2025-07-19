close all;
clear all;
clc;

%% Test Configuration
option = 0; % 0: tone; 1: nb signal
offline_sim = 0;
enable_pathdelay_est = 1;
skip_download_waveform = 0;
debug_info = 1;
serial_port = "COM7";

% signal generate
Fs = 245.76e6;
Fc = 50e6;%3e6, 15e6
L = 8192;
t = 1/Fs*(0:L-1);
Amp = 0.5; % amplitude of single tone
phi = 30; % degree
cap_size = 8192;

% txqec initial config
INITIAL_PHASE_VALUE = 512;
INITIAL_GAIN_VALUE = 0x4000;
TXQEC_GAIN_SCALE = 15400;
TXQEC_PHASE_SCALE = 20372;
PARAM_ADJ_SCALE = 0.5;

%% generate single tone waveform
if offline_sim
    phi = 2/180*pi; % phase error
    gain = 0.90; % gain error

    tu_aligned = Amp * exp(1i*2*pi*Fc*t); 
    %rx_aligned = tu; % no qec error
    rx_aligned = Amp * (cos(2*pi*Fc*t) + gain * 1j*sin(2*pi*Fc*t + phi));
else
    if option == 0 % tone as train signal
        tu = Amp * exp(1i*2*pi*Fc*t+phi/180*pi);
        plot_signal_in_freq_domain(tu, Fs, L, "generated tone");
    elseif option == 1 % nb signal
        tu = load('UMTS_3P84_UL_245P76_20M_32k.txt');
        tu = (tu(1:L, 1) + 1j*tu(1:L,2))./2^15;
        plot_signal_in_freq_domain(tu.', Fs, L, "nb signal");
    else % chirp signal
%         Fc1 = 15e6; % start frequency
%         Fc2 = 18e6; % end frequency
%         freq_scale = Fs/1e3;
%         fo = Fc1*1e3/Fs;
%         f1 = Fc2*1e3/Fs;
%         t = 0:1/1e3:(L-1)/1e3;
%         tu = Amp*chirp(t,fo,t(end),f1,'linear',0,'complex').';
%         plot_signal_in_freq_domain(tu, Fs, L, "generated chirp signal");
    end
end

download_waveform(serial_port, tu);

%[capData, capTu] = read_capture(serial_port, cap_size, 5);

%% todo
if 0


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
        pause(10);
        
        % capture data to calculate path delay between tu and orx
        [cap_rx_wb, cap_tu_wb] = read_capture(serial_port, cap_size, 5); %% dummy capture
        [cap_rx_wb, cap_tu_wb] = read_capture(serial_port, cap_size, 5);
        plot_signal_in_freq_domain([cap_tu_wb;cap_rx_wb], Fs, L, "capture signals");

        % path delay estimation
        [intDelay, fracDelay, rx_wb_aligned, m] = CalDelayPhase(cap_tu_wb, cap_rx_wb);
        fprintf("intDelay = %d, fracDelay = %d\n\n", intDelay, fracDelay);
        figure;
        rr = 10:3700;
        plot(imag(cap_tu_wb(rr)),'b'); hold on;
        plot(imag(rx_wb_aligned(rr)), '.--r');
    else % reuse calculated delay value
        intDelay = -318;
        fracDelay = 40;
    end

    % disable txqec tracking cal
    % NOTE: NOT allowed to disable track cals during radio ON, and
    % we stop tracking cals after bootup in FW.
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

    %% read orx capture (bench mark)
    if 0
        [cap_rx_good, cap_tu_good] = read_capture(serial_port, cap_size, 5); % dummy capture
        [cap_rx_good, cap_tu_good] = read_capture(serial_port, cap_size, 5);
        plot_signal_in_freq_domain([cap_tu_good;cap_rx_good], Fs, cap_size, "capture signals after txqec");
    end
%     cap_rx_good = std(cap_tu_good)/std(cap_rx_good).*cap_rx_good;
%     [tu_aligned, rx_aligned] = adjust_delay(cap_tu_good, cap_rx_good, intDelay, fracDelay);
%     
%     rr = 1:3700;
%     rx_aligned = rx_aligned(rr);
%     tu_aligned = tu_aligned(rr);
%     figure;
%     plot(imag(tu_aligned),'.-'); hold on
%     plot(imag(rx_aligned),'.--');

end

%% program good txqec phase/gain value
if offline_sim == 0
    set_txqec_phase_gain_gd(serial_port, good_gain, good_phase, good_gd);
end

%% channel estimation and compute phase/gain adjustment
updates = 0;
txqec.gain = INITIAL_GAIN_VALUE;
txqec.phase = INITIAL_PHASE_VALUE;
txqec.gd = zeros(1,2);
if offline_sim == 0
    set_txqec_phase_gain_gd(serial_port, txqec.gain(end), txqec.phase(end), good_gd);
end

%% iterations
for iter = 1:2
    % capture data and sync
    if offline_sim == 0
        close all
        [cap_rx, cap_tu] = read_capture(serial_port, cap_size, 8);
        
        rr = 10:4000;
        if option == 0
            %cap_rx = std(cap_tu) / std(cap_rx) * cap_rx;
            [tu_aligned, rx_aligned] = adjust_delay(cap_tu, cap_rx, intDelay, fracDelay);
            
            similar = [];
            SEARCH_RANGE = 8;
            for offset = -SEARCH_RANGE:SEARCH_RANGE

                tmp_y = circshift(rx_aligned, offset);
                tmp_y = tmp_y(rr); tmp_x = tu_aligned(rr);
                similar(end+1) = sum(abs(real(tmp_x - tmp_y)));
            end
            offset = find(similar == min(similar)) - (SEARCH_RANGE+1)
            rx_aligned = circshift(rx_aligned, offset);
    
        elseif option == 1
            tu_aligned = cap_tu;
            [intDelay, fracDelay, rx_aligned, m] = CalDelayPhase(cap_tu, cap_rx);
            fprintf("intDelay = %d, fracDelay = %d\n", intDelay, fracDelay);
        else
            fprintf("exit when invalid option = %d\n", option);
        end
        
        if debug_info
            rx_aligned = rx_aligned(rr);
            tu_aligned = tu_aligned(rr);
            figure;
            plot(real(tu_aligned),'.-'); hold on
            plot(real(rx_aligned),'.--');
        end
        plot_signal_in_freq_domain([cap_tu; cap_rx], Fs, cap_size, sprintf("updates=%d",updates-1));
    end
    
    % read and accumulate data from correlator
    if option == 0
        u = real(tu_aligned); v = imag(tu_aligned);
        z = real(rx_aligned); y = imag(rx_aligned);
        
        u = u - mean(u); v = v - mean(v);
        z = z - mean(z); y = y - mean(y);
    
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
        
        precision = (out.uu + out.vv) * (a * a + b * b)
    
    elseif option == 1 % WARNING: Don't work!!!
        fft_length = 4096;
    
        rx_fft = fftshift(fft(rx_aligned, fft_length));
        figure;
        plot(20*log10(abs(rx_fft)));
        max_pbin = find(abs(rx_fft) == max(abs(rx_fft)))
        max_nbin = fft_length - max_pbin + 2
    
        tu_fft = fftshift(fft(tu_aligned, fft_length));
        figure;
        plot(20*log10(abs(tu_fft)));
    
        u = abs(tu_fft(max_pbin)); v = abs(tu_fft(max_nbin));
        z = abs(rx_fft(max_pbin)); y = abs(rx_fft(max_nbin));
    
        out1.uu = u * u';
        out1.vv = v * v';
        out1.uv = u * v';
        out1.zu = z * u';
        out1.yu = y * u';
        out1.zv = z * v';
        out1.yv = y * v';
        
        % estimate gain and phase parameters 
        det = out1.uu * out1.vv - out1.uv * out1.uv
        
        % check if not nearly singular 
        % (signal too small or u nearly identical to v)
        if det >= 1e3 %1e4 %1e6
            a = (out1.zu * out1.vv - out1.zv * out1.uv) / det;
            b = (out1.yu * out1.vv - out1.yv * out1.uv) / det;
            c = (-out1.zu * out1.uv + out1.zv * out1.uu) / det;
            d = (-out1.yu * out1.uv + out1.yv * out1.uu) / det;
        else
            a = 0; b = 0; c = 0; d = 0;
        end
    
        precision = (out1.uu + out1.vv) * (a * a + b * b)
    end
    
    % Check if the precision in our calculation is 'good' enough
    % Compute the quadrature error correction adjustment
    if precision > 30
        delta_gain = 0.002;%0.001;
        gain_correction = sqrt((a*a + b*b) / (c*c + d*d)) - (1+delta_gain);
        phase_correction =  -(a*c + b*d) / (a*d - b*c);
    
        fprintf("gain_est = %.3f, phase_est = %.3f\n", ...
            1/(gain_correction+1), -phase_correction);
    
        % Scale to code words
        %txqec_adj_gain_scale_factor = TXQEC_GAIN_SCALE * 0.2;
        txqec_adj_gain_scale_factor = TXQEC_GAIN_SCALE * 0.3;
        txqec_adj_phase_scale_factor = TXQEC_PHASE_SCALE;
        lo_ghz = 3.5; % GHz
        adj.gain  = gain_correction * txqec_adj_gain_scale_factor;
        adj.phase = (phase_correction * txqec_adj_phase_scale_factor) / lo_ghz;
    
        % Only adjust by a fraction of the correction to avoid overshooting
        gain_adjust  = adj.gain * PARAM_ADJ_SCALE;
        phase_adjust = adj.phase * PARAM_ADJ_SCALE;
    
        txqec.gain(end+1) = round(txqec.gain(end) + gain_adjust)
        txqec.phase(end+1) = round(txqec.phase(end) - phase_adjust)
    
        % program txqec hw
        if offline_sim == 0
            set_txqec_phase_gain_gd(serial_port, txqec.gain(end), txqec.phase(end), txqec.gd);
        end
        updates = updates + 1;
        fprintf("txqec updates = %d\n", updates);
    else
        adj.gain = 0;
        adj.phase = 0;
    end
end

%% correction
if offline_sim
    rr = Amp*(cos(2*pi*Fc*t) + ...
        (gain_correction+1) * gain * 1j*sin(2*pi*Fc*t + phi + phase_correction));
end

if debug_info
    plot_signal_in_freq_domain([rx_aligned; rr], Fs, cap_size, "before vs after txqec");
end

end


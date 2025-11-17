close all;
clear all;
clc;

%% Test Configuration
offline_sim = 1; % 0: real-time debugging; 1: offline simulation
waveform_type = 0; % 0: tone; 1: nb signal; 2: chirp; 3: DDS; 4: PN15

enable_pathdelay_est = 0;
skip_download_waveform = 0;
debug_info = 1;
serial_port = "COM4";

% signal generate
Fs = 245.76e6;
Fc = 60e6;%3e6, 15e6
L = 8192;
t = 1/Fs*(0:L-1);
Amp = 0.5; % amplitude of single tone
phi = 30; % degree
cap_size = 4096;

% txqec initial config
chan = 2; % tx3
INITIAL_PHASE_VALUE = 512;
INITIAL_GAIN_VALUE = 0x4000;
TXQEC_GAIN_SCALE = 15400;
TXQEC_PHASE_SCALE = 84883;
PARAM_ADJ_SCALE = 0.5;
varianceThres = 80*1e-13;
maxNumEstLoops = 20;

%% generate waveform
if offline_sim == 1
    phi = 2/180*pi; % phase error
    gain = 0.90; % gain error > 0

    tu_aligned = Amp * exp(1i*2*pi*Fc*t); 
    rx_aligned = Amp * (cos(2*pi*Fc*t) + gain * 1j*sin(2*pi*Fc*t + phi));
else
    if waveform_type == 0 % tone as train signal
        tu = Amp * exp(1i*2*pi*Fc*t+phi/180*pi);
        plot_signal_in_freq_domain(tu, Fs, L, "generated tone");
    elseif waveform_type == 1 % nb signal
        %tu = load('UMTS_3P84_UL_245P76_20M_32k.txt');
        tu = load('lTE20_245P76_N11BackOff_32k.txt');
        tu = (tu(1:L, 1) + 1j*tu(1:L,2))./2^15;
        plot_signal_in_freq_domain(tu.', Fs, L, "nb signal");
    elseif waveform_type == 2 % chirp signal
        Fc1 = 15e6; % start frequency
        Fc2 = 18e6; % end frequency
        freq_scale = Fs/1e3;
        fo = Fc1*1e3/Fs;
        f1 = Fc2*1e3/Fs;
        t = 0:1/1e3:(L-1)/1e3;
        tu = Amp*chirp(t,fo,t(end),f1,'linear',0,'complex').';
        plot_signal_in_freq_domain(tu, Fs, L, "generated chirp signal");
    elseif waveform_type == 3 % DDS in FPGA
        datasel = 0; % 0: DDS, 1: PN15
        tone_freq = 60; % MHz
        tone_scale = 500; % max is 1000
    elseif waveform_type == 4 % PN15 in FPGA
        datasel = 1; % 0: DDS, 1: PN15
    else
        disp("please select a waveform_type first, exit now.");
        return;
    end

    if ismember(waveform_type, [0,1,2])
        % download waveform into FPGA
        download_waveform(serial_port, tu);
        disp("download waveform ends.");
        [fw_intDelay, fw_fracDelay] = read_extpath_delay(serial_port);
    else
        % select DDS
        set_dac_datasel(serial_port, datasel, tone_freq, tone_scale);
    end

end

%% set path delay (Debug)
if 0
    if offline_sim == 0
        int_delay = 0;
        frac_delay = 0;
        set_capture_delay(serial_port, int_delay, frac_delay);
    end
end

%% read capture (Debug)
debug_capture = 1; % 1: debug capture; 0: skip debug
if (offline_sim == 0) && (debug_capture == 1)
    % trigger capture and read orx
    [capORx] = read_capture(serial_port, 0, 5);
    if debug_info
        plot_signal_in_freq_domain(capORx, Fs*4, 4096, "capORx");
    end
    % read tx
    [capTx] = read_capture(serial_port, 1, 5);
    if debug_info
        plot_signal_in_freq_domain(capTx, Fs*4, 4096, "capTx");
    end
    % read tu
    [capTu] = read_capture(serial_port, 2, 5);
    if debug_info
        plot_signal_in_freq_domain(capTu, Fs*4, L, "capTu");
    end

    alignment_option = 0; % 0: no alignment; 1: estimate pathdelay; 2: use estimated
    
    % (intDelay, fracDelay) as default
    intDelay = -296;
    fracDelay = 9;

    if alignment_option == 1
        tx_aligned = capTx;
        [intDelay, fracDelay, orx_aligned, m] = CalDelayPhase(capTx, capORx);
        orx_aligned = std(tx_aligned)/std(orx_aligned).*orx_aligned;
        fprintf("intDelay = %d, fracDelay = %d\n", intDelay, fracDelay);
        figure;
        plot(abs(xcorr(capTx, capORx)));
    elseif alignment_option == 2
        intDelay = -292-3; %-293-3: tone
        fracDelay = 9;
        [tx_aligned, orx_aligned] = adjust_delay(capTx, capORx, intDelay, fracDelay);
        orx_aligned = std(tx_aligned)/std(orx_aligned).*orx_aligned;
    else
        tx_aligned = capTx;
        orx_aligned = std(tx_aligned)/std(capORx).*capORx;
    end

    figure;
    rr = 300+(1:3500);
    plot(real(tx_aligned(rr)), '.b--'); hold on
    plot(real(orx_aligned(rr)), '.r--');

    ratio = sum(abs(tx_aligned(rr)))/sum(abs(orx_aligned(rr)));
    diff_ratio = sum(abs(diff(tx_aligned(rr) - orx_aligned(rr))))/sum(abs(tx_aligned(rr)));
    fprintf("ratio = %f\n", ratio);
    fprintf("diff_ratio = %f\n", diff_ratio);
    return;
end

%% sort the best frac_delay for tone (Not required?)
if offline_sim == 0 && waveform_type == 0
    diff_ratio = [];
    for fracDelay = 1:1:64
        [tx_aligned, orx_aligned] = adjust_delay(capTx, capORx, intDelay, fracDelay);
        orx_aligned = std(tx_aligned)/std(orx_aligned).*orx_aligned;
        rr = 300+(1:3500);
        diff_ratio(end+1) = sum(abs(diff(tx_aligned(rr) - orx_aligned(rr))))/sum(abs(tx_aligned(rr)));
    end
    min(diff_ratio)
    find(diff_ratio == min(diff_ratio))
end

%% read calibrated phase/gain/group delay and save
% power on adrv9026 and wait the calibration is done
% monitor tu is transfering waveform
% reuse calculated delay value
% intDelay = -296;
% fracDelay = 9;
% disable txqec tracking cal via changing initMask in profile
if offline_sim == 0
    [good_phase, good_gain, good_gd] = get_txqec_phase_gain_gd(serial_port, chan);
end

%% read orx capture (bench mark)
if offline_sim == 0
    [capORx] = read_capture(serial_port, 0, 5);
    [capTx] = read_capture(serial_port, 1, 5);

    % sync tx and orx
    [tx_aligned, orx_aligned] = adjust_delay(capTx, capORx, intDelay, fracDelay);
    orx_aligned = std(tx_aligned)/std(orx_aligned).*orx_aligned;

    figure;
    rr = 300+(1:3500);
    plot(real(tx_aligned(rr)), '.b--'); hold on
    plot(real(orx_aligned(rr)), '.r--');

    ratio = sum(abs(tx_aligned(rr)))/sum(abs(orx_aligned(rr)));
    diff_ratio = sum(abs(diff(tx_aligned(rr) - orx_aligned(rr))))/sum(abs(tx_aligned(rr)));
    fprintf("ratio = %f\n", ratio);
    fprintf("diff_ratio = %f\n", diff_ratio);

    tx_aligned_good = tx_aligned;
    orx_aligned_good = orx_aligned;

    plot_signal_in_freq_domain([tx_aligned_good;orx_aligned_good], Fs*4, length(tx_aligned_good), "capture signals after txqec");
end

%% program calibrated txqec phase/gain
if offline_sim == 0
    set_txqec_phase_gain_gd(serial_port, chan, good_gain, good_phase, good_gd);
end

%% program init txqec phase/gain
updates = 0;
txqec.gain = INITIAL_GAIN_VALUE;
txqec.phase = INITIAL_PHASE_VALUE;
txqec.gd = zeros(1,2);
if offline_sim == 0
    set_txqec_phase_gain_gd(serial_port, chan, [0,0,txqec.gain(end),0,0], txqec.phase(end), good_gd);
end

%% iterations
for iter = 1:maxNumEstLoops
    % capture data and sync
    if offline_sim == 0
        close all;
        [cap_rx] = read_capture(serial_port, 0, 5);
        [cap_tu] = read_capture(serial_port, 1, 5);

        %cap_rx = std(cap_tu) / std(cap_rx) * cap_rx;
        [tu_aligned, rx_aligned] = adjust_delay(cap_tu, cap_rx, intDelay, fracDelay);

        similar = [];
        SEARCH_RANGE = 4;
        for offset = -SEARCH_RANGE:SEARCH_RANGE
            tmp_y = circshift(rx_aligned, offset);
            tmp_y = tmp_y(rr); tmp_x = tu_aligned(rr);
            similar(end+1) = sum(abs(real(tmp_x - tmp_y)));
        end
        offset = find(similar == min(similar)) - (SEARCH_RANGE+1)
        rx_aligned = circshift(rx_aligned, offset);

        if debug_info
            rr = 300+(1:3500);
            rx_aligned = rx_aligned(rr);
            tu_aligned = tu_aligned(rr);
            figure;
            plot(real(tu_aligned),'.-'); hold on
            plot(real(rx_aligned .* std(tu_aligned)/std(rx_aligned)),'.--');
            title("tu vs orx aligned");
        end
        plot_signal_in_freq_domain([cap_tu; cap_rx], Fs, cap_size, sprintf("updates=%d",updates-1));
    end

    % channel estimation
    [txqec, estVariance, corrData, debugInfo] = txqecInit_FindChannelEstimate(tu_aligned, rx_aligned);
    % print txqec.gain and txqec.phase as debug info
    fprintf("Estimated txqec parameters:\n");
    fprintf("txqec.gain = %d, txqec.phase = %d\n", txqec.gain(end), txqec.phase(end));

    % program txqec hw
    if offline_sim == 0
        set_txqec_phase_gain_gd(serial_port, chan, [0,0,txqec.gain(end),0,0], txqec.phase(end), txqec.gd);
    end
    updates = updates + 1;
    fprintf("txqec updates = %d\n", updates);

    % exit if estVariance meets the requirement
    if(estVariance < varianceThres)
        break;
    end
end

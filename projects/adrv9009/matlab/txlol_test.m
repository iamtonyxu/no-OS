close all;
clear all;
clc;

%% Test Configuration
% cap_tx channel is fixed for now, which is tx0
option = 0; % 0: perturbation method; 1: sweep method
offline_sim = 1; % 0: online; 1: offline; 2: capture data
Fs = 491.52e6; % Hz
cap_size = 16384; % 16k samples
PnPowerDb = -50; % dB
PnNumSamples = 1024; % One +/-Pt set
PerturbScaler = 128; % fixed
DacBits = 14; % bits
serial_port = "COMx"; % Uart port

if ~offline_sim
    serial_port = "COM3";
elseif offline_sim == 1
    setPhs = 3 / 180 * pi; % 3 degree
    setGain = 0.85; % -3.4 dB
    lol_i = randi([-30, 30]); lol_q = randi([-30, 30]); % dc leakage
    fprintf("sim: setGain = %.4f, setPhs = %.2f deg\n", setGain, setPhs*180/pi);
    fprintf("sim: set_lol_i = %.4f, set_lol_q = %.2f deg\n", lol_i, lol_q);
end

% generate waveform of PN and perturbation
[waveform, iData, qData, info] = txlol_waveform_gen('NumSamples', PnNumSamples, ...
                                                    'PnPowerDb', PnPowerDb, ...
                                                    'PerturbScaler', PerturbScaler, ...
                                                    'DacBits', DacBits, ...
                                                    'EnableI', true, ...
                                                    'EnableQ', false, ...
                                                    'PlotEnable', true);

%% read tracking cal mask and check txlol is dsabled
if ~offline_sim
    enable_mask = get_tracking_cal_mask(serial_port);
    if bitand(enable_mask, 0x10) == 0x10
        exit("TXLOL is enabled, disable it for testing.");
    else
        disp("TXLOL is disabled, good for testing.");
    end
end

%% read txlol dc offset
if ~offline_sim
    dc_offset_get = get_txlol_dc_offset(serial_port);
else
    dc_offset_get = struct('i', int16(0), 'q', int16(0));
end

%% set txlol dc offset
if ~offline_sim
    dc_offset_set = dc_offset_get;
    dc_offset_set.i = -50;
    dc_offset_set.q = 603;
    set_txlol_dc_offset(serial_port, dc_offset_set);
end

%% download waveform to ADRV9009
if ~offline_sim
    % the size of tx buffer is 32k
    waveform  = [waveform, waveform, waveform, waveform, ...
                 waveform, waveform, waveform, waveform];
    % download waveform
    download_waveform(serial_port, waveform);
end

%% run the test and collect data
if ~offline_sim
    [cap_orx, cap_tx] = read_capture(serial_port, 4096, 5); % dummy capture
    % loop capture for cap_size/4k times to get enough data for analysis
    for ii = 1:cap_size
        [cap_orx, cap_tx] = read_capture(serial_port, cap_size, 5);
        if ii == 1
            orx = cap_orx;
            tx = cap_tx;
        else
            orx = [orx, cap_orx];
            tx = [tx, cap_tx];
        end
    end
    plot_signal_in_freq_domain([orx;tx], Fs, cap_size, "capture signals in freq domain");
elseif (offline_sim == 1)
    % for offline simulation, apply txlol model to tx and generate orx
    waveform = waveform .* 2^DacBits;
    tx = repmat(waveform(:), cap_size / length(waveform), 1);

    leakage = ones(length(tx),1) * (lol_i + 1j*lol_q);
    orx = txlol_model(tx, setPhs, setGain) + txlol_model(leakage, setPhs, setGain);
else
    % load capture data
    load('capdata_wotxlol.mat');
    tx = tx.' .* 2^DacBits;
    orx = orx.' .* 2^DacBits;
end

%% sync and trim tx(x) and orx(y)
x = tx; y = orx;
[intDelay, fracDelay, yAligned, m] = CalDelayPhase(x, y);
fprintf("intDelay = %d, fracDelay = %d\n", intDelay, fracDelay);

acc_size = cap_size - PnNumSamples * 2;
x_trim = x(1 : acc_size);
y_trim = yAligned(1 : acc_size);

% plot tx and orx in time-domain
figure;
subplot(2,1,1);
plot(real(x_trim)); title('real(tx)');
subplot(2,1,2);
plot(imag(x_trim)); title('imag(tx)');

figure;
subplot(2,1,1);
plot(real(y_trim)); title('real(rx)');
subplot(2,1,2);
plot(imag(y_trim)); title('imag(rx)');

%% txlol hw engine: correlation and accumulation
txlol_hw_outputs = struct('TxDcTotalI', double(0), ...
                          'TxDcTotalQ', double(0), ...
                          'ObsDcTotalI', double(0), ...
                          'ObsDcTotalQ', double(0), ...
                          'TxPerturb', double(0), ...
                          'TxPerturbSq', double(0), ...
                          'RxDcDiffI', double(0), ...
                          'RxDcDiffQ', double(0), ...
                          'RxDcDiffSqI', double(0), ...
                          'RxDcDiffSqQ', double(0));

txlol_hw_outputs.TxDcTotalI = sum(real(x_trim));
txlol_hw_outputs.TxDcTotalQ = sum(imag(x_trim));
txlol_hw_outputs.ObsDcTotalI = sum(real(y_trim));
txlol_hw_outputs.ObsDcTotalQ = sum(imag(y_trim));
txlol_hw_outputs.TxPerturb = (PerturbScaler) * length(x_trim);
txlol_hw_outputs.TxPerturbSq = (PerturbScaler^2) * length(x_trim);

% 计算RxDcDiffI，当real(orx) > 0时， 将real(orx)的值乘以PerturbScaler并累加到RxDcDiffI中；当real(orx) < 0时，将real(orx)的值乘以-PerturbScaler并累加到RxDcDiffI中
txlol_hw_outputs.RxDcDiffI = sum(real(y_trim) .* (real(y_trim) > 0) * 1 + real(y_trim) .* (real(y_trim) < 0) * (-1));
% 计算RxDcDiffQ，当imag(orx) > 0时， 将imag(orx)的值乘以PerturbScaler并累加到RxDcDiffQ中；当imag(orx) < 0时，将imag(orx)的值乘以-PerturbScaler并累加到RxDcDiffQ中
txlol_hw_outputs.RxDcDiffQ = sum(imag(y_trim) .* (imag(y_trim) > 0) * 1 + imag(y_trim) .* (imag(y_trim) < 0) * (-1));
txlol_hw_outputs.RxDcDiffSqI = sum((real(y_trim) .* (real(y_trim) > 0) * PerturbScaler + real(y_trim) .* (real(y_trim) < 0) * -PerturbScaler).^2);
txlol_hw_outputs.RxDcDiffSqQ = sum((imag(y_trim) .* (imag(y_trim) > 0) * PerturbScaler + imag(y_trim) .* (imag(y_trim) < 0) * -PerturbScaler).^2);

disp("txlol_hw_outputs:");
disp(txlol_hw_outputs);

%% ProcessEngineResults 
txlol_status = struct('txAverageDCI', double(0), ...
                          'txAverageDCQ', double(0), ...
                          'rxAverageDCI', double(0), ...
                          'rxAverageDCQ', double(0), ...
                          'TxPerturb', double(0), ...
                          'RxDcDiffI', double(0), ...
                          'RxDcDiffQ', double(0), ...
                          'RxDcDiffSqI', double(0), ...
                          'RxDcDiffSqQ', double(0), ...
                          'ObsDcTotalI', double(0), ...
                          'ObsDcTotalQ', double(0));

avDCScale = 1 / acc_size;
txlol_status.txAverageDCI = txlol_hw_outputs.TxDcTotalI * avDCScale;
txlol_status.txAverageDCQ = txlol_hw_outputs.TxDcTotalQ * avDCScale;
txlol_status.rxAverageDCI = txlol_hw_outputs.ObsDcTotalI * avDCScale;
txlol_status.rxAverageDCQ = txlol_hw_outputs.ObsDcTotalQ * avDCScale;
txlol_status.TxPerturb = txlol_hw_outputs.TxPerturb;
txlol_status.RxDcDiffI = txlol_hw_outputs.RxDcDiffI;
txlol_status.RxDcDiffQ = txlol_hw_outputs.RxDcDiffQ;
txlol_status.RxDcDiffSqI = txlol_hw_outputs.RxDcDiffSqI;
txlol_status.RxDcDiffSqQ = txlol_hw_outputs.RxDcDiffSqQ;

txlol_state = struct('gainPt', double(0), ...
                     'phsPt', double(0), ...
                     'lolI', double(0), ...
                     'lolQ', double(0));

txlol_state.gainPt = sqrt(txlol_status.RxDcDiffI^2 + txlol_status.RxDcDiffQ^2);
curTx = txlol_status.TxPerturb;
txlol_state.gainPt = txlol_state.gainPt / curTx / 2;
txlol_state.phsPt = atan2(txlol_status.RxDcDiffQ, txlol_status.RxDcDiffI);

%txlol_state.phsPt = atan2(-txlol_status.RxDcDiffQ, txlol_status.RxDcDiffI);


fprintf('estGain = %.4f, estPhs = %.2f deg\n', txlol_state.gainPt, txlol_state.phsPt*180/pi);

%% channel estimation and correction
dc_offset_est = struct('i', int16(0), 'q', int16(0));

cosPhs = cos(txlol_state.phsPt);
sinPhs = sin(txlol_state.phsPt);
accGain = txlol_state.gainPt;

txlol_state.lolI = txlol_status.rxAverageDCI * cosPhs - txlol_status.rxAverageDCQ * sinPhs;
txlol_state.lolQ = txlol_status.rxAverageDCI * sinPhs + txlol_status.rxAverageDCQ * cosPhs;

txlol_state.lolI = txlol_state.lolI / accGain;
txlol_state.lolQ = txlol_state.lolQ / accGain;

fprintf("est_lol_i = %.4f, est_lol_q = %.2f\n", txlol_state.lolI, txlol_state.lolQ);

return;

%% verify txlol correction
Fc = 100e6; % Hz
L = 4096; 
t = 1/Fs*(0:L-1);
tx = 0.1 * exp(1i*2*pi*Fc*t).'; 

% apply the previous lol model
orx = txlol_model(tx, setPhs, setGain);

figure;
plot(20*log10(abs(fftshift(fft(orx)))));


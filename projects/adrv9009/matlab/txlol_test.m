close all;
clear all;
clc;

%% Test Configuration
% cap_tx channel is fixed for now, which is tx0
option = 0; % 0: perturbation method; 1: sweep method
offline_sim = 1; % 0: online; 1: offline
Fs = 491.52e6; % Hz
NumSamples = 4096;
PnPowerDb = -50; % dB
PerturbScaler = 128; % fixed
DacBits = 14; % bits
debug_info = 1;
serial_port = "COM3";

% generate waveform of PN and perturbation
[waveform, iData, qData, info] = txlol_waveform_gen('NumSamples', NumSamples, ...
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
    dc_offset_fw = get_txlol_dc_offset(serial_port);
else
    dc_offset_fw = struct('i', int16(0), 'q', int16(0));
end

%% set txlol dc offset
if ~offline_sim
    dc_offset_fw1 = dc_offset_fw;
    dc_offset_fw1.i = -50;
    dc_offset_fw1.q = 603;
    set_txlol_dc_offset(serial_port, dc_offset_fw1);
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
    cap_size = 16384; % 16k samples
    [cap_orx, cap_tx] = read_capture(serial_port, 4096, 5); % dummy capture
    % loop capture for cap_size/4k times to get enough data for analysis
    for ii = 1:cap_size/4096
        [cap_orx, cap_tx] = read_capture(serial_port, 4096, 5);
        if ii == 1
            orx = cap_orx;
            tx = cap_tx;
        else
            orx = [orx, cap_orx];
            tx = [tx, cap_tx];
        end
    end
    plot_signal_in_freq_domain([orx;tx], Fs, cap_size, "capture signals in freq domain");
else
    % for offline simulation, apply txlol model to tx and generate orx
    setPhs = 3 / 180 * pi; % 3 degree
    setGain = 0.85; % -3.4 dB
%     setLeakage = struct('lolI', 10, ...
%                         'lolQ', 20);

    fprintf("sim: setGain = %.4f, setPhs = %.2f deg\n", setGain, setPhs*180/pi);
    cap_size = 16384; % 16k samples
    waveform = waveform .* 2^DacBits;
    tx = repmat(waveform(:), cap_size / length(waveform), 1);
    %tx = tx + complex(setLeakage.lolI, setLeakage.lolQ);
    orx = txlol_model(tx, setPhs, setGain);
    if debug_info
        fprintf('offline txlol model: gain = %.4f, phase = %.2f deg\n', ...
                setGain, setPhs * 180 / pi);
    end
end

% plot tx and orx in time-domain
figure;
subplot(2,1,1);
plot(real(tx)); title('real(tx)');
subplot(2,1,2);
plot(imag(tx)); title('imag(tx)');

figure;
subplot(2,1,1);
plot(real(orx)); title('real(rx)');
subplot(2,1,2);
plot(imag(orx)); title('imag(orx)');

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

txlol_hw_outputs.TxDcTotalI = sum(real(tx));
txlol_hw_outputs.TxDcTotalQ = sum(imag(tx));
txlol_hw_outputs.ObsDcTotalI = sum(real(orx));
txlol_hw_outputs.ObsDcTotalQ = sum(imag(orx));
txlol_hw_outputs.TxPerturb = (PerturbScaler) * length(tx);
txlol_hw_outputs.TxPerturbSq = (PerturbScaler^2) * length(tx);

% 计算RxDcDiffI，当real(orx) > 0时， 将real(orx)的值乘以PerturbScaler并累加到RxDcDiffI中；当real(orx) < 0时，将real(orx)的值乘以-PerturbScaler并累加到RxDcDiffI中
txlol_hw_outputs.RxDcDiffI = sum(real(orx) .* (real(orx) > 0) * 1 + real(orx) .* (real(orx) < 0) * (-1));
% 计算RxDcDiffQ，当imag(orx) > 0时， 将imag(orx)的值乘以PerturbScaler并累加到RxDcDiffQ中；当imag(orx) < 0时，将imag(orx)的值乘以-PerturbScaler并累加到RxDcDiffQ中
txlol_hw_outputs.RxDcDiffQ = sum(imag(orx) .* (imag(orx) > 0) * 1 + imag(orx) .* (imag(orx) < 0) * (-1));
txlol_hw_outputs.RxDcDiffSqI = sum((real(orx) .* (real(orx) > 0) * PerturbScaler + real(orx) .* (real(orx) < 0) * -PerturbScaler).^2);
txlol_hw_outputs.RxDcDiffSqQ = sum((imag(orx) .* (imag(orx) > 0) * PerturbScaler + imag(orx) .* (imag(orx) < 0) * -PerturbScaler).^2);

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

avDCScale = 1 / cap_size;
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

fprintf('estGain = %.4f, estPhs = %.2f deg\n', txlol_state.gainPt, txlol_state.phsPt*180/pi);

%% channel estimation and correction
dc_offset_est = struct('i', int16(0), 'q', int16(0));

cosPhs = cos(txlol_state.phsPt);
sinPhs = sin(txlol_state.gainPt);
accGain = txlol_state.gainPt;

txlol_state.lolI = txlol_status.rxAverageDCI * cosPhs - txlol_status.rxAverageDCQ * sinPhs;
txlol_state.lolQ = txlol_status.rxAverageDCI * sinPhs + txlol_status.rxAverageDCQ * cosPhs;

txlol_state.lolI = txlol_state.lolI / accGain;
txlol_state.lolQ = txlol_state.lolQ / accGain;

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


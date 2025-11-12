close all;
clc;

Plot_Enable = false;

%% Test Configuration
gain = 0.79;          % Gain error
phase_deg = 3.01;     % Phase error (degree)
SNR_dB = 60;          % SNR in dB
Fs = 245.76e6;        % Sampling frequency
                      % Tone frequencies, [+f1, -f1, +f2, -f2, ...]
Fc = [10e6, -10e6, 20e6, -20e6, 30e6, -30e6, 40e6, -40e6];
txIQRateHz = 122.88e6; % TX IQ rate
L = 4096*1;            % Signal length
Amp = 0.5;            % Amplitude of single tone
ADC_Bits = 16;        % ADC bits affects varThreshold

fprintf("gain_set = %.3f, phase_set = %.3f\n", gain, phase_deg);

% Define gdFilterData for wideband filter calculation
FilterData = struct('freq', [], 'corrData', []);
gdFilterData = repmat(FilterData, numel(Fc), 1);

%% Run Simulation for each tone frequency
for idx = 1:numel(Fc)
        fprintf("\n--- Tone Frequency: %.2f MHz ---\n", Fc(idx)/1e6);

    % Generate signals
    t = (0:L-1) / Fs;
    phase_rad = deg2rad(phase_deg);

    % Ideal transmit signal
    tu_aligned = Amp * exp(1i * 2 * pi * Fc(idx) * t);

    % Calculate noise power from SNR
    signal_power = Amp^2;
    noise_power = signal_power / (10^(SNR_dB/10));
    noise = sqrt(noise_power/2) * (randn(size(t)) + 1j * randn(size(t)));

    % Received signal with gain and phase error, plus noise
    rx_aligned = Amp * (cos(2*pi*Fc(idx)*t) + gain * 1j * sin(2*pi*Fc(idx)*t + phase_rad)) + noise;

    % Plot signals in frequency domain
    if Plot_Enable
        plot_signal_in_freq_domain([tu_aligned; rx_aligned], Fs, L, "capture signal with simulation");
    end

    % Channel estimation
    %rx_aligned = std(tu_aligned)/std(rx_aligned)*rx_aligned; % No impact on est phase/gain
    % convert float to integer 
    %tu_aligned = round(2^(ADC_Bits-1) * tu_aligned / max(abs(tu_aligned))); 
    %rx_aligned = round(2^(ADC_Bits-1) * rx_aligned / max(abs(rx_aligned)));

    [txqec, estVariance, corrData] = txqecInit_FindChannelEstimate(tu_aligned, rx_aligned);
    fprintf("txqec.gain = %.4f, txqec.phase = %.4f\n", txqec.gain, txqec.phase);

    % collect correlation data for wideband filter calculation
    gdFilterData(idx).freq = Fc(idx)/txIQRateHz;
    gdFilterData(idx).corrData = corrData;
end

%% Estimate Wideband filter
% Call the function to calculate the wideband filter
[corrFilterCoeffs] = CalcWidebandFlt(gdFilterData, txqec.gain);

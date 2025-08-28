close all;
clc;

%% Test Configuration
gain = 0.9;           % Gain error
phase_deg = 2;        % Phase error (degree)
SNR_dB = 40;          % SNR in dB
Fs = 245.76e6;        % Sampling frequency
Fc = 60e6;            % Signal frequency
L = 8192;             % Signal length
Amp = 0.5;            % Amplitude of single tone

fprintf("gain_set = %.3f, phase_set = %.3f\n", gain, phase_deg);

%% Generate signals
t = (0:L-1) / Fs;
phase_rad = deg2rad(phase_deg);

% Ideal transmit signal
tu_aligned = Amp * exp(1i * 2 * pi * Fc * t);

% Calculate noise power from SNR
signal_power = Amp^2;
noise_power = signal_power / (10^(SNR_dB/10));
noise = sqrt(noise_power/2) * (randn(size(t)) + 1j * randn(size(t)));

% Received signal with gain and phase error, plus noise
rx_aligned = Amp * (cos(2*pi*Fc*t) + gain * 1j * sin(2*pi*Fc*t + phase_rad)) + noise;

%% Plot signals in frequency domain
plot_signal_in_freq_domain([tu_aligned; rx_aligned], Fs, L, "capture signal with simulation");

%% Channel estimation
txqec = txqecInit_FindChannelEstimate(tu_aligned, rx_aligned);
fprintf("txqec.gain = %.4f, txqec.phase = %.4f\n", txqec.gain, txqec.phase);


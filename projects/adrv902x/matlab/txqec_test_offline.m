close all;
clear all;
clc;

%% Test Configuration
phase = 2; % phase error (degree)
gain = 0.9; % gain error
fprintf("gain_set = %.3f, phase_set = %.3f\n", gain, phase);

%% generate signals
Fs = 245.76e6;
Fc = 60e6;%3e6, 15e6
L = 8192;
t = 1/Fs*(0:L-1);
Amp = 0.5; % amplitude of single tone
phi = phase/180*pi;

tu_aligned = Amp * exp(1i*2*pi*Fc*t); 
%rx_aligned = tu_aligned; % no qec error
rx_aligned = Amp * (cos(2*pi*Fc*t) + gain * 1j*sin(2*pi*Fc*t + phi));

plot_signal_in_freq_domain([tu_aligned;rx_aligned], Fs, length(tu_aligned), "capture signal with simulation");

%% txqecInit_ChannelEstimate
txqec = txqecInit_FindChannelEstimate(tu_aligned, rx_aligned);
fprintf("txqec.gain=%d, phase=%d\n", txqec.gain, txqec.phase);


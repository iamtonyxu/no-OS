clear all;
close all;

len = 4096;
osRate = 64;

% create tu
Fs = 122.88e6;
Fc1 = 15e6; % start frequency
Fc2 = 30e6; % end frequency
tu = chirp(0:1/Fs:len/Fs, Fc1, len/Fs, Fc2, 'complex')';
figure;
plot(real(tu), 'o--'); hold on;
plot(imag(tu), '.-');
figure;
plot(abs(fftshift(fft(tu))), 'o-');
title('FFT of the chirp signal');

%t = 0:1/len:1;
%tu = chirp(t,0,1,500, 'complex')';

int_delay_set = randi([10,100]);
frac_delay_set = randi(64); %[1,64]

% add path delay
[x, y] = generate_signals_with_delay(tu, int_delay_set, frac_delay_set);

% estimate path delay
[int_delay, frac_delay, y_aligned1, m] = CalDelayPhase(x, y, osRate);

% apply delay adjustment
[x_aligned, y_aligned] = adjust_delay(x, y, int_delay, frac_delay);

rr = 10:len-int_delay_set*2;
figure;
plot(real(x_aligned(rr)), 'o--'); hold on
plot(real(y_aligned(rr)), '.-');
plot(real(x_aligned(rr) - y_aligned(rr)), 'k');

figure;
plot(imag(x_aligned(rr) - y_aligned(rr)), 'k');

%% helper function
function [x, y_adjust] = generate_signals_with_delay(tu, int_delay_set, frac_delay_set)
    % Generate test signals with specified integer and fractional delays
    % Inputs:
    %   tu - input signal
    %   int_delay_set - integer delay to apply
    %   frac_delay_set - fractional delay index [1,64]
    % Outputs:
    %   x - original signal
    %   y_adjust - delayed and filtered signal
    
    x = tu(1:end-int_delay_set);
    
    % generate 64 filters with n/64 fractional delay,when n = 0, 1, 2, ..., 63
    osRate = 64;
    filt_length = 16;
    c = intfilt(osRate, filt_length/2, 0.8);
    c = [c, 0]; % c has only 64*16-1 interleaved coefficients, append 0 at the end to have 64*16 coefficients
    
    y_filt = conv(tu, c(frac_delay_set:osRate:end));
    y_trum = y_filt(filt_length/2:end-filt_length/2);
    
    y_adjust = y_trum(int_delay_set+1:end);
end
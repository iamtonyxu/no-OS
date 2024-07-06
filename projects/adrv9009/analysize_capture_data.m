close all;
clear all;
clc;

data = load('adrv9009_orx_4k.txt');
Fs = 122.88e6;
FFT_Length = 4096;
f = Fs*(0:(FFT_Length/2))/FFT_Length;


data_i = data(:,1);
data_q = data(:,2);

for i = 1:length(data)
    if data_i(i) > 2^15
        data_i(i) = data_i(i) - 2^16;
    end
    if data_q(i) > 2^15
        data_q(i) = data_q(i) - 2^16;
    end
end

orx = data_i + 1j*data_q;

%%
figure;
plot(data_i); hold on
plot(data_q);
title('signal in time domain');

Y = fft(orx, FFT_Length);
P2 = abs(Y/FFT_Length);
P1 = P2(1:FFT_Length/2+1);
figure;
plot(f,P1); title('signal in freq domain');



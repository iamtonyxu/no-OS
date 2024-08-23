close all;
clear all;
clc;

serialCOM = "COM3"; % COMx, COM3

Fs = 122.88e6;
Fc = 15e6;
signal_Length = 8192; % max is 16384
FFT_Length = 4096;
t = 1/Fs*(0:signal_Length-1);
capSize = 4096;
capTime = 5;
f = Fs*(0:(FFT_Length/2))/FFT_Length;

%signal = exp(1i*2*pi*Fc*t);
signal = load('LTE20_CD1_245P76.txt');
tu = (signal(:, 1) + 1j*signal(:,2))./2^15;
tu = tu(1:signal_Length, 1);

if serialCOM == "COM3"
    %spi_read(serialCOM, 0x400A);
    %spi_write(serialCOM, 0x400A, 0x12345678);
    download_waveform(serialCOM, tu);
    pause(3);

    orx = read_capture(serialCOM, capSize, 5);
else
    data = load('adrv9009_orx_4k.txt');
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
end

%%
figure;
plot(real(orx)); hold on
plot(imag(orx));
title('signal in time domain');

Y = fft(orx, FFT_Length);
P2 = abs(Y/FFT_Length);
P1 = P2(1:FFT_Length/2+1);
figure;
plot(f,P1); title('signal in freq domain');

%%
figure;
plot(abs(xcorr(orx, tu)));
title("abs(xcorr(orx, tu))");

capSize = length(orx);
start = find(abs(xcorr(orx, tu)) == max(abs(xcorr(orx, tu))))-length(tu);
tu = tu(1:signal_Length/2, 1);
tu = circshift(tu, start);
tu = tu(1:capSize, 1);
tx =tu;

%% performance check
analyzeCaptureData(Fs, tu, tx, orx);

close all;
clear all;
clc;

Fs = 30.72e6;
Fc = 3.84e6;
L = 4096;
Amp = 0.6;
t = 1/Fs*(0:L-1);

%% generate signal
signal = Amp * exp(1i*2*pi*Fc*t); 

figure;
plot(real(signal), '--.'); hold on
plot(imag(signal));

figure;
plot(20*log10(abs(fftshift(fft(signal)))), '-x');

%% download signal
serialport = "COM3";
download_waveform(serialport, signal);

%% save file
if 0
fileName = "singleTone.txt";
% Convert the complex waveform to fixed-point 16-bit and then to hexadecimal
I_fixed = int16(real(signal) * 32767);
Q_fixed = int16(imag(signal) * 32767);

I_hex = dec2hex(typecast(I_fixed, 'uint16'), 4); % Convert to 16-bit hex
Q_hex = dec2hex(typecast(Q_fixed, 'uint16'), 4); % Convert to 16-bit hex

% Combine the hex strings into the desired format
hexStrings = strcat('0x', I_hex, Q_hex);

% Save to a text file
fileID = fopen(fileName, 'w');
for i = 1:length(hexStrings)
    fprintf(fileID, '%s,\n', hexStrings(i, :));
end
fclose(fileID);
end


close all;
clear all;
clc;

fileName = "singleTone.txt";
Fs = 30.72e6;
Fc = 2.7e6;

L = 8192;
res = 16;
t = 1/Fs*(0:L-1);

%%
signal = 0.5/4*exp(1i*2*pi*Fc*t);


%% Add dithering to avoid quantization noise
dithering = (rand(size(signal)) - 0.5) * 2^(5-res); % 12 res bit dithering

%%
if 1
figure;
plot(real(signal)); hold on
plot(imag(signal))

figure;
plot(abs(fftshift(fft(signal))));
end
%%
if 1
% Convert the complex waveform to fixed-point 16-bit and then to hexadecimal
% I_fixed = int16(real(signal) * 2^(res-1)*16);
% Q_fixed = int16(imag(signal) *  2^(res-1)*16);
I_fixed = int16((real(signal)+dithering)*2^(res-5))*2^4;
Q_fixed = int16((imag(signal)+dithering)*2^(res-5))*2^4;

% I_fixed = int16((real(signal))*2^(res-5))*2^4;
% Q_fixed = int16((imag(signal))*2^(res-5))*2^4;

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



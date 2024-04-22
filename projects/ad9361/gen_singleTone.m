close all;
clear all;
clc;

fileName = "singleTone.txt";
Fs = 30.72e6/2;
Fc = 3.84e6;
L = 1024;
t = 1/Fs*(0:L-1);

%%
signal = exp(1i*2*pi*Fc*t); 

% signal1 = zeros(1, L*2);
% 
% for i = 1:length(signal)
%     signal1(1, 2*i-1) = signal(1, i);
%     signal1(1, 2*i) = signal(1, i);
% end
% signal = signal1;
% 
% Fc2 = Fs/2+1e5;
% disturb = exp(1i*2*pi*Fc2*t);
% 
% figure;
% plot(real(disturb), '.r'); hold on
% plot(imag(disturb),'-xb');
% 
% signal = signal + real(disturb); 


figure;
plot(real(signal)); hold on
plot(imag(signal))

figure;
plot(abs(fftshift(fft(signal))), '-x');

%%
if 1
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



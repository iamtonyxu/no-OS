close all;
clear all;
clc;

MBit = 16;
NFFT = 4096;
data = load('cap_data.txt');

% cap_data: 2t2r
% I0(H, L), Q0(H, L)
% I1, Q1
% I0, Q0
% I1, Q1
% ...
ch0_i = data(1:2:end, 1);
ch0_q = data(1:2:end, 2);
ch1_i = data(2:2:end, 1);
ch1_q = data(2:2:end, 2);

% for i = 1:length(ch0_i)
%     ch0_i(i, 1) = swap_bytes(ch0_i(i, 1));
%     ch0_q(i, 1) = swap_bytes(ch0_q(i, 1));
%     ch1_i(i, 1) = swap_bytes(ch1_i(i, 1));
%     ch1_q(i, 1) = swap_bytes(ch1_q(i, 1));
% end

for i = 1:length(ch0_i)
    if ch0_i(i, 1) >= 2^(MBit-1)
        ch0_i(i, 1) = -((2^MBit) - ch0_i(i, 1)); 
    end
    if ch0_q(i, 1) >= 2^(MBit-1)
        ch0_q(i, 1) = -((2^MBit) - ch0_q(i, 1)); 
    end
    if ch1_i(i, 1) >= 2^(MBit-1)
        ch1_i(i, 1) = -((2^MBit) - ch1_i(i, 1)); 
    end
    if ch1_q(i, 1) >= 2^(MBit-1)
        ch1_q(i, 1) = -((2^MBit) - ch1_q(i, 1)); 
    end    
end

rx0 = ch0_i + 1i*ch0_q;
rx1 = ch1_i + 1i*ch1_q;

%%
figure;
subplot(2,1,1);
plot(real(rx0)); hold on
plot(imag(rx0));
title('rx0 in time domain');

subplot(2,1,2);
plot(real(rx1)); hold on
plot(imag(rx1));
title('rx1 in time domain');

figure;
plot(20*log10(abs(fftshift(fft(rx0, NFFT)/NFFT/2^15)))); hold on
plot(20*log10(abs(fftshift(fft(rx1, NFFT)/NFFT/2^15))));
legend('rx0', 'rx1');
title('rx in freq domain');



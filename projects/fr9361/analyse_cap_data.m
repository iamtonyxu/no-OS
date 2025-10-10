close all;
clear all;
clc;

MBit = 16;
NFFT = 4096;
cfg_fs_bb = 30.72e6;
data = load('C:\tmp\cap_data.txt');

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

rx0_waveform = complex(ch0_q,ch0_i);
rx1_waveform = complex(ch1_q,ch1_i);
% figure;
%% Plot time domain data
subplot(2,2,1);
plot(real(rx0_waveform));hold on; 
plot(imag(rx0_waveform));
% title('Time-domain');
title('Rx0 Samples');
legend('real', 'imag');
hold off;

subplot(2,2,2);
plot(real(rx1_waveform));hold on; 
plot(imag(rx1_waveform));
% title('Time-domain');
title('Rx1 Samples');
legend('real', 'imag');
hold off;


%% FFT analysis
nBits = 12;
nHarmonics = 0;
useHann = true;
subplot(2,2,3);
PlotFFT(rx0_waveform, nHarmonics, nBits, useHann, cfg_fs_bb);
title('Rx0 Freq-domain');
hold off;

subplot(2,2,4);
PlotFFT(rx1_waveform, nHarmonics, nBits, useHann, cfg_fs_bb);
title('Rx1 Freq-domain');



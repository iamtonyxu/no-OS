close all;
clear;
clc;

%% BR3109 waveform generator
% 1) Generate complex single-tone (I/Q) with configurable Fs/Ftone/Amplitude
% 2) Quantize to signed 16-bit DAC samples
% 3) Save to file (default: one 32-bit HEX word per line, [31:16]=Q, [15:0]=I)

%% User config
cfg = struct();
cfg.Fs_Hz        = 491.52e6;   % sample rate, 491.52e6, 245.76e6
cfg.Ftone_Hz     = 20e6;      % tone frequency
cfg.ampFS        = 0.1;        % amplitude in full-scale (0..1)
cfg.phase_rad    = 0;          % initial phase
cfg.numSamples   = 16384;      % number of complex samples

% Quantization
cfg.dacBits      = 16;
cfg.fullScale    = (2^(cfg.dacBits-1) - 1); % 32767 for 16-bit
cfg.roundingMode = "round";    % "round" | "floor" | "ceil" | "fix"

% Output format
% "txdata32_hex": each line has 5 word, and each word = {Q[15:0], I[15:0]}
cfg.outFormat    = "txdata32_hex";
cfg.outFile      = fullfile(pwd, "br3109_tone_out.txt");

%% Generate complex single-tone
n = (0:cfg.numSamples-1).';
tone = cfg.ampFS .* exp(1j * (2*pi*(cfg.Ftone_Hz/cfg.Fs_Hz).*n + cfg.phase_rad));

if 1
N = cfg.numSamples;
spec = fftshift(abs(fft(tone)));
f_Hz = (-N/2:N/2-1).' * (cfg.Fs_Hz / N);
[~, tone_idx] = min(abs(f_Hz - cfg.Ftone_Hz));

figure;
plot(f_Hz / 1e6, spec, 'LineWidth', 1.1);
grid on;
xlabel('Frequency (MHz)');
ylabel('|FFT(tone)|');
title('Tone Spectrum');
hold on;
plot(f_Hz(tone_idx) / 1e6, spec(tone_idx), 'ro', 'MarkerSize', 8, 'LineWidth', 1.2);
text(f_Hz(tone_idx) / 1e6, spec(tone_idx), sprintf('  tone = %.3f MHz', cfg.Ftone_Hz / 1e6), ...
    'Color', 'r', 'VerticalAlignment', 'bottom');
hold off;
end

I = real(tone);
Q = imag(tone);

%%
I_fixed = int16(imag(tone) * 32767);
Q_fixed = int16(real(tone) * 32767);

I_hex = dec2hex(typecast(I_fixed, 'uint16'), 4); % Convert to 16-bit hex
Q_hex = dec2hex(typecast(Q_fixed, 'uint16'), 4); % Convert to 16-bit hex

% Combine the hex strings into the desired format
hexStrings = strcat('0x', I_hex, Q_hex);

% Save to a text file
ftone_MHz = cfg.Ftone_Hz / 1e6;
ftone_str = strrep(sprintf('%.3g', ftone_MHz), '.', 'p');
fileName = ['br3109_waveform_hex_', ftone_str, 'M.txt'];
fileID = fopen(fileName, 'w');
for i = 1:5:length(hexStrings)
    % Process up to 5 samples per line
    endIdx = min(i+4, length(hexStrings));
    for j = i:endIdx
        fprintf(fileID, '%s, ', hexStrings(j, :));
    end
    fprintf(fileID, '\n');
end
fclose(fileID);

%%
if 1
    %% Quantize to int16
    I_q = quantize_to_int16(I, cfg.fullScale, cfg.roundingMode);
    Q_q = quantize_to_int16(Q, cfg.fullScale, cfg.roundingMode);
else
    %% 生成递增序列，用于测试，I_q和Q_q分别为0,1,2,...,numSamples-1，且Q_q比I_q大1000
    I_q = int16(mod((0:cfg.numSamples-1), cfg.fullScale));
    Q_q = int16(mod((1000:cfg.numSamples-1+1000), cfg.fullScale));
end

% create a file and save  I_q, Q_q follow this example:
% 先将I和Q量化为int32格式，Q在高16位，I在低16位，然后保存为十六进制格式, 每行包含5个样本
fileID = fopen(cfg.outFile, 'w');
for k = 1:5:cfg.numSamples
    % Process up to 5 samples per line
    endIdx = min(k+4, cfg.numSamples);
    for j = k:endIdx
        word = bitshift(uint32(Q_q(j)), 16) + uint32(I_q(j));
        fprintf(fileID, '0x%08X, ', word);
    end
    fprintf(fileID, '\n');
end
fclose(fileID);
disp(['Waveform saved to file: ', cfg.outFile]);

%% Helper function: Quantize to int16
function x_q = quantize_to_int16(x, fullScale, roundingMode)
    % Scale
    x_scaled = x * fullScale;
    
    % Rounding
    switch roundingMode
        case "round"
            x_rounded = round(x_scaled);
        case "floor"
            x_rounded = floor(x_scaled);
        case "ceil"
            x_rounded = ceil(x_scaled);
        case "fix"
            x_rounded = fix(x_scaled);
        otherwise
            error('Invalid rounding mode');
    end
    
    % Clip to int16 range
    x_clipped = max(min(x_rounded, fullScale), -fullScale-1);
    
    % Convert to int16
    x_q = int16(x_clipped);
end
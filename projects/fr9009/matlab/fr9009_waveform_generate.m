function [iqWords, cfg] = fr9009_waveform_generate(cfgIn)
%FR9009_WAVEFORM_GENERATE Generate FR9009 IQ waveform and pack to uint32 words.
% iqWords format: [31:16]=Q(int16), [15:0]=I(int16)
%
% Usage:
%   [iqWords, cfg] = fr9009_waveform_generate();
%   cfg = struct('Fs_Hz', 245.76e6, 'numSamples', 8192);
%   [iqWords, cfg] = fr9009_waveform_generate(cfg);

    cfg = default_cfg();
    if nargin >= 1 && ~isempty(cfgIn)
        cfg = merge_cfg(cfg, cfgIn);
    end

    n = (0:cfg.numSamples-1).';
    tone = cfg.ampFS .* exp(1j * (2 * pi * (cfg.Ftone_Hz / cfg.Fs_Hz) .* n + cfg.phase_rad));

    I = real(tone);
    Q = imag(tone);

    I_q = quantize_to_int16(I, cfg.fullScale, cfg.roundingMode);
    Q_q = quantize_to_int16(Q, cfg.fullScale, cfg.roundingMode);

    iqWords = bitor(bitshift(typecast(Q_q, 'uint16'), 16), typecast(I_q, 'uint16'));
    iqWords = uint32(iqWords);

    if cfg.plotSpectrum
        figure;
        plot(abs(fft(tone)));
        title('Waveform Spectrum');
    end

    if cfg.saveHexFile
        save_words_as_hex(cfg.outFile, iqWords);
        disp(['Waveform saved to file: ', cfg.outFile]);
    end
end

function cfg = default_cfg()
    cfg = struct();
    cfg.Fs_Hz = 491.52e6;      % 491.52e6 or 245.76e6
    cfg.Ftone_Hz = 1.2e6;
    cfg.ampFS = 0.1;           % 0..1
    cfg.phase_rad = 0;
    cfg.numSamples = 16384;

    cfg.dacBits = 16;
    cfg.fullScale = 2^(cfg.dacBits - 1) - 1;
    cfg.roundingMode = 'round'; % round | floor | ceil | fix

    cfg.saveHexFile = true;
    cfg.outFile = fullfile(pwd, 'fr9009_tone_out.txt');
    cfg.plotSpectrum = false;
end

function cfg = merge_cfg(cfg, cfgIn)
    keys = fieldnames(cfgIn);
    for k = 1:numel(keys)
        cfg.(keys{k}) = cfgIn.(keys{k});
    end
end

function save_words_as_hex(filePath, words)
    fileID = fopen(filePath, 'w');
    if fileID < 0
        error('Cannot open output file: %s', filePath);
    end

    cleanupObj = onCleanup(@() fclose(fileID)); %#ok<NASGU>

    n = numel(words);
    for k = 1:5:n
        endIdx = min(k + 4, n);
        for j = k:endIdx
            fprintf(fileID, '0x%08X, ', words(j));
        end
        fprintf(fileID, '\n');
    end
end

function x_q = quantize_to_int16(x, fullScale, roundingMode)
    x_scaled = x * fullScale;

    switch roundingMode
        case 'round'
            x_rounded = round(x_scaled);
        case 'floor'
            x_rounded = floor(x_scaled);
        case 'ceil'
            x_rounded = ceil(x_scaled);
        case 'fix'
            x_rounded = fix(x_scaled);
        otherwise
            error('Invalid rounding mode');
    end

    x_clipped = max(min(x_rounded, fullScale), -fullScale - 1);
    x_q = int16(x_clipped);
end

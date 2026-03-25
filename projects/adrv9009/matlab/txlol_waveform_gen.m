
function [waveform, iData, qData, info] = txlol_waveform_gen(varargin)
%TXLOL_WAVEFORM_GEN Generate a PN waveform for Tx LOL calibration/debug.
%
%   waveform = TXLOL_WAVEFORM_GEN() generates a default 4096-sample
%   waveform. The waveform is returned as a normalized complex row vector
%   whose real part is I and imaginary part is Q. The returned waveform can
%   be passed directly to download_waveform().
%
%   waveform = TXLOL_WAVEFORM_GEN(Name, Value, ...) supports options:
%       'NumSamples'      : Number of samples to generate.
%       'PnPowerDb'       : PN amplitude relative to full scale, in dBFS.
%       'PerturbScaler'   : I-channel perturbation in DAC counts.
%       'DacBits'         : DAC resolution.
%       'EnableI'         : Enable I PN sequence.
%       'EnableQ'         : Enable Q PN sequence.
%       'PolynomialI'     : PN polynomial for I.
%       'PolynomialQ'     : PN polynomial for Q.
%       'InitStateI'      : Initial state for I PN.
%       'InitStateQ'      : Initial state for Q PN.
%       'PlotEnable'      : Plot time/frequency domain results.
%
%   [waveform, iData, qData, info] also returns the quantized I/Q sample
%   vectors in DAC counts and summary metadata.

    p = inputParser;
    p.FunctionName = mfilename;

    addParameter(p, 'NumSamples', 4096, @(x) isnumeric(x) && isscalar(x) && x > 0 && isfinite(x));
    addParameter(p, 'PnPowerDb', -40, @(x) isnumeric(x) && isscalar(x) && isfinite(x));
    addParameter(p, 'PerturbScaler', 32, @(x) isnumeric(x) && isscalar(x) && isfinite(x));
    addParameter(p, 'DacBits', 14, @(x) isnumeric(x) && isscalar(x) && x >= 2 && isfinite(x));
    addParameter(p, 'EnableI', true, @(x) islogical(x) || isnumeric(x));
    addParameter(p, 'EnableQ', false, @(x) islogical(x) || isnumeric(x));
    addParameter(p, 'PolynomialI', [7 6 0]);
    addParameter(p, 'PolynomialQ', [7 6 0]);
    addParameter(p, 'InitStateI', [0 0 0 0 0 0 1]);
    addParameter(p, 'InitStateQ', [1 0 0 0 0 0 1]);
    addParameter(p, 'PlotEnable', true, @(x) islogical(x) || isnumeric(x));
    parse(p, varargin{:});
    args = p.Results;

    numSamples = floor(double(args.NumSamples));
    dacBits = floor(double(args.DacBits));
    fullScale = 2^(dacBits - 1) - 1;
    pnAmplitudeCounts = max(1, round(fullScale * 10^(double(args.PnPowerDb) / 20)));

    [~, iPn, qPn] = pngen(numSamples, ...
        'IAmplitude', pnAmplitudeCounts, ...
        'QAmplitude', pnAmplitudeCounts, ...
        'PolynomialI', args.PolynomialI, ...
        'PolynomialQ', args.PolynomialQ, ...
        'InitStateI', args.InitStateI, ...
        'InitStateQ', args.InitStateQ, ...
        'EnableI', logical(args.EnableI), ...
        'EnableQ', logical(args.EnableQ));

    perturbation = localGeneratePerturbation(numSamples, double(args.PerturbScaler));

    iData = iPn + perturbation;
    qData = qPn;

    iData = localClipToDac(iData, fullScale);
    qData = localClipToDac(qData, fullScale);

    waveform = complex(iData, qData) ./ fullScale;
    waveform = waveform.';

    info = struct();
    info.numSamples = numSamples;
    info.dacBits = dacBits;
    info.fullScaleCounts = fullScale;
    info.pnPowerDb = double(args.PnPowerDb);
    info.pnAmplitudeCounts = pnAmplitudeCounts;
    info.perturbScaler = double(args.PerturbScaler);
    info.dcBinPowerDb = localComputeDcPowerDb(waveform);

    if logical(args.PlotEnable)
        localPlotWaveform(waveform, info);
    end
end

function perturbation = localGeneratePerturbation(numSamples, perturbScaler)
    firstHalfLength = ceil(numSamples / 2);
    secondHalfLength = floor(numSamples / 2);

    perturbation = perturbScaler .* [ ...
        ones(firstHalfLength, 1); ...
        -ones(secondHalfLength, 1) ...
    ];
end

function data = localClipToDac(data, fullScale)
    data = round(data);
    data = min(max(data, -fullScale), fullScale);
end

function dcBinPowerDb = localComputeDcPowerDb(waveform)
    spectrum = abs(fftshift(fft(waveform) / length(waveform)));
    dcBinPowerDb = 20 * log10(max(spectrum(floor(length(spectrum) / 2) + 1), eps));
end

function localPlotWaveform(waveform, info)
    figure;
    plot(real(waveform));
    title('Tx LOL I Waveform');
    xlabel('Sample Index');
    ylabel('Normalized Amplitude');

    spectrumDb = 20 * log10(max(fftshift(abs(fft(waveform) / length(waveform))), eps));
    x = (-length(spectrumDb) / 2:length(spectrumDb) / 2 - 1) * (1 / length(spectrumDb));

    figure;
    plot(x, spectrumDb);
    title(sprintf('Tx LOL Spectrum, DC = %.2f dB', info.dcBinPowerDb));
    xlabel('Normalized Frequency');
    ylabel('Magnitude (dB)');
    hold on;
    plot(0, info.dcBinPowerDb, 'ro');
    hold off;
end




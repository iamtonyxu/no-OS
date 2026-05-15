function retval = PlotFFT(h, numOfHarms, res, use_window, fs)
%PlotFFT Plot FFT magnitude with correct frequency axis and dBFS scaling.
%
%   retval = PlotFFT(h, numOfHarms, res, use_window, fs)
%
% Inputs
%   h           : time-domain samples (real or complex), vector
%   numOfHarms  : number of harmonics to report (excluding fundamental)
%   res         : bit width of signed fixed-point samples (e.g. 16)
%   use_window  : true -> apply Hann window with coherent-gain correction
%   fs          : sample rate in Hz
%
% Output
%   retval      : [DC_dBFS, fundamental_dBFS, harmonic2_dBFS, ...]
%
% Notes
%   - dBFS here is referenced to full-scale *peak* of a signed res-bit value:
%       FS_peak = 2^(res-1)
%   - For real input, a single-sided spectrum is plotted (0..fs/2).
%   - For complex input, a two-sided spectrum is plotted (-fs/2..fs/2).

if nargin < 1
    retval = [];
    return;
end

if nargin < 2 || isempty(numOfHarms)
    numOfHarms = 0;
end
if nargin < 3 || isempty(res)
    res = 16;
end
if nargin < 4 || isempty(use_window)
    use_window = false;
end
if nargin < 5 || isempty(fs)
    fs = 1;
end

validateattributes(h, {'numeric'}, {'vector'}, mfilename, 'h', 1);
validateattributes(numOfHarms, {'numeric'}, {'scalar', 'integer', '>=', 0}, mfilename, 'numOfHarms', 2);
validateattributes(res, {'numeric'}, {'scalar', 'real', 'finite', '>', 0}, mfilename, 'res', 3);
validateattributes(fs, {'numeric'}, {'scalar', 'real', 'finite', '>', 0}, mfilename, 'fs', 5);

% Force column-vector math (works for row/column input)
x = h(:);
N = numel(x);
if N < 2
    retval = [];
    return;
end

% Optional windowing (Hann) with coherent gain correction to keep tone
% amplitude consistent when measuring peaks.
if logical(use_window)
    if exist('hann', 'file') == 2
        w = hann(N);
    else
        w = hanning(N);
    end
    coherentGain = sum(w) / N;
    if coherentGain ~= 0
        w = w / coherentGain;
    end
    x = x .* w;
end

X = fft(x);
fullScalePeak = 2^(res - 1);

if isreal(h)
    % Single-sided bins: k = 0..floor(N/2)
    kMax = floor(N/2);
    k = (0:kMax).';
    Xk = X(k + 1);

    % Convert FFT bins to single-sided peak amplitude (in counts)
    mag = abs(Xk) / N;
    if kMax >= 2
        % Double non-DC and non-Nyquist bins
        mag(2:end-1) = 2 * mag(2:end-1);
    end
    % Normalize to dBFS (FS_peak)
    ratio = mag / fullScalePeak;

    f = (k * fs) / N;
    spectrum_dBFS = 20 * log10(max(ratio, realmin('double')));

    % Harmonic report (folding aliases back into 0..fs/2)
    dc_dBFS = spectrum_dBFS(1);
    searchBins = 2:numel(spectrum_dBFS); % ignore DC for fundamental search
    [~, p] = max(spectrum_dBFS(searchBins));
    k0 = (searchBins(p) - 1); % 0-based bin index

    harms = dc_dBFS;
    for m = 1:(numOfHarms + 1)
        kh = mod(m * k0, N);
        if kh > kMax
            kh = N - kh;
        end
        harms(end+1) = spectrum_dBFS(kh + 1); %#ok<AGROW>
    end
else
    % Two-sided complex spectrum with fftshift
    if mod(N, 2) == 0
        k = (-N/2:N/2-1).';
    else
        k = (-(N-1)/2:(N-1)/2).';
    end
    Xs = fftshift(X);

    mag = abs(Xs) / N;        % peak amplitude (in counts)
    ratio = mag / fullScalePeak;
    spectrum_dBFS = 20 * log10(max(ratio, realmin('double')));
    f = (k * fs) / N;

    % Harmonic report computed on unshifted bins (simpler modulo arithmetic)
    mag_unshifted = abs(X) / N;
    spectrum_dBFS_unshifted = 20 * log10(max(mag_unshifted / fullScalePeak, realmin('double')));
    dc_dBFS = spectrum_dBFS_unshifted(1);

    searchBins = 2:N; % ignore DC
    [~, p] = max(spectrum_dBFS_unshifted(searchBins));
    k0 = (searchBins(p) - 1); % 0-based

    harms = dc_dBFS;
    for m = 1:(numOfHarms + 1)
        kh = mod(m * k0, N);
        harms(end+1) = spectrum_dBFS_unshifted(kh + 1); %#ok<AGROW>
    end
end

retval = harms;

figure;
plot(f, spectrum_dBFS);
xlabel('Frequency (Hz)');
ylabel('Amplitude (dBFS)');
grid on;

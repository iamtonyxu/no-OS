
function retval = PlotFFT(h, numOfHarms, res, use_window, fs)

len = length(h);

if (use_window)
    w = hann(len);
    s_w = (w / sum(w)) * len;
    h = h .* s_w;
end

ufft = abs(fft(h));
fftdata = 4 * ufft / len / (2 ^ res);

for i = 1:len
    if (fftdata(i) < 1e-15)
        fftdata(i) = 1e-15;
    end
end

if (isreal(h))
    len = len/2;
    fftdata = 20 * log10(fftdata(1:len));
    xmin = 1;
    xmax = len;
    sf = fs / (xmax -xmin) / 2;
else
    fftdata = 20 * log10(fftdata/2);
    fftdata = [fftdata(len/2+1:len); fftdata(1:len/2)];
    xmin = -len/2;
    xmax = len/2-1;
    sf = fs / (xmax - xmin);    
end

p = 1;
max = fftdata(1);
for i=2:len
    if fftdata(i) > max
        max = fftdata(i);
        p = i;
    end
end

harms = [fftdata(1)];
for i=1:numOfHarms+1
    index = p*i-i+1;
    
    %freq wrap
    index = mod(index, len*2);
    if (index > len)
        index = len*2-index+2;
    end
    %end freq wrap
      
    harms = [harms fftdata(index)];
end

%report harmonics
retval = harms;

%figure;
plot((xmin:xmax)*sf, fftdata);
axis([xmin*sf xmax*sf -100 6]);
xlabel('Frequency (Hz)');
ylabel('Amplitude (dB)');
grid on

% x = linspace(xmin * sf, xmax * sf, length(fftdata));
% % 绘制数据
% plot(x, fftdata);
% axis([xmin*sf xmax*sf -130 6]);
% xlabel('Freq (Hz)');
% ylabel('Amp (dB)');

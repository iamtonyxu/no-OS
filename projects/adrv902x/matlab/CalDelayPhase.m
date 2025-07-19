function [intDelay, fracDelay, yAligned, m] = CalDelayPhase(x, y, osRate)
% align y to x

capPos = 1;
capLen = length(x);
r = (capPos:capLen + capPos - 1);
fRange = r;

if ~exist('osRate', 'var')
    osRate = 64;
end

b = intfilt(osRate, 8, .5);

% coarse y to x
y = y(:).';
x = x(:).';
gainPhase = std(x)/std(y);
y = y*gainPhase;
[~, p] = max(abs(xcorr(y, x)));
q = p - length(y);
m = abs(q);
yAc = circshift(y, [0 -q]);
xAligned = x(:);
xAligned = xAligned.';

% get final delay best polyPhase and conditional sample shift
vMax = 0;
for k = 1:osRate
    u = conv(yAc(fRange), b(k:osRate:end), 'same');
    [v, p] = max(abs(xcorr(x(:), u)));
    if v > vMax
        vMax = v;
        optPolyPhase = k;
        CSS = p - length(x(:));
    end
end

yAligned = circshift(conv(yAc(r), b(optPolyPhase:osRate:end), 'same'), [0 CSS]);

intDelay = CSS - q;
fracDelay = optPolyPhase;

gainPhase = gainPhase*std(xAligned)/std(yAligned);
yAligned = yAligned*(std(xAligned)/std(yAligned));
ix = abs(yAligned) > 0.1;
if isempty(ix)
    ix = abs(yAligned) > 0;
end

phaseDelta = angle(xAligned(ix)./yAligned(ix));
yAligned = yAligned*exp(1i*mean(phaseDelta));
gainPhase = gainPhase*exp(1i*mean(phaseDelta));

function [gainNb, phsNb] = estChan_usingNbCorr(tx, rx)
% Estimate narrowband channel gain/phase from tx and rx using correlation.

if nargin < 2
	error('estChan_usingNbCorr requires two inputs: tx and rx.');
end

tx = tx(:);
rx = rx(:);

N = min(length(tx), length(rx));
if N == 0
	gainNb = 0;
	phsNb = 0;
	return;
end

tx = tx(1:N);
rx = rx(1:N);

den = sum(abs(tx).^2);
if den <= eps
	gainNb = 0;
	phsNb = 0;
	return;
end

H = sum(rx .* conj(tx)) / den;
phsNb = angle(H);
%gainNb = abs(H);
gainNb = sum(abs(rx))/sum(abs(tx));

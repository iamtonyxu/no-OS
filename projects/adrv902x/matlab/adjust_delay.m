function [x_aligned, y_aligned] = adjust_delay(x, y, int_delay, frac_delay)

% generate 64 filters with n/64 fractional delay,when n = 0, 1, 2, ..., 63
osRate = 64;
filt_length = 16;
c = intfilt(osRate, filt_length/2, 0.8);
c = [c, 0]; % c has only 64*16-1 interleaved coefficients, append 0 at the end to have 64*16 coefficients

% if frac_delay < 0 || frac_delay > 64
if frac_delay < 1 || frac_delay > 64
    error('Fractional delay must be in the range [1, 64]');
elseif frac_delay ~= 64
    int_delay = int_delay-1;
end

x_aligned = circshift(x, -int_delay);

y_filt = conv(y, c(frac_delay:osRate:end));
y_aligned = y_filt(filt_length/2:end-filt_length/2);

end

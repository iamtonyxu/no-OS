function [] = plot_signal_in_freq_domain(signal, Fs, fft_length, description)
    %f = fs*(0:(fft_length/2))/fft_length;
    f = Fs*((-fft_length/2):(fft_length/2-1))/fft_length;
    Y = fftshift(fft(signal(1,:), fft_length));
    P2 = abs(Y/fft_length);
    %P1 = P2(1:fft_length/2+1);

    figure;
    plot(f,P2, '.--r'); title(description); hold on

    if min(size(signal)) > 1
        % plot fft result of other vectors
        for ii = 2:min(size(signal))
            Y = fftshift(fft(signal(ii, :), fft_length));
            P2 = abs(Y/fft_length);
            plot(f, P2, '*-b');
        end
    end
end
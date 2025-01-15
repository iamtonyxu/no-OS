function [capData] = read_capture(serialCOM, capSize, waitSecond)
HEAD = 0x5D;
baudRate = 115200;

serialCOM = upper(serialCOM);
freeports = serialportlist("available");
if find(freeports == serialCOM)
    fprintf('%s is available.\n',serialCOM);
else
    fprintf('%s is NOT found.\n',serialCOM);
    return;
end

if nargin < 2
    waitSecond = 5;
end

if capSize > 16384
    disp("capture length cannot be larger than 16384");
    return;
end

device = serialport(serialCOM, baudRate, "Timeout", 10);

message = [uint8(HEAD), zeros(1, 9, 'uint8')];
% Convert lengths to 3 bytes and append
bytesCount = capSize * 2 * 2;
bytesCount = typecast(swapbytes(uint32(bytesCount)), 'uint8'); % big-endian
message(2:4)=bytesCount(2:4);

% send read capture cmd
write(device, message, 'uint8');
pause(uint8(waitSecond)); % wait for response

% read capture data
%data = randi(16,1);
data = read(device, capSize * 2 * 2, 'uint8');

data_i = zeros(1, capSize);
data_q = zeros(1, capSize);
for ii = 1:capSize-1
    data_i(ii) = data(4*(ii-1)+2)*2^8 + data(4*(ii-1)+1);
    data_q(ii) = data(4*(ii-1)+4)*2^8 + data(4*(ii-1)+3);
end

for i = 1:length(data_i)
    if data_i(i) > 2^15
        data_i(i) = data_i(i) - 2^16;
    end
    if data_q(i) > 2^15
        data_q(i) = data_q(i) - 2^16;
    end
end

capData = (data_i + 1j*data_q)./2^15;
%capData = (data_i + 1j*data_q);

%%
Fs = 122.88e6;
FFT_Length = capSize;
f = Fs*(-FFT_Length/2:(FFT_Length/2-1))/FFT_Length;

Y = fft(capData, FFT_Length);
P2 = abs(Y/FFT_Length);
figure;
plot(f,20*log10(fftshift(P2))); title('signal in freq domain');

disp("read capture data done.");
end


function [capData] = read_capture(serialCOM, cap_point, waitSecond)
HEAD = 0x5D;
baudRate = 115200;
capSize = 4096;

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

if cap_point < 0 || cap_point > 3
    fprintf("cap_point must be 0,1,2. and it's %d now.\n", cap_point);
    return;
end

device = serialport(serialCOM, baudRate, "Timeout", 10);

message = [uint8(HEAD), zeros(1, 9, 'uint8')];
% Convert lengths to 3 bytes and append
bytesCount = capSize * 2 * 2;
bytesCount = typecast(swapbytes(uint32(bytesCount)), 'uint8'); % big-endian
message(2:4)=bytesCount(2:4);
message(5) = cap_point;

% send read capture cmd
write(device, message, 'uint8');
pause(uint8(waitSecond)); % wait for response

% read capture data
%data = randi(16,1);
data = read(device, capSize * 2 * 2, 'uint8');

data_i = [];
data_q = [];

offset = capSize * 2;
for ii = 0:capSize-1
    data_i(end+1:end+2) = data(ii*2+(1:2));
    data_q(end+1:end+2) = data(offset + ii*2+(1:2));
end

signal_i = [];
signal_q = [];
for ii = 0:capSize-1
    signal_i(end+1) = data_i(2*ii+2)*2^8 + data_i(2*ii+1);
    signal_q(end+1) = data_q(2*ii+2)*2^8 + data_q(2*ii+1);
end

for i = 1:length(signal_i)
    if signal_i(i) >= 2^15
        signal_i(i) = signal_i(i) - 2^16;
    end
    if signal_q(i) >= 2^15
        signal_q(i) = signal_q(i) - 2^16;
    end
end

capData = (signal_i + 1j*signal_q)./2^15;

%%
if 0
Fs = 245.76e6;
FFT_Length = capSize;
f = Fs*(0:(FFT_Length/2))/FFT_Length;

Y = fft(capData, FFT_Length);
P2 = abs(Y/FFT_Length);
P1 = P2(1:FFT_Length/2+1);
figure;
plot(f,P1); title('signal in freq domain');
end

disp("read capture data done.");
end


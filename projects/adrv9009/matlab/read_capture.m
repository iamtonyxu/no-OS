function [capData, capTu] = read_capture(serialCOM, capSize, waitSecond)
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

data_i = [];
data_q = [];

for ii = 0:length(data)/8-1
    data_i(end+1:end+4) = data(ii*8+(1:4));
    data_q(end+1:end+4) = data(ii*8+(5:8));
end

signal_i = [];
signal_q = [];
for ii = 0:length(data_i)/2-1
    signal_i(end+1) = data_i(2*ii+2)*2^8 + data_i(2*ii+1);
    signal_q(end+1) = data_q(2*ii+2)*2^8 + data_q(2*ii+1);
end

for i = 1:length(signal_i)
    if signal_i(i) > 2^15
        signal_i(i) = signal_i(i) - 2^16;
    end
    if signal_q(i) > 2^15
        signal_q(i) = signal_q(i) - 2^16;
    end
end

capData = (signal_i + 1j*signal_q)./2^15;

%% Debug: read capBuf
if  1
data = read(device, capSize * 2 * 2, 'uint8');

data_i = [];
data_q = [];

for ii = 0:length(data)/8-1
    data_i(end+1:end+4) = data(ii*8+(1:4));
    data_q(end+1:end+4) = data(ii*8+(5:8));
end

signal_i = [];
signal_q = [];
for ii = 0:length(data_i)/2-1
    signal_i(end+1) = data_i(2*ii+2)*2^8 + data_i(2*ii+1);
    signal_q(end+1) = data_q(2*ii+2)*2^8 + data_q(2*ii+1);
end

for i = 1:length(signal_i)
    if signal_i(i) > 2^15
        signal_i(i) = signal_i(i) - 2^16;
    end
    if signal_q(i) > 2^15
        signal_q(i) = signal_q(i) - 2^16;
    end
end

capTu = (signal_i + 1j*signal_q)./2^15;
end

%%
Fs = 245.76e6*2;
FFT_Length = capSize;
f = Fs*(0:(FFT_Length/2))/FFT_Length;

Y = fft(capData, FFT_Length);
P2 = abs(Y/FFT_Length);
P1 = P2(1:FFT_Length/2+1);
figure;
plot(f,P1); title('signal in freq domain');

disp("read capture data done.");
end


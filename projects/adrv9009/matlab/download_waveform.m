function [] = download_waveform(serialCOM, signal)
HEAD = 0x5C;
baudRate = 115200;

serialCOM = upper(serialCOM);
freeports = serialportlist("available");
if find(freeports == serialCOM)
    fprintf('%s is available.\n',serialCOM);
else
    fprintf('%s is NOT found.\n',serialCOM);
    return;
end

device = serialport(serialCOM, baudRate);

message = [uint8(HEAD), zeros(1, 9, 'uint8')];
% Convert lengths to 2 bytes and append
bytesCount = length(signal) * 2 * 2;
bytesCount = typecast(swapbytes(uint32(bytesCount)), 'uint8'); % big-endian
message(2:4)=bytesCount(2:4);

write(device, message, 'uint8');
pause(1); % wait for response

data_i = real(signal);
data_q = imag(signal);

% Convert the complex waveform to fixed-point 16-bit and then to hexadecimal
data_i = int16(data_i * 32767);
data_q = int16(data_q * 32767);

% NOTE: It's aimed to skip a bug inside FPGA module axi_dpd_actuator.v
if 0
data_i_1 = data_i(1:2:end);
data_i_2 = data_i(2:2:end);
data_i(1:2:end) = data_i_2;
data_i(2:2:end) = data_i_1;

data_q_1 = data_q(1:2:end);
data_q_2 = data_q(2:2:end);
data_q(1:2:end) = data_q_2;
data_q(2:2:end) = data_q_1;
end

%data_i = typecast((int16(data_i)), 'int8'); % little-endian
%data_q = typecast((int16(data_q)), 'int8'); % little-endian

data_i = typecast(swapbytes(int16(data_i)), 'int8'); % big-endian
data_q = typecast(swapbytes(int16(data_q)), 'int8'); % big-endian



message = [];
for ii = 0:length(data_i)/2-1
    message(end+1:end+2) = data_i(2*ii+(1:2));
    message(end+1:end+2) = data_q(2*ii+(1:2));
%      data_i = 0x1100 + rem(ii,256);
%      data_q = 0x2200 + rem(ii+10, 256);
%      message(end+1:end+2) = typecast(swapbytes(int16(data_i)), 'int8');
%      message(end+1:end+2) = typecast(swapbytes(int16(data_q)), 'int8');
end

write(device, message, 'int8');
pause(1);
disp("Waveform was sent.");

end


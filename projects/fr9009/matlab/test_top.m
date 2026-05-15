close all;
clear all;

if 1
%% read capture data from cap_buf
readdata = readtable('.\readdata_rx2.txt');

signal_i = zeros(1, size(readdata, 1));
signal_q = zeros(1, size(readdata, 1));

for i = 1:length(signal_i)

%     signal_i(i) = readdata(i,1).Var1;
%     signal_q(i) = readdata(i,2).Var2;

    if readdata(i,1).Var1 >= 2^15
        signal_i(i) = readdata(i,1).Var1 - 2^16;
    else
        signal_i(i) = readdata(i,1).Var1;
    end
   
    if readdata(i,2).Var2 >= 2^15
        signal_q(i) = readdata(i,2).Var2 - 2^16;
    else
        signal_q(i) = readdata(i,2).Var2;
    end

end
signal = signal_i + 1i*signal_q;
rx1 = signal(1:2:end);
rx2 = signal(2:2:end);

else
%% read capture data from ila
% 读取并解析'ila_data.txt'
% 每个数值A都是32bit的十六进制，且A=[data_q[7:0], data_q[15:8], data_i[7:0], data_i[15:8]]
% 将data_i, data_q解析出来

% 读取每行32-bit HEX（允许带/不带 0x 前缀）
fid = fopen('ila_data.txt', 'r');
assert(fid ~= -1, 'Cannot open ila_data.txt');
C = textscan(fid, '%s', 'Delimiter', {'\r','\n','\t',' '}, 'MultipleDelimsAsOne', true);
fclose(fid);

hexStr = string(C{1});
hexStr = strtrim(hexStr);
hexStr(hexStr == "") = [];
hexStr = erase(hexStr, "0x");
hexStr = erase(hexStr, "0X");

% HEX -> uint32（4e9 < 2^53，hex2dec 的 double 可精确表示，再转 uint32）
A_u32 = uint32(hex2dec(char(hexStr)));

% 拆分出两个“字节反序”的16-bit：高16位为Q，低16位为I
q_swapped_u16 = uint16(bitshift(A_u32, -16));
i_swapped_u16 = uint16(bitand(A_u32, uint32(65535)));

% 修正字节序：[7:0][15:8] -> [15:8][7:0]
q_u16 = swapbytes(q_swapped_u16);
i_u16 = swapbytes(i_swapped_u16);

%data_q = double(typecast(q_u16, 'int16'));
%data_i = double(typecast(i_u16, 'int16'));
data_q = zeros(length(q_u16), 1);
data_i = zeros(length(q_u16), 1);

% 转为有符号 int16，再转 double 便于后续运算/绘图
for i = 1:length(q_u16)
    if q_u16(i) >= 2^15
        data_q(i) = int16((2^16 - q_u16(i)))*(-1);
    else
        data_q(i) = q_u16(i);
    end
   
    if i_u16(i) >= 2^15
        data_i(i) =  int16((2^16 - i_u16(i)))*(-1);
    else
        data_i(i) = i_u16(i);
    end
end

% 复数IQ
rx1 = (data_i + 1j*data_q);
rx1 = rx1.'; rx2 = rx1;

end

figure;
subplot(2,1,1);
plot(real(rx1)/2^16);title("real(rx1) in time-domain");
subplot(2,1,2);
plot(imag(rx1)/2^16);title("imag(rx1) in time-domain");

figure;
subplot(2,1,1);
plot(real(rx2)/2^16);title("real(rx2) in time-domain");
subplot(2,1,2);
plot(imag(rx2)/2^16);title("imag(rx2) in time-domain");

%% FFT analysis
nBits = 16;
nHarmonics = 1;
useHann = true;%wrong if useHann = true!
cfg_fs_bb = 245.76e6;
PlotFFT(rx1, nHarmonics, nBits, useHann, cfg_fs_bb);
PlotFFT(rx2, nHarmonics, nBits, useHann, cfg_fs_bb);

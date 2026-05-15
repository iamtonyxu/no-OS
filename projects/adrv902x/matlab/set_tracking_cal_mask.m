function [] = set_tracking_cal_mask(serialCOM, enable_mask)
HEAD = 0x71;
baudRate = 115200;

%* enableMask[bit]  |  Bit description
%* -----------------|----------------------
%*             [0]  | TRACK_RX1_QEC
%*             [1]  | TRACK_RX2_QEC
%*             [2]  | TRACK_ORX1_QEC
%*             [3]  | TRACK_ORX2_QEC
%*             [4]  | TRACK_TX1_LOL
%*             [5]  | TRACK_TX2_LOL
%*             [6]  | TRACK_TX1_QEC
%*             [7]  | TRACK_TX2_QEC
%*             [8]  | TRACK_RX1_HD2
%*             [9]  | TRACK_RX2_HD2

serialCOM = upper(serialCOM);
freeports = serialportlist("available");
if find(freeports == serialCOM)
    fprintf('%s is available.\n',serialCOM);
else
    fprintf('%s is NOT found.\n',serialCOM);
    return;
end

% Send a message to request data and receive the data via UART.
% mode: one byte
% address: four bytes
% Returns: four bytes of data received
% Construct the message, length = 10, starting with 0x61

device = serialport(serialCOM, baudRate, "Timeout", 3);

maskBytes = typecast(swapbytes(uint32(enable_mask)), 'uint8');% big-endian
dataBytes = typecast(swapbytes(uint32(0)), 'uint8'); % big-endian

message = [HEAD, uint8(0), maskBytes, dataBytes];

write(device, message, "uint8");

fprintf("set_tracking_cal_mask 0x%04X \n", enable_mask);

end

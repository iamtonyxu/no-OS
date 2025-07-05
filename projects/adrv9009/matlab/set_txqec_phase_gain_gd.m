function [] = set_txqec_phase_gain_gd(serialCOM, gain, phase, gd)
HEAD = 0x73;
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

gainBytes = typecast(swapbytes(uint16(gain)), 'uint8');% big-endian
phaseBytes = typecast(swapbytes(uint16(phase)), 'uint8');% big-endian
gdBytes = typecast(swapbytes(uint32(gd)), 'uint8');% big-endian

message = [HEAD, uint8(0), gainBytes, phaseBytes, gdBytes];

write(device, message, "uint8");

fprintf("set_txqec gain = 0x%04X, phase = 0x%04X, gd = 0x%04X\n", ...
    gain, phase, gd);

end

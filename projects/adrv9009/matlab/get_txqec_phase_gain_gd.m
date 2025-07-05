function [phase, gain, gd] = get_txqec_phase_gain_gd(serialCOM)
HEAD = 0x62;
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
% Construct the message, length = 10, starting with 0x60

device = serialport(serialCOM, baudRate, "Timeout", 3);

message = [HEAD, zeros(1,9,'uint8')];
write(device, message, "uint8");

pause(1); % wait for response
response = read(device, 10, 'uint8');

%response = [0x60, (uint32)enable_mask,  0, 0, 0, 0];
if response(1) == 0x60
    enable_mask = uint32(response(6)) + uint32(response(5))*2^8 + uint32(response(4))*2^16 + uint32(response(3))*2^24;
else
   enable_mask = [];
end

fprintf("set_tracking_cal_mask 0x%04X \n", enable_mask);

end

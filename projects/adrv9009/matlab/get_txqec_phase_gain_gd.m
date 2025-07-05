function [phase, gain, gd] = get_txqec_phase_gain_gd(serialCOM)
HEAD = 0x72;
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

%response = [HEAD, 0u, (int16)gain, (int16)phase, (int16)gd[0], (int16)gd[1]];
if response(1) == HEAD
    gd = zeros(1,2);
    gain = int16(uint32(response(4)) + uint32(response(3))*2^8);
    phase = int16(uint32(response(6)) + uint32(response(5))*2^8);
    gd(1) = int16(uint32(response(8)) + uint32(response(7))*2^8);
    gd(2) = int16(uint32(response(10)) + uint32(response(9))*2^8);
    fprintf("get txqec gain = 0x%04X, phase = 0x%04X, gd(0) = 0x%04X, gd(1) = 0x%04X\n",...
        gain, phase, gd(1), gd(2));
else
   phase = [];
   gain = [];
   gd = zeros(1,2);
   disp("failed to get txqec phase_gain_gd.");
end

end

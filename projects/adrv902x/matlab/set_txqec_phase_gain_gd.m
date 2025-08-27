function [] = set_txqec_phase_gain_gd(serialCOM, chan, gain, phase, gd)
HEAD = 0x73;
baudRate = 115200;

serialCOM = upper(serialCOM);
freeports = serialportlist("available");
if find(freeports == serialCOM)
    fprintf('%s is available.\n',serialCOM);
else
    fprintf('%s is NOT found.\n',serialCOM);
    return;
end

if (chan > 3) || (chan < 0)
    fprintf("chan shall be within [0,3]. Exit!\n");
    return;
end

% Send a message to request data and receive the data via UART.
% mode: one byte
% address: four bytes
% Returns: four bytes of data received
% Construct the message, length = 20, starting with 0x73

device = serialport(serialCOM, baudRate, "Timeout", 3);

gain1Bytes = typecast(swapbytes(uint16(gain(1))), 'uint8');% big-endian
gain2Bytes = typecast(swapbytes(uint16(gain(1))), 'uint8');% big-endian
gain3Bytes = typecast(swapbytes(uint16(gain(1))), 'uint8');% big-endian
gain4Bytes = typecast(swapbytes(uint16(gain(1))), 'uint8');% big-endian
gain5Bytes = typecast(swapbytes(uint16(gain(1))), 'uint8');% big-endian

phaseBytes = typecast(swapbytes(uint16(phase)), 'uint8');% big-endian
gd1Bytes = typecast(swapbytes(uint16(gd(1))), 'uint8');% big-endian
gd2Bytes = typecast(swapbytes(uint16(gd(1))), 'uint8');% big-endian

message = [HEAD, uint8(chan), gain1Bytes, gain2Bytes, gain3Bytes, gain4Bytes, gain5Bytes,...
    phaseBytes, gd1Bytes, gd2Bytes, zeros(1,2,'uint8')];

write(device, message, "uint8");

fprintf("set_txqec gain[2] = 0x%04X, phase = 0x%04X, gd(1) = 0x%04X, gd(2) = 0x%04X\n", ...
    gain(2), phase, gd(1), gd(2));

end

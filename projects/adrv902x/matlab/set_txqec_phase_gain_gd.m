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
% Construct the message, length = 10, starting with 0x61

device = serialport(serialCOM, baudRate, "Timeout", 3);

gainBytes = typecast(swapbytes(uint16(gain)), 'uint8');% big-endian
phaseBytes = typecast(swapbytes(uint16(phase)), 'uint8');% big-endian
gdBytes = typecast(swapbytes(uint32(0)), 'uint8');% big-endian

message = [HEAD, uint8(chan), gainBytes, phaseBytes, gdBytes];

write(device, message, "uint8");

fprintf("set_txqec gain = 0x%04X, phase = 0x%04X, gd(1) = 0x%04X, gd(2) = 0x%04X\n", ...
    gain, phase, gd(1), gd(2));

end

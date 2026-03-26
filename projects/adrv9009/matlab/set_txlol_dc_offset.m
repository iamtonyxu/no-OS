function [] = set_txlol_dc_offset(serialCOM, dc_offset)
HEAD = 0x83;
baudRate = 115200;

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

iBytes = typecast(swapbytes(int16(dc_offset.i)), 'uint8');% big-endian
qBytes = typecast(swapbytes(int16(dc_offset.q)), 'uint8');% big-endian
gdBytes = typecast(swapbytes(uint32(0)), 'uint8');% big-endian

message = [HEAD, uint8(0), iBytes, qBytes, gdBytes];

write(device, message, "uint8");

fprintf("set_txlol dc_offset = 0x%04X, 0x%04X\n", dc_offset.i, dc_offset.q);

end

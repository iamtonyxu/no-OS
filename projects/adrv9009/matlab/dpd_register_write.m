function [] = dpd_register_write(serialCOM, offset, wrdata)
HEAD = 0x6A;
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
% Construct the message, length = 10, starting with HEAD

device = serialport(serialCOM, baudRate, "Timeout", 3);

%offset = 0x1234; wrdata = 0xabcd;
%addrBytes = typecast(uint32(offset), 'uint8'); % little-endian
%dataBytes = typecast(uint32(wrdata), 'uint8'); % little-endian

addrBytes = typecast(swapbytes(uint32(offset)), 'uint8');% big-endian
dataBytes = typecast(swapbytes(uint32(wrdata)), 'uint8'); % big-endian

message = [HEAD, uint8(0), addrBytes, dataBytes];

write(device, message, "uint8");

%offset = 1234;wrdata = 4567;
fprintf("dpd_register_write 0x%04X 0x%04X\n", offset, wrdata);

end
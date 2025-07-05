function [rddata] = spi_read(serialCOM, rdaddr)
HEAD = 0x5B;
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
% Construct the message, length = 10, starting with 0x5B

device = serialport(serialCOM, baudRate, "Timeout", 5);

%rdaddr = 0x1234;
message = [HEAD, uint8(0), zeros(1,4,'uint8'), zeros(1,4,'uint8')];
%addrBytes = typecast(uint32(rdaddr), 'uint8'); % little-endian
addrBytes = typecast(swapbytes(uint32(rdaddr)), 'uint8');% big-endian
message(2+(1:length(addrBytes))) = addrBytes;

write(device, message, "uint8");

pause(1); % wait for response
response = read(device, 10, 'uint8');

%response = [0x5B, 0, 52, 18,  0, 0, 0x12, 0x34, 0x56, 0x78];
if response(1) == 0x5B
    rdaddr = uint32(response(6)) + uint32(response(5))*2^8 + uint32(response(4))*2^16 + uint32(response(3))*2^24;
    rddata = uint32(response(10)) + uint32(response(9))*2^8 + uint32(response(8))*2^16 + uint32(response(7))*2^24;
else
   rddata = [];
end

fprintf("spi_read 0x%08X 0x%08X\n", rdaddr, rddata);

end


function [dc_offset] = get_txlol_dc_offset(serialCOM)
HEAD = 0x82;
baudRate = 115200;
% define dc_offset as a struct with two fields: i and q, both are int16
dc_offset = struct('i', int16(0), 'q', int16(0));

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

%response = [HEAD, 0u, (int16)dc_offset.i, (int16)dc_offset.q, zeros(1,4,'uint8')];
if response(1) == HEAD
    dc_offset.i = int16(uint32(response(4)) + uint32(response(3))*2^8);
    dc_offset.q = int16(uint32(response(6)) + uint32(response(5))*2^8);
    fprintf("get txlol dc_offset = 0x%04X, 0x%04X\n", dc_offset.i, dc_offset.q);
else
   dc_offset.i = int16(0);
   dc_offset.q = int16(0);
   disp("failed to get txlol dc_offset.");
end

end

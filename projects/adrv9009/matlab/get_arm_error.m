function [err_source, err_code] = get_arm_error(serialCOM)
HEAD = 0x74;
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

message = [HEAD, zeros(1,9,'uint8')];
write(device, message, "uint8");

pause(1); % wait for response
response = read(device, 10, 'uint8');

%response = [HEAD, 0u, (uint32)err_source, (uint32)err_code];
if response(1) == HEAD
    err_source = uint32(response(6)) + uint32(response(5))*2^8 + uint32(response(4))*2^16 + uint32(response(3))*2^24;
    err_code = uint32(response(10)) + uint32(response(9))*2^8 + uint32(response(8))*2^16 + uint32(response(7))*2^24;
else
   err_source = 0xFFFFFFFF;
   err_code = 0xFFFFFFFF;
end

fprintf("get arm err_source = 0x%04X, err_code = 0x%04X\n", ...
    err_source, err_code);

end

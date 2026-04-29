function [] = set_capture_delay(serialCOM, int_delay, frac_delay)
HEAD = 0x5F;
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

if int_delay > 255 || int_delay < 0
    int_delay = 0;
end

if frac_delay > 15 || frac_delay < 0
    frac_delay = 0;
end

message = [HEAD, uint8(int_delay), uint8(frac_delay), zeros(1,7,'uint8')];

write(device, message, "uint8");

fprintf("set int_delay = %d, frac_delay = %d.\n", int_delay, frac_delay);


end

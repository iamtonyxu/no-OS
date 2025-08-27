function [int_delay, frac_delay] = read_extpath_delay(serialCOM)
HEAD = 0x60;
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
% Construct the message, length = 20, starting with 0x60

device = serialport(serialCOM, baudRate, "Timeout", 3);
message = [HEAD, zeros(1,19,'uint8')];
write(device, message, "uint8");

pause(1); % wait for response
response = read(device, 20, 'uint8');

if response(1) == HEAD
    int_delay = response(2);
    frac_delay = response(3);
    fprintf("get int_delay = %d, frac_delay = %d.\n", int_delay, frac_delay);
else
    disp("error to read extpath delay.");
end

end

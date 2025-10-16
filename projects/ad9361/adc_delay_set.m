function [] = adc_delay_set(serialCOM, delay)
HEAD = 0x61;
baudRate = 115200;

serialCOM = upper(serialCOM);
freeports = serialportlist("available");
if find(freeports == serialCOM)
    fprintf('%s is available.\n',serialCOM);
else
    fprintf('%s is NOT found.\n',serialCOM);
    return;
end

% delay: 0~31
if delay < 0 || delay > 31
    fprintf('Error: delay should be 0~31.\n');
    return;
end

% Send a message to request data and receive the data via UART.
% mode: one byte
% address: four bytes
% Returns: four bytes of data received
% Construct the message, length = 10, starting with 0x61

device = serialport(serialCOM, baudRate, "Timeout", 3);

no_of_lanes = 8;
message = [HEAD, uint8(no_of_lanes), uint8(delay), zeros(1,7,'uint8')];

write(device, message, "uint8");

fprintf("adc_delay_set %d\n", delay);

end

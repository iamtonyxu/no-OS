function [delay] = adc_delay_get(serialCOM, lane)
HEAD = 0x70;
baudRate = 115200;

serialCOM = upper(serialCOM);
freeports = serialportlist("available");
if find(freeports == serialCOM)
    fprintf('%s is available.\n',serialCOM);
else
    fprintf('%s is NOT found.\n',serialCOM);
    return;
end

% lane: 1~8
if lane < 0 || lane > 7
    fprintf('Error: lane should be 1~8.\n');
    return;
end

% Send a message to request data and receive the data via UART.
% mode: one byte
% address: four bytes
% Returns: four bytes of data received
% Construct the message, length = 10, starting with 0x5B

device = serialport(serialCOM, baudRate, "Timeout", 5);

message = [HEAD, uint8(lane), zeros(1,8,'uint8')];
write(device, message, "uint8");

pause(1); % wait for response
response = read(device, 10, 'uint8');

if response(1) == 0x70
    delay = response(3);
else
    delay = [];
end

fprintf("adc_delay_get %d\n", delay);
end
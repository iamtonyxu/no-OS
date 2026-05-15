function [] = set_dac_datasel(serialCOM, datasel, tone_freq, tone_scale)
HEAD = 0x5E;
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

scaleBytes = typecast(swapbytes(uint16(tone_scale)), 'uint8');% big-endian

message = [HEAD, uint8(datasel), uint8(tone_freq), scaleBytes, zeros(1,5,'uint8')];

write(device, message, "uint8");

if datasel == 0
    fprintf("set_tone freq = %dMHz, scale = %4d.\n", tone_freq, tone_scale);
else
    fprintf("set PN data.\n");
end

end

function swapped = swap_bytes(number)
    % This function swaps the high byte and low byte of a 16-bit number.

    % Mask the low byte and shift it left by 8 bits
    low_byte = bitand(number, 255);   % 255 is 0xFF
    low_byte_shifted = bitshift(low_byte, 8);

    % Mask the high byte and shift it right by 8 bits
    high_byte = bitand(number, 65280);  % 65280 is 0xFF00
    high_byte_shifted = bitshift(high_byte, -8);

    % Combine the shifted bytes
    swapped = bitor(low_byte_shifted, high_byte_shifted);
end
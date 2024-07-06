import serial
import time

COM_PORT = 'COM3'
BAUD_RATE = 115200
MESSAGE_LEN = 10

import serial.tools.list_ports

def list_serial_ports():
    ports = serial.tools.list_ports.comports()
    available_ports = [port.device for port in ports]
    return available_ports
    
def spi_write(mode, address, data):
    """
    Send a message via UART.
    mode: one byte
    address: four bytes
    data: four bytes
    """
    # Construct the message, length = 10, starting with 0x5A
    message = bytes([mode]) + address.to_bytes(4, 'big') + data.to_bytes(4, 'big')
    # Send the message
    ser.write(bytes([0x5A]))
    ser.flush()
    
    ser.write(message)
    ser.flush()

    print('spi_write' + ' ' + str(mode) + ' ' + str(hex(address)) + ' ' + str(hex(data)))

def spi_read(mode, address):
    """
    Send a message to request data and receive the data via UART.
    mode: one byte
    address: four bytes
    Returns: four bytes of data received
    """
    # Construct the message, length = 10, starting with 0x5B
    data = 0
    message = bytes([0x5B, mode]) + address.to_bytes(4, 'big') +  data.to_bytes(4, 'big')
    # Send the message
    ser.write(message)
    #print('spi_read' + ' ' + str(mode) + ' ' + str(hex(address)))
    
    # Wait and read the response
    start_time = time.time()
    while True:
        if ser.in_waiting > 0:
            # Read the response: 0x5A, mode, address, 4 bytes data, 0xA5
            response = ser.read(MESSAGE_LEN)
            #print(response)
            if response[0] == 0x5B:
                # Extract and return the data part
                addr = (response[2] << 8*3) | (response[3] << 8*2) | (response[4] << 8) | response[5]
                data = (response[6] << 8*3) | (response[7] << 8*2) | (response[8] << 8) | response[9]
                print('spi_read' + ' ' + str(mode) + ' ' + str(hex(addr)) + ' ' + str(hex(data)))
                return data
        if time.time() - start_time > 1:
            # Timeout condition
            raise TimeoutError("UART read operation timed out.")
            return None

def float_to_16bit_signed(f):
    """
    Convert a float to a 16-bit representation with the highest bit as signed bit.
    Mapping 1.0 directly to 0xFFFF.
    """
    # Mapping the range assuming -1.0 maps to 0x8000 and 1.0 maps to 0x7FFF
    if f >= 0:
        return int(f * 0x7FFF) & 0xFFFF
    else:
        return int((f * 0x8000) + 0x10000) & 0xFFFF

def download_waveform(data_i, data_q):
    # Ensure data_i and data_q have the same length
    if len(data_i) != len(data_q):
        raise ValueError("data_i and data_q must have the same length")
    
    message = [0x5C]  # Start message with 0x5C
    
    # Convert lengths to 2 bytes and append
    length = len(data_i) * 2 * 2 # Assuming data_i and data_q are the same length
    message += [(length >> 16) & 0xFF, (length >> 8) & 0xFF, length & 0xFF]
    
    # zeros padding
    for i in range(6):
        message += [0x00]
        
    # Convert message to bytes and send over UART
    ser.write(bytearray(message))   
    time.sleep(1)

    message = []
    # Process data_i and data_q elements
    for i, q in zip(data_i, data_q):
        #i_16bit = float_to_16bit_signed(i)
        #q_16bit = float_to_16bit_signed(q)
        #message += [(i_16bit >> 8) & 0xFF, i_16bit & 0xFF, (q_16bit >> 8) & 0xFF, q_16bit & 0xFF]
        message += [(i >> 8) & 0xFF, i & 0xFF, (q >> 8) & 0xFF, q & 0xFF]
    
    # Convert message to bytes and send over UART
    ser.write(bytearray(message))
    print("Waveform data was sent.")

def read_iq_data(filename):
    I_data = []
    Q_data = []
    
    with open(filename, 'r') as file:
        for line in file:
            line = line.strip()  # Remove any leading/trailing whitespace
            if line.startswith('0x'):
                line = line[2:]  # Remove the '0x' prefix
                
                # Assuming each I and Q are 16 bits (4 hex digits)
                hex_I = line[:4]
                hex_Q = line[4:8]
                
                # Convert hex to signed integer
                I = int(hex_I, 16)
                if I >= 0x8000:  # Check if negative number in 16-bit two's complement
                    I -= 0x10000
                
                Q = int(hex_Q, 16)
                if Q >= 0x8000:  # Check if negative number in 16-bit two's complement
                    Q -= 0x10000
                
                I_data.append(I)
                Q_data.append(Q)
    
    return I_data, Q_data
    
def download_waveformfile(data_file = 'c:/tmp/waveform.txt'):
    data_i, data_q = read_iq_data(data_file)
    download_waveform(data_i, data_q)

def read_capture(cap_size = 1024, file_path = 'c:/tmp/cap_data.txt'):  
    if cap_size > 16384:
        raise ValueError("capture length cannot be larger than 16384")

    message = [0x5D]  # Start message with 0x5D
    
    # Convert cap_size to num_bytes
    num_bytes = cap_size * 2 * 2
    message += [(num_bytes >> 16) & 0xFF, (num_bytes >> 8) & 0xFF, num_bytes & 0xFF]
    
    # zeros padding
    for i in range(6):
        message += [0x00]
        
    # Convert message to bytes and send over UART
    ser.write(bytearray(message))  

    # wait 1s, may need 6s for 16384 samples
    time.sleep(1)

    # Read specified number of bytes
    data = ser.read_all()

    data_i = []
    data_q = []
    for i in range(cap_size):
        data_i += [((data[4*i+1] << 8) | data[4*i])]
        data_q += [((data[4*i+3] << 8)| data[4*i+2])]

    with open(file_path, 'w') as file:
        for i, q in zip(data_i, data_q):
            file.write(f'{i:05d}, {q:05d} \n')  # Writing bytes as dec
            
    print(f"Received {num_bytes} bytes and saved to {file_path}")

def select_sdcard_waveform(file_id, file_size):
    """
    Send a message via UART.
    file_id: 0,1,2,..., consists of file name: TEST0.BIN, TEST1.BIN ...
    file_size: total bytes number of this waveform file
    """
    # Construct the message, length = 10, starting with 0x5A
    message = bytes([0x5E]) + file_id.to_bytes(1, 'big') + file_size.to_bytes(4, 'big')
    
    # zeros padding
    for i in range(4):
        message += bytes([0x00])
        
    # Send the message 
    ser.write(message)
    ser.flush()

    print('select_sdcard_waveform' + ' ' + 'TEST' + str(file_id) + '.BIN' + ' with '+ str(file_size) + ' bytes')

######################################################
if __name__ == "__main__":
    ports = list_serial_ports()
    print("Available serial ports:")
    for port in ports:
        print(port)

# Initialize your serial connection here
ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
ser.isOpen()

## example: spi_write, spi_read
#spi_write(0, 0x000a, 0x12)
spi_read(0, 0x000a)

## example: download_waveformfile
download_waveformfile(r'C:/repo2/no-OS/projects/adrv9009/singleTone.txt')
#download_waveformfile(r'C:/repo2/no-OS/projects/adrv9009/demo_waveform.txt')

## example: save capture data
#read_capture(4096, 'C:/repo/project-d/matlab/iio_adc_data/spi_cap_data_1k.txt')

#select_sdcard_waveform(1, 1024)

ser.close()

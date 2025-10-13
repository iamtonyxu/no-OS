
import socket
# Message format:        OP     D1   D2_0  D2_1  D2_2  D2_3  D3_0  D3_1  D3_2  D3_3
MSG_SPI_WRITE = 0x5A
MSG_SPI_READ = 0x5B
MSG_TX_DMA_TRANSMIT = 0x5C
MSG_RX_DMA_READ = 0x5D
MSG_RX_RSSI_READ = 0x60
MSG_TX_TONE_SET = 0x61
MSG_RX_MGC_GAIN = 0x62
MSG_RX_MOD_DGB_ONOFF = 0x63
MSG_SXTRX_LOCK_STATUS = 0x64
MSG_TX_ATT_CHN = 0x65
MSG_TX_DIG_ATT = 0x66
MSG_ADC_RAM_DUMP = 0x67

# 使用示例
TARGET_IP = "192.168.1.10"
TARGET_PORT = 7
ECHO_MESSAGE = "Hello, Server!"


def gen_tcp_msg_str(msg, d1, d2, d3):
    msg_data = msg + d1 + d2 + d3
    print(f"msg: {msg}")
    print(f"d1: {d1}")
    print(f"d2: {d2}")
    print(f"d3: {d3}")
    print(f"msg_data: {msg_data}")
    return bytes(msg_data)
    '''
    send_msg = [chr(num) for num in msg_data]
    send_msg_str = "".join(send_msg)
    print("send_msg_str:",send_msg_str)
    return send_msg_str
    '''

def int32_bytes(data_32):
    data1 = (data_32 & 0xff000000) >> 24
    data2 = (data_32 & 0xff0000) >> 16
    data3 = (data_32 & 0xff00) >> 8
    data4 = data_32 & 0xff
    # return bytes([data1, data2, data3, data4])
    return [data1, data2, data3, data4]


def send_tcp_message(ip, port, message):
    """
    通过TCP协议向指定IP和端口发送字符串

    :param ip: 目标IP地址
    :param port: 目标端口号
    :param message: 要发送的字符串
    """
    try:
        # 创建TCP socket对象
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # 连接到目标服务器
        client_socket.connect((ip, port))
        # 发送数据（需要先编码为字节）
        client_socket.sendall(message)
        # client_socket.sendall(message)
        print(f"消息 '{message}' 已发送至 {ip}:{port}")

        # 可选：接收服务器响应（如果需要）
        # response = client_socket.recv(1024).decode('utf-8')
        # print(f"服务器响应: {response}")

    except Exception as e:
        print(f"连接失败: {e} 至 {ip}:{port}")
        client_socket.close()
    finally:
        # 关闭连接
        msg = client_socket.recv(1024)
        msg_data = [hex(mem) for mem in msg]
        print(f"回复消息: {msg}")
        print(f"hex_data: {msg_data}")
        #for data in msg_data:
        #    print(hex(data))
        client_socket.close()


def msg_spi_write(addr, data):
    addr_bytes = int32_bytes(addr)
    data_bytes = int32_bytes(data)
    spi_mode = [0x00]
    msg = [MSG_SPI_WRITE]
    msg_str = gen_tcp_msg_str(msg, spi_mode, addr_bytes, data_bytes)
    send_tcp_message(TARGET_IP, TARGET_PORT, msg_str)


def msg_spi_read(addr):
    addr_bytes = int32_bytes(addr)
    data_bytes = int32_bytes(0x00)
    spi_mode = [0x00]
    msg = [MSG_SPI_READ]
    msg_str = gen_tcp_msg_str(msg, spi_mode, addr_bytes, data_bytes)
    send_tcp_message(TARGET_IP, TARGET_PORT, msg_str)


def msg_tx_dma_transmit(data, data_len):
    # TBD
    data_bytes = data.to_bytes(1, byteorder='big').hex()
    d1 = []
    len_byte = int32_bytes(data_len)
    msg = [MSG_TX_DMA_TRANSMIT]
    msg_str = gen_tcp_msg_str(msg, d1, len_byte, data_bytes)
    send_tcp_message(TARGET_IP, TARGET_PORT, msg_str)


def msg_rx_dma_read(data, data_len):
    # TBD
    data_bytes = data.to_bytes(1, byteorder='big').hex()
    d1 = []
    len_byte = int32_bytes(data_len)
    msg = [MSG_RX_RSSI_READ]
    msg_str = gen_tcp_msg_str(msg, d1, len_byte, data_bytes)
    send_tcp_message(TARGET_IP, TARGET_PORT, msg_str)


def msg_rx_rssi_get(chan):
    d1 = [chan]
    d2 = int32_bytes(0x00)
    d3 = int32_bytes(0x00)
    msg = [MSG_RX_RSSI_READ]
    msg_str = gen_tcp_msg_str(msg, d1, d2, d3)
    send_tcp_message(TARGET_IP, TARGET_PORT, msg_str)


def msg_tx_tone_set(chan, onoff, freq):
    d1 = [chan]
    freq_bytes = int32_bytes(freq)
    d2 = [onoff, freq_bytes[0], freq_bytes[1], freq_bytes[2]]
    d3 = [freq_bytes[3], 0x00, 0x00, 0x00]
    msg = [MSG_TX_TONE_SET]
    msg_str = gen_tcp_msg_str(msg, d1, d2, d3)
    send_tcp_message(TARGET_IP, TARGET_PORT, msg_str)


def msg_rx_mgc_gain_set(chan, tb_index, gain):
    d1 = [chan]
    d2 = [tb_index, gain, 0x00, 0x00]
    d3 = [0x00, 0x00, 0x00, 0x00]
    msg = [MSG_RX_MGC_GAIN]
    msg_str = gen_tcp_msg_str(msg, d1, d2, d3)
    send_tcp_message(TARGET_IP, TARGET_PORT, msg_str)


def msg_rx_module_debug_onoff(en):
    en_bytes = int32_bytes(en)
    d1 = [en_bytes[0]]
    d2 = [en_bytes[1], en_bytes[2], en_bytes[3], 0x00]
    d3 = [0x00, 0x00, 0x00, 0x00]
    msg = [MSG_RX_MOD_DGB_ONOFF]
    msg_str = gen_tcp_msg_str(msg, d1, d2, d3)
    send_tcp_message(TARGET_IP, TARGET_PORT, msg_str)


def msg_sxtrx_lock_status(sx_dir):
    d1 = [sx_dir]
    d2 = [0x00, 0x00, 0x00, 0x00]
    d3 = [0x00, 0x00, 0x00, 0x00]
    msg = [MSG_SXTRX_LOCK_STATUS]
    msg_str = gen_tcp_msg_str(msg, d1, d2, d3)
    send_tcp_message(TARGET_IP, TARGET_PORT, msg_str)


def msg_tx_att_set(chan, flo, gain, immed):
    d1 = [chan]
    flo_bytes = int32_bytes(flo)
    d3 = [gain, immed, 0x00, 0x00]
    msg = [MSG_TX_ATT_CHN]
    msg_str = gen_tcp_msg_str(msg, d1, flo_bytes, d3)

    send_tcp_message(TARGET_IP, TARGET_PORT, msg_str)


def msg_tx_dig_att_set(chan, idx):
    d1 = [chan]
    idx_bytes = int32_bytes(idx)
    d3 = [0x00, 0x00, 0x00, 0x00]
    msg = [MSG_TX_DIG_ATT]
    msg_str = gen_tcp_msg_str(msg, d1, idx_bytes, d3)

    send_tcp_message(TARGET_IP, TARGET_PORT, msg_str)

if __name__ == "__main__":
    # send_tcp_message(TARGET_IP, TARGET_PORT, ECHO_MESSAGE)
    msg_spi_write(0x0A, 0x55AA)
    msg_spi_read(0x0b)
    msg_rx_rssi_get(0x01)
    msg_tx_tone_set(0x01, 0x1, 0x12345678)
    msg_rx_mgc_gain_set(0x01, 0x02, 0x03)
    msg_rx_module_debug_onoff(0x12345678)
    msg_sxtrx_lock_status(0x01)
    msg_tx_att_set(0x01, 0x12345678, 0x5a, 0xa5)
    msg_tx_dig_att_set(0x01, 0x02)



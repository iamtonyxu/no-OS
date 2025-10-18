#include <stdio.h>
#include <inttypes.h>

#include "main_uart.h"

/******************************************************************************/
/************************ Function Definitions ********************************/
/******************************************************************************/
void parse_spi_command(struct no_os_spi_desc *spi);


/******************************************************************************/
/************************ Main function for uart communication ****************/
/******************************************************************************/
int main_uart(void)
{

	// parse api command
	while(1)
	{
		parse_spi_command(*spi_desc);
	}

	return 0;
}

/******************************************************************************/
/************************ parse_spi_command Functions *************************/
/******************************************************************************/
void parse_spi_command(struct no_os_spi_desc *spi)
{
	struct xil_uart_init_param platform_uart_init_par = {
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
	};

	struct no_os_uart_init_param uart_param = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &platform_uart_init_par,
		.platform_ops = &xil_uart_ops
	};

	struct no_os_uart_desc *uart_desc;
#define MAX_SIZE (16384*4*2)
	uint32_t bytes_number = 10;
	uint8_t wr_data[MAX_SIZE] = {0};
	uint32_t bytes_recv = 0;
	int32_t error = 0;

	uint8_t spi_mode = 0u;
	uint32_t spi_addr = 0;
	uint32_t spi_data = 0;

	uint8_t chan = 0;
	error = no_os_uart_init(&uart_desc, &uart_param);

	if(error == 0)
	{
		while(1)
		{
			bytes_number = 10; // length of spi_write and spi_read
			// receive data
			bytes_recv = no_os_uart_read(uart_desc, wr_data, bytes_number);

			if(bytes_recv == bytes_number)
			{
				// spi write
				spi_mode = wr_data[1];
				spi_addr = (wr_data[2] << 3*8) | (wr_data[3] << 2*8) | (wr_data[4] << 1*8) | wr_data[5];
				spi_data = (wr_data[6] << 3*8) | (wr_data[7] << 2*8) | (wr_data[8] << 1*8) | wr_data[9];
				if(wr_data[0] == 0x5A)
				{
					AD9361_WR(spi_addr, spi_data);
				}
				else if(wr_data[0] == 0x5B)
				{
#if 1
					// spi read
					spi_data = AD9361_RD(spi_addr);
#else
					spi_data = 0xa1b2c3e4;
#endif
					// send data
					wr_data[6] = (spi_data >> 3*8) & 0xff;
					wr_data[7] = (spi_data >> 2*8) & 0xff;
					wr_data[8] = (spi_data >> 1*8) & 0xff;
					wr_data[9] = (spi_data >> 0*8) & 0xff;
					no_os_uart_write(uart_desc, wr_data, bytes_number);
				}
				else if(wr_data[0] == 0x5C)
				{
					bytes_number = (wr_data[1] << 2*8) | (wr_data[2] << 1*8) | (wr_data[3] << 0*8);
					bytes_recv = no_os_uart_read(uart_desc, wr_data, bytes_number);
					if(bytes_number/4 <= DAC_BUFFER_SAMPLES)
					{
						for(int sample = 0; sample < bytes_number/4; sample++)
						{
							uint32_t iq = (wr_data[sample*4 + 1] << 0) |
										(wr_data[sample*4 + 0] << 8) |
										(wr_data[sample*4 + 3] << 16) |
										(wr_data[sample*4 + 2] << 24);
							zero_lut_iq[sample] = iq;
						}
						/* Reload transfer data memory and transfer the data */
						/* Stop tranfering the data. */
						axi_dmac_transfer_stop(tx_dmac);

						/* Reload the waveform */
						axi_dac_load_custom_data_v2(*tx_dac, zero_lut_iq, zero_lut_iq,
									 NO_OS_ARRAY_SIZE(zero_lut_iq),
									 (uintptr_t)dac_buffer);
						Xil_DCacheFlush();

						/* Transfer the data. */
						transfer.size = bytes_number;
						axi_dmac_transfer_start(tx_dmac, &transfer);

						/* Flush cache data. */
						Xil_DCacheInvalidateRange((uintptr_t)dac_buffer, sizeof(zero_lut_iq));
					}
					no_os_mdelay(10);
				}
				else if(wr_data[0] == 0x5D)
				{
					int32_t status = -1;

					/* Read the data from the ADC DMA. */
					axi_dmac_transfer_start(rx_dmac, &read_transfer);

					/* Wait until transfer finishes */
					status = axi_dmac_transfer_wait_completion(rx_dmac, 500);

					/* Flush cache data. */
					Xil_DCacheInvalidateRange((uintptr_t)adc_buffer, sizeof(adc_buffer));

					bytes_number = (wr_data[1] << 2*8) | (wr_data[2] << 1*8) | (wr_data[3] << 0*8);
					if(status < 0)
					{
						memset(wr_data, 0, bytes_number);
					}
					else
					{
						memcpy(wr_data, adc_buffer, bytes_number);
					}
					uint32_t bytes_send = 0u;
					const uint32_t bytes_chunk = 4096u;

					while(bytes_send + bytes_chunk < bytes_number)
					{
						no_os_uart_write(uart_desc, &wr_data[bytes_send], bytes_chunk);
						bytes_send += bytes_chunk;
					}
					if(bytes_send < bytes_number)
					{
						no_os_uart_write(uart_desc, &wr_data[bytes_send], (bytes_number-bytes_send));
					}

					no_os_mdelay(10);
				}
				else if(wr_data[0] == 0x60)
				{
					// read rx power: rx_rssi_get
					chan = wr_data[1];
					int rssi = rx_rssi_get(&g_phy_obj[0], chan);
					// send data
					wr_data[2] = (rssi >> 3*8) & 0xff;
					wr_data[3] = (rssi >> 2*8) & 0xff;
					wr_data[4] = (rssi >> 1*8) & 0xff;
					wr_data[5] = (rssi >> 0*8) & 0xff;
					no_os_uart_write(uart_desc, wr_data, bytes_number);
				}
				else if(wr_data[0] == 0x61)
				{
					//set tx tone: cmd_api_tx_tone
					chan = wr_data[1];
					short on = wr_data[2];
					long freq = (wr_data[3] << 3*8) | (wr_data[4] << 2*8) | (wr_data[5] << 1*8) | wr_data[6];
					cmd_api_tx_tone((TRX_CHN_ENUM)chan, on, freq);
				}
				else if(wr_data[0] == 0x62)
				{
					//set rx gain: chip_rx_mgc_gain
					chan = wr_data[1];
					RX_MGC_GAIN_ENUM tb = (RX_MGC_GAIN_ENUM)wr_data[2];
					unsigned char val = wr_data[3];
					cmd_api_rx_mgc_split_table_gain((TRX_CHN_ENUM)chan, tb, val);
				}
				else if(wr_data[0] == 0x63)
				{
					//set debug option: module_debug_onoff
					unsigned long en = (wr_data[1] << 3*8) | (wr_data[2] << 2*8) | (wr_data[3] << 1*8) | wr_data[4];
					module_debug_onoff(&g_phy_obj[0], en);
				}
				else if(wr_data[0] == 0x64)
				{
					//read sxtrx_lock_status
					short dir = wr_data[1];
					int voltage = 0;
					short lock = sxtrx_lock_status(&g_phy_obj[0], dir, &voltage);
					// send data
					wr_data[2] = (lock >> 1*8) & 0xff;
					wr_data[3] = (lock >> 0*8) & 0xff;
					wr_data[4] = (voltage >> 3*8) & 0xff;
					wr_data[5] = (voltage >> 2*8) & 0xff;
					wr_data[6] = (voltage >> 1*8) & 0xff;
					wr_data[7] = (voltage >> 0*8) & 0xff;
					no_os_uart_write(uart_desc, wr_data, bytes_number);
				}
				else if(wr_data[0] == 0x65)
				{
					//set_tx_atten_chn
					chan = wr_data[1];
					unsigned long long flo = ( (unsigned long long)wr_data[2] << 3*8) |
											( (unsigned long long)wr_data[3] << 2*8) |
											( (unsigned long long)wr_data[4] << 1*8) |
											( (unsigned long long)wr_data[5] << 0*8);
					unsigned char gain = wr_data[6];
					short immed = wr_data[7];
					set_tx_atten_chn(&g_phy_obj[0], chan, flo, gain, immed);
				}
				else if(wr_data[0] == 0x66)
				{
					//set_tx_dig_atten
					chan = wr_data[1];
					unsigned short index = (wr_data[2] << 1*8) | wr_data[3];
					set_tx_dig_atten(&g_phy_obj[0], (TRX_CHN_ENUM)chan, index);
				}
				else if(wr_data[0] == 0x67)
				{
					//todo
					//read capture data from adc_ram: cmd_api_adc_ram_dump_v2
					chan = wr_data[1];
					uint16_t len = (wr_data[2] << 1*8) | wr_data[3];
					uint16_t I_data[1024] = {0};
					uint16_t Q_data[1024] = {0};
					cmd_api_adc_ram_dump_v2((TRX_CHN_ENUM)chan, I_data, Q_data, len);
					// send data
					bytes_number = len * 4 * 2;
					for(int i = 0; i < len; i++)
					{
						wr_data[i*4 + 0] = (I_data[i] >> 8) & 0xff;
						wr_data[i*4 + 1] = (I_data[i] >> 0) & 0xff;
						wr_data[i*4 + 2] = (Q_data[i] >> 8) & 0xff;
						wr_data[i*4 + 3] = (Q_data[i] >> 0) & 0xff;
					}

					uint32_t bytes_send = 0u;
					const uint32_t bytes_chunk = 4096u;

					while(bytes_send + bytes_chunk < bytes_number)
					{
						no_os_uart_write(uart_desc, &wr_data[bytes_send], bytes_chunk);
						bytes_send += bytes_chunk;
					}
					if(bytes_send < bytes_number)
					{
						no_os_uart_write(uart_desc, &wr_data[bytes_send], (bytes_number-bytes_send));
					}
					no_os_mdelay(10);
				}
			}
		}
	}
	no_os_uart_remove(&uart_desc);
}


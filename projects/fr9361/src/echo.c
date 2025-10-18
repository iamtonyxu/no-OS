/*
 * Copyright (C) 2009 - 2019 Xilinx, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <string.h>
#include <spi_rw.h>
#include "xparameters.h"
#include "xilinx_spi.h"
#include "xilinx_gpio.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "custom_cfg.h"
#include "main_init.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"

#include "common_para.h"

#include "axi_dmac.h"
#include "lwip/err.h"
#include "lwip/tcp.h"
#if defined (__arm__) || defined (__aarch64__)
#include "xil_printf.h"
#endif


int transfer_data() {
	return 0;
}

void print_app_header()
{
#if (LWIP_IPV6==0)
	xil_printf("\n\r\n\r-----lwIP TCP echo server ------\n\r");
#else
	xil_printf("\n\r\n\r-----lwIPv6 TCP echo server ------\n\r");
#endif
	xil_printf("TCP packets sent to port 6001 will be echoed back\n\r");
}

void parse_command(struct tcp_pcb *tpcb, struct pbuf *p)
{
#define MAX_SIZE (16384*4*2)
	uint32_t bytes_number = 10;
	int32_t error = 0;

	uint8_t spi_mode = 0u;
	uint32_t spi_addr = 0;
	uint32_t spi_data = 0;
	uint8_t chan = 0;

	if (p == NULL) {
		return;
	}

	if (p->len > MAX_SIZE) {
		return;
	}

	if (tpcb == NULL	)
	{
		return;
	}

	if(error == 0)
	{
		if(p->len == bytes_number)
		{
			// spi write
			spi_mode = ((char *)p->payload)[1];
			spi_addr = (((char *)p->payload)[2] << 3*8) | (((char *)p->payload)[3] << 2*8) |
			(((char *)p->payload)[4] << 1*8) | ((char *)p->payload)[5];
			spi_data = (((char *)p->payload)[6] << 3*8) | (((char *)p->payload)[7] << 2*8) |
			(((char *)p->payload)[8] << 1*8) | ((char *)p->payload)[9];
			if(((char *)p->payload)[0] == 0x5A)
			{
#if LWIP_DEBUG
				xil_printf("\n\rSPI Write\n\rWrite addr = 0x%x, value = 0x%x\n\r", spi_addr, spi_data);
				return;
#else
				AD9361_WR(spi_addr, spi_data);
#endif
			}
			else if(((char *)p->payload)[0] == 0x5B)
			{
#if LWIP_DEBUG
				xil_printf("\n\rSPI Read\n\rRead addr = 0x%x, value = 0x%x\n\r", spi_addr, spi_data);
				spi_data = 0xa1b2c3e4;
#else
				// spi read
				spi_data = AD9361_RD(spi_addr);
#endif
				// send data
				((char *)p->payload)[6] = (spi_data >> 3*8) & 0xff;
				((char *)p->payload)[7] = (spi_data >> 2*8) & 0xff;
				((char *)p->payload)[8] = (spi_data >> 1*8) & 0xff;
				((char *)p->payload)[9] = (spi_data >> 0*8) & 0xff;
				p->len = bytes_number;
			}
			else if(((char *)p->payload)[0] == 0x5C)
			{
				bytes_number = (((char *)p->payload)[1] << 2*8) | (((char *)p->payload)[2] << 1*8) | (((char *)p->payload)[3] << 0*8);
#if LWIP_DEBUG
				xil_printf("\n\rTX DMA transimit\n\rbytes_number = %d\n\r", bytes_number);
				p->len = 1;
				return;
#else
				//bytes_recv = no_os_uart_read(uart_desc, wr_data, bytes_number);
				if(bytes_number/4 <= DAC_BUFFER_SAMPLES)
				{
					for(int sample = 0; sample < bytes_number/4; sample++)
					{
						uint32_t iq = (((char *)p->payload)[(sample+1)*4 + 1] << 0) |
									(((char *)p->payload)[(sample+1)*4 + 0] << 8) |
									(((char *)p->payload)[(sample+1)*4 + 3] << 16) |
									(((char *)p->payload)[(sample+1)*4 + 2] << 24);
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
				p->len = 1;
#endif
				/* send back the operation code to check if the command is success */
				//no_os_mdelay(10);
			}
			else if(((char *)p->payload)[0] == 0x5D)
			{
				int32_t status = -1;
#if LWIP_DEBUG
				xil_printf("\n\rRX DMA Read\n\r");
				for (int i = 0; i < ADC_BUFFER_SAMPLES * ADC_CHANNELS; i++) {
					adc_buffer[i] = i % 65536;
				}
#else
				/* Read the data from the ADC DMA. */
				axi_dmac_transfer_start(rx_dmac, &read_transfer);

				/* Wait until transfer finishes */
				status = axi_dmac_transfer_wait_completion(rx_dmac, 500);

				/* Flush cache data. */
				Xil_DCacheInvalidateRange((uintptr_t)adc_buffer, sizeof(adc_buffer));

				bytes_number = (((char *)p->payload)[1] << 2*8) | (((char *)p->payload)[2] << 1*8) | (((char *)p->payload)[3] << 0*8);
				if(status < 0)
				{
					memset(((char *)p->payload), 0, bytes_number + 1);
					/* command failed, return 0xff */
					(((char *)p->payload)[0]) = 0xff;
				}
				else
#endif
				{
					memcpy((void *)&((char *)p->payload)[1], adc_buffer, bytes_number);
				}
				p->len = bytes_number + 1;

				//no_os_mdelay(10);
			}
			else if(((char *)p->payload)[0] == 0x60)
			{
				chan = ((char *)p->payload)[1];
#if LWIP_DEBUG
				xil_printf("\n\rRX RSSI GET\n\rchan = %d\n\r", chan);
				int rssi = 0x12345678;
				// read rx power: rx_rssi_get
#else
				int rssi = rx_rssi_get(&g_phy_obj[0], chan);
#endif
				// send data
				((char *)p->payload)[2] = (rssi >> 3*8) & 0xff;
				((char *)p->payload)[3] = (rssi >> 2*8) & 0xff;
				((char *)p->payload)[4] = (rssi >> 1*8) & 0xff;
				((char *)p->payload)[5] = (rssi >> 0*8) & 0xff;
				p->len = 6;
			}
			else if(((char *)p->payload)[0] == 0x61)
			{
				//set tx tone: cmd_api_tx_tone
				chan = ((char *)p->payload)[1];
				short on = ((char *)p->payload)[2];
				long freq = (((char *)p->payload)[3] << 3*8) | (((char *)p->payload)[4] << 2*8) | (((char *)p->payload)[5] << 1*8) | ((char *)p->payload)[6];
#if LWIP_DEBUG
				xil_printf("\n\rTX TONE\n\rChan = %d, on = %d, Freq = %ld\n\r", chan, on, freq);
				return;
#else
				cmd_api_tx_tone((TRX_CHN_ENUM)chan, on, freq);
#endif
				p->len = 1;
			}
			else if(((char *)p->payload)[0] == 0x62)
			{
				//set rx gain: chip_rx_mgc_gain
				chan = ((char *)p->payload)[1];
				RX_MGC_GAIN_ENUM tb = (RX_MGC_GAIN_ENUM)((char *)p->payload)[2];
				unsigned char val = ((char *)p->payload)[3];
#if LWIP_DEBUG
				xil_printf("\n\rRX GAIN\n\rChan = %d, Table = %d, Gain = %d\n\r", chan, tb, val);
				return;
#else
				cmd_api_rx_mgc_split_table_gain((TRX_CHN_ENUM)chan, tb, val);
#endif
				p->len = 1;
			}
			else if(((char *)p->payload)[0] == 0x63)
			{
				//set debug option: module_debug_onoff
				unsigned long en = (((char *)p->payload)[1] << 3*8) | (((char *)p->payload)[2] << 2*8) | (((char *)p->payload)[3] << 1*8) | ((char *)p->payload)[4];
#if LWIP_DEBUG
				xil_printf("\n\rDEBUG OPTION\n\rEnable = 0x%x\n\r", en);
				return;
#else
				module_debug_onoff(&g_phy_obj[0], en);
#endif
				p->len = 1;
			}
			else if(((char *)p->payload)[0] == 0x64)
			{
				//read sxtrx_lock_status
				short dir = ((char *)p->payload)[1];
				int voltage = 0;
#if LWIP_DEBUG
				xil_printf("\n\rLOCK STATUS\n\rDir = %d\n\r", dir);
				short lock = 0x55AA;
				voltage = 0x12345678;
				return;
#else
				short lock = sxtrx_lock_status(&g_phy_obj[0], dir, &voltage);
#endif
				// send data
				((char *)p->payload)[2] = (lock >> 8) & 0xff;
				((char *)p->payload)[3] = (lock >> 0) & 0xff;
				((char *)p->payload)[4] = (voltage >> 24) & 0xff;
				((char *)p->payload)[5] = (voltage >> 16) & 0xff;
				((char *)p->payload)[6] = (voltage >> 8) & 0xff;
				((char *)p->payload)[7] = (voltage >> 0) & 0xff;
				p->len = 8;
			}
			else if(((char *)p->payload)[0] == 0x65)
			{
				//set_tx_atten_chn
				chan = ((char *)p->payload)[1];
				unsigned long long flo = ( (unsigned long long)((char *)p->payload)[2] << 3*8) |
										( (unsigned long long)((char *)p->payload)[3] << 2*8) |
										( (unsigned long long)((char *)p->payload)[4] << 1*8) |
										( (unsigned long long)((char *)p->payload)[5] << 0*8);
				unsigned char gain = ((char *)p->payload)[6];
				short immed = ((char *)p->payload)[7];
#if LWIP_DEBUG
				xil_printf("\n\rTX ATTEN\n\rChan = %d, Flo = %lld, Gain = %d, Immed = %d\n\r", chan, flo, gain, immed);
				return;
#else
				set_tx_atten_chn(&g_phy_obj[0], chan, flo, gain, immed);
#endif
				p->len = 1;
			}
			else if(((char *)p->payload)[0] == 0x66)
			{
				//set_tx_dig_atten
				chan = ((char *)p->payload)[1];
				unsigned short index = (((char *)p->payload)[2] << 1*8) | ((char *)p->payload)[3];
#if LWIP_DEBUG
				xil_printf("\n\rTX DIG ATTEN\n\rChan = %d, Index = %d\n\r", chan, index);
				return;
#else
				set_tx_dig_atten(&g_phy_obj[0], (TRX_CHN_ENUM)chan, index);
#endif
				p->len = 1;
			}
			else if(((char *)p->payload)[0] == 0x67)
			{
				//todo
				//read capture data from adc_ram: cmd_api_adc_ram_dump_v2
				chan = ((char *)p->payload)[1];
				uint16_t len = (((char *)p->payload)[2] << 1*8) | ((char *)p->payload)[3];
				uint16_t I_data[1024] = {0};
				uint16_t Q_data[1024] = {0};
				xil_printf("\n\rADC RAM DUMP\n\rChan = %d, Len = %d\n\r", chan, len);
				return;
				cmd_api_adc_ram_dump_v2((TRX_CHN_ENUM)chan, I_data, Q_data, len);
				// send data
				bytes_number = len * 4 * 2;
				for(int i = 1; i < len + 1; i++)
				{
					((char *)p->payload)[i*4 + 0] = (I_data[i] >> 8) & 0xff;
					((char *)p->payload)[i*4 + 1] = (I_data[i] >> 0) & 0xff;
					((char *)p->payload)[i*4 + 2] = (Q_data[i] >> 8) & 0xff;
					((char *)p->payload)[i*4 + 3] = (Q_data[i] >> 0) & 0xff;
				}

				p->len = bytes_number + 1;
			}
		}
	}
}


err_t recv_callback(void *arg, struct tcp_pcb *tpcb,
                               struct pbuf *p, err_t err)
{
	uint32_t send_bytes = 0;
	/* do not read the packet if we are not in ESTABLISHED state */
	if (!p) {
		tcp_close(tpcb);
		tcp_recv(tpcb, NULL);
		return ERR_OK;
	}

	/* indicate that the packet has been received */
	tcp_recved(tpcb, p->len);
	// printf("recv_callback: received %d bytes\n\r", p->len);
	// printf("recv msg: %s\n\r", (char *)p->payload);
	parse_command(tpcb, p);

	/* echo back the payload */
	/* in this case, we assume that the payload is < TCP_SND_BUF */
	if (tcp_sndbuf(tpcb) > p->len) {
		err = tcp_write(tpcb, p->payload, p->len, 1);
	} else
		xil_printf("no space in tcp_sndbuf\n\r");

	/* free the received pbuf */
	pbuf_free(p);

	return ERR_OK;
}

err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	static int connection = 1;

	/* set the receive callback for this connection */
	tcp_recv(newpcb, recv_callback);

	/* just use an integer number indicating the connection id as the
	   callback argument */
	tcp_arg(newpcb, (void*)(UINTPTR)connection);

	/* increment for subsequent accepted connections */
	connection++;

	return ERR_OK;
}


int start_application()
{
	struct tcp_pcb *pcb;
	err_t err;
	unsigned port = 7;

	/* create new TCP PCB structure */
	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\n\r");
		return -1;
	}

	/* bind to specified @port */
	err = tcp_bind(pcb, IP_ANY_TYPE, port);
	if (err != ERR_OK) {
		xil_printf("Unable to bind to port %d: err = %d\n\r", port, err);
		return -2;
	}

	/* we do not need any arguments to callback functions */
	tcp_arg(pcb, NULL);

	/* listen for connections */
	pcb = tcp_listen(pcb);
	if (!pcb) {
		xil_printf("Out of memory while tcp_listen\n\r");
		return -3;
	}

	/* specify callback to use for incoming connections */
	tcp_accept(pcb, accept_callback);

	xil_printf("TCP echo server started @ port %d\n\r", port);

	return 0;
}

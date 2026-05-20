/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Platform dependent parameters.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_drivers.h"
#include "xparameters.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define FR9009_DEVICE		 0

#define GPIO_OFFSET          54

#define CLK_CS               1

#define FR9009_CS            2

#define FR9009_RESETB        (GPIO_OFFSET + 0)
#define CLK_HPC_RESETB       (GPIO_OFFSET + 1)

#define ADRV_SYSREF_REQ      (GPIO_OFFSET + 4)
#define JESD_RX_RESET        (GPIO_OFFSET + 5)
#define JESD_TX_RESET        (GPIO_OFFSET + 6)
#define BR3109_GPIO_0_CONTRL	GPIO_OFFSET + 7
#define DDS_VIO_CONTRL       (GPIO_OFFSET + 8)
#define JESD_MAP_CONTRL_BIT0 (GPIO_OFFSET + 9)
#define JESD_MAP_CONTRL_BIT1 (GPIO_OFFSET + 10)
#define JESD_MAP_CONTRL_BIT2 (GPIO_OFFSET + 11)


#define CLK_RESETB CLK_HPC_RESETB



#define RX_JESD_BASEADDR XPAR_JESD_JESD204_RX_BASEADDR
#define TX_JESD_BASEADDR XPAR_JESD_JESD204_TX_BASEADDR


#define DDR_MEM_BASEADDR XPAR_DDR_MEM_BASEADDR
#define ADC_DDR_BASEADDR (XPAR_DDR_MEM_BASEADDR + 0x800000)
#define DAC_DDR_BASEADDR (XPAR_DDR_MEM_BASEADDR + 0xA000000)

#define JESD_TRIGGER_REG_ADDR 0x43C60000 // XPAR_CONFIG_STATUS_REG_0_S00_AXI_BASEADDR
//#define JESD0_DATA_ADDR_LANE_0     XPAR_BRAM_0_BASEADDR
//#define JESD0_DATA_ADDR_LANE_1     XPAR_BRAM_1_BASEADDR
//#define JESD1_DATA_ADDR_LANE_0     XPAR_BRAM_2_BASEADDR

#if FR9009_DEVICE
#define FR9009_CONFIG_REG_BASEADDR XPAR_AXI_FR9009_CONFIG_0_BASEADDR
#else
#define CONFIG_8_REG_BASEADDR   XPAR_AXI_CONFIG_8_REG_0_BASEADDR
#endif

#define CAPBUF_BASEADDR            XPAR_BRAM_0_BASEADDR

// TX_BUF_ADDR is fixed in data2fpga.v (wrapped in user ip axi_fr9009_config)
// TX_BUF_LEN is 32k bytes (16k samples) as default, and can be configured dynamically
// w_SADDR = 'h3f000000, w_BTT <= 'd32768;
#define TX_BUF_ADDR 0x3F000000
#define TX_BUF_LEN  16384

#endif

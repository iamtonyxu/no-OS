#ifndef __APP_RXQEC_H
#define __APP_RXQEC_H

#include <stdint.h>
#include "adi_hal.h"
#include "parameters.h"

#define QFIR_TAPS (7*2+1)
#define IFIR_TAPS (3*2+1)

#define RXQEC_BASEADDR		XPAR_AXI_RXQEC_0_BASEADDR

////////////////////////////////////////////////////////////////////////////
// registers        axi-addr    up_addr     bit       R/W     Default
// IP_VERSION       0x0000      0x0000      [31:0]    R       32'h2025_0109
// scratch          0x0004      0x0001      [31:0]    R/W     32'h0000_0000
// enable           0x0008      0x0002      [0:0]     R/W     1'b0
// reserved         0x000C      0x0003      [31:0]    R/W     16'h0000
// hi0              0x0010      0x0004      [15:0]    R/W     16'h0000
// hi1              0x0014      0x0005      [15:0]    R/W     16'h0000
// hi2              0x0018      0x0006      [15:0]    R/W     16'h0000
// hi3              0x001C      0x0007      [15:0]    R/W     16'h0000
// hi4              0x0020      0x0008      [15:0]    R/W     16'h0000
// hi5              0x0024      0x0009      [15:0]    R/W     16'h0000
// hi6              0x0028      0x000A      [15:0]    R/W     16'h0000
// hq0              0x0030      0x000C      [15:0]    R/W     16'h0000
// hq1              0x0034      0x000D      [15:0]    R/W     16'h0000
// hq2              0x0038      0x000E      [15:0]    R/W     16'h0000
// hq3              0x003C      0x000F      [15:0]    R/W     16'h0000
// hq4              0x0040      0x0010      [15:0]    R/W     16'h0000
// hq5              0x0044      0x0011      [15:0]    R/W     16'h0000
// hq6              0x0048      0x0012      [15:0]    R/W     16'h0000
// hq7              0x004C      0x0013      [15:0]    R/W     16'h0000
// hq8              0x0050      0x0014      [15:0]    R/W     16'h0000
// hq9              0x0054      0x0015      [15:0]    R/W     16'h0000
// hq10             0x0058      0x0016      [15:0]    R/W     16'h0000
// hq11             0x005C      0x0017      [15:0]    R/W     16'h0000
// hq12             0x0060      0x0018      [15:0]    R/W     16'h0000
// hq13             0x0064      0x0019      [15:0]    R/W     16'h0000
// hq14             0x0068      0x001A      [15:0]    R/W     16'h0000
////////////////////////////////////////////////////////////////////////////
#define OFFSET_IP_VERSION	0x0000
#define OFFSET_SCRATCH		0x0004
#define OFFSET_ENABLE		0x0008
#define OFFSET_RESERVED		0x000C
#define OFFSET_HI0			0x0010
#define OFFSET_HQ0			0x0030

uint32_t rxqec_read_ipVersion(void);

void rxqec_write_scratch(uint32_t scratch);
uint32_t rxqec_read_scratch(void);

void rxqec_write_enable(uint8_t enable);
uint32_t rxqec_read_enable(void);

void rxqec_write_hi(uint16_t hi[IFIR_TAPS]);
void rxqec_write_hq(uint16_t hq[QFIR_TAPS]);

uint32_t rxqec_read_hi(uint16_t hi[IFIR_TAPS]);
uint32_t rxqec_read_hq(uint16_t hq[QFIR_TAPS]);

void rxqec_write(uint16_t offset, uint32_t wrdata);
uint32_t rxqec_read(uint16_t offset);

#endif

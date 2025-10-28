#ifndef __AXI_TDD_H__
#define __AXI_TDD_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include "no_os_util.h"
#include "no_os_axi_io.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AXI_TDD_REG_PCORE_VERSION       0x0000
#define AXI_TDD_REG_ID                  0x0004
#define AXI_TDD_REG_SCRATCH             0x0008
#define AXI_TDD_REG_PCORE_MAGIC         0x000c
#define AXI_TDD_REG_CTRL                0x0040
#define AXI_TDD_GATED_TX_DMAPATH        NO_OS_BIT(5)
#define AXI_TDD_GATED_RX_DMAPATH        NO_OS_BIT(4)
#define AXI_TDD_TX_ONLY                 NO_OS_BIT(3)
#define AXI_TDD_RX_ONLY                 NO_OS_BIT(2)
#define AXI_TDD_SECONDARY               NO_OS_BIT(1)
#define AXI_TDD_ENABLE                  NO_OS_BIT(0)


/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct axi_tdd {
    /** Base Address */
    uint32_t base;
    /* PCORE_VERSION */
    uint32_t pcore_version;
    uint32_t id;
    uint32_t scratch;
    uint32_t pcore_magic;
    uint32_t ctrl;
};

struct axi_tdd_init {
    /** Base Address */
    uint32_t base;
    
    /* scratchpad */
    uint32_t scratch;

    /* tdd configuration */
    uint8_t tdd_enable;
    uint8_t tdd_secondary;
    uint8_t tdd_rx_only;
    uint8_t tdd_tx_only;
    uint8_t tdd_gated_tx_dmapath;
    uint8_t tdd_gated_rx_dmapath;
    uint8_t tdd_terminal_type;

    uint8_t tdd_burst_count;
    uint32_t tdd_counter_init;
    uint32_t tdd_frame_length;
    uint32_t tdd_vco_rx_on_1;
    uint32_t tdd_vco_rx_off_1;
    uint32_t tdd_vco_tx_on_1;
    uint32_t tdd_vco_tx_off_1;
    uint32_t tdd_rx_on_1;
    uint32_t tdd_rx_off_1;
    uint32_t tdd_rx_dp_on_1;
    uint32_t tdd_rx_dp_off_1;
    uint32_t tdd_tx_on_1;
    uint32_t tdd_tx_off_1;
    uint32_t tdd_tx_dp_on_1;
    uint32_t tdd_tx_dp_off_1;
    uint32_t tdd_vco_rx_on_2;
    uint32_t tdd_vco_rx_off_2;
    uint32_t tdd_vco_tx_on_2;
    uint32_t tdd_vco_tx_off_2;
    uint32_t tdd_rx_on_2;
    uint32_t tdd_rx_off_2;
    uint32_t tdd_rx_dp_on_2;
    uint32_t tdd_rx_dp_off_2;
    uint32_t tdd_tx_on_2;
    uint32_t tdd_tx_off_2;
    uint32_t tdd_tx_dp_off_2;
    uint32_t tdd_tx_dp_on_2;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t axi_tdd_read(uint32_t tdd_base, uint32_t reg_addr, uint32_t *reg_data);
int32_t axi_tdd_write(uint32_t tdd_base, uint32_t reg_addr, uint32_t reg_data);

int32_t axi_tdd_init(struct axi_tdd *tdd, struct axi_tdd_init *init_param);
int32_t axi_tdd_tx_only(struct axi_tdd *tdd);
int32_t axi_tdd_rx_only(struct axi_tdd *tdd);

#endif // __AXI_TDD_H__
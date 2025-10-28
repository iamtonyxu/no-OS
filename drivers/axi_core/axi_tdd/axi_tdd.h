#ifndef __AXI_TDD_H__
#define __AXI_TDD_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include "no_os_util.h"
#include "no_os_axi_io.h"
#include "no_os_delay.h"

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
#define AXI_TDD_REG_BURST_COUNT         0x0044
#define AXI_TDD_REG_COUNTER_INIT        0x0048
#define AXI_TDD_REG_FRAME_LENGTH        0x004C
#define AXI_TDD_REG_TERMINAL_TYPE       0x0050
#define AXI_TDD_REG_STATUS              0x0060

#define AXI_TDD_REG_VCO_RX_ON_1         0x0080
#define AXI_TDD_REG_VCO_RX_OFF_1        0x0084
#define AXI_TDD_REG_VCO_TX_ON_1         0x0088
#define AXI_TDD_REG_VCO_TX_OFF_1        0x008C
#define AXI_TDD_REG_RX_ON_1             0x0090
#define AXI_TDD_REG_RX_OFF_1            0x0094
#define AXI_TDD_REG_TX_ON_1             0x0098
#define AXI_TDD_REG_TX_OFF_1            0x009C
#define AXI_TDD_REG_RX_DP_ON_1          0x00A0
#define AXI_TDD_REG_RX_DP_OFF_1         0x00A4
#define AXI_TDD_REG_TX_DP_ON_1          0x00A8
#define AXI_TDD_REG_TX_DP_OFF_1         0x00AC

#define AXI_TDD_REG_VCO_RX_ON_2         0x00C0
#define AXI_TDD_REG_VCO_RX_OFF_2        0x00C4
#define AXI_TDD_REG_VCO_TX_ON_2         0x00C8
#define AXI_TDD_REG_VCO_TX_OFF_2        0x00CC
#define AXI_TDD_REG_RX_ON_2             0x00D0
#define AXI_TDD_REG_RX_OFF_2            0x00D4
#define AXI_TDD_REG_TX_ON_2             0x00D8
#define AXI_TDD_REG_TX_OFF_2            0x00DC
#define AXI_TDD_REG_RX_DP_ON_2          0x00E0
#define AXI_TDD_REG_RX_DP_OFF_2         0x00E4
#define AXI_TDD_REG_TX_DP_ON_2          0x00E8
#define AXI_TDD_REG_TX_DP_OFF_2         0x00EC


/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef struct {
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
    uint32_t tdd_tx_on_1;
    uint32_t tdd_tx_off_1;
    uint32_t tdd_rx_dp_on_1;
    uint32_t tdd_rx_dp_off_1;
    uint32_t tdd_tx_dp_on_1;
    uint32_t tdd_tx_dp_off_1;
    uint32_t tdd_vco_rx_on_2;
    uint32_t tdd_vco_rx_off_2;
    uint32_t tdd_vco_tx_on_2;
    uint32_t tdd_vco_tx_off_2;
    uint32_t tdd_rx_on_2;
    uint32_t tdd_rx_off_2;
    uint32_t tdd_tx_on_2;
    uint32_t tdd_tx_off_2;
    uint32_t tdd_rx_dp_on_2;
    uint32_t tdd_rx_dp_off_2;
    uint32_t tdd_tx_dp_on_2;
    uint32_t tdd_tx_dp_off_2;
} tdd_configuration;

typedef struct{
    /** Base Address */
    uint32_t base;

    /* PCORE_VERSION */
    uint32_t pcore_version;
    uint32_t id;
    uint32_t pcore_magic;

    /* scratchpad */
    uint32_t scratch;

    /* tdd configuration */
    tdd_configuration *pconfig;

    /* tdd status */
    uint32_t tdd_status;
} axi_tdd;

typedef struct{
    /** Base Address */
    uint32_t base;

    /* tdd configuration */
    tdd_configuration config;
} axi_tdd_initParam;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t axi_tdd_read(uint32_t tdd_base, uint32_t reg_addr, uint32_t *reg_data);
int32_t axi_tdd_write(uint32_t tdd_base, uint32_t reg_addr, uint32_t reg_data);

int32_t axi_tdd_init(axi_tdd *tdd, axi_tdd_initParam *init_param);
int32_t axi_tdd_tx_only(axi_tdd *tdd);
int32_t axi_tdd_rx_only(axi_tdd *tdd);

#endif // __AXI_TDD_H__
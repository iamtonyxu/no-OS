#include "axi_tdd.h"

const int32_t AD9361_TDD_PCORE_VERSION = 0x10061;
const int32_t AD9361_TDD_PCORE_MAGIC = 0x54444443;

int32_t axi_tdd_read(uint32_t tdd_base, uint32_t reg_addr, uint32_t *reg_data)
{
    no_os_axi_io_read(tdd_base, reg_addr, reg_data);
    return 0;
}

int32_t axi_tdd_write(uint32_t tdd_base, uint32_t reg_addr, uint32_t reg_data)
{
    no_os_axi_io_write(tdd_base, reg_addr, reg_data);
    return 0;
}

int32_t axi_tdd_init(axi_tdd *tdd, axi_tdd_initParam *init_param)
{
    int32_t ret = -1;
    tdd->base = init_param->base;
    axi_tdd_read(tdd->base, AXI_TDD_REG_PCORE_VERSION, &tdd->pcore_version);
    axi_tdd_read(tdd->base, AXI_TDD_REG_PCORE_MAGIC, &tdd->pcore_magic);

    if((tdd->pcore_version == AD9361_TDD_PCORE_VERSION) &&
       (tdd->pcore_magic == AD9361_TDD_PCORE_MAGIC))
       {
           int32_t scratch_val = 0xA5A5A5A5;
           axi_tdd_write(tdd->base, AXI_TDD_REG_SCRATCH, scratch_val);
           axi_tdd_read(tdd->base, AXI_TDD_REG_SCRATCH, &tdd->scratch);
           if(tdd->scratch == scratch_val)
           {
               ret = 0; // Successful initialization
           }
       }

    // tdd configuration
    if(ret == 0)
    {
        // copy configuration
        tdd->pconfig = &(init_param->config);

        // write configuration to registers
        uint32_t ctrl_reg = 0;
        ctrl_reg |= (tdd->pconfig->tdd_enable & 0x1) << 0;
        ctrl_reg |= (tdd->pconfig->tdd_secondary & 0x1) << 1;
        ctrl_reg |= (tdd->pconfig->tdd_rx_only & 0x1) << 2;
        ctrl_reg |= (tdd->pconfig->tdd_tx_only & 0x1) << 3;
        ctrl_reg |= (tdd->pconfig->tdd_gated_tx_dmapath & 0x1) << 4;
        ctrl_reg |= (tdd->pconfig->tdd_gated_rx_dmapath & 0x1) << 5;
        ctrl_reg |= (tdd->pconfig->tdd_terminal_type & 0x1) << 6;
        axi_tdd_write(tdd->base, AXI_TDD_REG_CTRL, ctrl_reg);

        axi_tdd_write(tdd->base, AXI_TDD_REG_BURST_COUNT, tdd->pconfig->tdd_burst_count);
        axi_tdd_write(tdd->base, AXI_TDD_REG_COUNTER_INIT, tdd->pconfig->tdd_counter_init);
        axi_tdd_write(tdd->base, AXI_TDD_REG_FRAME_LENGTH, tdd->pconfig->tdd_frame_length);
        axi_tdd_write(tdd->base, AXI_TDD_REG_TERMINAL_TYPE, tdd->pconfig->tdd_terminal_type);
        axi_tdd_write(tdd->base, AXI_TDD_REG_VCO_RX_ON_1, tdd->pconfig->tdd_vco_rx_on_1);
        axi_tdd_write(tdd->base, AXI_TDD_REG_VCO_RX_OFF_1, tdd->pconfig->tdd_vco_rx_off_1);
        axi_tdd_write(tdd->base, AXI_TDD_REG_VCO_TX_ON_1, tdd->pconfig->tdd_vco_tx_on_1);
        axi_tdd_write(tdd->base, AXI_TDD_REG_VCO_TX_OFF_1, tdd->pconfig->tdd_vco_tx_off_1);
        axi_tdd_write(tdd->base, AXI_TDD_REG_RX_ON_1, tdd->pconfig->tdd_rx_on_1);
        axi_tdd_write(tdd->base, AXI_TDD_REG_RX_OFF_1, tdd->pconfig->tdd_rx_off_1);
        axi_tdd_write(tdd->base, AXI_TDD_REG_TX_ON_1, tdd->pconfig->tdd_tx_on_1);
        axi_tdd_write(tdd->base, AXI_TDD_REG_TX_OFF_1, tdd->pconfig->tdd_tx_off_1);
        axi_tdd_write(tdd->base, AXI_TDD_REG_RX_DP_ON_1, tdd->pconfig->tdd_rx_dp_on_1);
        axi_tdd_write(tdd->base, AXI_TDD_REG_RX_DP_OFF_1, tdd->pconfig->tdd_rx_dp_off_1);
        axi_tdd_write(tdd->base, AXI_TDD_REG_TX_DP_ON_1, tdd->pconfig->tdd_tx_dp_on_1);
        axi_tdd_write(tdd->base, AXI_TDD_REG_TX_DP_OFF_1, tdd->pconfig->tdd_tx_dp_off_1);
        axi_tdd_write(tdd->base, AXI_TDD_REG_VCO_RX_ON_2, tdd->pconfig->tdd_vco_rx_on_2);
        axi_tdd_write(tdd->base, AXI_TDD_REG_VCO_RX_OFF_2, tdd->pconfig->tdd_vco_rx_off_2);
        axi_tdd_write(tdd->base, AXI_TDD_REG_VCO_TX_ON_2, tdd->pconfig->tdd_vco_tx_on_2);
        axi_tdd_write(tdd->base, AXI_TDD_REG_VCO_TX_OFF_2, tdd->pconfig->tdd_vco_tx_off_2);
        axi_tdd_write(tdd->base, AXI_TDD_REG_RX_ON_2, tdd->pconfig->tdd_rx_on_2);
        axi_tdd_write(tdd->base, AXI_TDD_REG_RX_OFF_2, tdd->pconfig->tdd_rx_off_2);
        axi_tdd_write(tdd->base, AXI_TDD_REG_TX_ON_2, tdd->pconfig->tdd_tx_on_2);
        axi_tdd_write(tdd->base, AXI_TDD_REG_TX_OFF_2, tdd->pconfig->tdd_tx_off_2);
        axi_tdd_write(tdd->base, AXI_TDD_REG_RX_DP_ON_2, tdd->pconfig->tdd_rx_dp_on_2);
        axi_tdd_write(tdd->base, AXI_TDD_REG_RX_DP_OFF_2, tdd->pconfig->tdd_rx_dp_off_2);
        axi_tdd_write(tdd->base, AXI_TDD_REG_TX_DP_ON_2, tdd->pconfig->tdd_tx_dp_on_2);
        axi_tdd_write(tdd->base, AXI_TDD_REG_TX_DP_OFF_2, tdd->pconfig->tdd_tx_dp_off_2);
    }

    no_os_mdelay(10); // wait for tdd to stabilize
    // read tdd status
    axi_tdd_read(tdd->base, AXI_TDD_REG_STATUS, &tdd->tdd_status);

    return ret;
}

int32_t axi_tdd_tx_only(axi_tdd *tdd)
{
    uint32_t ctrl_reg = 0;
    axi_tdd_read(tdd->base, AXI_TDD_REG_CTRL, &ctrl_reg);
    ctrl_reg &= ~AXI_TDD_RX_ONLY;  // clear tdd_rx_only bit
    ctrl_reg |= AXI_TDD_TX_ONLY;     // set tdd_tx_only bit
    axi_tdd_write(tdd->base, AXI_TDD_REG_CTRL, ctrl_reg);
    return 0;
}

int32_t axi_tdd_rx_only(axi_tdd *tdd)
{
    uint32_t ctrl_reg = 0;
    axi_tdd_read(tdd->base, AXI_TDD_REG_CTRL, &ctrl_reg);
    ctrl_reg |= AXI_TDD_RX_ONLY;  // set tdd_rx_only bit
    ctrl_reg &= ~AXI_TDD_TX_ONLY;     // clear tdd_tx_only bit
    axi_tdd_write(tdd->base, AXI_TDD_REG_CTRL, ctrl_reg);
    return 0;
}

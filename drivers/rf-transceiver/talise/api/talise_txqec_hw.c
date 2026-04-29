#include "talise_hal.h"
#include "talise_txqec_hw.h"

static void __NOP()
{
    for(int i = 100; i > 0; i--);
}

/**
 *************************************************************************** 
 * Function: TALISE_set_phase_gain_gd 
 *
 * @brief    Set phase, gain, or group delay correction value for a channel
 *
 * Parameters:
 * @param [in]  chan  - channel (0 or 1)
 * @param [in]  param - parameter type to set/write to hardware
 * @param [in]  value - number to set/write
 *
 * @return   None
 *
 ***************************************************************************
*/
void TALISE_set_phase_gain_gd(void *devHalInfo, uint32_t chan, txqec_params_t param, int16_t value) 
{
	uint8_t tempdata = 0;
    /* Saturation is done in the main algorithm.  Adjustments are assumed to be in range in this routine. */
    switch (param) {
        case PARAM_PHASE: /* Adjust phase of Q */
            if (chan == 0) {
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX1_LO_DELAY_CORR_DAC_1, (value >> 2) & 0xff);
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX1_LO_DELAY_CORR_DAC_0, (value & 0x03u) | BITM_SPI_TX1_LO_DELAY_CORR_DAC_0_TX1_LO_DELAY_LATCH_EN);
                __NOP(); __NOP(); __NOP();
                talSpiReadByte(devHalInfo, TALISE_ADDR_TX1_LO_DELAY_CORR_DAC_0, &tempdata);
                tempdata = (tempdata & ~BITM_SPI_TX1_LO_DELAY_CORR_DAC_0_TX1_LO_DELAY_LATCH_EN) | 0x0u;
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX1_LO_DELAY_CORR_DAC_0, tempdata);
            } else {
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX2_LO_DELAY_CORR_DAC_1, (value >> 2) & 0xff);
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX2_LO_DELAY_CORR_DAC_0, (value & 0x03u) | BITM_SPI_TX2_LO_DELAY_CORR_DAC_0_TX2_LO_DELAY_LATCH_EN);
                __NOP(); __NOP(); __NOP();
                talSpiReadByte(devHalInfo, TALISE_ADDR_TX1_LO_DELAY_CORR_DAC_0, &tempdata);
                tempdata = (tempdata & ~BITM_SPI_TX1_LO_DELAY_CORR_DAC_0_TX1_LO_DELAY_LATCH_EN) | 0x0u;
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX1_LO_DELAY_CORR_DAC_0, tempdata);
            }
            break;

        case PARAM_GAIN: /* Adjust gain of Q */
            if (chan == 0) {
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX1_TXQEC_FILTER_COEFF_Q_LSB_2, value & 0xff);    /* program LSB's */
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX1_TXQEC_FILTER_COEFF_Q_MSB_2, (value >> 8) & 0xff);   /* and MSB's */
            } else {
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX2_TXQEC_FILTER_COEFF_Q_LSB_2, value & 0xff);   /* program LSB's */
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX2_TXQEC_FILTER_COEFF_Q_MSB_2, (value >> 8) & 0xff);   /* and MSB's */
            }
            __NOP(); __NOP(); __NOP(); /* Force update */
            talSpiWriteByte(devHalInfo, TALISE_ADDR_TX_TXQEC_FILTER_CFG, (BITM_SPI_TXQEC_FILTER_CFG_TXQEC_FILTER_CH1_COEFF_UPDATE << chan)); /* self clears after update */

            break;

        case PARAM_GROUP_DELAY: /* Adjust first-order group delay in BBF */
            if (chan == 0) {
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX1_ABBF_C1_QEC_TRIM_MSBS, (value >> 8) & 0x03);
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX1_ABBF_C1_QEC_TRIM_LSBS, (value & 0xff));
            } else {
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX2_ABBF_C1_QEC_TRIM_MSBS, (value >> 8) & 0x03);
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX2_ABBF_C1_QEC_TRIM_LSBS, (value & 0xff));
            }
            break;

        case PARAM_GROUP_DELAY_2ND: /* Adjust second-order group delay in BBF */
            if (chan == 0) {
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX1_ABBF_C2_QEC_TRIM_MSBS, (value >> 8) & 0x03);
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX1_ABBF_C2_QEC_TRIM_LSBS, (value & 0xff));
            } else {
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX2_ABBF_C2_QEC_TRIM_MSBS, (value >> 8) & 0x03);
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX2_ABBF_C2_QEC_TRIM_LSBS, (value & 0xff));
            }
            break;

        case PARAM_DIGITAL_PHASE: /* Digital phase correction */
            value = -value; /* sign is flipped relative to Tx lo delay knob */
            if (chan == 0) {
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX1_TXQEC_FILTER_COEFF_I_LSB_2, value & 0xff);
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX1_TXQEC_FILTER_COEFF_I_MSB_2, value >> 8);
            } else {
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX2_TXQEC_FILTER_COEFF_I_LSB_2, value & 0xff);
                talSpiWriteByte(devHalInfo, TALISE_ADDR_TX2_TXQEC_FILTER_COEFF_I_MSB_2, value >> 8);
            }
            __NOP(); __NOP(); __NOP();
            talSpiWriteByte(devHalInfo, TALISE_ADDR_TX_TXQEC_FILTER_CFG, (BITM_SPI_TXQEC_FILTER_CFG_TXQEC_FILTER_CH1_COEFF_UPDATE << chan)); /* self clears after update */
            break;

        default:
            break;
    } /* switch (param) */

} /* TALISE_set_phase_gain_gd */

/**
 ***************************************************************************
 * Function: TALISE_get_phase_gain_gd
 *
 * @brief    Read the correction parameters for the channel specified by 'param_mask'.
 *
 * Parameters:
 * @param [in]   ch  - channel (0 or 1)
 * @param [out]  out - txqec_outputs_t pointer containing the values
 * @param [in]   param_mask - mask indicating which parameters to read/get
 *
 * @return      None
 *
 ***************************************************************************
*/
void TALISE_get_phase_gain_gd(void *devHalInfo, uint32_t ch, txqec_outputs_t *out, uint32_t param_mask) 
{
    uint8_t tempdata = 0;
    if (param_mask & MASK_PARAM_PHASE) {
        if (ch == 0) {
            talSpiReadByte(devHalInfo, TALISE_ADDR_TX1_LO_DELAY_CORR_DAC_0, &tempdata);
            out->phase  = (uint16_t)tempdata & 0x3;
            talSpiReadByte(devHalInfo, TALISE_ADDR_TX1_LO_DELAY_CORR_DAC_1, &tempdata);
            out->phase |= (uint16_t)tempdata << 2;
        } else {
            out->phase  = (uint16_t)tempdata & 0x3;
            talSpiReadByte(devHalInfo, TALISE_ADDR_TX2_LO_DELAY_CORR_DAC_1, &tempdata);
            out->phase |= (uint16_t)tempdata << 2;
        }
    }

    if (param_mask & MASK_PARAM_GAIN) {
        if (ch == 0) {
            talSpiReadByte(devHalInfo, TALISE_ADDR_TX1_TXQEC_FILTER_COEFF_Q_LSB_2, &tempdata);
            out->gain  = (uint16_t)tempdata & 0xff;
            talSpiReadByte(devHalInfo, TALISE_ADDR_TX1_TXQEC_FILTER_COEFF_Q_MSB_2, &tempdata);
            out->gain |= (uint16_t)tempdata << 8;
        } else {
            talSpiReadByte(devHalInfo, TALISE_ADDR_TX2_TXQEC_FILTER_COEFF_Q_LSB_2, &tempdata);
            out->gain  = (uint16_t)tempdata & 0xff;
            talSpiReadByte(devHalInfo, TALISE_ADDR_TX2_TXQEC_FILTER_COEFF_Q_MSB_2, &tempdata);
            out->gain |= (uint16_t)tempdata << 8;
        }
    }

    if (param_mask & (MASK_PARAM_GROUP_DELAY | MASK_PARAM_GROUP_DELAY_2ND)) {
        if (ch == 0) {
            talSpiReadByte(devHalInfo, TALISE_ADDR_TX1_ABBF_C1_QEC_TRIM_MSBS, &tempdata);
            out->gd[0]  = (uint16_t)(tempdata & 0x3) << 8;
            talSpiReadByte(devHalInfo, TALISE_ADDR_TX1_ABBF_C1_QEC_TRIM_LSBS, &tempdata);
            out->gd[0] |= (uint16_t)(tempdata & 0xFF);

            talSpiReadByte(devHalInfo, TALISE_ADDR_TX1_ABBF_C2_QEC_TRIM_MSBS, &tempdata);
            out->gd[1]  = (uint16_t)(tempdata & 0x3) << 8;
            talSpiReadByte(devHalInfo, TALISE_ADDR_TX1_ABBF_C2_QEC_TRIM_LSBS, &tempdata);
            out->gd[1] |= (uint16_t)(tempdata & 0xFF);
        } else {
            talSpiReadByte(devHalInfo, TALISE_ADDR_TX2_ABBF_C1_QEC_TRIM_MSBS, &tempdata);
            out->gd[0]  = (uint16_t)(tempdata & 0x3) << 8;
            talSpiReadByte(devHalInfo, TALISE_ADDR_TX2_ABBF_C1_QEC_TRIM_LSBS, &tempdata);
            out->gd[0] |= (uint16_t)(tempdata & 0xFF);

            talSpiReadByte(devHalInfo, TALISE_ADDR_TX2_ABBF_C2_QEC_TRIM_MSBS, &tempdata);
            out->gd[1]  = (uint16_t)(tempdata & 0x3) << 8;
            talSpiReadByte(devHalInfo, TALISE_ADDR_TX2_ABBF_C2_QEC_TRIM_LSBS, &tempdata);
            out->gd[1] |= (uint16_t)(tempdata & 0xFF);
        }
        if (out->gd[0] & TXQEC_GRP_SIGN_CHECK) {
            out->gd[0] |= TXQEC_GRP_SIGN_EXTENSION;
        }
        if (out->gd[1] & TXQEC_GRP_SIGN_CHECK) {
            out->gd[1] |= TXQEC_GRP_SIGN_EXTENSION;
        }
    }

} /* TALISE_get_phase_gain_gd */
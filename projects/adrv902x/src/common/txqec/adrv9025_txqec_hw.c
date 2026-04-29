#include "adrv9025_txqec_hw.h"
#include "adrv9025_txqec_reg_addr_macros.h"
#include "adi_adrv9025_hal.h"

static void __NOP()
{
    for(int i = 100; i > 0; i--);
}

/**
 *************************************************************************** 
 * Function: ADRV9025_set_phase_gain_gd 
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
void ADRV9025_set_phase_gain_gd_tx3(void *devHalInfo, txqec_params_t param, int16_t value) 
{
    uint32_t chan = 2; /* Tx3 is channel 2 */
	uint8_t tempdata = 0;
    /* Saturation is done in the main algorithm.  Adjustments are assumed to be in range in this routine. */
    switch (param) {
        case PARAM_PHASE: /* Adjust phase of Q */
            adi_adrv9025_SpiByteWrite(devHalInfo, ADRV9025_ADDR_TX3_LO_DELAY_CORR_DAC_1, (value >> 2) & 0xff);
            adi_adrv9025_SpiByteWrite(devHalInfo, ADRV9025_ADDR_TX3_LO_DELAY_CORR_DAC_0, (value & 0x03u) | BITM_SPI_TX1_LO_DELAY_CORR_DAC_0_TX1_LO_DELAY_LATCH_EN);
            __NOP(); __NOP(); __NOP();
            adi_adrv9025_SpiByteRead(devHalInfo, ADRV9025_ADDR_TX3_LO_DELAY_CORR_DAC_0, &tempdata);
            tempdata = (tempdata & ~BITM_SPI_TX1_LO_DELAY_CORR_DAC_0_TX1_LO_DELAY_LATCH_EN) | 0x0u;
            adi_adrv9025_SpiByteWrite(devHalInfo, ADRV9025_ADDR_TX3_LO_DELAY_CORR_DAC_0, tempdata);
            break;

        case PARAM_GAIN: /* Adjust gain of Q */
            adi_adrv9025_SpiByteWrite(devHalInfo, ADRV9025_ADDR_TX3_TXQEC_FILTER_COEFF_Q_LSB_2, value & 0xff);    /* program LSB's */
            adi_adrv9025_SpiByteWrite(devHalInfo, ADRV9025_ADDR_TX3_TXQEC_FILTER_COEFF_Q_MSB_2, (value >> 8) & 0xff);   /* and MSB's */

            __NOP(); __NOP(); __NOP(); /* Force update */
            adi_adrv9025_SpiByteWrite(devHalInfo, ADRV9025_ADDR_TX3_TXQEC_FILTER_CFG, (BITM_SPI_TXQEC_FILTER_CFG_TXQEC_FILTER_CH1_COEFF_UPDATE << chan)); /* self clears after update */
            break;

        case PARAM_GROUP_DELAY: /* Adjust first-order group delay in BBF */
            adi_adrv9025_SpiByteWrite(devHalInfo, ADRV9025_ADDR_TX3_ABBF_C1_QEC_TRIM_MSBS, (value >> 8) & 0x03);
            adi_adrv9025_SpiByteWrite(devHalInfo, ADRV9025_ADDR_TX3_ABBF_C1_QEC_TRIM_LSBS, (value & 0xff));
            break;

        case PARAM_GROUP_DELAY_2ND: /* Adjust second-order group delay in BBF */
            adi_adrv9025_SpiByteWrite(devHalInfo, ADRV9025_ADDR_TX3_ABBF_C2_QEC_TRIM_MSBS, (value >> 8) & 0x03);
            adi_adrv9025_SpiByteWrite(devHalInfo, ADRV9025_ADDR_TX3_ABBF_C2_QEC_TRIM_LSBS, (value & 0xff));
            break;

        case PARAM_DIGITAL_PHASE: /* Digital phase correction */
            value = -value; /* sign is flipped relative to Tx lo delay knob */
            adi_adrv9025_SpiByteWrite(devHalInfo, ADRV9025_ADDR_TX3_TXQEC_FILTER_COEFF_I_LSB_2, value & 0xff);
            adi_adrv9025_SpiByteWrite(devHalInfo, ADRV9025_ADDR_TX3_TXQEC_FILTER_COEFF_I_MSB_2, value >> 8);

            __NOP(); __NOP(); __NOP();
            adi_adrv9025_SpiByteWrite(devHalInfo, ADRV9025_ADDR_TX3_TXQEC_FILTER_CFG, (BITM_SPI_TXQEC_FILTER_CFG_TXQEC_FILTER_CH1_COEFF_UPDATE << chan)); /* self clears after update */
            break;

        default:
            break;
    } /* switch (param) */

} /* ADRV9025_set_phase_gain_gd */

/**
 ***************************************************************************
 * Function: ADRV9025_get_phase_gain_gd
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
void ADRV9025_get_phase_gain_gd_tx3(void *devHalInfo, txqec_outputs_t *out, uint32_t param_mask) 
{
    uint32_t chan = 2; /* Tx3 is channel 2 */
    uint8_t tempdata = 0;
    if (param_mask & MASK_PARAM_PHASE) {
        adi_adrv9025_SpiByteRead(devHalInfo, ADRV9025_ADDR_TX3_LO_DELAY_CORR_DAC_0, &tempdata);
        out->phase  = (uint16_t)tempdata & 0x3;
        adi_adrv9025_SpiByteRead(devHalInfo, ADRV9025_ADDR_TX3_LO_DELAY_CORR_DAC_1, &tempdata);
        out->phase |= (uint16_t)tempdata << 2;
    }

    if (param_mask & MASK_PARAM_GAIN) {
        adi_adrv9025_SpiByteRead(devHalInfo, ADRV9025_ADDR_TX3_TXQEC_FILTER_COEFF_Q_LSB_2, &tempdata);
        out->gain  = (uint16_t)tempdata & 0xff;
        adi_adrv9025_SpiByteRead(devHalInfo, ADRV9025_ADDR_TX3_TXQEC_FILTER_COEFF_Q_MSB_2, &tempdata);
        out->gain |= (uint16_t)tempdata << 8;
    }

    if (param_mask & (MASK_PARAM_GROUP_DELAY | MASK_PARAM_GROUP_DELAY_2ND)) {
        adi_adrv9025_SpiByteRead(devHalInfo, ADRV9025_ADDR_TX3_ABBF_C1_QEC_TRIM_MSBS, &tempdata);
        out->gd[0]  = (uint16_t)(tempdata & 0x3) << 8;
        adi_adrv9025_SpiByteRead(devHalInfo, ADRV9025_ADDR_TX3_ABBF_C1_QEC_TRIM_LSBS, &tempdata);
        out->gd[0] |= (uint16_t)(tempdata & 0xFF);

        adi_adrv9025_SpiByteRead(devHalInfo, ADRV9025_ADDR_TX3_ABBF_C2_QEC_TRIM_MSBS, &tempdata);
        out->gd[1]  = (uint16_t)(tempdata & 0x3) << 8;
        adi_adrv9025_SpiByteRead(devHalInfo, ADRV9025_ADDR_TX3_ABBF_C2_QEC_TRIM_LSBS, &tempdata);
        out->gd[1] |= (uint16_t)(tempdata & 0xFF);

        if (out->gd[0] & TXQEC_GRP_SIGN_CHECK) {
            out->gd[0] |= TXQEC_GRP_SIGN_EXTENSION;
        }
        if (out->gd[1] & TXQEC_GRP_SIGN_CHECK) {
            out->gd[1] |= TXQEC_GRP_SIGN_EXTENSION;
        }
    }
} // ADRV9025_get_phase_gain_gd_tx3

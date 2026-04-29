#include <stdio.h>
#include "txqec_hw.h"
#include "spi.h"

static void __NOP()
{
    for(int i = 100; i > 0; i--);
}

/**
 *************************************************************************** 
 * Function: txqec_init_fir 
 *
 * @brief    Initialize/clear the TXQEC filter to create an impulse response
 *
 * Parameters:  None
 *
 * @return   None
 *
 ***************************************************************************
*/
static void txqec_init_fir(void *devHalInfo, uint32_t chan) {
  
    uint32_t  i;

   for (i = SPI_IDX_TX1_TXQEC_FILTER_COEFF_Q_MSB_0 ; i <= SPI_IDX_TX1_TXQEC_FILTER_COEFF_Q_LSB_1; i++)
   {
       spi_write(devHalInfo, i, 0x00);
   }
   spi_write(devHalInfo, SPI_IDX_TX_TXQEC_FILTER_CFG, (BITM_SPI_TXQEC_FILTER_CFG_TXQEC_FILTER_CH1_COEFF_UPDATE << 0)); /* self clears after update */

   for (i = SPI_IDX_TX2_TXQEC_FILTER_COEFF_Q_MSB_0 ; i <= SPI_IDX_TX2_TXQEC_FILTER_COEFF_Q_LSB_1; i++)
   {
       spi_write(devHalInfo, i, 0x00);
   }
   spi_write(devHalInfo, SPI_IDX_TX_TXQEC_FILTER_CFG, (BITM_SPI_TXQEC_FILTER_CFG_TXQEC_FILTER_CH2_COEFF_UPDATE << 1)); /* self clears after update */

} /* txqec_init_fir */

/**
 *************************************************************************** 
 * Function: txqec_hw_init_all_correction_values 
 *
 * @brief    Write all of the initial correction values to hardware
 *
 * Parameters:
 * @param [in]  chan - channel 0 or 1
 *
 * @return   None
 ***************************************************************************
*/
void txqec_hw_init_all_correction_values(void *devHalInfo, uint32_t chan)
{
    txqec_set_phase_gain_gd(devHalInfo, chan, PARAM_PHASE, INITIAL_PHASE_VALUE);
    txqec_set_phase_gain_gd(devHalInfo, chan, PARAM_GAIN, INITIAL_GAIN_VALUE);
    txqec_set_phase_gain_gd(devHalInfo, chan, PARAM_GROUP_DELAY, INITIAL_GD_VALUE);
    txqec_set_phase_gain_gd(devHalInfo, chan, PARAM_GROUP_DELAY_2ND, INITIAL_GD_VALUE);
    txqec_set_phase_gain_gd(devHalInfo, chan, PARAM_DIGITAL_PHASE, INITIAL_DIG_PHASE_VALUE);
}

void txqec_init(void *devHalInfo, uint32_t chan)
{
    // Initialize the hardware for TxQEC
    if (devHalInfo == NULL) {
        printf("Error: Device handle info is NULL.\n");
        return;
    }

    /* Enable TxQEC and Rx clocks */
    spi_read_mod_write(devHalInfo, SPI_IDX_CLOCK_CONTROL_1,
                       BITM_SPI_CLOCK_CONTROL_1_TXQEC_CLOCK_ENABLE,
                       BITM_SPI_CLOCK_CONTROL_1_TXQEC_CLOCK_ENABLE);

    /* Init the FIR to an impulse filter. */
    txqec_init_fir(devHalInfo, chan);

    /* power up the TX Lo-delay correction blocks (this also appears to power the C2 second-order group delay correction) */
    spi_read_mod_write(devHalInfo, SPI_IDX_LO_DELAY_CORR_CONTROL_1, BITM_SPI_LO_DELAY_CORR_CONTROL_1_TX1_LO_DELAY_CORR_PD, 0);
    spi_read_mod_write(devHalInfo, SPI_IDX_LO_DELAY_CORR_CONTROL_1, BITM_SPI_LO_DELAY_CORR_CONTROL_1_TX2_LO_DELAY_CORR_PD, 0);

    /* Reset corrections for CH1/2 */
    for (uint32_t ch = 0; ch < 2; ch++) {
        txqec_hw_init_all_correction_values(devHalInfo, ch);
    }
    printf("TxQEC hardware initialized.\n");
}

void txqec_set_phase_gain_gd(void *devHalInfo, uint32_t chan, txqec_params_t param, int16_t value)
{
    /* Saturation is done in the main algorithm.  Adjustments are assumed to be in range in this routine. */
    switch (param) {
        case PARAM_PHASE: /* Adjust phase of Q */
            if (chan == 0) {
                spi_write(devHalInfo, SPI_IDX_TX1_LO_DELAY_CORR_DAC_1, (value >> 2) & 0xff);
                spi_write(devHalInfo, SPI_IDX_TX1_LO_DELAY_CORR_DAC_0, (value & 0x03u) | BITM_SPI_TX1_LO_DELAY_CORR_DAC_0_TX1_LO_DELAY_LATCH_EN);
                __NOP(); __NOP(); __NOP();
                spi_read_mod_write(devHalInfo, SPI_IDX_TX1_LO_DELAY_CORR_DAC_0, BITM_SPI_TX1_LO_DELAY_CORR_DAC_0_TX1_LO_DELAY_LATCH_EN, 0x0u);
            } else {
                spi_write(devHalInfo, SPI_IDX_TX2_LO_DELAY_CORR_DAC_1, (value >> 2) & 0xff);
                spi_write(devHalInfo, SPI_IDX_TX2_LO_DELAY_CORR_DAC_0, (value & 0x03u) | BITM_SPI_TX2_LO_DELAY_CORR_DAC_0_TX2_LO_DELAY_LATCH_EN);
                __NOP(); __NOP(); __NOP();
                spi_read_mod_write(devHalInfo, SPI_IDX_TX2_LO_DELAY_CORR_DAC_0, BITM_SPI_TX2_LO_DELAY_CORR_DAC_0_TX2_LO_DELAY_LATCH_EN, 0x0u);
            }
            break;

        case PARAM_GAIN: /* Adjust gain of Q */
            if (chan == 0) {
                spi_write(devHalInfo, SPI_IDX_TX1_TXQEC_FILTER_COEFF_Q_LSB_2, value & 0xff);    /* program LSB's */
                spi_write(devHalInfo, SPI_IDX_TX1_TXQEC_FILTER_COEFF_Q_MSB_2, (value >> 8) & 0xff);   /* and MSB's */
            } else {
                spi_write(devHalInfo, SPI_IDX_TX2_TXQEC_FILTER_COEFF_Q_LSB_2, value & 0xff);   /* program LSB's */
                spi_write(devHalInfo, SPI_IDX_TX2_TXQEC_FILTER_COEFF_Q_MSB_2, (value >> 8) & 0xff);   /* and MSB's */
            }
            __NOP(); __NOP(); __NOP(); /* Force update */
            spi_write(devHalInfo, SPI_IDX_TX_TXQEC_FILTER_CFG, (BITM_SPI_TXQEC_FILTER_CFG_TXQEC_FILTER_CH1_COEFF_UPDATE << chan)); /* self clears after update */

            break;

        case PARAM_GROUP_DELAY: /* Adjust first-order group delay in BBF */
            if (chan == 0) {
                spi_write(devHalInfo, SPI_IDX_TX1_ABBF_C1_QEC_TRIM_MSBS, (value >> 8) & 0x03);
                spi_write(devHalInfo, SPI_IDX_TX1_ABBF_C1_QEC_TRIM_LSBS, (value & 0xff));
            } else {
                spi_write(devHalInfo, SPI_IDX_TX2_ABBF_C1_QEC_TRIM_MSBS, (value >> 8) & 0x03);
                spi_write(devHalInfo, SPI_IDX_TX2_ABBF_C1_QEC_TRIM_LSBS, (value & 0xff));
            }
            break;

        case PARAM_GROUP_DELAY_2ND: /* Adjust second-order group delay in BBF */
            if (chan == 0) {
                spi_write(devHalInfo, SPI_IDX_TX1_ABBF_C2_QEC_TRIM_MSBS, (value >> 8) & 0x03);
                spi_write(devHalInfo, SPI_IDX_TX1_ABBF_C2_QEC_TRIM_LSBS, (value & 0xff));
            } else {
                spi_write(devHalInfo, SPI_IDX_TX2_ABBF_C2_QEC_TRIM_MSBS, (value >> 8) & 0x03);
                spi_write(devHalInfo, SPI_IDX_TX2_ABBF_C2_QEC_TRIM_LSBS, (value & 0xff));
            }
            break;

        case PARAM_DIGITAL_PHASE: /* Digital phase correction */
            value = -value; /* sign is flipped relative to Tx lo delay knob */
            if (chan == 0) {
                spi_write(devHalInfo, SPI_IDX_TX1_TXQEC_FILTER_COEFF_I_LSB_2, value & 0xff);
                spi_write(devHalInfo, SPI_IDX_TX1_TXQEC_FILTER_COEFF_I_MSB_2, value >> 8);
            } else {
                spi_write(devHalInfo, SPI_IDX_TX2_TXQEC_FILTER_COEFF_I_LSB_2, value & 0xff);
                spi_write(devHalInfo, SPI_IDX_TX2_TXQEC_FILTER_COEFF_I_MSB_2, value >> 8);
            }
            __NOP(); __NOP(); __NOP();
            spi_write(devHalInfo, SPI_IDX_TX_TXQEC_FILTER_CFG, (BITM_SPI_TXQEC_FILTER_CFG_TXQEC_FILTER_CH1_COEFF_UPDATE << chan)); /* self clears after update */
            break;

        default:
            break;
    } /* switch (param) */
}

void txqec_get_phase_gain_gd(void *devHalInfo, uint32_t chan, txqec_outputs_t *out, uint32_t param_mask)
{
    uint8_t tempdata = 0;
    if (param_mask & MASK_PARAM_PHASE) {
        if (chan == 0) {
            spi_read(devHalInfo, SPI_IDX_TX1_LO_DELAY_CORR_DAC_0, &tempdata);
            out->phase  = (uint16_t)tempdata & 0x3;
            spi_read(devHalInfo, SPI_IDX_TX1_LO_DELAY_CORR_DAC_1, &tempdata);
            out->phase |= (uint16_t)tempdata << 2;
        } else {
            out->phase  = (uint16_t)tempdata & 0x3;
            spi_read(devHalInfo, SPI_IDX_TX2_LO_DELAY_CORR_DAC_1, &tempdata);
            out->phase |= (uint16_t)tempdata << 2;
        }
    }

    if (param_mask & MASK_PARAM_GAIN) {
        if (chan == 0) {
            spi_read(devHalInfo, SPI_IDX_TX1_TXQEC_FILTER_COEFF_Q_LSB_2, &tempdata);
            out->gain  = (uint16_t)tempdata & 0xff;
            spi_read(devHalInfo, SPI_IDX_TX1_TXQEC_FILTER_COEFF_Q_MSB_2, &tempdata);
            out->gain |= (uint16_t)tempdata << 8;
        } else {
            spi_read(devHalInfo, SPI_IDX_TX2_TXQEC_FILTER_COEFF_Q_LSB_2, &tempdata);
            out->gain  = (uint16_t)tempdata & 0xff;
            spi_read(devHalInfo, SPI_IDX_TX2_TXQEC_FILTER_COEFF_Q_MSB_2, &tempdata);
            out->gain |= (uint16_t)tempdata << 8;
        }
    }

    if (param_mask & (MASK_PARAM_GROUP_DELAY | MASK_PARAM_GROUP_DELAY_2ND)) {
        if (chan == 0) {
            spi_read(devHalInfo, SPI_IDX_TX1_ABBF_C1_QEC_TRIM_MSBS, &tempdata);
            out->gd[0]  = (uint16_t)(tempdata & 0x3) << 8;
            spi_read(devHalInfo, SPI_IDX_TX1_ABBF_C1_QEC_TRIM_LSBS, &tempdata);
            out->gd[0] |= (uint16_t)(tempdata & 0xFF);

            spi_read(devHalInfo, SPI_IDX_TX1_ABBF_C2_QEC_TRIM_MSBS, &tempdata);
            out->gd[1]  = (uint16_t)(tempdata & 0x3) << 8;
            spi_read(devHalInfo, SPI_IDX_TX1_ABBF_C2_QEC_TRIM_LSBS, &tempdata);
            out->gd[1] |= (uint16_t)(tempdata & 0xFF);
        } else {
            spi_read(devHalInfo, SPI_IDX_TX2_ABBF_C1_QEC_TRIM_MSBS, &tempdata);
            out->gd[0]  = (uint16_t)(tempdata & 0x3) << 8;
            spi_read(devHalInfo, SPI_IDX_TX2_ABBF_C1_QEC_TRIM_LSBS, &tempdata);
            out->gd[0] |= (uint16_t)(tempdata & 0xFF);

            spi_read(devHalInfo, SPI_IDX_TX2_ABBF_C2_QEC_TRIM_MSBS, &tempdata);
            out->gd[1]  = (uint16_t)(tempdata & 0x3) << 8;
            spi_read(devHalInfo, SPI_IDX_TX2_ABBF_C2_QEC_TRIM_LSBS, &tempdata);
            out->gd[1] |= (uint16_t)(tempdata & 0xFF);
        }
        if (out->gd[0] & TXQEC_GRP_SIGN_CHECK) {
            out->gd[0] |= TXQEC_GRP_SIGN_EXTENSION;
        }
        if (out->gd[1] & TXQEC_GRP_SIGN_CHECK) {
            out->gd[1] |= TXQEC_GRP_SIGN_EXTENSION;
        }
    }
}

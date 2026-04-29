#include "headless.h"


/// @brief  debug used

enum auxAdc_src_t
{
    ATEST0 = 22,
    ATEST1 = 23,
};

enum astest_src_t
{
    ATEST_SRC_NC0 = 0, // atest 0
    ATEST_SRC_RX_ORX_LN0,
    ATEST_SRC_RX_ORX_LN1,
    ATEST_SRC_TX0_LN0,
    ATEST_SRC_TX0_LN1,
    ATEST_SRC_DAC_LN1,
    ATEST_SRC_JESD,
    ATEST_SRC_ORFPLL,
    ATEST_SRC_LOPATH,
    ATEST_SRC_NC1 = 16, // atest 1
    ATEST_SRC_ADC_LN0,
    ATEST_SRC_ADC_LN1,
    ATEST_SRC_TX1_LN0,
    ATEST_SRC_TX1_LN1,
    ATEST_SRC_DAC_LN0,
    ATEST_SRC_BBPLL,
    ATEST_SRC_RFPLL,
    ATEST_SRC_MISC
};

char *vcheck_str[] = {
    "ADC1-Q         [ 1200]",
    "ADC1-VRP       [  950]",
    "ADC1-VRN       [  250]",
    "ADC2-Q         [ 1200]",
    "ADC2-VRP       [  950]",
    "ADC2-VRN       [  250]",
    "RX1-TIA_I      [ 1700]",
    "RX1-TIA_Q      [ 1700]",
    "RX1-TIA_I_VCM  [  750]",
    "RX1-TIA_Q_VCM  [  750]",
    "RX1-MIXER_VB   [  960]",
    "RX1-RFVDD      [ 1200]",
    "RX2-TIA_I      [ 1700]",
    "RX2-TIA_Q      [ 1700]",
    "RX2-TIA_I_VCM  [  750]",
    "RX2-TIA_Q_VCM  [  750]",
    "RX2-MIXER_VB   [  960]",
    "RX2-RFVDD      [ 1200]",
    "DAC1-I         [ 1200]",
    "DAC1-Q         [ 1200]",
    "DAC2-I         [ 1200]",
    "DAC2-Q         [ 1200]",
    "TX1-VBLO       [ 1340]",
    "TX1-VCASC      [  865]",
    "TX1-VCM2_Q     [  775]",
    "TX1-VCM1_Q     [  775]",
    "TX1-VCM        [  775]",
    "TX1-VCM2_I     [  775]",
    "TX1-VCM1_I     [  775]",
    "TX2-VBLO       [ 1340]",
    "TX2-VCASC      [  865]",
    "TX2-VCM2_Q     [  775]",
    "TX2-VCM1_Q     [  775]",
    "TX2-VCM        [  775]",
    "TX2-VCM2_I     [  775]",
    "TX2-VCM1_I     [  775]",
    "BBPLL-MMD      [ 1000]",
    "BBPLL-DTC      [ 1000]",
    "BBPLL-CP       [ 1800]",
    "BBPLL-1P2      [ 1200]",
    "BBPLL-S1       [ 1050]",
    "BBPLL-VCOL     [0|900]",
    "BBPLL-VCOM     [0|900]",
    "BBPLL-VCOH     [0|900]",
    "BBPLL-VCOBUF   [  900]",
    "BBPLL-BIASVARA [  900]",
    "RFPLL-MMD      [ 1000]",
    "RFPLL-DTC      [ 1000]",
    "RFPLL-CP       [ 1800]",
    "RFPLL-1P2      [ 1200]",
    "RFPLL-S1       [ 1050]",
    "RFPLL-VCOL     [0|900]",
    "RFPLL-VCOM     [0|900]",
    "RFPLL-VCOH     [0|900]",
    "RFPLL-VCOBUF   [  900]",
    "RFPLL-BIASVARA [  900]",
    "ORFPLL-MMD     [ 1000]",
    "ORFPLL-DTC     [ 1000]",
    "ORFPLL-DVDD    [ 1000]",
    "ORFPLL-CP      [ 1400]",
    "ORFPLL-1P2     [ 1200]",
    "ORFPLL-S1      [ 1050]",
    "ORFPLL-VCOL    [0|900]",
    "ORFPLL-VCOM    [0|900]",
    "ORFPLL-VCOH    [0|900]",
    "ORFPLL-VCOBUF  [  900]",
    "ORFPLL-BIASVARA[  900]",    
    "END...",
};
uint32_t vcheck[][6] = {
  // atest id,  asest src,              spi_id,                 spi_reg,        spi_bitvalue,   spi_bitmask  
   {ATEST1,     ATEST_SRC_ADC_LN0,      SPI_ADC_Q_L0_ID,        0x34,           2<<26,          0x7<<26}, // ADC1-Q 
   {ATEST1,     ATEST_SRC_ADC_LN0,      SPI_ADC_Q_L0_ID,        0x34,           3<<26,          0x7<<26}, // ADC1-VRP 
   {ATEST1,     ATEST_SRC_ADC_LN0,      SPI_ADC_Q_L0_ID,        0x34,           4<<26,          0x7<<26}, // ADC1-VRN
   {ATEST1,     ATEST_SRC_ADC_LN1,      SPI_ADC_Q_L1_ID,        0x34,           2<<26,          0x7<<26}, // ADC2-Q
   {ATEST1,     ATEST_SRC_ADC_LN1,      SPI_ADC_Q_L1_ID,        0x34,           3<<26,          0x7<<26}, // ADC2-VRP 
   {ATEST1,     ATEST_SRC_ADC_LN1,      SPI_ADC_Q_L1_ID,        0x34,           4<<26,          0x7<<26}, // ADC2-VRN

   // atest id,  asest src,              spi_id,                 spi_reg,        spi_bitvalue,   spi_bitmask  
   {ATEST0,     ATEST_SRC_RX_ORX_LN0,   SPI_RX_ORX_L0_ID,       0x14,           1<<13,          0x3FFFFF},// RX1-TIA_I
   {ATEST0,     ATEST_SRC_RX_ORX_LN0,   SPI_RX_ORX_L0_ID,       0x14,           1<<12,          0x3FFFFF},// RX1-TIA_Q
   {ATEST0,     ATEST_SRC_RX_ORX_LN0,   SPI_RX_ORX_L0_ID,       0x14,           1<<10,          0x3FFFFF},// RX1-TIA_I_VCM
   {ATEST0,     ATEST_SRC_RX_ORX_LN0,   SPI_RX_ORX_L0_ID,       0x14,           1<<11,          0x3FFFFF},// RX1-TIA_Q_VCM
   {ATEST0,     ATEST_SRC_RX_ORX_LN0,   SPI_RX_ORX_L0_ID,       0x14,           1<<16,          0x3FFFFF},// RX1-MIXER_VB
   {ATEST0,     ATEST_SRC_RX_ORX_LN0,   SPI_RX_ORX_L0_ID,       0x14,           1<<6,           0x3FFFFF},// RX1-RFVDD
   
   {ATEST0,     ATEST_SRC_RX_ORX_LN1,   SPI_RX_ORX_L1_ID,       0x14,           1<<13,          0x3FFFFF},// RX2-TIA_I
   {ATEST0,     ATEST_SRC_RX_ORX_LN1,   SPI_RX_ORX_L1_ID,       0x14,           1<<12,          0x3FFFFF},// RX2-TIA_Q
   {ATEST0,     ATEST_SRC_RX_ORX_LN1,   SPI_RX_ORX_L1_ID,       0x14,           1<<10,          0x3FFFFF},// RX2-TIA_I_VCM
   {ATEST0,     ATEST_SRC_RX_ORX_LN1,   SPI_RX_ORX_L1_ID,       0x14,           1<<11,          0x3FFFFF},// RX2-TIA_Q_VCM
   {ATEST0,     ATEST_SRC_RX_ORX_LN1,   SPI_RX_ORX_L1_ID,       0x14,           1<<16,          0x3FFFFF},// RX2-MIXER_VB
   {ATEST0,     ATEST_SRC_RX_ORX_LN1,   SPI_RX_ORX_L1_ID,       0x14,           1<<6,           0x3FFFFF},// RX2-RFVDD
   //atest id,  asest src,              spi_id,                 spi_reg,        spi_bitvalue,   spi_bitmask  //
   {ATEST1,     ATEST_SRC_DAC_LN0,      SPI_DAC_IQ_L0_ID,       0x64,           1<<28,          0x1<<28}, // DAC1-I 
   {ATEST1,     ATEST_SRC_DAC_LN0,      SPI_DAC_IQ_L0_ID,        0x8,           1<<31,          0x1<<31}, // DAC1-Q
   {ATEST0,     ATEST_SRC_DAC_LN1,      SPI_DAC_IQ_L1_ID,       0x64,           1<<28,          0x1<<28}, // DAC2-I 
   {ATEST0,     ATEST_SRC_DAC_LN1,      SPI_DAC_IQ_L1_ID,        0x8,           1<<31,          0x1<<31}, // DAC2-Q
    //atest id,  asest src,              spi_id,                 spi_reg,        spi_bitvalue,   spi_bitmask  //
   {ATEST0,     ATEST_SRC_TX0_LN0,      SPI_TRANSMITTER_L0_ID,  0x3C,           0x1,            0x3F},    // TX1-VBLO
   {ATEST0,     ATEST_SRC_TX0_LN0,      SPI_TRANSMITTER_L0_ID,  0x3C,           0x2,            0x3F},    // TX1-VCASC
   {ATEST0,     ATEST_SRC_TX0_LN0,      SPI_TRANSMITTER_L0_ID,  0x3C,           0x3,            0x3F},    // TX1-VCM2_Q
   {ATEST0,     ATEST_SRC_TX0_LN0,      SPI_TRANSMITTER_L0_ID,  0x3C,           0x4,            0x3F},    // TX1-VCM1_Q
   {ATEST1,     ATEST_SRC_TX1_LN0,      SPI_TRANSMITTER_L0_ID,  0x3C,           0xA,            0x3F},    // TX1-VCM
   {ATEST1,     ATEST_SRC_TX1_LN0,      SPI_TRANSMITTER_L0_ID,  0x3C,           0xB,            0x3F},    // TX1-VCM2_I
   {ATEST1,     ATEST_SRC_TX1_LN0,      SPI_TRANSMITTER_L0_ID,  0x3C,           0xC,            0x3F},    // TX1-VCM1_I

   {ATEST0,     ATEST_SRC_TX0_LN1,      SPI_TRANSMITTER_L1_ID,  0x3C,           0x1,            0x3F},    // TX2-VBLO
   {ATEST0,     ATEST_SRC_TX0_LN1,      SPI_TRANSMITTER_L1_ID,  0x3C,           0x2,            0x3F},    // TX2-VCASC
   {ATEST0,     ATEST_SRC_TX0_LN1,      SPI_TRANSMITTER_L1_ID,  0x3C,           0x3,            0x3F},    // TX2-VCM2_Q
   {ATEST0,     ATEST_SRC_TX0_LN1,      SPI_TRANSMITTER_L1_ID,  0x3C,           0x4,            0x3F},    // TX2-VCM1_Q
   {ATEST1,     ATEST_SRC_TX1_LN1,      SPI_TRANSMITTER_L1_ID,  0x3C,           0xA,            0x3F},    // TX2-VCM
   {ATEST1,     ATEST_SRC_TX1_LN1,      SPI_TRANSMITTER_L1_ID,  0x3C,           0xB,            0x3F},    // TX2-VCM2_I
   {ATEST1,     ATEST_SRC_TX1_LN1,      SPI_TRANSMITTER_L1_ID,  0x3C,           0xC,            0x3F},    // TX2-VCM1_I 
    //atest id,  asest src,              spi_id,                 spi_reg,        spi_bitvalue,   spi_bitmask  //
   {ATEST1,     ATEST_SRC_BBPLL,        SPI_BBPLL_ID,           0x2C,           0x1<<20,        0xFFF<<20},    // BBPLL-MMD
   {ATEST1,     ATEST_SRC_BBPLL,        SPI_BBPLL_ID,           0x2C,           0x2<<20,        0xFFF<<20},    // BBPLL-DTC
   {ATEST1,     ATEST_SRC_BBPLL,        SPI_BBPLL_ID,           0x2C,           0x4<<20,        0xFFF<<20},    // BBPLL-CP
   {ATEST1,     ATEST_SRC_BBPLL,        SPI_BBPLL_ID,           0x2C,           0x20<<20,       0xFFF<<20},    // BBPLL-1P2
   {ATEST1,     ATEST_SRC_BBPLL,        SPI_BBPLL_ID,           0x2C,           0x40<<20,       0xFFF<<20},    // BBPLL-S1
   {ATEST1,     ATEST_SRC_BBPLL,        SPI_BBPLL_ID,           0x2C,           0x80<<20,       0xFFF<<20},    // BBPLL-VCOL
   {ATEST1,     ATEST_SRC_BBPLL,        SPI_BBPLL_ID,           0x2C,           0x100<<20,      0xFFF<<20},    // BBPLL-VCOM
   {ATEST1,     ATEST_SRC_BBPLL,        SPI_BBPLL_ID,           0x2C,           0x200<<20,      0xFFF<<20},    // BBPLL-VCOH
   {ATEST1,     ATEST_SRC_BBPLL,        SPI_BBPLL_ID,           0x2C,           0x400<<20,      0xFFF<<20},    // BBPLL-VCOBUF
   {ATEST1,     ATEST_SRC_BBPLL,        SPI_BBPLL_ID,           0x2C,           0x800<<20,      0xFFF<<20},    // BBPLL-BIASVARA

   {ATEST1,     ATEST_SRC_RFPLL,        SPI_RFPLL_ID,           0x44,           0x1<<20,        0xFFF<<20},    // RFPLL-MMD
   {ATEST1,     ATEST_SRC_RFPLL,        SPI_RFPLL_ID,           0x44,           0x2<<20,        0xFFF<<20},    // RFPLL-DTC
   {ATEST1,     ATEST_SRC_RFPLL,        SPI_RFPLL_ID,           0x44,           0x4<<20,        0xFFF<<20},    // RFPLL-CP
   {ATEST1,     ATEST_SRC_RFPLL,        SPI_RFPLL_ID,           0x44,           0x20<<20,       0xFFF<<20},    // RFPLL-1P2
   {ATEST1,     ATEST_SRC_RFPLL,        SPI_RFPLL_ID,           0x44,           0x40<<20,       0xFFF<<20},    // RFPLL-S1
   {ATEST1,     ATEST_SRC_RFPLL,        SPI_RFPLL_ID,           0x44,           0x80<<20,       0xFFF<<20},    // RFPLL-VCOL
   {ATEST1,     ATEST_SRC_RFPLL,        SPI_RFPLL_ID,           0x44,           0x100<<20,      0xFFF<<20},    // RFPLL-VCOM
   {ATEST1,     ATEST_SRC_RFPLL,        SPI_RFPLL_ID,           0x44,           0x200<<20,      0xFFF<<20},    // RFPLL-VCOH
   {ATEST1,     ATEST_SRC_RFPLL,        SPI_RFPLL_ID,           0x44,           0x400<<20,      0xFFF<<20},    // RFPLL-VCOBUF
   {ATEST1,     ATEST_SRC_RFPLL,        SPI_RFPLL_ID,           0x44,           0x800<<20,      0xFFF<<20},    // RFPLL-BIASVARA  

   {ATEST0,     ATEST_SRC_ORFPLL,       SPI_ORFPLL_ID,          0x4C,           0x1,            0x7F},    		// ORFPLL-MMD
   {ATEST0,     ATEST_SRC_ORFPLL,       SPI_ORFPLL_ID,          0x4C,           0x2,            0x7F},    		// ORFPLL-DTC
   {ATEST0,     ATEST_SRC_ORFPLL,       SPI_ORFPLL_ID,          0x4C,           0x4,            0x7F},    		// ORFPLL-DVDD
   {ATEST0,     ATEST_SRC_ORFPLL,       SPI_ORFPLL_ID,          0x4C,           0x8,            0x7F},    		// ORFPLL-CP
   {ATEST0,     ATEST_SRC_ORFPLL,       SPI_ORFPLL_ID,          0x4C,           0x10,           0x7F},    		// ORFPLL-1P2
   {ATEST0,     ATEST_SRC_ORFPLL,       SPI_ORFPLL_ID,          0x4C,           0x20,           0x7F},    		// ORFPLL-S1
   {ATEST0,     ATEST_SRC_ORFPLL,       SPI_ORFPLL_ID,          0x4C,           0x40,           0x7F},    		// ORFPLL-VCOL
   {ATEST0,     ATEST_SRC_ORFPLL,       SPI_ORFPLL_ID,          0x4C,           0x80,           0x7F},    		// ORFPLL-VCOM
   {ATEST0,     ATEST_SRC_ORFPLL,       SPI_ORFPLL_ID,          0x4C,           0x100,          0x7F},   		// ORFPLL-VCOH
   {ATEST0,     ATEST_SRC_ORFPLL,       SPI_ORFPLL_ID,          0x4C,           0x200,          0x7F},    		// ORFPLL-VCOBUF
   {ATEST0,     ATEST_SRC_ORFPLL,       SPI_ORFPLL_ID,          0x4C,           0x400,          0x7F},    		// ORFPLL-BIASVARA

};

static uint32_t read_aux_val(fr9009Device_t *device, uint32_t rdAddr, uint32_t* rdDataBuf, uint32_t rdWordSz)
{
    uint32_t reg_v;
    uint32_t ret = FR9009_armSpiCmd_readWords(device, FR9009_ADDR_ARMSPI(SPI_AUX_ADDA_IVREF_ID, 0x7C), &reg_v, 1);
    rdDataBuf[0] = reg_v & 0xffff;
    return ret;
}

void fr9009_voltage_check()
{
    fr9009Device_t *device = &brDev[0];
    uint32_t value_0v = 0;
    uint32_t value_3P3 = 0;
    uint32_t value_auxadc = 0;
    int16_t  temp;
    float L0_v = 0;
    int check_total = sizeof(vcheck) / sizeof(uint32_t) / 6;
    uint32_t adcsel=0;
    
    FR9009_armSpiCmd_readWords(device, FR9009_ADDR_ARMSPI(SPI_AUX_ADDA_IVREF_ID, 0x8), &adcsel, 1);
    
    for (int chip = 0; chip < DEVICE_NUMS; chip++)
    {
        device = &brDev[chip];
       // FR9009_armSpiCmd_pll_OnOff_set(device, FR_ORF_PLL, 0);
        //FR9009_setRxTxEnable(device, FR_RX1RX2, FR_TX1TX2);
        FR9009_armSpiCmd_temp_protect_en(device, 0);
        FR9009_getTemparature (device, &temp);
        UART_Printf(">>>>> Vcheck Chip %d (Temperature %d)>>>>>\r\n", chip, temp);
        FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(SPI_AUX_ADDA_IVREF_ID, 0x8), 0xf, 0xFFFFFFFF, 0);
        FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(SPI_AUX_ADDA_IVREF_ID, 0x34), 0x0, 0xFFFFFFFF, 0);
        delay_ms(10);
        read_aux_val(device, FR9009_ADDR_ARMSPI(SPI_AUX_ADDA_IVREF_ID, 0x7C), &value_0v, 1);
        FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(SPI_AUX_ADDA_IVREF_ID, 0x34), 0x3FF, 0xFFFFFFFF, 0);
        delay_ms(10);
        read_aux_val(device, FR9009_ADDR_ARMSPI(SPI_AUX_ADDA_IVREF_ID, 0x7C), &value_3P3, 1);

        // ADC
        for (int k = 0; k < check_total; k++)
        {
            // Choose auxADC src: ATEST0 or ATEST1
            FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(SPI_AUX_ADDA_IVREF_ID, 0x8), vcheck[k][0], 0xFFFFFFFF, 0);
            // Enable ATEST0/1 and choose src
            uint32_t reg_val = (1 << ((vcheck[k][1] & 0xf))) | 1;
            // uint32_t reg_val = ((vcheck[k][1] & 0xf)<<1) | 1;
            reg_val = (vcheck[k][0] == ATEST0) ? reg_val : (reg_val << 9);
            FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(SPI_AUX_ADDA_IVREF_ID, 0x4C), reg_val, 0x3FFFF, 0);
            // Module ATEST enable and choose buffer
            if(vcheck[k][1] != ATEST_SRC_ORFPLL)
            {
                FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(vcheck[k][2], vcheck[k][3]), vcheck[k][4], vcheck[k][5], 0);
            }                
            else
            {
                FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(vcheck[k][2], vcheck[k][3]), vcheck[k][4]>>4, vcheck[k][5], 0);
                FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(vcheck[k][2], 0x40), (vcheck[k][4]&0x1)<<8, 1<<8, 0);
                FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(vcheck[k][2], 0x40), (vcheck[k][4]&0x2)<<14, 1<<15, 0);
                FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(vcheck[k][2], 0x58), (vcheck[k][4]&0x4)<<11, 1<<13, 0);
                FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(vcheck[k][2], 0x64), (vcheck[k][4]&0x8)<<28, 1<<31, 0);
            }
            delay_ms(10);
            // Read AuxAdc
            read_aux_val(device, FR9009_ADDR_ARMSPI(SPI_AUX_ADDA_IVREF_ID, 0x7C), &value_auxadc, 1);
            // Module ATEST disable
            if(vcheck[k][1] != ATEST_SRC_ORFPLL)
            {
                FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(vcheck[k][2], vcheck[k][3]), 0, vcheck[k][5], 0);
            }
            else
            {
                FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(vcheck[k][2], vcheck[k][3]), 0, vcheck[k][5], 0);
                FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(vcheck[k][2], 0x40), 0, 1<<8, 0);
                FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(vcheck[k][2], 0x40), 0, 1<<15, 0);
                FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(vcheck[k][2], 0x58), 0, 1<<13, 0);
                FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(vcheck[k][2], 0x64), 0, 1<<31, 0);
            }
            L0_v = 1.0f * (value_0v - value_auxadc) / (value_0v - value_3P3) * 3.3f;
            UART_Printf("%-24s: %4d mV\r\n", vcheck_str[k], (uint32_t)(L0_v * 1000));
        }
        //
        FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(SPI_AUX_ADDA_IVREF_ID, 0x8), adcsel, 0xFFFFFFFF, 0);
        FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(SPI_AUX_ADDA_IVREF_ID, 0x4C), 0x0000, 0xFFFFFFFF, 0);
        FR9009_armSpiCmd_temp_protect_en(device, 1);

        FR9009_getTemparature (device, &temp);
        UART_Printf("<<<<< Vcheck End (Temperature %d)<<<<<\r\n", temp);
        //delay_ms(1000);
    }
}

void fr9009_pll_check()
{
    fr9009Device_t *device = &brDev[0];
    int16_t  temp;
    uint32_t ocs[3];
    uint8_t lockstatus;
    
    for (int chip = 0; chip < DEVICE_NUMS; chip++)
    {
        device = &brDev[chip];
        FR9009_getTemparature (device, &temp);
        FR9009_armSpiCmd_readWords(device, FR9009_ADDR_ARMSPI(SPI_BBPLL_ID, 0x7C), &ocs[0], 1);
        ocs[0] = ocs[0] & 0x1ff;
        FR9009_armSpiCmd_readWords(device, FR9009_ADDR_ARMSPI(SPI_RFPLL_ID, 0x68), &ocs[1], 1);
        ocs[1] = (ocs[1]>>23) & 0x1ff;
        FR9009_armSpiCmd_readWords(device, FR9009_ADDR_ARMSPI(SPI_ORFPLL_ID, 0x78), &ocs[2], 1);
        ocs[2] = ocs[2] & 0x1ff;

        FR9009_getPllsLockStatus(device, &lockstatus);
        
        UART_Printf(">>>>> PLLcheck Chip %d (Temperature %d)>>>>>\r\n", chip, temp);
        UART_Printf("lockStatus: %d, bbpll 0x%03x, rfpll 0x%03x, orfpll 0x%03x\r\n", lockstatus, ocs[0], ocs[1], ocs[2]);
        UART_Printf("<<<<< PLLcheck End<<<<<\r\n");
    }
}

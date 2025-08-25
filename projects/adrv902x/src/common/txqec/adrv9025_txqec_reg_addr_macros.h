#ifndef ADRV9025_TXQEC_REG_ADDR_MACROS_H_
#define ADRV9025_TXQEC_REG_ADDR_MACROS_H_

#define ADRV9025_ADDR_TX0_BASE_ADDR                 0x1E00
#define ADRV9025_ADDR_TX1_BASE_ADDR                 0x2000
#define ADRV9025_ADDR_TX2_BASE_ADDR                 0x2200
#define ADRV9025_ADDR_TX3_BASE_ADDR                 0x2400

// TXQEC
#define IDX_ABBF_C1_QEC_TRIM_MSBS                   0x0064
#define IDX_ABBF_C1_QEC_TRIM_LSBS                   0x0065
#define IDX_ABBF_C2_QEC_TRIM_MSBS                   0x0066
#define IDX_ABBF_C2_QEC_TRIM_LSBS                   0x0067

#define IDX_TXQEC_FILTER_CFG                        0x006E
#define IDX_TXQEC_FILTER_COEFF_Q_MSB_0              0x006F
#define IDX_TXQEC_FILTER_COEFF_Q_LSB_0              0x0070
#define IDX_TXQEC_FILTER_COEFF_Q_MSB_1              0x0071
#define IDX_TXQEC_FILTER_COEFF_Q_LSB_1              0x0072
#define IDX_TXQEC_FILTER_COEFF_Q_MSB_2              0x0073
#define IDX_TXQEC_FILTER_COEFF_Q_LSB_2              0x0074
#define IDX_TXQEC_FILTER_COEFF_Q_MSB_3              0x0075
#define IDX_TXQEC_FILTER_COEFF_Q_LSB_3              0x0076
#define IDX_TXQEC_FILTER_COEFF_Q_MSB_4              0x0077
#define IDX_TXQEC_FILTER_COEFF_Q_LSB_4              0x0078

#define IDX_TXQEC_FILTER_COEFF_I_MSB_0              0x0079
#define IDX_TXQEC_FILTER_COEFF_I_LSB_0              0x007A
#define IDX_TXQEC_FILTER_COEFF_I_MSB_1              0x007B
#define IDX_TXQEC_FILTER_COEFF_I_LSB_1              0x007C
#define IDX_TXQEC_FILTER_COEFF_I_MSB_2              0x007D
#define IDX_TXQEC_FILTER_COEFF_I_LSB_2              0x007E
#define IDX_TXQEC_FILTER_COEFF_I_MSB_3              0x007F
#define IDX_TXQEC_FILTER_COEFF_I_LSB_3              0x0080
#define IDX_TXQEC_FILTER_COEFF_I_MSB_4              0x0081
#define IDX_TXQEC_FILTER_COEFF_I_LSB_4              0x0082

// Add missing IDX defines for offset
#define IDX_TXQEC_FILTER_OFFSET_I_MSB               0x0083
#define IDX_TXQEC_FILTER_OFFSET_I_LSB               0x0084
#define IDX_TXQEC_FILTER_OFFSET_Q_MSB               0x0085
#define IDX_TXQEC_FILTER_OFFSET_Q_LSB               0x0086

// Add missing IDX defines for LO delay correction
#define IDX_LO_DELAY_CORR_DAC_1                     0x0158
#define IDX_LO_DELAY_CORR_DAC_0                     0x0159

#endif // ADRV9025_TXQEC_REG_ADDR_MACROS_H_

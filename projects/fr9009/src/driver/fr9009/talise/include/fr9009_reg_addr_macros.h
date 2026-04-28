/**
 * \file fr9009_reg_addr_macros.h
 * \brief Contains FR9009 API address macro definitions
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef _FR9009_ADDR_MACROS_H
#define _FR9009_ADDR_MACROS_H

#ifdef __cplusplus
extern "C" {
#endif


#ifdef USED_LIB
#define MYDLL_API  __declspec(dllexport)   //主要就是加入这个宏定义
#else
#define MYDLL_API 
#endif
/**
 * *(uint64_t) 的强制转换是为了解决gcc报警告的问题, 64位系统中指针是64位/8字节的, 直接转换为32位的变量会报错,
 * 添加强制转换(uint32_t)(uint64_t)后,该警告消失,
 * 后续可能需要评估该方法是否有意义, FR9009是不是64位还是32位
*/
#define type_OffSet(type, field) 	((uint32_t)(uint64_t)&(((type*)0)->field))


/******************************************************************************/
/*                         Memory Map                              */
/******************************************************************************/
#define FR9009_FLASH_BASE        (0x00000000UL)  /*!< (FLASH     ) Base Address */
#define FR9009_SRAM_BASE         (0x00000000UL)  /*!< (SRAM      ) Base Address */

#define FR9009_RAM_BASE                                 (0x00000000UL)
#define FR9009_ADDR_EXTDATA_FLAG                        (0x80000000UL)
#define FR9009_ADDR_DPRAM_BASEADDR                      (0x00000400UL)
#define FR9009_ADDR_DPRAM_ENDADDR                       (0x00000FFCUL)

#define FR9009_ADDR_ARM_GLOBLE_CONFIG_DATA              (0x00000C00UL)

#define FR9009_ADDR_CMDBUF_BASE     			 	  	(0x00000400UL)
#define FR9009_ADDR_CMDBUF_MAX     			 	  	    (0x00000BFCUL)
#define FR9009_ADDR_CMDBUF_STATUS     			 	  	(FR9009_ADDR_CMDBUF_BASE)
#define FR9009_ADDR_CMDBUF_OPCODE     			 	  	(FR9009_ADDR_CMDBUF_BASE+0x04)
#define FR9009_ADDR_CMDBUF_EXTDATA                      (FR9009_ADDR_CMDBUF_BASE+0x20)
#define FR9009_CMDBUF_WORDSIZE			                (((FR9009_ADDR_CMDBUF_MAX-FR9009_ADDR_CMDBUF_BASE) >> 2) + 1)
#define FR9009_CMDBUF_PARAM_SIZE                        (((FR9009_ADDR_CMDBUF_EXTDATA-FR9009_ADDR_CMDBUF_OPCODE) >> 2) - 1)
#define FR9009_CMDBUF_EXTDATA_SIZE                      (((FR9009_ADDR_CMDBUF_MAX-FR9009_ADDR_CMDBUF_EXTDATA) >> 2) + 1)

#define HOST_SYSCTRL_BASEADDR			        0x44000000UL
#define HOST_SYSCTRL_ENDADDR			        0x4400FFFFUL
#define FR9009_ADDR_HW_REVISION				    (HOST_SYSCTRL_BASEADDR+0X0000)
#define FR9009_ADDR_REFCLK_FREQ				    (HOST_SYSCTRL_BASEADDR+0X0008)
#define FR9009_ADDR_ARMCLK_DIV				    (HOST_SYSCTRL_BASEADDR+0X0014)
#define FR9009_ADDR_ARMCLK_DIV_ACTION		    (HOST_SYSCTRL_BASEADDR+0X001C)
#define FR9009_ADDR_SPI_INIT_RESET			    (HOST_SYSCTRL_BASEADDR+0X002C)
#define FR9009_ADDR_WAKE_MCU				    (HOST_SYSCTRL_BASEADDR+0X0030)
#define FR9009_ADDR_WAKE_MCU_MASK			    1

#define FR9009_ADDR_SPI_SDIO_BDIR_OFF           (FR9009_ADDR_EXTDATA_FLAG + 0x4000C000UL+0x68)  /*!< APB_IOCTRL_BASEADDR */
#define FR9009_ADDR_SPI_SDIO_BDIR_OFF_MASK      1
#define FR9009_ADDR_SPI_SDIO_BDIR_OFF_OFFSET    0

/******************************************************************************/
/*                         APB addrbase                        				  */
/******************************************************************************/
#define APB_BASEADDR                            (0x40000000UL)
#define APB_CLK_RST_BASEADDR           			(0x40000000UL)
#define APB_IOCTRL_BASEADDR                     (0x4000C000UL)
#define APB_SPI_M_BASEADDR                      (0x40010000UL)
#define APB_DIG_MUX_BASEADDR                    (0x40080000UL)
#define APB_TX_PATH_TOP_BASEADDR                (0x40084000UL)
#define APB_TX_DGAIN_BASEADDR                   (0x40084400UL)
#define APB_TX_IFCONV_BASEADDR                  (0x40084800UL)
#define APB_TX_CPLXCFIR_CH1_BASEADDR            (0x40084C00UL)
#define APB_TX_CPLXCFIR_CH2_BASEADDR            (0x40085000UL)
#define APB_TX_QEC_QFIR_BASEADDR                (0x40085800UL)
#define APB_TX_PHSCMPS_BASEADDR                 (0x40085C00UL)
#define APB_TX_CFIR_BASEADDR                    (0x40086000UL)
#define APB_TX_QEC_CFIR_CH1_BASEADDR            (0x40086400UL)
#define APB_TX_QEC_CFIR_CH2_BASEADDR            (0x40086800UL)
#define APB_RX_PATH_TOP_BASEADDR                (0x40088000UL)
#define APB_RX_RCFIR_BASEADDR     	            (0x40088400UL)
#define APB_RX_AGC_BASEADDR                     (0x40088800UL)
#define APB_RX_ATTN_LUT_BASEADDR                (0x40088C00UL)
#define APB_RX_DIG_GAIN_1_BASEADDR              (0x40089000UL)
#define APB_RX_DIG_GAIN_2_BASEADDR              (0x40089400UL)
#define APB_RX_FE_LUT_BASEADDR  	            (0x40089800UL)
#define APB_RX_PHSCMPS_BASEADDR                 (0x40089C00UL)
#define APB_RX_QEC_QFIR_BASEADDR                (0x4008A000UL)
#define APB_RX_CPLXCFIR_CH1_BASEADDR            (0x4008A800UL)
#define APB_RX_CPLXCFIR_CH2_BASEADDR            (0x4008AC00UL)
#define APB_RX_DC_CORR_BASEADDR                 (0x4008B000UL)
#define APB_RX_IFCONV_BASEADDR                  (0x4008B400UL)
#define APB_RX_QEC_CFIR_CH1_BASEADDR            (0x4008B800UL)
#define APB_RX_QEC_CFIR_CH2_BASEADDR            (0x4008BC00UL)
#define APB_JESD_TX_BASEADDR                    (0x4008C000UL)
#define APB_JESD_RX_BASEADDR                    (0x4008C400UL)
#define APB_TDD_BASEADDR                        (0x40090000UL)
#define APB_PLAYCAP_BASEADDR                    (0x40094000UL)  


/* **********************************APB peripherals*********************************/   
/******************************************************************************/
/*                        APB_CLK_RST		                           		  */
/******************************************************************************/
#define FR9009_ADDR_APB_CLK_RST_BB_PLL					(APB_CLK_RST_BASEADDR+0X000C)
#define FR9009_ADDR_APB_CLK_RST_RF_PLL					(APB_CLK_RST_BASEADDR+0X0010)
#define FR9009_ADDR_APB_CLK_RST_RF_FCW					(APB_CLK_RST_BASEADDR+0X0014)	
#define FR9009_ADDR_APB_CLK_RST_ORF_PLL					(APB_CLK_RST_BASEADDR+0X0018)
#define FR9009_ADDR_APB_CLK_RST_ORF_FCW					(APB_CLK_RST_BASEADDR+0X001C)	
#define FR9009_ADDR_APB_CLK_RST_JESD_SYNC_DIV_EN		(APB_CLK_RST_BASEADDR+0X0030)
#define FR9009_ADDR_APB_CLK_RST_FUNC_RESET				(APB_CLK_RST_BASEADDR+0X003C)	 
/******************************************************************************/
/*                        TX_PATH_TOP		                           		  */
/******************************************************************************/
#define APB_TX_PATH_TOP_VIL_SEL					(APB_TX_PATH_TOP_BASEADDR+0X0008)	
#define APB_TX_PATH_TOP_PLAY_MODE				(APB_TX_PATH_TOP_BASEADDR+0X0038)	

/******************************************************************************/
/*                        APB_RX_IFCONV		                           		  */
/******************************************************************************/

#define FR9009_ADDR_APB_RX_IFCONV_				(APB_RX_IFCONV_BASEADDR+0X003C)	


/******************************************************************************/
/*                        APB_DIG_MUX		                           		  */
/******************************************************************************/

#define FR9009_ADDR_APB_DIG_MUX_LOOPBACK_FIXDATA						(APB_DIG_MUX_BASEADDR+ 0x0008)
#define FR9009_ADDR_APB_DIG_MUX_DP_TX_IQ_CH1							(APB_DIG_MUX_BASEADDR+ 0x000C)
#define FR9009_ADDR_APB_DIG_MUX_DP_TX_IQ_CH2							(APB_DIG_MUX_BASEADDR+ 0x0010)
#define FR9009_ADDR_APB_DIG_MUX_JESD_TX_IQ_CH1							(APB_DIG_MUX_BASEADDR+ 0x0014)
#define FR9009_ADDR_APB_DIG_MUX_JESD_TX_IQ_CH2							(APB_DIG_MUX_BASEADDR+ 0x001C)


/******************************************************************************/
/*                        JESD		                           				  */
/******************************************************************************/
#define FR9009_ADDR_JESD_FRAMER_IRQ								   (APB_JESD_TX_BASEADDR+0x0008)
#define FR9009_ADDR_JESD_FRAMER_CONFIG_0                           (APB_JESD_TX_BASEADDR+0x0010)
#define FR9009_ADDR_JESD_FRAMER_CONFIG_1                           (APB_JESD_TX_BASEADDR+0x001C)
#define FR9009_ADDR_JESD_FRAMER_CONFIG1_0                          (APB_JESD_TX_BASEADDR+0x0020)
#define FR9009_ADDR_JESD_FRAMER_CONFIG2_0                          (APB_JESD_TX_BASEADDR+0x0024)
#define FR9009_ADDR_JESD_FRAMER_CONFIG3_0                          (APB_JESD_TX_BASEADDR+0x0028)
#define FR9009_ADDR_JESD_FRAMER_CONFIG4_0                          (APB_JESD_TX_BASEADDR+0x002C)
#define FR9009_ADDR_JESD_FRAMER_CONFIG5_0                          (APB_JESD_TX_BASEADDR+0x0030)
#define FR9009_ADDR_JESD_FRAMER_LANE_XBAR	                       (APB_JESD_TX_BASEADDR+0x0048)
//#define FR9009_ADDR_JESD_FRAMER_SAMPLE_XBAR		                   (APB_JESD_TX_BASEADDR+0x004C)
#define FR9009_ADDR_JESD_FRAMER_SAMPLE_XBAR_0123_0                 (APB_JESD_TX_BASEADDR+0x004C)
#define FR9009_ADDR_JESD_FRAMER_SAMPLE_XBAR_4567_0                 (APB_JESD_TX_BASEADDR+0x0050)
#define FR9009_ADDR_JESD_FRAMER_SAMPLE_XBAR_89AB_0                 (APB_JESD_TX_BASEADDR+0x0054)
#define FR9009_ADDR_JESD_FRAMER_SAMPLE_XBAR_CDEF_0                 (APB_JESD_TX_BASEADDR+0x0058)
#define FR9009_ADDR_JESD_FRAMER_SAMPLE_AUX_XBAR_0123_0             (APB_JESD_TX_BASEADDR+0x008C)
#define FR9009_ADDR_JESD_FRAMER_SAMPLE_AUX_XBAR_4567_0             (APB_JESD_TX_BASEADDR+0x0090)
#define FR9009_ADDR_JESD_FRAMER_SAMPLE_AUX_XBAR_89AB_0             (APB_JESD_TX_BASEADDR+0x0094)
#define FR9009_ADDR_JESD_FRAMER_SAMPLE_AUX_XBAR_CDEF_0             (APB_JESD_TX_BASEADDR+0x0098)
#define FR9009_ADDR_JESD_FRAMER_TEST_CFG_0			               (APB_JESD_TX_BASEADDR+0x00EC)
#define FR9009_ADDR_JESD_FRAMER_LANE_PN_CFG			               (APB_JESD_TX_BASEADDR+0x00F0)
#define FR9009_ADDR_JESD_FRAMER_SYNC				               (APB_JESD_TX_BASEADDR+0x00F4)
#define FR9009_ADDR_JESD_DEFRAMER_CONFIG                           (APB_JESD_RX_BASEADDR+0x0008)
#define FR9009_ADDR_JESD_DEFRAMER_IRQ			                   (APB_JESD_RX_BASEADDR+0x000C)
#define FR9009_ADDR_JESD_DEFRAMER_IRQ_MASK_0                       (APB_JESD_RX_BASEADDR+0x0010)
#define FR9009_ADDR_JESD_DEFRAMER_CONFIG_0                         (APB_JESD_RX_BASEADDR+0x0014)
#define FR9009_ADDR_JESD_DEFRAMER_CONFIG_1                         (APB_JESD_RX_BASEADDR+0x0018)
#define FR9009_ADDR_JESD_DEFRAMER_CONFIG_2                         (APB_JESD_RX_BASEADDR+0x001C)
#define FR9009_ADDR_JESD_DEFRAMER_IP_OBS0_0                        (APB_JESD_RX_BASEADDR+0x0024)
#define FR9009_ADDR_JESD_DEFRAMER_IP_OBS1_0                        (APB_JESD_RX_BASEADDR+0x0034)
#define FR9009_ADDR_JESD_DEFRAMER_IP_OBS2_0                        (APB_JESD_RX_BASEADDR+0x0044)
#define FR9009_ADDR_JESD_DEFRAMER_IP_OBS3_0                        (APB_JESD_RX_BASEADDR+0x0054)
#define FR9009_ADDR_JESD_DEFRAMER_SAMPLE_XBAR_0L                  	(APB_JESD_RX_BASEADDR+0x0064)
#define FR9009_ADDR_JESD_DEFRAMER_SAMPLE_XBAR_0H                  	(APB_JESD_RX_BASEADDR+0x0068)
#define FR9009_ADDR_JESD_DEFRAMER_SAMPLE_XBAR_1L                  	(APB_JESD_RX_BASEADDR+0x006C)
#define FR9009_ADDR_JESD_DEFRAMER_SAMPLE_XBAR_1H                  	(APB_JESD_RX_BASEADDR+0x0070)
#define FR9009_ADDR_JESD_DEFRAMER_SAMPLE_XBAR_2L                  	(APB_JESD_RX_BASEADDR+0x0074)
#define FR9009_ADDR_JESD_DEFRAMER_SAMPLE_XBAR_2H                  	(APB_JESD_RX_BASEADDR+0x0078)
#define FR9009_ADDR_JESD_DEFRAMER_SAMPLE_XBAR_3L                  	(APB_JESD_RX_BASEADDR+0x007C)
#define FR9009_ADDR_JESD_DEFRAMER_SAMPLE_XBAR_3H                  	(APB_JESD_RX_BASEADDR+0x0080)
#define FR9009_ADDR_JESD_DEFRAMER_RX_CGS_FRAME_SYNC				 	(APB_JESD_RX_BASEADDR+0x0084)
#define FR9009_ADDR_JESD_DEFRAMER_LANE_PN_CFG			            (APB_JESD_RX_BASEADDR+0x0090)
#define FR9009_ADDR_JESD_DEFRAMER_NOTINTAB_DISERR_PRBS_COUNT_CLEAR 	(APB_JESD_RX_BASEADDR+0x00B4)
#define FR9009_ADDR_JESD_DEFRAMER_PRBS_CFG                         	(APB_JESD_RX_BASEADDR+0x00B8)
#define FR9009_ADDR_JESD_DEFRAMER_PRBS_COUNT0          				(APB_JESD_RX_BASEADDR+0x00BC)
#define FR9009_ADDR_JESD_DEFRAMER_PRBS_ERROR_COUNT0                	(APB_JESD_RX_BASEADDR+0x00C4)
#define FR9009_ADDR_JESD_DEFRAMER_PRBS_COUNT1          				(APB_JESD_RX_BASEADDR+0x00CC)
#define FR9009_ADDR_JESD_DEFRAMER_PRBS_ERROR_COUNT1                	(APB_JESD_RX_BASEADDR+0x00D4)
#define FR9009_ADDR_JESD_DEFRAMER_PRBS_COUNT2          				(APB_JESD_RX_BASEADDR+0x00DC)
#define FR9009_ADDR_JESD_DEFRAMER_PRBS_ERROR_COUNT2                	(APB_JESD_RX_BASEADDR+0x00E4)
#define FR9009_ADDR_JESD_DEFRAMER_PRBS_COUNT3          				(APB_JESD_RX_BASEADDR+0x00EC)
#define FR9009_ADDR_JESD_DEFRAMER_PRBS_ERROR_COUNT3                	(APB_JESD_RX_BASEADDR+0x00F4)
#define FR9009_ADDR_JESD_DEFRAMER_RAW_PRBS_CFG                         	(APB_JESD_RX_BASEADDR+0x00FC)
#define FR9009_ADDR_JESD_DEFRAMER_RAW_PRBS_COUNT_CLEAR                 	(APB_JESD_RX_BASEADDR+0x0100)
#define FR9009_ADDR_JESD_DEFRAMER_RAW_PRBS_COUNT0          				(APB_JESD_RX_BASEADDR+0x0104)
#define FR9009_ADDR_JESD_DEFRAMER_RAW_PRBS_ERROR_COUNT0                	(APB_JESD_RX_BASEADDR+0x010C)
#define FR9009_ADDR_JESD_DEFRAMER_RAW_PRBS_COUNT1          				(APB_JESD_RX_BASEADDR+0x0114)
#define FR9009_ADDR_JESD_DEFRAMER_RAW_PRBS_ERROR_COUNT1                	(APB_JESD_RX_BASEADDR+0x011C)
#define FR9009_ADDR_JESD_DEFRAMER_RAW_PRBS_COUNT2          				(APB_JESD_RX_BASEADDR+0x0124)
#define FR9009_ADDR_JESD_DEFRAMER_RAW_PRBS_ERROR_COUNT2                	(APB_JESD_RX_BASEADDR+0x012C)
#define FR9009_ADDR_JESD_DEFRAMER_RAW_PRBS_COUNT3          				(APB_JESD_RX_BASEADDR+0x0134)
#define FR9009_ADDR_JESD_DEFRAMER_RAW_PRBS_ERROR_COUNT3                	(APB_JESD_RX_BASEADDR+0x013C)

/******************************************************************************/
/*                        TDD		                           				  */
/******************************************************************************/
#define FR9009_ADDR_TDD_RF_CONFIG0				          				(APB_TDD_BASEADDR+0x0008)
#define FR9009_ADDR_TDD_RF_CONFIG1				          			    (APB_TDD_BASEADDR+0x000c)
#define FR9009_ADDR_TDD_RF_CONFIG2				          				(APB_TDD_BASEADDR+0x0010)
#define FR9009_ADDR_TDD_RF_CONFIG3				          				(APB_TDD_BASEADDR+0x016C)
#define FR9009_ADDR_TDD_RF_CONFIG4				          				(APB_TDD_BASEADDR+0x0170)
#define FR9009_ADDR_TDD_RF_CONFIG5				          				(APB_TDD_BASEADDR+0x0214)


/******************************************************************************/
/*                        AGC		                           				  */
/******************************************************************************/

#define FR9009_ADDR_AGC_CONFIG1                                 	(APB_RX_AGC_BASEADDR+0x08)
#define FR9009_ADDR_AGC_GAIN_UPDATE_COUNTER                       	(APB_RX_AGC_BASEADDR+0x08)
#define FR9009_ADDR_SLOWLOOP_CONFIG                                 (APB_RX_AGC_BASEADDR+0x14)
#define FR9009_ADDR_AGC_UNDERRANGE0                                 (APB_RX_AGC_BASEADDR+0x18)
#define FR9009_ADDR_UPPER_LOWER_LEVEL_BLOCKER_THRESHOLD             (APB_RX_AGC_BASEADDR+0x20)
#define FR9009_ADDR_AGC_GAIN_STEP_DEC_OVERLOAD_CONFIG 				(APB_RX_AGC_BASEADDR+0x24)
#define FR9009_ADDR_DEC_OVERLOAD_UPPER_UNDER_RANGE_LOW_THRESHOLD	(APB_RX_AGC_BASEADDR+0x28)
#define FR9009_ADDR_DEC_UNDERRANGE_THRESHOLD		 				(APB_RX_AGC_BASEADDR+0x2C)
#define FR9009_ADDR_AGC_HB2_THRESH_EXCEEDED_CNT		 				(APB_RX_AGC_BASEADDR+0x30)
#define FR9009_ADDR_AGC_HB2_GAIN_STEP				 				(APB_RX_AGC_BASEADDR+0x34)
#define FR9009_ADDR_AGC_HB2_OVERLOAD_POWER			 				(APB_RX_AGC_BASEADDR+0x38)
#define FR9009_ADDR_AGC_LOCK_LEVEL_THRESHOLDS                      	(APB_RX_AGC_BASEADDR+0x3C)
#define FR9009_ADDR_POWER_THRESHOLDS								(APB_RX_AGC_BASEADDR+0x40)
#define FR9009_ADDR_DEC_POWER										(APB_RX_AGC_BASEADDR+0x44)
#define FR9009_ADDR_RX1_UL_SIG_POWER_MEAS_DURATION_DELAY			(APB_RX_AGC_BASEADDR+0x48)
#define FR9009_ADDR_RX1_UL_SIG_POWER_MEAS_DURATION_DELAY_CNT		(APB_RX_AGC_BASEADDR+0x4C)
#define FR9009_ADDR_RX2_UL_SIG_POWER_MEAS_DURATION_DELAY			(APB_RX_AGC_BASEADDR+0x50)
#define FR9009_ADDR_RX2_UL_SIG_POWER_MEAS_DURATION_DELAY_CNT		(APB_RX_AGC_BASEADDR+0x54)
#define FR9009_ADDR_AGC_POWER_OVER_RANGE_HIGH_THRESHOLD				(APB_RX_AGC_BASEADDR+0x58)
#define FR9009_ADDR_AGC_POWER_OVER_RANGE_LOW_THRESHOLD				(APB_RX_AGC_BASEADDR+0x5C)
#define FR9009_ADDR_AGC_POWER_UPPER_LOW_SHIFT_CONFIG				(APB_RX_AGC_BASEADDR+0x60)

#define FR9009_ADDR_AGC_MANUAL_GAIN_SET_FSM_CNT_RESET_RX1_RX2      	(APB_RX_AGC_BASEADDR+0x64)
#define FR9009_ADDR_MANUAL_GAIN_INDEX_DELAY_RX1                     (APB_RX_AGC_BASEADDR+0x70)
#define FR9009_ADDR_MAX_MIN_GAIN_INDEX_RX1                          (APB_RX_AGC_BASEADDR+0x7C)
#define FR9009_ADDR_DEC_POWER_CH1                                   (APB_RX_AGC_BASEADDR+0xD8)
#define FR9009_ADDR_DEC_POWER_CH2                                   (APB_RX_AGC_BASEADDR+0xDC)
#define FR9009_ADDR_MANUAL_GAIN_INDEX_DELAY_RX2                     (APB_RX_AGC_BASEADDR+0x8C)
#define FR9009_ADDR_MANUAL_GAIN_INDEX_ORX1_ORX2                    	(APB_RX_AGC_BASEADDR+0xB0)
#define FR9009_ADDR_MAX_MIN_GAIN_INDEX_RX2                          (APB_RX_AGC_BASEADDR+0x98)
#define FR9009_ADDR_RX1_RX2_GAIN_TAB_IDX                            (APB_RX_AGC_BASEADDR+0xD0)


/******************************************************************************/
/*                                FIR                              			  */
/******************************************************************************/

#define FR9009_ADDR_TX_CH1_CFIR_RX_COEF_START    (APB_TX_CFIR_BASEADDR+0x14) 
#define FR9009_ADDR_TX_CH1_CFIR_RX_COEF_CFG      (APB_TX_CFIR_BASEADDR+0x8) 
#define FR9009_ADDR_TX_CH2_CFIR_ORX_COEF_START   (APB_TX_CFIR_BASEADDR+0x94) 
#define FR9009_ADDR_TX_CH2_CFIR_ORX_COEF_CFG     (APB_TX_CFIR_BASEADDR+0x8) 
    
#define FR9009_ADDR_RX_CFIR_RX_COEF_START   (APB_RX_RCFIR_BASEADDR+0x14) 
#define FR9009_ADDR_RX_CFIR_RX_COEF_CFG     (APB_RX_RCFIR_BASEADDR+0x8) 

#define FR9009_ADDR_RX_CFIR_ORX_COEF_START  (APB_RX_RCFIR_BASEADDR+0x94) 
#define FR9009_ADDR_RX_CFIR_ORX_COEF_CFG    (APB_RX_RCFIR_BASEADDR+0x8) 

#define FR9009_ADDR_CFIR_POST_GAIN_MASK         (0x3 << 2)
#define FR9009_ADDR_CFIR_POST_GAIN_OFFSET       2
#define FR9009_ADDR_RX_COEF_SYMM_MASK           (1 << 4)
#define FR9009_ADDR_RX_COEF_SYMM_OFFSET         4
#define FR9009_ADDR_ORX_COEF_SYMM_MASK          (1 << 5)
#define FR9009_ADDR_ORX_COEF_SYMM_OFFSET        5
#define FR9009_ADDR_RX_COEF_EVEN_MASK           (1 << 6)
#define FR9009_ADDR_RX_COEF_EVEN_OFFSET         6
#define FR9009_ADDR_ORX_COEF_EVEN_MASK          (1 << 7)
#define FR9009_ADDR_ORX_COEF_EVEN_OFFSET        7
#define FR9009_ADDR_RX_COEF_VLD_MASK            (0xFF << 16)
#define FR9009_ADDR_RX_COEF_VLD_OFFSET          16
#define FR9009_ADDR_ORX_COEF_VLD_MASK           (0xFF << 24)
#define FR9009_ADDR_ORX_COEF_VLD_OFFSET         24



/******************************************************************************/
/*                                RX GAIN TABLE                               */
/******************************************************************************/
#define FR9009_ADDR_AGC_RF_ATTN_FE_ATTN_IDX_0       (APB_RX_ATTN_LUT_BASEADDR+0x80)
#define FR9009_ADDR_AGC_RF_ATTN_EXT_RSA_IDX_0       (APB_RX_ATTN_LUT_BASEADDR)
#define FR9009_ADDR_AGC_DIG_LUT_RX1_IDX_0           (APB_RX_DIG_GAIN_1_BASEADDR)
#define FR9009_ADDR_AGC_DIG_LUT_RX2_IDX_0           (APB_RX_DIG_GAIN_2_BASEADDR)

#define FR9009_ADDR_AGC_DIG_LUT_DIG_COMP_Q_OFFSET   16

/******************************************************************************/
/*                                RX DC OFFSRT                                */
/******************************************************************************/

#define FR9009_ADDR_DC_CORR_MAN_MODE_SETTING    (APB_RX_DC_CORR_BASEADDR+0x8)

#define FR9009_ADDR_DC_CORR_MAN_MODE_RX_CH1_EN_MASK       1
#define FR9009_ADDR_DC_CORR_MAN_MODE_RX_CH1_EN_OFFSET     0

#define FR9009_ADDR_DC_CORR_MAN_MODE_RX_CH2_EN_MASK       1<<1
#define FR9009_ADDR_DC_CORR_MAN_MODE_RX_CH2_EN_OFFSET     1

#define FR9009_ADDR_DC_CORR_MAN_MODE_ORX_CH1_EN_MASK      1<<2
#define FR9009_ADDR_DC_CORR_MAN_MODE_ORX_CH1_EN_OFFSET    2

#define FR9009_ADDR_DC_CORR_MAN_MODE_ORX_CH2_EN_MASK      1<<3
#define FR9009_ADDR_DC_CORR_MAN_MODE_ORX_CH2_EN_OFFSET    3


#define FR9009_ADDR_DC_CORR_MAN_RX_CH1_IQ       (APB_RX_DC_CORR_BASEADDR+0x14)
#define FR9009_ADDR_DC_CORR_MAN_ORX_CH1_IQ      (APB_RX_DC_CORR_BASEADDR+0x18)
#define FR9009_ADDR_DC_CORR_MAN_RX_CH2_IQ       (APB_RX_DC_CORR_BASEADDR+0x20)
#define FR9009_ADDR_DC_CORR_MAN_ORX_CH2_IQ      (APB_RX_DC_CORR_BASEADDR+0x24)


/******************************************************************************/
/*          SPI              tansmitter lane 		               			  */
/******************************************************************************/

#define FR9009_ADDR_TX1_ATTENUATION	                              (0x0058)
#define FR9009_ADDR_TX2_ATTENUATION	                              (0x0058)
//#define FR9009_ADDR_TX1_GAIN_0                                     0x0E87
//#define FR9009_ADDR_TX1_GAIN_1                                     0x0E88
//#define FR9009_ADDR_TX1_GAIN_2                                     0x0E89
//#define FR9009_ADDR_TX2_GAIN_0                                     0x0E8A
//#define FR9009_ADDR_TX2_GAIN_1                                     0x0E8B
//#define FR9009_ADDR_TX2_GAIN_2                                     0x0E8C
//#define FR9009_ADDR_TX_INCR_DECR_WORD                              0x0E8D
//#define FR9009_ADDR_TX_TPC_CONFIG                                  0x0E8E
//#define FR9009_ADDR_TDD_RAMP_TX1                                   0x0E92
//#define FR9009_ADDR_TDD_RAMP_TX2                                   0x0E93



/******************************************************************************/
/*                         SPI id                            				  */
/******************************************************************************/
#define SPI_JESD_RX_ID							    0
#define SPI_JESD_TX_ID                 				1
#define SPI_ADC_I_L1_ID                             2
#define SPI_ADC_Q_L1_ID                             3
#define SPI_ADC_I_L0_ID                             4
#define SPI_ADC_Q_L0_ID                             5
#define SPI_DAC_IQ_L1_ID                            6
#define SPI_FAFC_ORF_ID                             7
#define SPI_DAC_IQ_L0_ID                            8
#define SPI_FAFC_RF_ID                              9
#define SPI_TRANSMITTER_L1_ID                       10
#define SPI_TRANSMITTER_L0_ID                       11
#define SPI_RX_ORX_L1_ID                            12
#define SPI_RX_ORX_L0_ID                            13
#define SPI_ORFPLL_ID                               14
#define SPI_RFPLL_ID                                15
#define SPI_BBPLL_ID                                16
#define SPI_AUX_ADDA_IVREF_ID                       17
#define SPI_LO_TXALLWORK_REFPATH_ID                 18
#define SPI_ID_NOUSED				                0x1F

#define FR9009_ARMSPI_ID_POS                        16
#define FR9009_ARMSPI_ID_MASK                       0xFF0000
#define FR9009_ARMSPI_ADDR_MASK                     0xFFFF
#define FR9009_ADDR_ARMSPI(id, spi_addr)		    (0x48000000UL | ((id) << FR9009_ARMSPI_ID_POS) | ((spi_addr) & FR9009_ARMSPI_ADDR_MASK))

#ifdef __cplusplus
}
#endif

#endif

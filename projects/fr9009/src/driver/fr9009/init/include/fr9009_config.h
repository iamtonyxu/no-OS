/**
 * \file fr9009_config.h
 * \brief FR9009 configuration external declarations
 *
 * \brief Contains structure definitions for FR_config.c
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 *
 * The top level structure fr9009Device_t talDevice uses keyword
 * extern to allow the application layer main() to have visibility
 * to these settings.
 */

#ifndef FR_INIT_H_
#define FR_INIT_H_

#include "fr9009_types.h"
#include "fr_hal.h"
#ifdef __cplusplus
extern "C" {
#endif

//#define REFCLK_FREQ_KHZ     122880
#define REFCLK_FREQ_KHZ     125000
#define RX_OUTRATE_IS_REF_MULT  2
#define ORX_OUTRATE_IS_REF_MULT  4
#define TX_INRATE_IS_REF_MULT  4

#define LIF_NCO_FREQ_HZ	0
#define SYNC_NUM    1

#define PREDICT_CALI_ENALBE 0
#define FHM_MODE_EN 0
#define FHM_TYPE   FR_FHM_AUTO_RF_ORF_MODE //FR_FHM_MANUAL_RF_MODE 
#define FHM_SWITCH_FREQ_HZ      30
#define FHM_RF_TRIGGER_PIN_NUM  FR_GPIO_01
#define FHM_RF_TRIGGER_MODE   FR_FHM_GPIO_MODE  
#define FHM_ORF_TRIGGER_PIN_NUM  FHM_RF_TRIGGER_PIN_NUM
#define FHM_ORF_TRIGGER_MODE     FHM_RF_TRIGGER_MODE

#define FR9009_MCU_LOG_LEVEV 0  //0:鎵撳紑鎵�鏈塴og锛� 10锛氬叧闂墍鏈塴og
// #define FR9009_LOG_TORAM    1
extern fr9009Init_t talInit;

#ifdef __cplusplus
}
#endif

#endif

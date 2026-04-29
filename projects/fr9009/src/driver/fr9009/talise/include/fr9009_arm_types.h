/**
 * \file fr9009_arm_types.h
 * \brief Contains FR9009 ARM data types
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef FR9009_ARM_TYPES_H_
#define FR9009_ARM_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif


#pragma pack(push)
#pragma pack(1)
typedef struct{
	uint32_t version;	//[0:7] reversion	[8:15]minorVer [16:23] majorVer
	uint32_t jesd_phy_status;	// lane[3:0] cdr lock status 1:err 0:ok
	uint32_t checksum;
	uint32_t ext_refclk_hz;
	uint32_t refclk_hz;
	uint64_t rf_pll_hz;
	uint64_t orflo_pll_hz;
	uint64_t bbvco_freq_hz;
	uint32_t band:8;
	uint32_t lb_mode:4;
	uint32_t tx_ch:4;
	uint32_t rx_orx_ch:4;
	uint32_t LoBwIndex:3;
	uint32_t is_uartcmd:1;
	uint32_t track_cali_flag:8;
	uint32_t jesd_auto_reset:4;
}Globle_conf_t;


#pragma pack(pop)

typedef enum {
	TALAPI_ARMERR_BOOTUP_TIMEOUT_ERROR, /*!< Timed out waiting for ARM bootup to happen*/
	TALAPI_ARMERR_BOOTUP_IDLE,          /*!< ARM in IDLE mode after bootup*/
	TALAPI_ARMERR_BOOTUP_RADIO_ON,      /*!< ARM in RADIO_ON mode after bootup*/
	TALAPI_ARMERR_BOOTUP_PROFILE_ERROR, /*!< ARM Profile error during bootup*/
	TALAPI_ARMERR_BOOTUP_UNKNOWN_ERROR  /*!< ARM unknown error during bootup*/
} talApiArmErr_t;

typedef enum {
	FR_ARM_BUILD_DEBUG,                /*!< ARM binary is Debug Object*/
	FR_ARM_BUILD_TEST_OBJECT,          /*!< ARM binary is Test Object*/
	FR_ARM_BUILD_RELEASE               /*!< ARM binary is Release*/
} fr9009ArmBuildType_t;

typedef struct {
	uint8_t majorVer;                   /*!< The ARM Major revision*/
	uint8_t minorVer;                   /*!< The ARM Minor revision*/
	uint8_t rcVer;                      /*!< The release candidate version (build number)*/
	fr9009ArmBuildType_t buildType;     /*!< What type of ARM binary build*/
	uint8_t  build_time_str[32];		/*!< Build date*/
	uint8_t  customer_info_str[32];		/*!< customer and project information*/
} fr9009ArmVersionInfo_t;

#ifdef __cplusplus
}
#endif

#endif /* FR9009_ARM_TYPES_H_ */

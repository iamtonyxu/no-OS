#ifndef _APP_CMD_H
#define _APP_CMD_H

#include "platform.h"
#include "fr9361_common.h"
#include "custom_cfg.h"
#include "driver.h"

void cmd_api_tx_tone(TRX_CHN_ENUM chn, short on, long freq);
void cmd_api_rx_mgc_split_table_gain(TRX_CHN_ENUM chn, RX_MGC_GAIN_ENUM tb, unsigned char val);
void cmd_api_adc_ram_dump(TRX_CHN_ENUM chn, char *name);

#endif

#ifndef _DPD_TOP_H_
#define _DPD_TOP_H_

#include <stdint.h>
#include <complex.h>
#include <string.h>
#include "dpd_utils.h"
#include "dpd_t_p.h"
#include "dpd_filter.h"
#include "dpd_delay_filter_p.h"
#include "dpd_err_codes_t.h"
#include "dpd_utils.h"
#include "math_utils.h"

dpd_ErrCode_e dpd_Init(dpd_TrackData_t *pData);
dpd_ErrCode_e dpd_Finish(void);

dpd_ErrCode_e dpd_TrackConfig(dpd_TrackData_t *pData, dpd_TrackCfg_t *pNewTrackCfg);
dpd_ErrCode_e dpd_ModelConfig(dpd_TrackData_t *pData, uint8_t modelIndex);

dpd_ErrCode_e dpd_CoeffEstimate(dpd_TrackData_t *pData,
                                double complex *pTx,
                                double complex *pORx,
                                uint16_t capSize,
                                uint8_t  capBatch);

#endif
#ifndef _DPD_ACT_P_H_
#define _DPD_ACT_P_H_
#include "dpd_act_t.h"
#include <stdbool.h>
#include "dpd_err_codes_t.h"
#include "dpd_t_p.h"
#include "dpd_utils.h"

/*!
 * \brief     write into the vBank associated with the lut
 *
 * \note   write into the vBank associated with the lut
 *
 * \param [in]       pData - dpd track data
 * \param [in]       pBuf - pinter to the vBank
 * \param [in]       lut - lut index
 * \param [in]       lusScale - scale of lut entry coefficients
 * \param [in]       modelDepth  - depth of the model
 *
 *\retval ADI_ERROR_CODE
 *
 */
dpd_ErrCode_e WriteVBankLuts(dpd_TrackData_t *pData,
                                    uint32_t * pBuf,
                                    uint8_t lut,
                                    uint32_t lutScale,
                                    uint16_t modelDepth);

#endif
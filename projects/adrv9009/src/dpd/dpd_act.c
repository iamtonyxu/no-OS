#include <math.h>
#include <stdint.h>
#include <string.h>
#include "dpd_act_t.h"
#include "dpd_act_p.h"
//#include "adi_svc_dfe_actuator.h"

/*!
 * \brief      process entry value
 *
 * \note  convert the float point to integer,
 *        13 bits used to represent fraction and 2 bits for 12 dB headroom,
 *        1 bit for sign
 *        log error if the value is saturated
 *
 * \param [in]       pData - pointer to dpd track structure
 * \param [in]       f - double variable
 *
 *
 *\retval converted integer
 *
 */
static inline int16_t processEntryValue(dpd_TrackData_t *pData, double f)
{
    /*  saturate if out of bounds */
    if (f > (double)INT16_MAX)
    {
        f = (double)INT16_MAX;
        pData->actLutSatFlag = true;
    }
    else if (f < (double)INT16_MIN)
    {
        f = (double)INT16_MIN;
        pData->actLutSatFlag = true;
    }

    return ((int16_t)lround(f));
}

dpd_ErrCode_e WriteVBankLuts(dpd_TrackData_t *pData,
                                    uint32_t * pBuf,
                                    uint8_t lut,
                                    uint32_t lutScale,
                                    uint16_t modelDepth)
{
    dpd_ErrCode_e err = DPD_ERR_CODE_NO_ERROR;
    uint16_t vBankIndex = 0;
    dpd_ModelDesc_t *pModelDesc = (dpd_ModelDesc_t *)pData->pDpdModelDesc;
    uint64_t(*featureMask)[DPD_NUM_FEAT_MASK_BLKS] = pData->pLut->featMask;

    if (pBuf == NULL)
    {
        err = DPD_LUT_COPY_FAILED_ERROR;
    }
    else
    {
        /* calculate the entry content */
        for (int e = 0; e < modelDepth; e++)
        {
            double complex lutEntry = 0.0 + 0.0 * I;
            for (uint8_t fIdx = 0u; fIdx < pModelDesc->features; fIdx++)
            {
                uint8_t bIndex, fIndex;
#if 0
                dpd_UtilGetBlockFeatureMaskIndex(fIdx, &bIndex, &fIndex);
                if (featureMask[lut][bIndex] & (1ull << fIndex))
#else
                uint8_t curLut = pModelDesc->feature[fIdx].i + pModelDesc->feature[fIdx].j * DPD_SIGNAL_I_RANGE;
                if(lut == curLut)
#endif
                {
#if 0
    MEX_LOG("calculate entry value: fIdx = %d\n", fIdx);
#endif
                    /* calculate entry value */
                    double xMagPwr = dpd_UtilShiftedLegendre(e * 1.0 / modelDepth, pModelDesc->feature[fIdx].k);
                    lutEntry += lutScale * pModelDesc->feature[fIdx].a * xMagPwr;
#if 0
    MEX_LOG("xMagPwr = %f, lutEntry = %f + I*%f\n", xMagPwr, creal(lutEntry), cimag(lutEntry));
#endif
                }
            }
            /* scale to integer level */
            int16_t  entryValueReal = processEntryValue(pData, creal(lutEntry));
            int16_t  entryValueImag = processEntryValue(pData, cimag(lutEntry));
            //uint32_t val            = ((uint32_t)entryValueImag << 16) | ((uint32_t)entryValueReal & 0x0000ffffu);
            uint32_t val            = ((uint32_t)entryValueReal << 16) | ((uint32_t)entryValueImag & 0x0000ffffu);
#if 1
    MEX_LOG("lutEntry = %f + I*%f\n", creal(lutEntry), cimag(lutEntry));
    MEX_LOG("entryValueReal = %d, entryValueImag = %d\n", entryValueReal, entryValueImag);
    MEX_LOG("val = 0x%x\n", val);
#endif
            if(pData->actLutSatFlag)
            {
                err = DPD_ACT_LUT_ENTRY_SAT_ERROR;
            }

            /* vBank update */
            vBankIndex       = (uint16_t)((uint16_t)lut * modelDepth + e);
            pBuf[vBankIndex] = val;
#if 0
    //MEX_LOG("pBuf[%d] = %d\n", vBankIndex, pBuf[vBankIndex]);
#endif
        }
    }
    return (err);
}

dpd_ErrCode_e dpd_ActPopulateVBank(dpd_TrackData_t *pData, uint32_t lutScale)
{
    dpd_ErrCode_e     err        = DPD_ERR_CODE_NO_ERROR;
    dpd_ActModelCfg_t *pModelConfig = (dpd_ActModelCfg_t*)pData->pActModelCfg;
    dpd_ActModel_e    modelSel   = pData->pModelStateInfo->modelSel;
    uint16_t          modelDepth = 16u;       /* initialize to 16 avoid compiler error */

    uint8_t           * pPingPongReadyTogFlag;
    uint8_t           * pPingBufReady;
    uint8_t           * pPongBufReady;

    uint32_t          * pPingBuf;
    uint32_t          * pPongBuf;
    uint32_t          * pBuf;

    pPingPongReadyTogFlag = &(pData->pModelStateInfo->pingPongReadyTogFlag[modelSel]); /*!< Toggling Flag for Ping/Pong  */
    pPingBufReady         = &(pData->pModelStateInfo->pingBufReady[modelSel]);         /*!< Ping Buffer Ready Indicator  */
    pPongBufReady         = &(pData->pModelStateInfo->pongBufReady[modelSel]);         /*!< Pong Buffer ready Indicator  */

    pPingBuf = pData->pModelStateInfo->vBankPingBuf[modelSel];
    pPongBuf = pData->pModelStateInfo->vBankPongBuf[modelSel];
    pBuf     = *pPingPongReadyTogFlag ? pPongBuf:pPingBuf;

    switch(pModelConfig->actDepth)
    {
        case(DPD_ACT_DEPTH_16):
            modelDepth = 16u;
            break;
        case(DPD_ACT_DEPTH_32):
            modelDepth = 32u;
            break;
        case(DPD_ACT_DEPTH_64):
            modelDepth = 64u;
            break;
        case(DPD_ACT_DEPTH_128):
            modelDepth = 128u;
            break;
        case(DPD_ACT_DEPTH_256):
            modelDepth = 256u;
            break;
        default:
            modelDepth = 16u;
            break;
    }

    memset((void*)pBuf, 0, sizeof(uint32_t) * (uint32_t)DPD_ACT_DEPTH_MAX * (uint32_t)DPD_ACT_NUM_LUTS);

    /* go over all the luts */
    for (uint8_t n = 0u; (n < pData->pLut->cnt) && (err == DPD_ERR_CODE_NO_ERROR); n++)
    {
        err = WriteVBankLuts(pData,
                             pBuf,
                             (uint8_t)pData->pLut->idx[n],
                             lutScale,
                             modelDepth);
    }


#if (0)
    /*
     * This block of the code is only for testing purposes
     * It performs
     *
     * 1) programms a known pattern into the vBank
     * 2) Resets LUTs to zero
     * 3) sets bankIndexSwap to 2.
     */

    uint32_t *pvBank = (uint32_t *)pBuf;

    for (uint32_t n = 0u; (n < (uint32_t)64u * (uint32_t)DPD_ACT_NUM_LUTS); n++)
    {
        pvBank[n] = n;
    }

    for (uint8_t n = 0u; n < DPD_ACT_NUM_LUTS; n++)
    {
        for (uint8_t m = 0u; m < (uint32_t)64u; m++)
        {
            adi_svc_DfeActuatorLutUpdate(pData->txCh,
                                         n,
                                         m,
                                         0);
        }
    }

    pData->pModelStateInfo->bankIndexSwap = 2u;
#endif

    if (*pPingPongReadyTogFlag)
    {
        *pPongBufReady = 1;
        *pPingBufReady = 0;
    }
    else
    {
        *pPingBufReady = 1;
        *pPongBufReady = 0;
    }

    *pPingPongReadyTogFlag ^= 0x1;

    return (err);
}
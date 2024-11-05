/**
 * \file
 * \brief delay and TX filter
 *
 */

#ifndef _DPD_DELAY_FILTER_P_H_
#define _DPD_DELAY_FILTER_P_H_

#define DPD_FILTER_FRAC_INT_GROUP_DELAY    (7)       /* Group delay of fractional delay FIR filter */
#define DPD_FILTER_FRAC_LEN                (16)      /* Length of fractional delay FIR filter  */
#define DPD_FILTER_FRAC_DELAY_NUM          (64)      /* Number of fractional delay FIR filters */

#define DPD_FILTER_TX_GROUP_DELAY          (8)       /* Group delay of Tx samples FIR filter */
#define DPD_FILTER_TX_LEN                  (17)      /* Length of Tx samples FIR filter */
#define DPD_FILTER_TX_NUM                  (2)       /* Number of Tx samples FIR filter */

#define DPD_FILTER_BUF_LEN                 ((DPD_FILTER_TX_LEN > DPD_FILTER_FRAC_LEN) ?  DPD_FILTER_TX_LEN : DPD_FILTER_FRAC_LEN)

extern double dpd_FilterFracCoefs[DPD_FILTER_FRAC_DELAY_NUM][DPD_FILTER_FRAC_LEN];
extern double dpd_FilterTxCoefs[DPD_FILTER_TX_NUM][DPD_FILTER_TX_LEN];

#endif /* _DPD_DELAY_FILTER_P_H_  */

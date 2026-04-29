#include "txqec_corr.h"

void txqec_tone_download(uint32_t channel, int32_t qbin)
{
    // TODO
    return;
}

uint32_t txqec_data_capture(uint32_t channel,
							uint32_t num_samples,
							double complex *pTx,
							double complex *pOrx)
{
    // TODO
    return 0u;
}

uint32_t txqec_corr_compute(double complex *pTx,
							double complex *pOrx,
							uint32_t num_samples,
							const pathdelay_out_t *pathdelay,
							txqec_corr__outputs_t *out)
{
    // TODO
	// sync Tx and Orx

	// calculate corr(tx, orx) on lag = 0

    return 0u;
}


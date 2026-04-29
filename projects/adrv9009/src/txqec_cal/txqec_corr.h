#ifndef TXQEC_CORR_H
#define TXQEC_CORR_H

#include <stdint.h>
#include <complex.h>
#include "txqec_hw.h"

// Function prototypes
void txqec_tone_download(uint32_t channel, int32_t qbin);

uint32_t txqec_data_capture(uint32_t channel,
							uint32_t num_samples,
							double complex *pTx,
							double complex *pOrx);

uint32_t txqec_corr_compute(double complex *tx,
							double complex *rx,
							uint32_t num_samples,
							const pathdelay_out_t *pathdelay,
							txqec_corr__outputs_t *out);

#endif // TXQEC_CORR_H

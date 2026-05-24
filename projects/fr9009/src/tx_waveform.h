#ifndef _TX_WAVEFORM_H
#define _TX_WAVEFORM_H

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "parameters.h"

extern const uint32_t sine_lut_iq[1024];
extern const uint32_t tone_lut_iq_245M[TX_BUF_LEN];
extern const uint32_t tone_lut_iq_491M[TX_BUF_LEN];

void copy_waveform_to_ddr(const uint32_t *src, uint32_t length);

#endif

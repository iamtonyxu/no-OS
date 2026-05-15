#ifndef _RX_CAPTURE_H_
#define _RX_CAPTURE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xparameters.h"
#include "xil_io.h"
#include "xil_types.h"
#include "xstatus.h"

#define CAP_LENGTH_MAX  16384u
#define CAP_LENGTH_READ (CAP_LENGTH_MAX / 4)

void read_capture(uint32_t *cap_buf, uint32_t cap_length);

#endif

#ifndef _SDCARD_ACCESS_H
#define _SDCARD_ACCESS_H

#include "xparameters.h"	/* SDK generated parameters */
#include "xsdps.h"		/* SD device driver */
#include "xil_printf.h"
#include "ff.h"
#include "xil_cache.h"
#include "xplatform_info.h"

//#define MAX_FILE_SIZE (4*307200) //TO large for PS Memory
#define MAX_FILE_SIZE (4*16384)

extern uint32_t load_lut_iq[MAX_FILE_SIZE/4];

int read_wavefrom_from_sdcard(char *file_name,
							uint32_t file_size,
							uint32_t *load_lut_iq);

int read_sdcard(char *file_name,
				uint32_t file_size,
				uint8_t *read_buf,
				uint32_t *num_of_bytes_read);

void generate_waveform(uint8_t *read_buf, uint32_t num_of_bytes_read, uint32_t *load_lut_iq);

#endif

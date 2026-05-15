#include "parameters.h"
#include "rx_capture.h"

void read_capture(uint32_t *cap_buf, uint32_t cap_length)
{
	uint32_t *BaseAddress = (uint32_t *)CAPBUF_BASEADDR; // 0x4000_0000
	memset(cap_buf, 0, cap_length);

	for (int i = 0; i < cap_length; i++)
	{
#if 0
		cap_buf[i] = Xil_In32((BaseAddress) + (i * 4u)); // Wrong: don't copy all to cap_buf
#else
		cap_buf[i] = Xil_In32((BaseAddress) + i);
#endif
	}
}

#include <stdio.h>
#include <spi_rw.h>
#include "xparameters.h"
#include "xilinx_spi.h"
#include "no_os_spi.h"
#include "custom_cfg.h"
#include "main_init.h"

struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
	.type = SPI_PL,
#else
	.type = SPI_PS,
#endif
	.flags = 0
};

#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define SPI_CS                  0
#define SPI_PARAM				&xil_spi_param
#define SPI_OPS					&xil_spi_ops

//struct no_os_spi_desc *spi_desc
struct no_os_spi_desc **spi_desc;

struct no_os_spi_init_param	spi_param =
{
	.device_id = SPI_DEVICE_ID,
	.mode = NO_OS_SPI_MODE_1,
	.chip_select = SPI_CS,
	.platform_ops = SPI_OPS,
	.extra = SPI_PARAM,
	.max_speed_hz = 640000,
};

int main(void)
{
	int ret = -1;

	// spi init
	if(no_os_spi_init(spi_desc, &spi_param) != 0)
	{
		ret = -2;
		printf("\nfr9361 spi init failed, ret = %d\n", ret);
	}

	// spi hook
	spi_hook(*spi_desc);

    ret = fr936x_init(&g_phy_obj[0], &g_phy_config[0]);
    if (ret < 0)
    {
        printf("chip init failed!\n");
    }

	while(1);
	return 0;
}

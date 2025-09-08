#include <stdio.h>
#include <spi_rw.h>
#include "xparameters.h"
#include "xilinx_spi.h"
#include "xilinx_gpio.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "custom_cfg.h"
#include "main_init.h"
#include "driver.h"

#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define SPI_CS                  0
#define SPI_OPS					&xil_spi_ops

#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OPS				&xil_gpio_ops
#define GPIO_RESET_PIN			100
#define GPIO_ENABLE_PIN			101
#define GPIO_TXNRX_PIN			102

struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
	.type = SPI_PL,
#else
	.type = SPI_PS,
#endif
	.flags = 0
};

struct xil_gpio_init_param xil_gpio_param = {
#ifdef PLATFORM_MB
	.type = GPIO_PL,
#else
	.type = GPIO_PS,
#endif
	.device_id = GPIO_DEVICE_ID
};
#define SPI_PARAM				&xil_spi_param
#define GPIO_PARAM				&xil_gpio_param

//struct no_os_spi_desc *spi_desc
struct no_os_spi_desc **spi_desc;

struct no_os_spi_init_param	spi_param =
{
	.device_id = SPI_DEVICE_ID,
	.mode = NO_OS_SPI_MODE_1,
	.chip_select = SPI_CS,
	.platform_ops = SPI_OPS,
	.extra = SPI_PARAM,
	.max_speed_hz = 0, //0:prescaler_default(64) is used, spi_clk = 167/64 = 2.6MHz
};

//gpio_resetb
struct no_os_gpio_init_param gpio_resetb =
{
	.number = GPIO_RESET_PIN,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_PARAM,
};

struct no_os_gpio_desc 	*gpio_desc_resetb;

static void cmd_api_tx_tone(TRX_CHN_ENUM chn, short on, long freq);

int main(void)
{
	int ret = -1;
	uint8_t spi_rddata = 0;

	no_os_gpio_get(&gpio_desc_resetb, &gpio_resetb);

	//fr9361_reset
	if(gpio_desc_resetb)
	{
		no_os_gpio_direction_output(gpio_desc_resetb, 0);
		no_os_gpio_set_value(gpio_desc_resetb, 0); // DMM to check resetb = 0
		no_os_mdelay(1);
		no_os_gpio_set_value(gpio_desc_resetb, 1);// DMM to check resetb = 1
		no_os_mdelay(1);
	}

	// spi init
	if(no_os_spi_init(spi_desc, &spi_param) != 0)
	{
		ret = -2;
		printf("\nfr9361 spi init failed, ret = %d\n", ret);
	}

	// spi hook
	spi_hook(*spi_desc);

	AD9361_WR(0x900, 0x07);
	AD9361_WR(0x904, 0xA4);

	spi_rddata = AD9361_RD(0x615);
	AD9361_WR(0x615, 0x04);
	spi_rddata = AD9361_RD(0x615);

	module_debug_onoff(&g_phy_obj[0], 0xFFF);

    ret = fr936x_init(&g_phy_obj[0], &g_phy_config[0]);

    if (ret < 0)
    {
        printf("chip init failed!\n");
    }

	while(1);
	return 0;
}

static void cmd_api_tx_tone(TRX_CHN_ENUM chn, short on, long freq)
{
	LOG_MAIN("tx tone, chn:%d, on:%d, freq=%d\n", chn, on, freq);
	//fn_tx_send_tone(chn, on, freq);
	send_cordic_signal(&g_phy_obj[g_phy_select], chn, g_phy_obj[g_phy_select].config->bandwidth, on, freq);
}

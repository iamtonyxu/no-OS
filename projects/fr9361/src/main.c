#include <stdio.h>
#include "custom_cfg.h"
#include "main_init.h"

int main(void)
{
	int ret = -1;

    ret = fr936x_init(&g_phy_obj[0], &g_phy_config[0]);
    if (ret < 0)
    {
        printf("chip init failed!\n");
    }

	while(1);
	return 0;
}

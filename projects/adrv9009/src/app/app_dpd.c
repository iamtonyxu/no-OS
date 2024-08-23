#include "app_dpd.h"

uint8_t dpd_luts_access_test(void)
{
	uint8_t errCode = 0;
	uint32_t entry_addr[5] = {ADDR_IP_VERSION, ADDR_ID_MASK_LOW, ADDR_ID_MASK_HIGH, ADDR_SCRATCH, ADDR_BYPASS};
	uint32_t wr_data[5] = {0x0000, 0x0000, 0x0000, 0x1234, 0x0001};
	uint32_t rd_data[5] = {0};

	// DPD Control Register Access Test
	for(int i = 0; i < 5; i++)
	{
		no_os_axi_io_write(DPD_CTRL_BASEADDR, entry_addr[i], wr_data[i]);

		no_os_axi_io_read(DPD_CTRL_BASEADDR, entry_addr[i], &rd_data[i]);
	}

    // ADDR_SCRATCH, scratch register
	if(wr_data[3] != rd_data[3])
	{
		errCode = 1;
	}

	// DPD Luts Access Test
    // Only verify the first and last 5 words
    if(errCode == 0)
    {
        for(int i = 0; i < 5; i++)
        {
            no_os_axi_io_write(DPD_MEM_BASEADDR, i*4, 0x11111111*(i+1));
            no_os_axi_io_read(DPD_MEM_BASEADDR, i*4, &rd_data[i]);

            if(rd_data[i] != 0x11111111*(i+1))
            {
                errCode = 1;
                break;
            }
        }
    }

    if(errCode == 0)
    {
        for(int i = 0; i < 5; i++)
        {
            no_os_axi_io_write(DPD_MEM_BASEADDR, 0x8000-(i+1)*4, 0x11111111*(i+1));
            no_os_axi_io_read(DPD_MEM_BASEADDR, 0x8000-(i+1)*4, &rd_data[i]);

            if(rd_data[i] != 0x11111111*(i+1))
            {
                errCode = 1;
                break;
            }
        }
    }

	return errCode;
}

uint8_t dpd_write_luts(uint8_t lutId, uint32_t *pLut)
{
    uint8_t errCode = 0u;
    uint32_t offset = lutId * (DPD_LUT_DEPTH * 4);

    if((lutId >= DPD_LUT_LIMIT) || (pLut == NULL))
    {
        errCode = 1;
    }
    
    if(errCode == 0)
    {
        for(int i = 0; i < DPD_LUT_DEPTH; i++)
        {
            no_os_axi_io_write(DPD_MEM_BASEADDR, offset+i*4, pLut[i]);
        }
    }

    return errCode;
}

uint8_t dpd_read_luts(uint8_t lutId, uint32_t *pLut)
{
    uint8_t errCode = 0u;
    uint32_t offset = lutId * (DPD_LUT_DEPTH * 4);

    if((lutId >= DPD_LUT_LIMIT) || (pLut == NULL))
    {
        errCode = 1;
    }

    if(errCode == 0)
    {
        for(int i = 0; i < DPD_LUT_DEPTH; i++)
        {
            no_os_axi_io_read(DPD_MEM_BASEADDR, offset+i*4, &(pLut[i]));
        }
    }

    return errCode;
}

uint32_t dpd_read_ipVersion(void)
{
    uint32_t ipVersion = 0;
    no_os_axi_io_read(DPD_CTRL_BASEADDR, ADDR_IP_VERSION, &ipVersion);
    return ipVersion;
}

uint32_t dpd_read_idMask_low(void)
{
    uint32_t idMask_low = 0;
    no_os_axi_io_read(DPD_CTRL_BASEADDR, ADDR_ID_MASK_LOW, &idMask_low);
    return idMask_low;
}

uint32_t dpd_read_idMask_high(void)
{
    uint32_t idMask_high = 0;
    no_os_axi_io_read(DPD_CTRL_BASEADDR, ADDR_ID_MASK_LOW, &idMask_high);
    return idMask_high;
}

uint32_t dpd_write_scratch_reg(uint32_t scratch)
{
    no_os_axi_io_write(DPD_CTRL_BASEADDR, ADDR_SCRATCH, scratch);
    return dpd_read_scratch_reg();
}

uint32_t dpd_read_scratch_reg(void)
{
    uint32_t scratch = 0;
    no_os_axi_io_read(DPD_CTRL_BASEADDR, ADDR_SCRATCH, &scratch);
    return scratch;
}

uint8_t dpd_write_bypass_reg(uint8_t enable)
{
    no_os_axi_io_write(DPD_CTRL_BASEADDR, ADDR_BYPASS, enable);
    return dpd_read_bypass_reg();
}

uint8_t dpd_read_bypass_reg(void)
{
    uint32_t bypassStatus;
    no_os_axi_io_read(DPD_CTRL_BASEADDR, ADDR_BYPASS, &bypassStatus);
    return (uint8_t)bypassStatus;
}

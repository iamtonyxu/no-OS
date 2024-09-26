#include "app_dpd.h"

uint8_t dpd_luts_access_test(void)
{
	uint8_t errCode = 0;
	uint32_t entry_addr[4] = {ADDR_IP_VERSION, ADDR_ID_MASK_LOW, ADDR_ID_MASK_HIGH, ADDR_SCRATCH};
	uint32_t wr_data[4] = {0x0000, 0x0000, 0x0000, 0x1234};
	uint32_t rd_data[4] = {0};

	// DPD Control Register Access Test
	for(int i = 0; i < sizeof(entry_addr)/sizeof(uint32_t); i++)
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
    	int end = DPD_LUT_DEPTH * 4;
        for(int i = 0; i < 5; i++)
        {
            no_os_axi_io_write(DPD_MEM_BASEADDR, end-(i+1)*4, 0x11111111*(i+1));
            no_os_axi_io_read(DPD_MEM_BASEADDR, end-(i+1)*4, &rd_data[i]);

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
    uint64_t idMask = (1lu << lutId);

    // write lutid
    dpd_write_lutid(idMask);
    if((lutId >= DPD_LUT_MAX) || (pLut == NULL))
    {
        errCode = 1;
    }
    
    if(errCode == 0)
    {
        for(int i = 0; i < DPD_LUT_DEPTH; i++)
        {
            no_os_axi_io_write(DPD_MEM_BASEADDR, i*4, pLut[i]);
        }
    }
    dpd_write_lutid(0u);

    return errCode;
}

uint8_t dpd_read_luts(uint8_t lutId, uint32_t *pLut)
{
    uint8_t errCode = 0u;
    uint64_t idMask = (1lu << lutId);

    // write lutid
    dpd_write_lutid(idMask);

    if((lutId >= DPD_LUT_MAX) || (pLut == NULL))
    {
        errCode = 1;
    }

    if(errCode == 0)
    {
        for(int i = 0; i < DPD_LUT_DEPTH; i++)
        {
            no_os_axi_io_read(DPD_MEM_BASEADDR, i*4, &(pLut[i]));
        }
    }
    dpd_write_lutid(0u);

    return errCode;
}

uint32_t dpd_read_ipVersion(void)
{
    uint32_t ipVersion = 0;
    no_os_axi_io_read(DPD_CTRL_BASEADDR, ADDR_IP_VERSION, &ipVersion);
    return ipVersion;
}

uint64_t dpd_read_idMask(void)
{
    uint32_t idMask_high, idMask_low;
    no_os_axi_io_read(DPD_CTRL_BASEADDR, ADDR_ID_MASK_LOW, &idMask_low);
    no_os_axi_io_read(DPD_CTRL_BASEADDR, ADDR_ID_MASK_HIGH, &idMask_high);
    return (idMask_high << 32u) | idMask_low;
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

uint8_t dpd_write_act_out_sel(Dpd_ActOut_Sel sel)
{
	no_os_axi_io_write(DPD_CTRL_BASEADDR, ADDR_ACT_OUT_SEL, (uint32_t)sel);
	return dpd_read_act_out_sel();
}

uint8_t dpd_read_act_out_sel(void)
{
    uint32_t outSel;
    no_os_axi_io_read(DPD_CTRL_BASEADDR, ADDR_ACT_OUT_SEL, &outSel);
    return (uint8_t)outSel;
}

uint64_t dpd_write_lutid(uint64_t lutid)
{
	uint32_t lutid_low, lutid_high;
	lutid_high = (lutid >> 32u) & 0xFFFFFFFF;
	lutid_low = lutid & 0xFFFFFFFF;
	no_os_axi_io_write(DPD_CTRL_BASEADDR, ADDR_LUTID_H, lutid_high);
	no_os_axi_io_write(DPD_CTRL_BASEADDR, ADDR_LUTID_L, lutid_low);
	return dpd_read_lutid();
}

uint64_t dpd_read_lutid(void)
{
	uint32_t lutid_low, lutid_high;
    no_os_axi_io_read(DPD_CTRL_BASEADDR, ADDR_LUTID_L, &lutid_low);
    no_os_axi_io_read(DPD_CTRL_BASEADDR, ADDR_LUTID_H, &lutid_high);
    return ((lutid_high << 32u) | (lutid_low));
}



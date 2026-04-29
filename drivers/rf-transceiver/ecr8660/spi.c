#include "spi.h"
#include "stdio.h"

static struct no_os_spi_desc *desc = NULL;

uint8_t spi_hook(struct no_os_spi_desc *spi_desc)
{
	desc = spi_desc;
	return (!desc);
}

uint64_t spi_readwrite(uint32_t write_data1, uint32_t write_data2, uint32_t len)
{
	uint64_t read_data = 0u; //only the lower 32bits are used.
	uint8_t data[8], bytes_number;

	if(desc)
	{
		if(len == 32u)
		{
			/* The order of bytes sent:
			 * wr_data1[31]
			 * wr_data1[30]
			 * ...
			 * wr_data1[0]
			 * */
			bytes_number = 4;
			for(int i = 0; i < bytes_number; i++)
			{
				int bit_shift = (bytes_number - 1 - i) * 8;
				data[i] = (write_data1 >> bit_shift) & 0xff;
			}
		}
		else if(len == 64u)
		{
			bytes_number = 6;
			/* The order of bytes sent
			 * wr_data1[31]
			 * wr_data1[30]
			 * ...
			 * wr_data1[0]
			 * wr_data2[31]
			 * wr_data2[30]
			 * ...
			 * wr_data2[0]
			 * */
			for(int i = 0; i < bytes_number; i++)
			{
				int bit_shift = 0;
				if(i < 4)
				{
					bit_shift = (4 - 1 - i) * 8;
					data[i] = (write_data1 >> bit_shift) & 0xff;
				}
				else
				{
					bit_shift = (8 - 1 - i) * 8;
					data[i] = (write_data2 >> bit_shift) & 0xff;
				}
			}
		}
		/* spi write and read */
		int32_t ret = no_os_spi_write_and_read(desc, data, bytes_number);

		if(ret == 0)
		{
			if(len == 32)
			{
				read_data |= data[0] << (3 * 8);
				read_data |= data[1] << (2 * 8);
				read_data |= data[2] << (1 * 8);
				read_data |= data[3] << 0;
			}
			else
			{
				read_data |= data[2] << (3 * 8);
				read_data |= data[3] << (2 * 8);
				read_data |= data[4] << (1 * 8);
				read_data |= data[5] << 0;
			}
		}
		else
		{
			printf("\n no_os_spi_write_and_read() failed.\n");
		}
	}

	return read_data;
}

void Delayms(uint32_t msecs)
{
	no_os_mdelay(msecs);
}

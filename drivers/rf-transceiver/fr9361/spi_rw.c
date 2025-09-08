#ifdef LINUX_OS
#include <pthread.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <linux/types.h>
#include <linux/mii.h>
#include <arpa/inet.h>
#else
//no-os for xilinx fpga

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <getopt.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include "platform.h"
#include "no_os_delay.h"

#define IOC_MAGIC 'k'
#define GETDATA _IOR(IOC_MAGIC,1,int)
#define SETDATA _IOW(IOC_MAGIC,2,int)
#define FLUSHDATA _IOW(IOC_MAGIC,3,int)

typedef struct params_s params_t;
struct params_s {
    unsigned int reg;
    unsigned int value;
    unsigned int type;
};

enum  ENUM_DEV{
	ENUM_DEV_FPGA = 0,
	ENUM_DEV_AD9361,
	ENUM_DEV_CPU,
};

#if 1

static struct no_os_spi_desc *desc = NULL;

uint8_t spi_hook(struct no_os_spi_desc *spi_desc)
{
	desc = spi_desc;
	return (!desc);
}

unsigned int FPGA_WR(unsigned int reg_addr,unsigned int value)
{
	printf("dummy FPGA_WR(%d, %d)", reg_addr, value);
    return 0;
}

unsigned int FPGA_RD(unsigned int reg_addr)
{
	printf("dummy FPGA_RD(%d)", reg_addr);
    return 0;
}

unsigned int AD9361_WR(unsigned int reg_addr,unsigned int value)
{
	uint8_t data[3];
	const uint8_t bytes_number = 3;
	int32_t ret = 0;


	data[2] = value;
	data[1] = reg_addr & 0xff;
	data[0] = 0x80 | (reg_addr >> 8);
	ret = no_os_spi_write_and_read(desc, data, bytes_number);
	no_os_mdelay(1);
	if(ret==0)
	{
		return 0;
	}
    return 1;
}

unsigned int AD9361_RD(unsigned int reg_addr)
{
	uint8_t data[3];
	const uint8_t bytes_number = 3;
	int32_t ret = 0;
	uint8_t rdbyte = 0xff;

	data[2] = 0u;
	data[1] = reg_addr & 0xff;
	data[0] = 0x0f & (reg_addr >> 8);
	ret = no_os_spi_write_and_read(desc, data, bytes_number);
	no_os_mdelay(1);
	if(ret==0)
	{
		rdbyte = data[2];
	}
    return rdbyte;
}

unsigned int CPU_WR(unsigned int reg_addr,unsigned int value)
{
	printf("dummy CPU_WR(%d, %d)", reg_addr, value);
    return 0;
}

unsigned int CPU_RD(unsigned int reg_addr)
{
	printf("dummy CPU_RD(%d)", reg_addr);
    return 0;
}

/* fpga rw */
unsigned int fpga_read_reg(unsigned int reg)
{
	return FPGA_RD(reg);
}

short fpga_write_reg(unsigned int reg, unsigned int val)
{
	FPGA_WR(reg, val);
	return 0;
}

/* spi rw */
unsigned char spi_read_reg(unsigned short reg)
{
	unsigned char ret=0;
	ret = AD9361_RD(reg);
	return ret;
}

short spi_write_reg(unsigned short reg, unsigned char val)
{
    unsigned char ret = 1;
	ret = AD9361_WR(reg, val);
	return ret;
}

int get_spi_fd(void)
{
	printf("dummy get_spi_fd");
    return 0;
}

#else
pthread_mutex_t     Driver_Lock;

static int SPI_FD = -1;

static unsigned int api(unsigned int device,unsigned int action,unsigned int reg_addr,unsigned int value, int type)
{
    int fd = SPI_FD;
    int ret=0;
    params_t params;
    params.reg   = reg_addr;
    params.value = value;
	params.type = type;

    if(device == 1)  //1:FPGA
    {
        if(action == 0)  //Write
        {
            ret = ioctl(fd,SETDATA,(void*)&params);
            //LOG_INFO("W:address=0x%x val=0x%x\n", reg_addr,value);
            return 0;
        }
        else if(action == 1)  //Read
        {
            ret = ioctl(fd,GETDATA,(void*)&params);
            //LOG_INFO("R:address=0x%x val=0x%x\n", reg_addr,read_value);
            return (unsigned int)params.value;
        }
        else if(action == 2)  //ext
        {
            ret = ioctl(fd,FLUSHDATA,(void*)&params);
            //LOG_INFO("EXT:address=0x%x val=0x%x\n", reg_addr,value);
            return 0;
        }
        else
        {
            return 0;
        }
    }
    else if(device == 2)  //2:AD9364
    {
        if(action == 0)  //Write
        {
            ret = ioctl(fd,SETDATA,(void*)&params);
            //LOG_INFO("W:address=0x%x val=0x%x\n", reg_addr,value);
            return 0;
        }
        else if(action == 1)  //Read
        {
            ret = ioctl(fd,GETDATA,(void*)&params);
            //LOG_INFO("R:address=0x%x val=0x%x\n", reg_addr,read_value);
            return (unsigned int)params.value;
        }
        else
        {
            return 0;
        }
    }
    else
    {
		if(action == 0)  //Write
		{
			ret = ioctl(fd, SETDATA,(void*)&params);
			return 0;
		}
		else if(action == 1)  //Read
		{
			ret = ioctl(fd, GETDATA,(void*)&params);
			return (unsigned int)params.value;
		}
		else
		{
			return 0;
		}
    }
}

unsigned int FPGA_WR(unsigned int reg_addr,unsigned int value)
{
    unsigned int tem;
    pthread_mutex_lock(&Driver_Lock);
    tem = api(1,0,reg_addr,value, ENUM_DEV_FPGA);
    //LOG_INFO("W:address=0x%x val=0x%x\n", reg_addr,value&0xFF);
    pthread_mutex_unlock(&Driver_Lock);
    return tem;
}

unsigned int FPGA_RD(unsigned int reg_addr)
{
    unsigned int tem;
    pthread_mutex_lock(&Driver_Lock);
    tem = api(1,1,reg_addr,0, ENUM_DEV_FPGA);
    //LOG_INFO("R:address=0x%x val=0x%x\n", reg_addr,tem);
    pthread_mutex_unlock(&Driver_Lock);
    return tem;
}

unsigned int AD9361_WR(unsigned int reg_addr,unsigned int value)
{
    unsigned int tem,cpu_fpga_sel;
    cpu_fpga_sel = FPGA_RD(0x70) >> 1;
    if(cpu_fpga_sel == 0)
    {
        pthread_mutex_lock(&Driver_Lock);
        tem = api(2,0,reg_addr,value&0xFF,  ENUM_DEV_AD9361);
        //LOG_INFO("W:address=0x%x val=0x%x\n", reg_addr,value&0xFF);
        pthread_mutex_unlock(&Driver_Lock);
        return tem;
    }
    else
    {
        pthread_mutex_lock(&Driver_Lock);
		tem = api(1, 0,  reg_addr, value&0xFF, ENUM_DEV_AD9361);
        //LOG_INFO("W:address=0x%x val=0x%x\n", reg_addr,value&0xFF);
        pthread_mutex_unlock(&Driver_Lock);
        return tem;
    }
}

unsigned int AD9361_RD(unsigned int reg_addr)
{
    unsigned int tem,cpu_fpga_sel;
    cpu_fpga_sel = FPGA_RD(0x70) >> 1;
    if(cpu_fpga_sel == 0)
    {
        pthread_mutex_lock(&Driver_Lock);
        tem = api(2,1,reg_addr,0, ENUM_DEV_AD9361);
        //LOG_INFO("R:address=0x%x val=0x%x\n", reg_addr,tem);
        pthread_mutex_unlock(&Driver_Lock);
        return tem;
    }
    else
    {
        pthread_mutex_lock(&Driver_Lock);
		tem = api(1,  1,  reg_addr,  0, ENUM_DEV_AD9361); 
        //LOG_INFO("R:address=0x%x val=0x%x\n", reg_addr,tem);
        pthread_mutex_unlock(&Driver_Lock);
        return tem;
    }
}

unsigned int CPU_WR(unsigned int reg_addr,unsigned int value)
{
	unsigned int tem;
	pthread_mutex_lock(&Driver_Lock);
	tem = api(3,0,reg_addr,value, ENUM_DEV_CPU);
	pthread_mutex_unlock(&Driver_Lock);
	return tem;
}

unsigned int CPU_RD(unsigned int reg_addr)
{
	unsigned int tem;
	pthread_mutex_lock(&Driver_Lock);
	tem = api(3,1,reg_addr,0, ENUM_DEV_CPU);
	pthread_mutex_unlock(&Driver_Lock);
	return tem;
}

/* fpga rw */
unsigned int fpga_read_reg(unsigned int reg)
{
	return FPGA_RD(reg);
}

short fpga_write_reg(unsigned int reg, unsigned int val)
{
	FPGA_WR(reg, val);
	return 0;
}

/* spi rw */
unsigned char spi_read_reg(unsigned short reg)
{
	unsigned char ret=0;
	ret = AD9361_RD(reg);
	return ret;
}

short spi_write_reg(unsigned short reg, unsigned char val)
{
	AD9361_WR(reg, val);
	return 0;
}

int get_spi_fd(void)
{
    char * SPI_DEVICE = "/dev/axi_fpga";
	if(SPI_FD < 0)
	{
	    SPI_FD = open(SPI_DEVICE,O_RDWR);
	    if(SPI_FD < 0)
	    {
	        perror("open");
	        LOG_ERROR("Open spi dev file error!\n");
	        exit(1);
	    }
	}
    return SPI_FD;
}
#endif

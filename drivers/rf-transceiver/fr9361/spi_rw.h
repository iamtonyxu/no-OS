#ifndef  __SPI_RW_H
#define  __SPI_RW_H

extern unsigned char spi_read_reg(unsigned short reg);
extern short spi_write_reg(unsigned short reg, unsigned char val);
extern unsigned int fpga_read_reg(unsigned int reg);
extern short fpga_write_reg(unsigned int reg, unsigned int val);

extern unsigned int CPU_WR(unsigned int reg_addr,unsigned int value);
extern unsigned int CPU_RD(unsigned int reg_addr);
extern unsigned int FPGA_WR(unsigned int reg_addr,unsigned int value);
extern unsigned int FPGA_RD(unsigned int reg_addr);
extern unsigned int AD9361_WR(unsigned int reg_addr,unsigned int value);
extern unsigned int AD9361_RD(unsigned int reg_addr);

#endif /* __SPI_RW_H */

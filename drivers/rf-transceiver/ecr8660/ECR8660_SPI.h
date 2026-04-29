#ifndef __ECR8660_SPI_H
#define __ECR8660_SPI_H
#include    "Bit_macro.h"

// SPI_REG_Ext_Int
typedef enum _spi_reg_ext_int_e_{
    SPI_RW_INTERIOR                     = 0x1,  // 表示 SPI 页内寄存器
    SPI_RW_EXTERNAL                     = 0x0,  // 表示 SPI 业外寄存器
}Spi_Reg_Ext_Int_E;

// SPI_Read_Write
typedef enum _spi_read_write_e_{
    ECR8660_SPI_READ                    = 0x0,  // SPI读模式
    ECR8660_SPI_WRIT                    = 0x1,  // SPI写模式
    ECR8660_SPI_SLEEP                   = 0x2,
}Spi_Read_Write_E;

// Trans_size
typedef enum _spi_trans_size_e_{
    TRANS_ONEBYTE_DATA                  = 0x0,  // 传输 1Byte 数据
    TRANS_TWOBYTE_DATA                  = 0x1,  // 传输 2Byte 数据
    TRANS_FOURBYTE_DATA                 = 0x3,  // 传输 4Byte 数据
    TRANS_EIGHTBYTE_DATA                = 0x7,  // 传输 8Byte 数据
    TRANS_INTERNAL_REGISTER             = 0x6,  // 作为 SPI 内部寄存器读写状态指示
    TRANS_INTERNAL_DATA                 = 0x1,  // SPI 内部寄存器传输 2Byte 数据
}Spi_Trans_Size_E;

// 用于拼凑 SPI_CMD 时序（SPI 时序前 16Bit） 
typedef union _ecr8660_spi_comd_u_{
    INT16U  Word;
    struct{
        INT16U          Addr            : 12;   // 地址
        INT16U          Trans_size      :  3;   // SPI 页内外指示，长度指示
        INT16U          Read_Write      :  1;   // 读写标志
    }Bits;
}Ecr8660_Fm_Spi_Cmd_U;

// 用于存放 SPI_BIT 信息
typedef  union _global_spi_transfer_u_{
    INT32U word[2];
    struct{
        INT32U          writecmdlen     :  8;   // 用于指示 MOSI 需要 发送 的数据长度
        INT32U          readdatalen     :  8;   // 用于指示 MISO 需要 接受 的数据长度
        INT32U          reserved        : 16;
        INT32U          spidata1        : 32;
        INT32U          spidata2        : 32;
    }Bits;
}*pGlobal_Spi_Trans_U, Global_Spi_Trans_U;

// 用于做对齐
typedef union _ecr8660e_spi_data_u_{  // union 低地址对齐
    INT64U  Clear;              //元长度8
    struct{
        INT64U          Reserve         : 56;
        INT64U          Data            :  8;
    }OneByte;                           // 元长度1 Byte
    struct{
        INT64U          Reserve         : 48;
        INT64U          Data            : 16;
    }TwoByte;                           // 元长度2
    struct{
        INT64U          Reserve         : 32;
        INT64U          Data            : 32;
    }FourByte;                          // 元长度4
    struct{
        INT64U          Data            : 64;
    }EightByte;                         // 元长度4
    struct{
        INT64U          Low_16          : 16;
        INT64U          Middle_32       : 32;
        INT64U          High_16         : 16;
    }Bits;
}Ecr8660_Fm_Spi_Data_U;

// 用于做对齐
typedef union _ecr8660_fm_spi_time_series_u_{
    INT32U  Word;
    struct{
        INT32U          Low_Half_Word   : 16;
        INT32U          Hight_Half_Word : 16;
    }Bits;
}Ecr8660_Fm_Spi_Time_Series_U;

// 用于存放基地址映射表
typedef struct _ecr8660_fm_base_addr_dict_t_{
    INT32U              Base_Addr;
    INT16U              Base_Log;
}Ecr8660_Fm_Base_Addr_Dict_T;

// 用于存寄存器值序列
typedef struct _ecr8660_reg_list_t_{
    Spi_Read_Write_E    readwrite;
    Spi_Reg_Ext_Int_E   mode;
    INT32U              addr;
    INT32U              data;
    CHAR                *log;
}Ecr8660_Reg_List_T;

/********************************************************************************
*                Macro Definition  eg:#define Macro Definition
********************************************************************************/
//          SPI_BIT_LEN
#define     USER_ECR8660_SPI_BIT_08             (1)
#define     USER_ECR8660_SPI_BIT_16             (2)
#define     USER_ECR8660_SPI_BIT_32             (3)
//          SPI_LOG
#define     ECR8660_REG_08DATA_LOG              (0x00000021)        // SPI 数据长度设为 1Byte 时的 Log
#define     ECR8660_REG_16DATA_LOG              (0x00004321)        // SPI 数据长度设为 2Byte 时的 Log
#define     ECR8660_REG_32DATA_LOG              (0x87654321)        // SPI 数据长度设为 4Byte 时的 Log
#if         (USER_ECR8660_SPI_BIT_LEN  ==       USER_ECR8660_SPI_BIT_08)
#define     USER_ECR8660_SPI_SIZE               (TRANS_ONEBYTE_DATA)            // SPI 数据长度设为 1Byte 时的 枚举
#define     ECR8660_SPI_REG_LOG                 (ECR8660_REG_08DATA_LOG)        // SPI 数据长度设为 1Byte 时的 Log
#elif       (USER_ECR8660_SPI_BIT_LEN  ==       USER_ECR8660_SPI_BIT_16)
#define     USER_ECR8660_SPI_SIZE               (TRANS_TWOBYTE_DATA)            // SPI 数据长度设为 2Byte 时的 枚举
#define     ECR8660_SPI_REG_LOG                 (ECR8660_REG_16DATA_LOG)        // SPI 数据长度设为 2Byte 时的 Log
#else
#define     USER_ECR8660_SPI_SIZE               (TRANS_FOURBYTE_DATA)           // SPI 数据长度设为 4Byte 时的 枚举
#define     ECR8660_SPI_REG_LOG                 (ECR8660_REG_32DATA_LOG)        // SPI 数据长度设为 4Byte 时的 Log
#endif
//          Addr
#define     ECR8660_SIZE_ADDR                   (0x10)
#define     ECR8660_SPI_ADDR(Addr)              ((Addr) &  0xFFF)   // ECR8660 的 SP I时序中地址长度限制
#define     ECR8660_SPI_BASE_ADDR(Addr)         ((Addr) >> 12)      // 获取基地址
#define     ECR8660_SPI_BYTE(Trans_size)        ((Trans_size) + 1)  // 由 Trans_size 计算 SPI 读写的数据长度(Byte)
#define     ECR8660_SPI_BIT(Trans_size)         (ECR8660_SPI_BYTE(Trans_size) << 3) // 由 Trans_size 计算 SPI 读写的数据长度(BIT)
#define     USE_ONE_DATAREG                     (0x1)               // SPI总时序小于 32Bit
#define     USE_TWO_DATAREG                     (0x2)               // SPI总时序小于 64Bit 大于32Bit
#define     USE_THR_DATAREG                     (0x3)               // SPI总时序小于 80Bit 大于64Bit
//          ERR
#define     BASE_ADDR_ERR                       (0xFFFFF)
//          LIST_END_LOG
#define     ECR8660_LIST_END_LOG                (0xFFFFFFFF)
/********************************************************************************
*           Function Definitions  eg:void Function_Definitions(void);
********************************************************************************/
INT64U      ECR8660_read_write(Spi_Read_Write_E ReadWrite, Spi_Trans_Size_E Trans_size, INT32U Addr, INT64U SPI_Data);
void        ECR8660_write(Spi_Reg_Ext_Int_E  ECR8660_mode,  INT32U ECR8660_Addr, INT32U ECR8660_Data);
INT64U      ECR8660_Read(Spi_Reg_Ext_Int_E ECR8660_mode,  INT32U ECR8660_Addr, INT32U ECR8660_Len);
void        ECR8660_Set_List(Ecr8660_Reg_List_T *List);
INT32U      ECR8660_Buff_Write(Spi_Trans_Size_E Trans_size, INT32U Addr, void *Buff, INT32U Size);
/********************************************************************************
*             Global variable  eg:extern unsigned int Global_variable;
********************************************************************************/
#ifdef __cplusplus
#endif
#endif /* __ECR8660_SPI_H */

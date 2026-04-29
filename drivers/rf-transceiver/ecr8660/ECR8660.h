#ifndef __ECR8660_H
#define __ECR8660_H
#ifdef  __cplusplus
extern "C" {
#endif
/********************************************************************************
*                 Include file  eg:#include "header_file.h"
********************************************************************************/
#include    "Bit_macro.h"
#include    "ECR8660_SPI.h"
#include    <stdio.h>
/********************************************************************************
*             enum/struct/union/ Definition  eg:typedef struct{...}
********************************************************************************/
typedef enum _ecr8660_log_level_e_{
    LOG_MIN_LEVEL   ,
    ERROR_LOG       ,       // 报错
    ENGLISH_LOG     ,       // 阶段性的提示(英)
    CHINESE_LOG     ,       // 阶段性的提示(中)
    PRINTF_LOG      ,       // 回显
    LOG_MAX_LEVEL   ,
}Ecr8660_Log_Level_E;
/********************************************************************************
*               Macro Definition  eg:#define Macro Definition
********************************************************************************/
//          ECR8660_MEM_ADDR
#define     MCU_LOG_ADDR_HAND                   (0x20000000)
#define     MCU_VCO_ADDR_HAND                   (0x20001000)
#define     MCU_VER_ADDR_HAND                   (0x20002000)
#define     MCU_API_ADDR_HAND                   (0x20004000)
#define     MCU_REG_ADDR_HAND                   (0x20005000)
#define     MCU_EFU_ADDR_HAND                   (0x20006000)
#define     MCU_CFG_ADDR_HAND                   (0x20006800)
#define     MCU_CFG_ADDR_REF                    MCU_CFG_ADDR_HAND
#define     MCU_LOG_ADDR_LIST(x)                (MCU_LOG_ADDR_HAND + ((x) << 2))
#define     MCU_LOG_ADDR_WORK                   (MCU_LOG_ADDR_HAND + 0x4)
#define     MCU_VER_ADDR_VER0                   (MCU_VER_ADDR_HAND)
#define     MCU_VER_ADDR_VER1                   (MCU_VER_ADDR_HAND + 0x04)
#define     MCU_VER_ADDR_ERROR0                 (MCU_VER_ADDR_HAND + 0x08)
#define     MCU_VER_ADDR_ERROR1                 (MCU_VER_ADDR_HAND + 0x0C)
#define     MCU_EFU_ADDR_ID0                    (MCU_EFU_ADDR_HAND)
#define     MCU_EFU_ADDR_ID1                    (MCU_EFU_ADDR_HAND + 0x4)
#define     ECR8660_CODE_ADDR_HEAD              (0x00000000)        // ECR8660 的代码段起始地址
#define     ECR8660_CODE_MAX_SIZE               BUFF_64K            // ECR8660 的代码最大Size
#define     MCU_CODE_SIZE_ADDR                  (0x00000010)        // ECR8660 的复位寄存器
#define     SPI_REG_SOC_RESET                   (0xFFC)             // ECR8660 的复位寄存器
#define     SPI_REG_MCU_RESET                   (0x7FC)             // ECR8660 内 MCU 的复位寄存器
#define     SPI_REG_BASE_ADDR                   (0xFF0)             // 基地址寄存器
#define     SPI_REG_DCXO_EN_L                   (0xF08)             // DCXO 的配置寄存器
#define     SPI_REG_DCXO_EN_H                   (0xF04)             // DCXO 的配置寄存器
#define     SPI_REG_SPI_PADPC                   (0xF00)             // SPI  驱动能力设置
#define     SPI_REG_LDO0_EN_L                   (0xF1C)             // LDO  的配置寄存器
#define     SPI_REG_LDO0_EN_H                   (0xF20)             // LDO  的配置寄存器
#define     SPI_REG_SOC_ISO_E                   (0xF40)             // 配置电源隔离的寄存器

#define     ECR8660_INIT_OVER                   (0x00000123)        // 初始化完成的 LOG

/********************************************************************************
*           Function Definitions  eg:void Function_Definitions(void);
********************************************************************************/
userErrorStatus ECR8660_INIT(void);
userErrorStatus ECR8660_CodeDownload();
void        ECR8660_CodeUpData(INT32U Code_Addr, INT32U Code_Size);
/********************************************************************************
*            Global variable  eg:extern unsigned int Global_variable;
********************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* __ECR8660_H */
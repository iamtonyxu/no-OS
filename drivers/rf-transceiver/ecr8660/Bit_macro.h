/********************************************************************************
 * @ Copyright    : Transa Semi.
 * @
 * @ File Name    : Bit_macro.h
 * @ File Mark    : 
 * @ Author       : GuoHao
 * @ E-Mail       : 844396800@qq.com
 * @ Website      : www.GuoHowe.com
 * @ Version      : v1.0
 * @ Date         : 2021-04-14 09:41:27
 * @ Description  : 
 * @ History:  
 * @       Version      : v1.0
 * @       LastEditors  : GuoHao
 * @       LastEditTime : 2021-06-08 09:40:10
 * @       Modification :  
********************************************************************************/

#ifndef __BIT_MACRO_H
#define __BIT_MACRO_H

#include <stdbool.h>

//#ifdef  __cplusplus
//extern "C" {
//#endif

/********************************************************************************
*                 Include file  eg:#include "header_file.h"
********************************************************************************/
//#include    "User_Includes.h"

/********************************************************************************
*             enum/struct/union/ Definition  eg:typedef struct{...}
********************************************************************************/

typedef     void                            VOID;
typedef     char                            CHAR;                   /*  8-bit character            */
typedef     unsigned    char                BOOLEAN;                /*  8-bit boolean or logical   */
typedef     unsigned    char                INT8U;                  /*  8-bit unsigned integer     */
typedef     signed      char                INT8S;                  /*  8-bit   signed integer     */
typedef     unsigned    short               INT16U;                 /* 16-bit unsigned integer     */
typedef     signed      short               INT16S;                 /* 16-bit   signed integer     */
typedef     unsigned    int                 INT32U;                 /* 32-bit unsigned integer     */
typedef     signed      int                 INT32S;                 /* 32-bit   signed integer     */
typedef     unsigned    long long           INT64U;                 /* 64-bit unsigned integer     */
typedef     signed      long long           INT64S;                 /* 64-bit   signed integer     */

typedef     float                           FP32;                   /* 32-bit floating point       */
typedef     double                          FP64;                   /* 64-bit floating point       */

/* ------------------------ Exported_types ---------------------------------- */

typedef     enum        {rese_zero  =  0                        }   reserved_e;
//typedef     enum        {false      =  0,   true   =  !false    }   bool;
typedef     enum        {USERRESET      =  0,   USERSET    =  !USERRESET    }   userFlagStatus, userITStatus;
typedef     enum        {USERDISABLE    =  0,   USERENABLE =  !USERDISABLE  }   userFunctionalState;
typedef     enum        {USERSUCCESS    =  0,   USERERROR  =  !USERSUCCESS  }   userErrorStatus;
typedef     enum        {off        =  0,   on     =  !off      }   ON_OFF;

/* -------------------------------------------------------------------------*/

typedef union _whb_to_16bit_u_{
    INT8U       Byte[2];
    INT16U      HalfWord;
}Whb_To_16Bit_U;

typedef union _whb_to_32bit_u_{
    INT32U      Word;
    INT16U      HalfWord[2];
    INT8U       Byte[4];
}Whb_To_32Bit_U;

typedef union _whb_to_64bit_u_{
    INT64U      LONG;
    INT32U      Word[2];
    INT16U      HalfWord[4];
    INT8U       Byte[8];
}Whb_To_64Bit_U;

/********************************************************************************
*               Macro Definition  eg:#define Macro Definition
********************************************************************************/
#define     TRUE                            (true)
#define     FALSE                           (false)
#define     NULL                            ((void *)0U)

#define     BYTE(x)                         (x)
#define     BYTE_2_BIT(x)                   ((x) << 0x3)
#define     BUFF_128BYTE                    (128)
#define     BUFF_1K                         (0x400)
#define     BUFF_16K                        (0x4000)
#define     BUFF_64K                        (0x10000)

#define     KHZ(x)                          (x)
#define     MHZ(x)                          (x##000)
#define     GHZ(x)                          (x##000000)

#define     BIT_00                          (0x00000001UL)
#define     BIT_01                          (0x00000002UL)
#define     BIT_02                          (0x00000004UL)
#define     BIT_03                          (0x00000008UL)
#define     BIT_04                          (0x00000010UL)
#define     BIT_05                          (0x00000020UL)
#define     BIT_06                          (0x00000040UL)
#define     BIT_07                          (0x00000080UL)
#define     BIT_08                          (0x00000100UL)
#define     BIT_09                          (0x00000200UL)
#define     BIT_10                          (0x00000400UL)
#define     BIT_11                          (0x00000800UL)
#define     BIT_12                          (0x00001000UL)
#define     BIT_13                          (0x00002000UL)
#define     BIT_14                          (0x00004000UL)
#define     BIT_15                          (0x00008000UL)
#define     BIT_16                          (0x00010000UL)
#define     BIT_17                          (0x00020000UL)
#define     BIT_18                          (0x00040000UL)
#define     BIT_19                          (0x00080000UL)
#define     BIT_20                          (0x00100000UL)
#define     BIT_21                          (0x00200000UL)
#define     BIT_22                          (0x00400000UL)
#define     BIT_23                          (0x00800000UL)
#define     BIT_24                          (0x01000000UL)
#define     BIT_25                          (0x02000000UL)
#define     BIT_26                          (0x04000000UL)
#define     BIT_27                          (0x08000000UL)
#define     BIT_28                          (0x10000000UL)
#define     BIT_29                          (0x20000000UL)
#define     BIT_30                          (0x40000000UL)
#define     BIT_31                          (0x80000000UL)

#define     LEN_01_BIT                      (0x00000001UL)
#define     LEN_02_BIT                      (0x00000003UL)
#define     LEN_03_BIT                      (0x00000007UL)
#define     LEN_04_BIT                      (0x0000000FUL)
#define     LEN_05_BIT                      (0x0000001FUL)
#define     LEN_06_BIT                      (0x0000003FUL)
#define     LEN_07_BIT                      (0x0000007FUL)
#define     LEN_08_BIT                      (0x000000FFUL)
#define     LEN_09_BIT                      (0x000001FFUL)
#define     LEN_10_BIT                      (0x000003FFUL)
#define     LEN_11_BIT                      (0x000007FFUL)
#define     LEN_12_BIT                      (0x00000FFFUL)
#define     LEN_13_BIT                      (0x00001FFFUL)
#define     LEN_14_BIT                      (0x00003FFFUL)
#define     LEN_15_BIT                      (0x00007FFFUL)
#define     LEN_16_BIT                      (0x0000FFFFUL)
#define     LEN_17_BIT                      (0x0001FFFFUL)
#define     LEN_18_BIT                      (0x0003FFFFUL)
#define     LEN_19_BIT                      (0x0007FFFFUL)
#define     LEN_20_BIT                      (0x000FFFFFUL)
#define     LEN_21_BIT                      (0x001FFFFFUL)
#define     LEN_22_BIT                      (0x003FFFFFUL)
#define     LEN_23_BIT                      (0x007FFFFFUL)
#define     LEN_24_BIT                      (0x00FFFFFFUL)
#define     LEN_25_BIT                      (0x01FFFFFFUL)
#define     LEN_26_BIT                      (0x03FFFFFFUL)
#define     LEN_27_BIT                      (0x07FFFFFFUL)
#define     LEN_28_BIT                      (0x0FFFFFFFUL)
#define     LEN_29_BIT                      (0x1FFFFFFFUL)
#define     LEN_30_BIT                      (0x3FFFFFFFUL)
#define     LEN_31_BIT                      (0x7FFFFFFFUL)
#define     LEN_32_BIT                      (0xFFFFFFFFUL)

// 杈撳嚭鎸囧畾闀垮害鐨� 1 鏈�澶� 32 Bit
#define     LEN_XX_BIT(x)                   (LEN_32_BIT >> (32 - (x)))

#if 0
#define     SET_REG(REG, VAL)               ((REG) =   (VAL))
#define     GET_REG(variate, len, exc)      (((variate) >> (exc)) & (len))
#endif

#define     SET_BIT(REG, BIT)               ((REG) |=  (BIT))
#define     CLE_BIT(REG, BIT)               ((REG) &= ~(BIT)) // CLEAR_BIT
#define     READ_BIT(REG, BIT)              ((REG) &   (BIT))
#define     READ_REG(REG)                   (REG)
#define     CLE_REG(REG)                    ((REG) =   (0x0)) // CLEAR_REG
#define     BIT_NUM(BIT)                    ((1U)  <<  (BIT))

#define     SET_CLE_BIT(OFFORON, REG, BIT)  ((REG) = ((OFFORON) ? (REG)|(BIT) : ((REG) &~ (BIT))))

#define     SET_REG_BIT(variate, len, exc, valu)    ((variate) = ((variate) & ~((len) << (exc))) | (((valu) & (len)) << (exc)))
#define     GET_VAL_BIT(valu, move, len)    (((valu)  >>  (move) )  &  (len))
#define     SET_VAL_BIT(valu, move, len)    (((valu)  &   (len)  )  << (move))
#define     BUF_LEN(buffer)                 (sizeof(buffer) / sizeof(buffer[0]))

#define     REG(x)                          (*(volatile INT32U *)(x))
#define     REG64(x)                        (*(volatile INT64U *)(x))
#define     V2R_WRITE(x,y)                  (REG(x) = (y))

#define     CS_LOG_ARG(x,y)                 (((volatile INT32U *)(x))[y])
#define     CS_ARG_write(x,y)               (*(volatile INT32U *)((x) + ((y) << 2)))

// 鍒ゆ柇濂囧伓锛� 杩斿洖 1 涓哄鏁帮紝 0 涓哄伓鏁�
#define     JUDGE_PARITY(x)                 ((x) & 0x1)
// 璁＄畻鏈�楂� Bit  [0:0  1:1  2:2  3:2  4:3  8:4]
#define     COMPUTED_MAXIMUM_BIT(x)         ((sizeof(INT32U) << 3) - (INT32U)__CLZ(x))
// LOG 浠� 2 涓哄簳 [0:0  1:0  2:1  3:1  4:2  8:3]
#define     LOG2(x)                         ((x) ? (COMPUTED_MAXIMUM_BIT(x) - 1) : (0))

#define     TURE_ASSIG(x,y)                 ((x) = (y) ? (y) : (x))
#define     TURE_RETUR(x,y)                 ((y) ? (y) : (x))

/********************************************************************************
*           Function Definitions  eg:void Function_Definitions(void);
********************************************************************************/

/********************************************************************************
*            Global variable  eg:extern unsigned int Global_variable;
********************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __BIT_MACRO_H */

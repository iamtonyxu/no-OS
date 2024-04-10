#include    "ECR8660.h"
#include    "bin2.h"

#if 1
// ECR8660 鐨勪笂鐢靛簭鍒�
Ecr8660_Reg_List_T  PwrUp_ECR8660_EN[] = {
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_L    ,  0x83                 ,  "璁剧疆 DCXO 鍙傛暟" },  // 璁剧疆 DCXO 鍙傛暟
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  0x0                  ,  "鍏ㄥ眬澶嶄綅"       },  // 鎷夎捣鍏ㄥ眬澶嶄綅
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  0x1                  ,  NULL             },  // 閲婃斁鍏ㄥ眬澶嶄綅
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SPI_PADPC    ,  0x0                  ,  "璁剧疆 SPI 椹卞姩"  },  // 璁剧疆 SPI 椹卞姩
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_LDO0_EN_L    ,  0x1                  ,  "璁剧疆 LDO"       },  // 璁剧疆 LDO
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_LDO0_EN_H    ,  0xFCD0               ,  NULL             },  // 璁剧疆 LDO
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_H    ,  0x4741               ,  "璁剧疆 DCXO 鍙傛暟" },  // 璁剧疆 DCXO 鍙傛暟
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_ISO_E    ,  0x03                 ,  "璁剧疆闅旂"       },  // 闅旂鎷夎捣
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_ISO_E    ,  0x01                 ,  NULL             },  // 閲婃斁闅旂
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  0x0                  ,  "鍏ㄥ眬澶嶄綅"       },  // 鎷夎捣鍏ㄥ眬澶嶄綅
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  0x1                  ,  NULL             },  // 閲婃斁鍏ㄥ眬澶嶄綅
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_MCU_RESET    ,  0x00                 ,  "MCU 澶嶄綅"       },  // 鎷夎捣 MCU 澶嶄綅
    { ECR8660_SPI_WRIT  ,   SPI_RW_EXTERNAL,   0x002002B0           ,  0x0                  ,  NULL             },
    { ECR8660_SPI_WRIT  ,   SPI_RW_EXTERNAL ,  0x00200494           ,  0x0                  ,  NULL             },
    { ECR8660_SPI_WRIT  ,   SPI_RW_EXTERNAL ,  0x002004D4           ,  0x0                  ,  NULL             },
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_L    ,  0xC3                 ,  "璁剧疆 DCXO 鍙傛暟"  },  // 璁剧疆 DCXO 鍙傛暟
    { ECR8660_SPI_WRIT  ,   SPI_RW_EXTERNAL ,  ECR8660_LIST_END_LOG ,  ECR8660_LIST_END_LOG ,  "缁撳熬鏍囧織"        }
};
#endif

#if 0
// ECR8660 power on seq
Ecr8660_Reg_List_T  PwrUp_ECR8660_EN[] = {
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_L    ,  2                 	,  " " 						},
	{ ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_L    ,  0x83                 ,  "config DCXO" 			},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_L    ,  2                 	,  " " 						},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  2                  	,  " "   					},
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  0x0                  ,  "global reset"			},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  2                  	,  " "   					},
	{ ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  0x1                  ,  "global reset"			},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  2                  	,  " "   					},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SPI_PADPC    ,  2                  	,  "config SPI"  			},
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SPI_PADPC    ,  0x0                  ,  "config SPI"  			},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SPI_PADPC    ,  2                  	,  "config SPI"  			},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_LDO0_EN_L    ,  2                  	,  " "     					},
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_LDO0_EN_L    ,  0x1                  ,  "config LDO"				},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_LDO0_EN_L    ,  2                  	,  " "     					},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_LDO0_EN_H    ,  2               		,  " "     					},
	{ ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_LDO0_EN_H    ,  0xFCD0               ,  "config LDO"     		},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_LDO0_EN_H    ,  2               		,  " "     					},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_H    ,  2               		,  " " 						},
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_H    ,  0x4741               ,  "config DCXO" 			},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_H    ,  2               		,  " " 						},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_ISO_E    ,  2                 	,  " "						},
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_ISO_E    ,  0x03                 ,  "config isolation"		},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_ISO_E    ,  2                 	,  " "						},

	{ ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_ISO_E    ,  0x01                 ,  "config isolation"		},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_ISO_E    ,  2                 	,  " "						},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  2                  	,  " "       				},
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  0x0                  ,  "global reset"	 		},

	{ ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  0x1                  ,  "global reset"	 		},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  2                  	,  " "       				},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_MCU_RESET    ,  2                 	,  " "       				},
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_MCU_RESET    ,  0x00                 ,  "MCU reset"       		},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_MCU_RESET    ,  2                 	,  " "       				},

    { ECR8660_SPI_READ  ,   SPI_RW_EXTERNAL,   0x002002B0           ,  4                  	,  " "						},
    { ECR8660_SPI_WRIT  ,   SPI_RW_EXTERNAL,   0x002002B0           ,  0x0                  ,  "External: 0x002002B0"	},
    { ECR8660_SPI_READ  ,   SPI_RW_EXTERNAL,   0x002002B0           ,  4                  	,  " "						},

    { ECR8660_SPI_READ  ,   SPI_RW_EXTERNAL ,  0x00200494           ,  4                  	,  " "						},
    { ECR8660_SPI_WRIT  ,   SPI_RW_EXTERNAL ,  0x00200494           ,  0x0                  ,  "External: 0x00200494"	},
    { ECR8660_SPI_READ  ,   SPI_RW_EXTERNAL ,  0x00200494           ,  4                  	,  " "						},

    { ECR8660_SPI_READ  ,   SPI_RW_EXTERNAL ,  0x002004D4           ,  4                  	,  " "						},
    { ECR8660_SPI_WRIT  ,   SPI_RW_EXTERNAL ,  0x002004D4           ,  0x0                  ,  "External: 0x002004D4"	},
    { ECR8660_SPI_READ  ,   SPI_RW_EXTERNAL ,  0x002004D4           ,  4                  	,  " "						},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_L    ,  2                 	,  " "  					},
	{ ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_L    ,  0xC3                 ,  "config DCXO"  			},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_L    ,  2                 	,  " "  					},

    { ECR8660_SPI_WRIT  ,   SPI_RW_EXTERNAL ,  ECR8660_LIST_END_LOG ,  ECR8660_LIST_END_LOG ,  "end flag"        		}
};
#endif

/********************************************************************************
 * @ Description : ECR8660_Device_Prepare
 * @ Parameter   : Mode : [ON_OFF]
 *                      on                         !0  // 璇诲啓鍚庢仮澶�   MCU_LOG_ADDR_HAND 鐨勫��
 *                      off                        0   // 璇诲啓鍚庝笉鎭㈠ MCU_LOG_ADDR_HAND 鐨勫�硷紝
 * @ Return      : [userErrorStatus]
 *                      USERSUCCESS  : 妫�娴嬫垚鍔�
 *                      ERROR    : 妫�娴嬪け璐ワ紝骞舵墦鍗� Log
 * @ Note        : 鍒ゆ柇 ECR8660 鐨� Memory 鏄惁姝ｅ父
********************************************************************************/
userErrorStatus ECR8660_Device_Prepare(ON_OFF Mode)
{
    INT64U Data;
    if(Mode)
    {
        Data = ECR8660_Read(SPI_RW_EXTERNAL, MCU_LOG_ADDR_HAND, 1);
    }
    ECR8660_write(SPI_RW_EXTERNAL, MCU_LOG_ADDR_HAND, ECR8660_SPI_REG_LOG);
    if(ECR8660_SPI_REG_LOG != ECR8660_Read(SPI_RW_EXTERNAL, MCU_LOG_ADDR_HAND, 1))
    {
        return USERERROR;
    }
    if(Mode)
    {
        ECR8660_write(SPI_RW_EXTERNAL, MCU_LOG_ADDR_HAND, Data);
    }
    return USERSUCCESS;
}

/********************************************************************************
 * @ Description : ECR8660_INIT
 * @ Parameter   : None
 * @ Return      : [userErrorStatus]
 *                      USERSUCCESS  : 鍒濆鍖栨垚鍔�
 *                      ERROR    : 鍒濆鍖栧け璐ワ紝骞舵墦鍗� Log
 * @ Note        : 瀵� ECR8660 杩涜鍒濆鍖栫殑鎿嶄綔搴忓垪
********************************************************************************/
userErrorStatus ECR8660_INIT(void)
{
    ECR8660_Set_List(PwrUp_ECR8660_EN);
    if(USERERROR == ECR8660_Device_Prepare(on))
    {
        return USERERROR;
    }
    return USERSUCCESS;
}

/********************************************************************************
 * @ Description : ECR8660_CodeDownload
 * @ Parameter   : Flash_Addr_Head : 瀛樻斁浠ｇ爜鐨� Flash 棣栧湴鍧�
 * @ Return      : [userErrorStatus]
 *                      USERSUCCESS  : 涓嬭浇鎴愬姛
 *                      ERROR    : 涓嬭浇澶辫触锛屽苟鎵撳嵃 Log
 * @ Note        : 瀵� ECR8660 杩涜浠ｇ爜涓嬭浇鎿嶄綔
********************************************************************************/
userErrorStatus ECR8660_CodeDownload()
{
    INT32U  CodeSize, Addr_Offset,binWriteData,testRead;
    CodeSize = *(INT32U *)(((INT32U)&bin_text) + ECR8660_SIZE_ADDR);
    if((CodeSize < BUFF_1K) || (CodeSize > BUFF_64K))// Bin鏂囦欢澶у皬闄愬埗涓� 1K 鍒� 64K
    {     
        return USERERROR;
    }
    for(INT32U Addr_Offset = 0;Addr_Offset <= CodeSize + 4;Addr_Offset += 4)//姣忔鍐�4瀛楄妭bin鏂囦欢浜岃繘鍒舵暟鎹�
    { 
       binWriteData = *(INT64U *)(bin_text + Addr_Offset);
       ECR8660_read_write(ECR8660_SPI_WRIT, TRANS_FOURBYTE_DATA, ECR8660_CODE_ADDR_HEAD + Addr_Offset, binWriteData); 
    }		
    ECR8660_read_write(ECR8660_SPI_WRIT, TRANS_INTERNAL_REGISTER, SPI_REG_MCU_RESET, USERRESET);
    Delayms(10);//寤舵椂100ms
    ECR8660_read_write(ECR8660_SPI_WRIT, TRANS_INTERNAL_REGISTER, SPI_REG_MCU_RESET, USERSET);
    Delayms(5000);//寤舵椂5s

    testRead = ECR8660_Read(SPI_RW_EXTERNAL, MCU_LOG_ADDR_WORK, 1);//璇嗗埆鏄惁涓嬭浇bin鏂囦欢鎴愬姛
	if(testRead != ECR8660_INIT_OVER)
	{
		 return USERERROR;
	}
    return USERSUCCESS;
}

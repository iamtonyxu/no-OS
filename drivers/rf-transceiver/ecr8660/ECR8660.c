#include    "ECR8660.h"
#include    "bin2.h"

#if 1
// ECR8660 閻ㄥ嫪绗傞悽闈涚碍閸掞拷
Ecr8660_Reg_List_T  PwrUp_ECR8660_EN[] = {
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_L    ,  0x83                 ,  "鐠佸墽鐤� DCXO 閸欏倹鏆�" },  // 鐠佸墽鐤� DCXO 閸欏倹鏆�
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  0x0                  ,  "閸忋劌鐪径宥勭秴"       },  // 閹峰鎹ｉ崗銊ョ湰婢跺秳缍�
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  0x1                  ,  NULL             },  // 闁插﹥鏂侀崗銊ョ湰婢跺秳缍�
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SPI_PADPC    ,  0x0                  ,  "鐠佸墽鐤� SPI 妞瑰崬濮�"  },  // 鐠佸墽鐤� SPI 妞瑰崬濮�
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_LDO0_EN_L    ,  0x1                  ,  "鐠佸墽鐤� LDO"       },  // 鐠佸墽鐤� LDO
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_LDO0_EN_H    ,  0xFCD0               ,  NULL             },  // 鐠佸墽鐤� LDO
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_H    ,  0x4741               ,  "鐠佸墽鐤� DCXO 閸欏倹鏆�" },  // 鐠佸墽鐤� DCXO 閸欏倹鏆�
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_ISO_E    ,  0x03                 ,  "鐠佸墽鐤嗛梾鏃傤瀲"       },  // 闂呮梻顬囬幏澶庢崳
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_ISO_E    ,  0x01                 ,  NULL             },  // 闁插﹥鏂侀梾鏃傤瀲
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  0x0                  ,  "閸忋劌鐪径宥勭秴"       },  // 閹峰鎹ｉ崗銊ョ湰婢跺秳缍�
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  0x1                  ,  NULL             },  // 闁插﹥鏂侀崗銊ョ湰婢跺秳缍�
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_MCU_RESET    ,  0x00                 ,  "MCU 婢跺秳缍�"       },  // 閹峰鎹� MCU 婢跺秳缍�
    { ECR8660_SPI_WRIT  ,   SPI_RW_EXTERNAL,   0x002002B0           ,  0x0                  ,  NULL             },
    { ECR8660_SPI_WRIT  ,   SPI_RW_EXTERNAL ,  0x00200494           ,  0x0                  ,  NULL             },
    { ECR8660_SPI_WRIT  ,   SPI_RW_EXTERNAL ,  0x002004D4           ,  0x0                  ,  NULL             },
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_L    ,  0xC3                 ,  "鐠佸墽鐤� DCXO 閸欏倹鏆�"  },  // 鐠佸墽鐤� DCXO 閸欏倹鏆�
    { ECR8660_SPI_WRIT  ,   SPI_RW_EXTERNAL ,  ECR8660_LIST_END_LOG ,  ECR8660_LIST_END_LOG ,  "缂佹挸鐔弽鍥х箶"        }
};
#endif

#if 0
// ECR8660 power on seq
Ecr8660_Reg_List_T  PwrUp_ECR8660_EN[] = {
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_L    ,  1                 	,  " " 						},
	{ ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_L    ,  0x83                 ,  "config DCXO" 			},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_L    ,  1                 	,  " " 						},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  1                  	,  " "   					},
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  0x0                  ,  "global reset"			},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  1                  	,  " "   					},
	{ ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  0x1                  ,  "global reset"			},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  1                  	,  " "   					},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SPI_PADPC    ,  1                  	,  "config SPI"  			},
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SPI_PADPC    ,  0x0                  ,  "config SPI"  			},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SPI_PADPC    ,  1                  	,  "config SPI"  			},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_LDO0_EN_L    ,  1                  	,  " "     					},
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_LDO0_EN_L    ,  0x1                  ,  "config LDO"				},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_LDO0_EN_L    ,  1                  	,  " "     					},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_LDO0_EN_H    ,  1               		,  " "     					},
	{ ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_LDO0_EN_H    ,  0xFCD0               ,  "config LDO"     		},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_LDO0_EN_H    ,  1               		,  " "     					},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_H    ,  1               		,  " " 						},
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_H    ,  0x4741               ,  "config DCXO" 			},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_H    ,  1               		,  " " 						},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_ISO_E    ,  1                 	,  " "						},
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_ISO_E    ,  0x03                 ,  "config isolation"		},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_ISO_E    ,  1                 	,  " "						},

	{ ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_ISO_E    ,  0x01                 ,  "config isolation"		},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_ISO_E    ,  1                 	,  " "						},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  1                  	,  " "       				},
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  0x0                  ,  "global reset"	 		},

	{ ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  0x1                  ,  "global reset"	 		},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_SOC_RESET    ,  1                  	,  " "       				},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_MCU_RESET    ,  1                 	,  " "       				},
    { ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_MCU_RESET    ,  0x00                 ,  "MCU reset"       		},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_MCU_RESET    ,  1                 	,  " "       				},

    { ECR8660_SPI_READ  ,   SPI_RW_EXTERNAL,   0x002002B0           ,  1                  	,  " "						},
    { ECR8660_SPI_WRIT  ,   SPI_RW_EXTERNAL,   0x002002B0           ,  0x0                  ,  "External: 0x002002B0"	},
    { ECR8660_SPI_READ  ,   SPI_RW_EXTERNAL,   0x002002B0           ,  1                  	,  " "						},

    { ECR8660_SPI_READ  ,   SPI_RW_EXTERNAL ,  0x00200494           ,  1                  	,  " "						},
    { ECR8660_SPI_WRIT  ,   SPI_RW_EXTERNAL ,  0x00200494           ,  0x0                  ,  "External: 0x00200494"	},
    { ECR8660_SPI_READ  ,   SPI_RW_EXTERNAL ,  0x00200494           ,  1                  	,  " "						},

    { ECR8660_SPI_READ  ,   SPI_RW_EXTERNAL ,  0x002004D4           ,  1                  	,  " "						},
    { ECR8660_SPI_WRIT  ,   SPI_RW_EXTERNAL ,  0x002004D4           ,  0x0                  ,  "External: 0x002004D4"	},
    { ECR8660_SPI_READ  ,   SPI_RW_EXTERNAL ,  0x002004D4           ,  1                  	,  " "						},

    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_L    ,  1                 	,  " "  					},
	{ ECR8660_SPI_WRIT  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_L    ,  0xC3                 ,  "config DCXO"  			},
    { ECR8660_SPI_READ  ,   SPI_RW_INTERIOR ,  SPI_REG_DCXO_EN_L    ,  1                 	,  " "  					},

    { ECR8660_SPI_WRIT  ,   SPI_RW_EXTERNAL ,  ECR8660_LIST_END_LOG ,  ECR8660_LIST_END_LOG ,  "end flag"        		}
};
#endif

/********************************************************************************
 * @ Description : ECR8660_Device_Prepare
 * @ Parameter   : Mode : [ON_OFF]
 *                      on                         !0  // 鐠囪鍟撻崥搴划婢讹拷   MCU_LOG_ADDR_HAND 閻ㄥ嫬锟斤拷
 *                      off                        0   // 鐠囪鍟撻崥搴濈瑝閹垹顦� MCU_LOG_ADDR_HAND 閻ㄥ嫬锟界》绱�
 * @ Return      : [userErrorStatus]
 *                      USERSUCCESS  : 濡拷濞村鍨氶崝锟�
 *                      ERROR    : 濡拷濞村銇戠拹銉礉楠炶埖澧﹂崡锟� Log
 * @ Note        : 閸掋倖鏌� ECR8660 閻拷 Memory 閺勵垰鎯佸锝呯埗
********************************************************************************/
userErrorStatus ECR8660_Device_Prepare(ON_OFF Mode)
{
	uint32_t Data;
	uint32_t data_temp = 0 ;
    if(Mode)
    {
        Data = ECR8660_Read(SPI_RW_EXTERNAL, MCU_LOG_ADDR_HAND, 1);
        Delayms(1000);
    }
    ECR8660_write(SPI_RW_EXTERNAL, MCU_LOG_ADDR_HAND, ECR8660_SPI_REG_LOG);
    Delayms(1000);
//    if(ECR8660_SPI_REG_LOG != ECR8660_Read(SPI_RW_EXTERNAL, MCU_LOG_ADDR_HAND, ECR8660_SPI_BYTE(USER_ECR8660_SPI_SIZE)))
    data_temp = ECR8660_Read(SPI_RW_EXTERNAL, MCU_LOG_ADDR_HAND, 1);
    if(ECR8660_SPI_REG_LOG != data_temp)
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
 *                      USERSUCCESS  : 閸掓繂顫愰崠鏍ㄥ灇閸旓拷
 *                      ERROR    : 閸掓繂顫愰崠鏍с亼鐠愩儻绱濋獮鑸靛ⅵ閸楋拷 Log
 * @ Note        : 鐎碉拷 ECR8660 鏉╂稖顢戦崚婵嗩潗閸栨牜娈戦幙宥勭稊鎼村繐鍨�
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
 * @ Parameter   : Flash_Addr_Head : 鐎涙ɑ鏂佹禒锝囩垳閻拷 Flash 妫ｆ牕婀撮崸锟�
 * @ Return      : [userErrorStatus]
 *                      USERSUCCESS  : 娑撳娴囬幋鎰
 *                      ERROR    : 娑撳娴囨径杈Е閿涘苯鑻熼幍鎾冲祪 Log
 * @ Note        : 鐎碉拷 ECR8660 鏉╂稖顢戞禒锝囩垳娑撳娴囬幙宥勭稊
********************************************************************************/
userErrorStatus ECR8660_CodeDownload()
{
    INT32U  CodeSize, Addr_Offset,binWriteData,testRead;
    CodeSize = *(INT32U *)(((INT32U)&bin_text) + ECR8660_SIZE_ADDR);
    if((CodeSize < BUFF_1K) || (CodeSize > BUFF_64K))// Bin閺傚洣娆㈡径褍鐨梽鎰煑娑擄拷 1K 閸掞拷 64K
    {     
        return USERERROR;
    }
    for(INT32U Addr_Offset = 0;Addr_Offset <= CodeSize + 4;Addr_Offset += 4)//濮ｅ繑顐奸崘锟�4鐎涙濡璪in閺傚洣娆㈡禍宀冪箻閸掕埖鏆熼幑锟�
    { 
       binWriteData = *(INT64U *)(bin_text + Addr_Offset);
       ECR8660_read_write(ECR8660_SPI_WRIT, TRANS_FOURBYTE_DATA, ECR8660_CODE_ADDR_HEAD + Addr_Offset, binWriteData); 
    }		
    ECR8660_read_write(ECR8660_SPI_WRIT, TRANS_INTERNAL_REGISTER, SPI_REG_MCU_RESET, USERRESET);
    Delayms(100);//瀵よ埖妞�100ms
    ECR8660_read_write(ECR8660_SPI_WRIT, TRANS_INTERNAL_REGISTER, SPI_REG_MCU_RESET, USERSET);
    Delayms(6000);//瀵よ埖妞�5s

    testRead = ECR8660_Read(SPI_RW_EXTERNAL, MCU_LOG_ADDR_WORK, 4);//鐠囧棗鍩嗛弰顖氭儊娑撳娴嘼in閺傚洣娆㈤幋鎰
	if(testRead != ECR8660_INIT_OVER)
	{
		 return USERERROR;
	}
    return USERSUCCESS;
}

#include    "ECR8660.h"
#include    "spi.h"

// 鐎规矮绠熼崺鍝勬勾閸э拷閻拷
INT32U  Base_Addr = 0xFFFFFFFF;
// 閻€劋绨幏鐓庡櫨閺冭泛绨惃鍕敄闂傦拷
Ecr8660_Fm_Spi_Time_Series_U    ECR8660_spi_time_series[3];
// 閸╁搫婀撮崸锟介惃鍕Ё鐏忓嫯銆�
INT64U spidata64 = 0;
Ecr8660_Fm_Base_Addr_Dict_T     ECR8660_BaseAddr_Dict[] = 
{
    {0x00000,   0x00},
    {0x00001,   0x01},
    {0x00002,   0x02},
    {0x00003,   0x03},
    {0x00004,   0x04},
    {0x00005,   0x05},
    {0x00006,   0x06},
    {0x00007,   0x07},
    {0x00008,   0x08},
    {0x00009,   0x09},
    {0x0000A,   0x0A},
    {0x0000B,   0x0B},
    {0x0000C,   0x0C},
    {0x0000D,   0x0D},
    {0x0000E,   0x0E},
    {0x0000F,   0x0F},
    {0x20000,   0x10},
    {0x20001,   0x11},
    {0x20002,   0x12},
    {0x20003,   0x13},
    {0x20004,   0x14},
    {0x20005,   0x15},
    {0x20006,   0x16},
    {0x20007,   0x17},
    {0x00200,   0x18},
    {0x00201,   0x19},
    {0xFFFFF,   0xFF},
};

/********************************************************************************
 * @ Description : ECR8660_Base_Address_Dict_Find
 * @ Parameter   : Addr : 闂囷拷鐟曚焦鐓＄拠銏㈡畱閸︽澘娼�
 * @ Return      : None
 * @ Note        : 閸︺劌鐔�閸︽澘娼冪�涙鍚�(ECR8660_BaseAddr_Dict)閸愬懏鐓＄拠銏犳勾閸э拷Addr閻ㄥ嫬鐔�閸︽澘娼冮弽鍥у娇
********************************************************************************/
INT8U   ECR8660_Base_Address_Dict_Find(INT32U Addr)
{
    INT16U i;
    for(i = 0; (BASE_ADDR_ERR != ECR8660_BaseAddr_Dict[i].Base_Addr) && (ECR8660_BaseAddr_Dict[i].Base_Addr != Addr) ; i++ );
    return ECR8660_BaseAddr_Dict[i].Base_Log;
}

/********************************************************************************
 * @ Description : ECR8660_SPI_WR
 * @ Parameter   : ReadWrite   : [Spi_Read_Write_E]
 *                      ECR8660_SPI_READ           0x0 // SPI鐠囩粯膩瀵拷
 *                      ECR8660_SPI_WRIT           0x1 // SPI閸愭瑦膩瀵拷
 *                 Trans_size  : [Spi_Trans_Size_E]
 *                      TRANS_ONEBYTE_DATA         0x0 // 娴肩姾绶�1娑撶寵yte閺佺増宓�
 *                      TRANS_TWOBYTE_DATA         0x1 // 娴肩姾绶�2娑撶寵yte閺佺増宓�
 *                      TRANS_FOURBYTE_DATA        0x3 // 娴肩姾绶�4娑撶寵yte閺佺増宓�
 *                      TRANS_INTERNAL_REGISTER    0x6 // 娴ｆ粈璐烻PI閸愬懘鍎寸�靛嫬鐡ㄩ崳銊嚢閸愭瑧濮搁幀浣瑰瘹缁�锟�
 *                 Addr        : 闂囷拷鐟曚焦鎼锋担婊呮畱閸︽澘娼�
 *                 Data        : 闂囷拷鐟曚礁鍟撻崗銉ф畱閸婏拷
 * @ Return      : None
 * @ Note        : 閻€劋绨幏鐓庡櫨閺冭泛绨敍灞界殺閺冭泛绨幏鐓庡櫨娑擄拷 Data
********************************************************************************/
static  INT8U   ECR8660_SPI_WR(Spi_Read_Write_E Read_write, Spi_Trans_Size_E Trans_size, INT16U Addr, INT64U Data)
{
    Ecr8660_Fm_Spi_Cmd_U            ECR8660_spi_cmd;
    Ecr8660_Fm_Spi_Data_U           ECR8660_spi_data;
    ECR8660_spi_cmd.Bits.Addr       = ECR8660_SPI_ADDR(Addr);
    ECR8660_spi_cmd.Bits.Trans_size = Trans_size;
    ECR8660_spi_cmd.Bits.Read_Write = Read_write;
    ECR8660_spi_time_series[0].Bits.Hight_Half_Word =   ECR8660_spi_cmd.Word;
    ECR8660_spi_data.Clear = 0;
    switch (Trans_size)
    {
        case TRANS_ONEBYTE_DATA:
            ECR8660_spi_data.OneByte.Data                   =   (INT8U)Data;
            ECR8660_spi_time_series[0].Bits.Low_Half_Word   =   ECR8660_spi_data.Bits.High_16;
            return (INT8U)USE_ONE_DATAREG;
        case TRANS_TWOBYTE_DATA:
        case TRANS_INTERNAL_REGISTER:
            ECR8660_spi_data.TwoByte.Data                   =   (INT16U)Data;
            ECR8660_spi_time_series[0].Bits.Low_Half_Word   =   ECR8660_spi_data.Bits.High_16;
            return (INT8U)USE_ONE_DATAREG;
        case TRANS_FOURBYTE_DATA:
            ECR8660_spi_data.FourByte.Data                  =   (INT32U)Data;
            ECR8660_spi_time_series[0].Bits.Low_Half_Word   =   ECR8660_spi_data.Bits.High_16;
            ECR8660_spi_time_series[1].Word                 =   ECR8660_spi_data.Bits.Middle_32;
            return (INT8U)USE_TWO_DATAREG;
        case TRANS_EIGHTBYTE_DATA:
            ECR8660_spi_data.EightByte.Data                 =   Data;
            ECR8660_spi_time_series[0].Bits.Low_Half_Word   =   ECR8660_spi_data.Bits.High_16;
            ECR8660_spi_time_series[1].Word                 =   ECR8660_spi_data.Bits.Middle_32;
            ECR8660_spi_time_series[2].Bits.Hight_Half_Word =   ECR8660_spi_data.Bits.Low_16;
            return (INT8U)USE_THR_DATAREG;
        default:
            return (INT8U)FALSE;
    }
}

/********************************************************************************
 * @ Description : ECR8660_read_write_Basic
 * @ Parameter   : ReadWrite   : [Spi_Read_Write_E]
 *                      ECR8660_SPI_READ           0x0 // SPI鐠囩粯膩瀵拷
 *                      ECR8660_SPI_WRIT           0x1 // SPI閸愭瑦膩瀵拷
 *                 Trans_size  : [Spi_Trans_Size_E]
 *                      TRANS_ONEBYTE_DATA         0x0 // 娴肩姾绶�1娑撶寵yte閺佺増宓�
 *                      TRANS_TWOBYTE_DATA         0x1 // 娴肩姾绶�2娑撶寵yte閺佺増宓�
 *                      TRANS_FOURBYTE_DATA        0x3 // 娴肩姾绶�4娑撶寵yte閺佺増宓�
 *                      TRANS_INTERNAL_REGISTER    0x6 // 娴ｆ粈璐烻PI閸愬懘鍎寸�靛嫬鐡ㄩ崳銊嚢閸愭瑧濮搁幀浣瑰瘹缁�锟�
 *                 Addr        : 闂囷拷鐟曚焦鎼锋担婊呮畱閸︽澘娼�
 *                 Data        : 闂囷拷鐟曚礁鍟撻崗銉ф畱閸婏拷
 * @ Return      : 鐠囪褰�/閸愭瑥鍙� 閻ㄥ嫬锟斤拷
 * @ Note        : SPI 鐠囪鍟撻惃鍕彆閻€劌鍤遍弫锟�
********************************************************************************/
static  INT64U  ECR8660_read_write_Basic(Spi_Read_Write_E ReadWrite, Spi_Trans_Size_E Trans_size, INT32U Addr, INT64U SPI_Data)
{
    Global_Spi_Trans_U      data = {0};
    // 鐠囥儵鍎撮崚鍡曡礋鐎碉拷 SPI master 閻ㄥ嫭甯堕崚璁圭礉鎼存洖鐪伴棁锟介弽瑙勫祦鐎圭偤妾幆鍛枌娣囶喗鏁�
    // 闁板秶鐤� 閸欐垿锟斤拷 閻ㄥ嫭鏆熼幑顔炬畱闂�鍨 : MOSI 娑撳﹤鐨㈡导姘絺闁胶娈戦惇鐔风杽 Bit 闂�鍨(娑撳秷锟藉啳妾婚崡鐘辩秴)      // 閸愬懘鍎撮崘娆惸佸蹇ョ窗2Byte_CMD + 2ByteWriteData  婢舵牠鍎撮崘娆惸佸蹇ョ窗2Byte_CMD + 閸愭瑦鏆熼幑顔炬畱闂�鍨  鐠囩粯膩瀵骏绱�2Byte_CMD
    data.Bits.writecmdlen   =  (ECR8660_SPI_WRIT == ReadWrite) ? ((TRANS_INTERNAL_REGISTER == Trans_size) ? BYTE_2_BIT(4) : BYTE_2_BIT(Trans_size + 3)) : BYTE_2_BIT(2);
    // 闁板秶鐤� 閹恒儲鏁� 閻ㄥ嫭鏆熼幑顔炬畱闂�鍨 : MISO 娑撳﹤鐨㈡导姘复閺�鍓佹畱閻喎鐤� Bit 闂�鍨(娑撳秷锟藉啳妾荤粔璁崇秴)      // 閸愬懘鍎寸拠缁樐佸蹇ョ窗2ByteReadData               婢舵牠鍎寸拠缁樐佸蹇ョ窗鐠囩粯鏆熼幑顔炬畱闂�鍨              閸愭瑦膩瀵骏绱�0
    data.Bits.readdatalen   =  (ECR8660_SPI_READ == ReadWrite) ? ((TRANS_INTERNAL_REGISTER == Trans_size) ? BYTE_2_BIT(2) : BYTE_2_BIT(Trans_size + 1)) : BYTE_2_BIT(0);
    // Clk 閻ㄥ嫭锟界粯妞傞柦鐔告殶娑擄拷 data.Bits.cmdlen + data.Bits.datalen
    // 閹风厧鍣鹃弮璺虹碍閿涘苯鐨� ECR8660 閹碉拷闂囷拷鐟曚胶娈� SPI 閺冭泛绨幏鐓庡櫨娑擄拷 Data 閺佺増宓�
    ECR8660_SPI_WR(ReadWrite, Trans_size, Addr, SPI_Data);
    int len = 0;  
    if(Trans_size == TRANS_FOURBYTE_DATA)
    {
        len = 64;
    }
    else
    {
        len = 32;
    }
    return spi_readwrite(ECR8660_spi_time_series[0].Word,ECR8660_spi_time_series[1].Word,len);
}

/********************************************************************************
 * @ Description : ECR8660_Base_Address
 * @ Parameter   : Addr 閸︽澘娼�
 * @ Return      : None
 * @ Note        : 鐎靛綊銆夋径鏍х槑鐎涙ê娅掔拋鍧楁６閺冭绱濇潻娑滎攽妞ら潧鍨忛幑銏㈡畱閸戣姤鏆�
********************************************************************************/
static  void    ECR8660_Base_Address(INT32U Addr)
{
    ECR8660_read_write_Basic(ECR8660_SPI_WRIT, TRANS_INTERNAL_REGISTER, SPI_REG_BASE_ADDR, ECR8660_Base_Address_Dict_Find(ECR8660_SPI_BASE_ADDR(Addr)));
}

/********************************************************************************
 * @ Description : ECR8660_read_write
 * @ Parameter   : ReadWrite   : [Spi_Read_Write_E]
 *                      ECR8660_SPI_READ           0x0 // SPI鐠囩粯膩瀵拷
 *                      ECR8660_SPI_WRIT           0x1 // SPI閸愭瑦膩瀵拷
 *                 Trans_size  : [Spi_Trans_Size_E]
 *                      TRANS_ONEBYTE_DATA         0x0 // 娴肩姾绶�1娑撶寵yte閺佺増宓�
 *                      TRANS_TWOBYTE_DATA         0x1 // 娴肩姾绶�2娑撶寵yte閺佺増宓�
 *                      TRANS_FOURBYTE_DATA        0x3 // 娴肩姾绶�4娑撶寵yte閺佺増宓�
 *                      TRANS_INTERNAL_REGISTER    0x6 // 娴ｆ粈璐烻PI閸愬懘鍎寸�靛嫬鐡ㄩ崳銊嚢閸愭瑧濮搁幀浣瑰瘹缁�锟�
 *                 Addr        : 闂囷拷鐟曚焦鎼锋担婊呮畱閸︽澘娼�
 *                 Data        : 闂囷拷鐟曚礁鍟撻崗銉ф畱閸婏拷
 * @ Return      : 鐠囪褰�/閸愭瑥鍙� 閻ㄥ嫬锟斤拷
 * @ Note        : SPI 鐠囪鍟撻惃鍕彆閻€劌鍤遍弫锟�
********************************************************************************/
INT64U  ECR8660_read_write(Spi_Read_Write_E ReadWrite, Spi_Trans_Size_E Trans_size, INT32U Addr, INT64U SPI_Data)
{
    if((TRANS_INTERNAL_REGISTER != Trans_size) && (ECR8660_SPI_BASE_ADDR(Addr) != Base_Addr)){
        Base_Addr = ECR8660_SPI_BASE_ADDR(Addr);
        ECR8660_Base_Address(Addr);
    }
    return  ECR8660_read_write_Basic(ReadWrite, Trans_size, Addr, SPI_Data);
}

/********************************************************************************
 * @ Description : ECR8660_Read
 * @ Parameter   : ECR8660_mode : [Spi_Reg_Ext_Int_E]
 *                      SPI_RW_INTERIOR           0x1 // 閹稿洨銇氭い闈涘敶鐎靛嫬鐡ㄩ崳锟�
 *                      SPI_RW_EXTERNAL           0x0 // 妞ら潧顦荤�靛嫬鐡ㄩ崳锟�
 *                 ECR8660_Addr : 闂囷拷鐟曚焦鎼锋担婊呮畱閸︽澘娼�
 *                 ECR8660_Len  : 闂囷拷鐟曚浇顕伴崣鏍畱閺佺増宓侀梹鍨(Byte)
 * @ Return      : ECR8660_Data : 鐠囪褰囬惃鍕拷锟�
 * @ Note        : 鐎碉拷 ECR8660 鏉╂稖顢戠�靛嫬鐡ㄩ崳锟� "鐠囷拷" 閹垮秳缍�
********************************************************************************/
INT64U  ECR8660_Read(Spi_Reg_Ext_Int_E ECR8660_mode, INT32U ECR8660_Addr, INT32U ECR8660_Len)
{
    Spi_Trans_Size_E    Spi_Datalen;
    INT64U  Data, ECR8660_Data = 0;
    INT32U  i;
    Spi_Datalen =   (SPI_RW_INTERIOR == ECR8660_mode) ? TRANS_INTERNAL_DATA : USER_ECR8660_SPI_SIZE;
    for(i = 0; i < ECR8660_Len; i += ECR8660_SPI_BYTE(Spi_Datalen))
    {
        Data = ECR8660_read_write(ECR8660_SPI_READ, (SPI_RW_INTERIOR == ECR8660_mode) ? TRANS_INTERNAL_REGISTER : Spi_Datalen, ECR8660_Addr + i, 0);
        ECR8660_Data <<= BYTE_2_BIT(ECR8660_SPI_BYTE(Spi_Datalen));
        ECR8660_Data  |= Data;
    }
    return ECR8660_Data;
}

/********************************************************************************
 * @ Description : ECR8660_write
 * @ Parameter   : ECR8660_mode : [Spi_Reg_Ext_Int_E]
 *                      SPI_RW_INTERIOR           0x1 // 閹稿洨銇氭い闈涘敶鐎靛嫬鐡ㄩ崳锟�
 *                      SPI_RW_EXTERNAL           0x0 // 妞ら潧顦荤�靛嫬鐡ㄩ崳锟�
 *                 ECR8660_Addr : 闂囷拷鐟曚焦鎼锋担婊呮畱閸︽澘娼�
 *                 ECR8660_Data : 闂囷拷鐟曚焦鎼锋担婊呮畱閸婏拷
 * @ Return      : None
 * @ Note        : 鐎碉拷 ECR8660 鏉╂稖顢戠�靛嫬鐡ㄩ崳锟� "閸愶拷" 閹垮秳缍� [閺冪姾顔戞担璺ㄦ暏閸濐亞顫掑Ο鈥崇础閿涘苯娼庤ぐ鎺嶇閸栨牔璐熼崘锟�
 *                 32Bit 閻ㄥ嫬鑸板寤�
********************************************************************************/
void    ECR8660_write(Spi_Reg_Ext_Int_E ECR8660_mode, INT32U ECR8660_Addr, INT32U ECR8660_Data)
{
    Spi_Trans_Size_E    Spi_Datalen;
    INT32U  i;
    Spi_Datalen =   (SPI_RW_INTERIOR == ECR8660_mode) ? TRANS_INTERNAL_DATA : USER_ECR8660_SPI_SIZE;
    for(i = 0;i < ((SPI_RW_INTERIOR == ECR8660_mode) ? ECR8660_SPI_BYTE(TRANS_INTERNAL_DATA) : sizeof(ECR8660_Data));
        ECR8660_Data >>= BYTE_2_BIT(ECR8660_SPI_BYTE(Spi_Datalen)), i += ECR8660_SPI_BYTE(Spi_Datalen))
    {
        ECR8660_read_write(ECR8660_SPI_WRIT, (SPI_RW_INTERIOR == ECR8660_mode) ? TRANS_INTERNAL_REGISTER : Spi_Datalen, ECR8660_Addr + i, ECR8660_Data);
    }
}

/********************************************************************************
 * @ Description : ECR8660_Set_List_Basic
 * @ Parameter   : List : 闂囷拷鐟曚礁鍟撻崗銉ф畱鐎靛嫬鐡ㄩ崳銊ョ碍閸掞拷
 * @ Return      : None
 * @ Note        : None
********************************************************************************/
static userErrorStatus ECR8660_Set_List_Basic(Ecr8660_Reg_List_T List)
{
#define DEBUG
    switch(List.readwrite)
    {
        case ECR8660_SPI_WRIT:
            ECR8660_write(List.mode, List.addr, List.data);
#ifdef DEBUG
            printf("%s: ", List.log);
            printf("spi write: addr = 0x%X, data = 0x%X\n", List.addr, List.data);
#endif
            break;
        case ECR8660_SPI_READ:
            int ret = ECR8660_Read(List.mode, List.addr, List.data);
#ifdef DEBUG
            printf("%s: ", List.log);
            printf("spi read: addr = 0x%X, data = 0x%X\n", List.addr, ret);
#endif
            break;
        case ECR8660_SPI_SLEEP:
            //User_Delay_ms(List.data);
            break;
        default:
            return USERERROR;
    }
    return USERSUCCESS;
}

/********************************************************************************
 * @ Description : ECR8660_Set_List
 * @ Parameter   : List : 闂囷拷鐟曚礁鍟撻崗銉ф畱鐎靛嫬鐡ㄩ崳銊ョ碍閸掞拷
 * @ Return      : None
 * @ Note        : None
********************************************************************************/
void    ECR8660_Set_List(Ecr8660_Reg_List_T *List)
{
    INT32U i = 0;
    for(i = 0; List[i].addr != ECR8660_LIST_END_LOG; i++)
    {
        if(ERROR == ECR8660_Set_List_Basic(List[i])) 
        {
            return;
        }
        Delayms(100);//瀵よ埖妞�10ms
    }  
}

/********************************************************************************
 * @ Description : ECR8660_Buff_Write
 * @ Parameter   : Addr : 闂囷拷鐟曚礁鍟撻崗銉ф畱 ECR8660 閸︽澘娼�
 *                 Buff : 闂囷拷鐟曚礁鍟撻崗銉ф畱閺佺増宓侀弫鎵矋妫ｆ牕婀撮崸锟�
 *                 Size : 闂囷拷鐟曚礁鍟撻崗銉ф畱闂�鍨
 *                 Trans_size  : [Spi_Trans_Size_E]
 *                      TRANS_ONEBYTE_DATA         0x0 // 娴肩姾绶�1娑撶寵yte閺佺増宓�
 *                      TRANS_TWOBYTE_DATA         0x1 // 娴肩姾绶�2娑撶寵yte閺佺増宓�
 *                      TRANS_FOURBYTE_DATA        0x3 // 娴肩姾绶�4娑撶寵yte閺佺増宓�
 *                      TRANS_INTERNAL_REGISTER    0x6 // 娴ｆ粈璐烻PI閸愬懘鍎寸�靛嫬鐡ㄩ崳銊嚢閸愭瑧濮搁幀浣瑰瘹缁�锟�
 * @ Return      : Addr : 閺堫偄鐔崷鏉挎絻
 * @ Note        : 鐏忥拷 [Buff] 閸愬懘鏆辨惔锔胯礋 [Size]Byte 閻ㄥ嫭鏆熼幑顕嗙礉闁俺绻� SPI 閻拷 Trans_size 濡�崇础閸愭瑥鍙� ECR8660 閻拷 [Addr] 閸︽澘娼�
********************************************************************************/
INT32U  ECR8660_Buff_Write(Spi_Trans_Size_E Trans_size, INT32U Addr, void *Buff, INT32U Size)
{
    INT32U  i;
    for(i = 0; i < Size; Addr += ECR8660_SPI_BYTE(USER_ECR8660_SPI_SIZE), i += ECR8660_SPI_BYTE(USER_ECR8660_SPI_SIZE) )
    {
        if(ECR8660_SPI_BASE_ADDR(Addr) != Base_Addr)
        {
            Base_Addr = ECR8660_SPI_BASE_ADDR(Addr);
            ECR8660_Base_Address(Addr);
        }
        ECR8660_read_write_Basic(ECR8660_SPI_WRIT, USER_ECR8660_SPI_SIZE, Addr, *((INT64U *)((INT32U)Buff + i)));
    }
    return Addr;
}

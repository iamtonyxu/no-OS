#ifndef  __PLATFORM_H
#define  __PLATFORM_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <time.h>


#include "fr9361_common.h"
#include "utility.h"
#include "no_os_delay.h"

#define  DEVICE_VERSION              ("V23R1")
#define  LUT_VERSION_D1              ("231116-1.11")
#define  LUT_VERSION_D2F1            ("250521-2.12")
#define  LUT_VERSION_GSREDA1B1       ("250421-2.16")
#define  LUT_VERSION_E1              ("231010-4.9")

#define  FPGA_BIT_FILE               ("/home/FPGA_TOP_PIN_4.bit")

#define  VENDOR_SPI_RW           (1)
#define  LINUX_OS                (0)
#define  HAVE_FS                 (0)
#define  RX_MGC_TRACKING         (0)
#define  CMD_DEBGU_ON            (0)

#define  LOAD_LUT_FROM_BIN_FILE  (0)
#define  LUT_BIN_PATH            ("./lut_bin")

#define  CH1_FDD_LUT_ON             (0)
#define  RX1_TX2_FDD_LUT_ON         (0)
#define  RX2_TX1_FDD_LUT_ON         (0)
#define  CH2_FDD_LUT_ON             (0)
#define  CH1_CH2_FDD_LUT_ON         (1)
#define  CH1_TDD_LUT_ON             (0)
#define  CH2_TDD_LUT_ON             (0)
#define  CH1_CH2_TDD_LUT_ON         (0)
#define  HYBRID_FDD_CH1CH2_LUT_ON   (0)
#define  HYBRID_TDD_CH1CH2_LUT_ON   (0)
#define  HYBRID_FDD_CH1_LUT_ON      (0)
#define  HYBRID_FDD_CH2_LUT_ON      (0)

#define FPGA_CTRL_RST_PERL_MASK        UINT8_C(1<<0)
#define FPGA_CTRL_RST_FPGA_MASK        UINT8_C(1<<1)
#define FPGA_CTRL_RST_QEC_MASK         UINT8_C(1<<2)

#define RX_DC_TRACK_MAX_CNT   		(100)

#if LINUX_OS //+++++++++++++++++++++++++++++
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/param.h>
#include <syslog.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <pthread.h>

#define  CHIP_DELAY(time)     usleep(1000*time)
#define  CHIP_UDELAY(time)    usleep(time)
#define  CHIP_SDELAY(time)    sleep(time)

typedef struct task_struct
{
    pthread_t pid;
} task_t;

#define  TASK_HANDLE_VALID(task)  (task.pid)
#define  TASK_HANDLE_INVALID(task) (task.pid = 0)

#else //+++++++++++++++++++++++++++++
#define  CHIP_DELAY(time)   no_os_mdelay(time)
#define  CHIP_UDELAY(time)  no_os_udelay(time)
#define  CHIP_SDELAY(time)  no_os_mdelay(time*1000)

typedef struct task_struct
{
} task_t;

#define  TASK_HANDLE_VALID(task)
#define  TASK_HANDLE_INVALID(task)

#endif //+++++++++++++++++++++++++++++

#define  RX_BW_CAL         (1 << 0)
#define  RX_DC_CAL         (1 << 1)
#define  RX_ADC_CAL        (1 << 2)
#define  TX_BW_CAL         (1 << 3)
#define  TX_DC_CAL         (1 << 4)
#define  TX_DAC_CAL        (1 << 5)
#define  TRX_QEC_CAL       (1 << 6)
#define  SX_TXLO_CAL       (1 << 7)
#define  SX_TRX_CAL        (1 << 8)
#define  CHIP_LDO_CAL      (1 << 9)
#define  LVDS_DELAY_CAL    (1 << 10)
#define  RX_DC_OFFSET_CAL   RX_DC_CAL
#define  SPI_RW_LOG        (1 << 11)

//extern unsigned long g_module_debug;

#if USE_LOG_BUFFER
#include <sys/time.h>
#include <unistd.h>
#include "log.h"
#undef LOG_INFO

#define  LOG_INFO(fmt,...)    //printf(fmt, ##__VA_ARGS__)
#define  LOG_WARN(fmt,...)    \
    do { \
        char ring_buffer_temp[512]; \
        memset(ring_buffer_temp, 0, 512); \
        sprintf(ring_buffer_temp, fmt, ##__VA_ARGS__); \
        system_log_append(ring_buffer_temp); \
    } while(0)

#define  LOG_ERROR(fmt,...)   \
    do { \
        char ring_buffer_temp[512]; \
        memset(ring_buffer_temp, 0, 512); \
        sprintf(ring_buffer_temp, fmt, ##__VA_ARGS__); \
        system_log_append(ring_buffer_temp); \
    } while(0)

#define  LOG_MAIN(fmt,...)    \
    do { \
        char ring_buffer_temp[512]; \
        memset(ring_buffer_temp, 0, 512); \
        sprintf(ring_buffer_temp, fmt, ##__VA_ARGS__); \
        system_log_append(ring_buffer_temp); \
    } while(0)

#define  LOG_MDEBUG(phy, module, fmt,...) \
    do { \
        char ring_buffer_temp[512]; \
        memset(ring_buffer_temp, 0, 512); \
        if(phy->module_debug & module) {  \
            sprintf(ring_buffer_temp, fmt, ##__VA_ARGS__); \
            system_log_append(ring_buffer_temp); \
        }  \
    } while(0)

#define  DEFINE_TIME()  \
        struct timeval tvBegin, tvEnd; \
        double dDurationS;

#define  START_TIME() \
    do { \
        gettimeofday(&tvBegin, NULL); \
    } while (0)

#define  END_TIME()      \
    do { \
        gettimeofday(&tvEnd, NULL); \
        dDurationS = (tvEnd.tv_sec - tvBegin.tv_sec) + ((tvEnd.tv_usec - tvBegin.tv_usec) / 1000.0) / 1000.0; \
    } while (0)

#define  PRINT_TIME(str)  \
    do { \
        LOG_MAIN("+++++++++++++++[%s]seconds: %.3f\n", str, dDurationS); \
    } while (0)
#else
#undef   LOG_INFO
#define  LOG_INFO(fmt,...)    //printf(fmt, ##__VA_ARGS__)
#define  LOG_WARN(fmt,...)    printf(fmt, ##__VA_ARGS__)
#define  LOG_ERROR(fmt,...)   printf(fmt, ##__VA_ARGS__)
#define  LOG_MAIN(fmt,...)    printf(fmt, ##__VA_ARGS__)
#define  LOG_MDEBUG(phy, module, fmt,...) \
    do { \
        if(phy->module_debug & module) {  \
            printf(fmt, ##__VA_ARGS__); \
        }  \
    } while(0)
#define  DEFINE_TIME()
#define  START_TIME()
#define  END_TIME();
#define  PRINT_TIME(str)  \
    do { \
        LOG_MAIN("+++++++++++++++[%s]\n", str); \
    } while (0)
#endif

int get_trigger_num();
void set_trigger_num(int num);


#if CMD_DEBGU_ON
    #define ENTER_CMD(trigger_point) \
    {   \
        int trigger_num = get_trigger_num();\
        if(trigger_num!=0 && trigger_num == trigger_point) { \
            cmd_debug(); \
        } \
    }
#else 
    extern void cmd_debug(void);
    #define ENTER_CMD(trigger_point)
#endif

typedef enum FPGA_BOARD_E {
    FPGA_7020,
    FPGA_706
} fpga_board_e;

typedef struct _tracking_thread 
{
    task_t tid;    
    volatile int need_exit;
    volatile int tracking_times;    
} tracking_thread;

typedef struct _rx_qec_tracking_thread_var 
{   
    tracking_thread             thread;
    int dbfs_threshold;
} rx_qec_tracking_thread_var;

typedef struct _rx_ana_tracking_thread_var 
{   
    tracking_thread             thread;
    int                         debug;
    int                         tia;
    int                         hold_cnt;
    int                         track_cnt;
    RX_DC_TRACK_CFG track_cfg;
} rx_ana_tracking_thread_var;

int create_task(task_t *ptask, void* (*start_rtn)(void*), void *arg);

typedef struct rf_chip_phy_struct
{
    chip_config_t *config;
    unsigned char tx_bw_setting[15];
    unsigned char rx_bw_setting[13];
    RX_GCTRL_CFG_T rx_gain_ctrl[2];
    unsigned char tx_bb_gain_config[50][16];

    char r_cal_flag;
    unsigned char r_cal;

    char ldo_cal_flag;
    unsigned char ldo_cal[6];

    char lvds_cal_flag;
    unsigned char lvds_rx;
    unsigned char lvds_tx;
    
    char auxadc1_cal_flag;
    int auxadccal_slope;
    int auxadccal_ordinate;

    char vco_range_cal_flag;
    unsigned long long sxrx_fvco_min;
    unsigned long long sxtx_fvco_min;

    char sx_cal_flag;
    unsigned char sx_cal_1[512];
    unsigned char sx_cal_2[512];

    char txlo_cal_flag;
    unsigned char txlo_cal_1[32];
    unsigned char txlo_cal_2[32];

    char tx_dc_cal_flag[2];
    unsigned char tx_dc_cal[2][4];

    char tx_dac_cal_flag[2];
    unsigned char tx_dac_cal[2][65*2];

    unsigned char tia[2][RX_PORT_GAIN_MAX * 2];
    unsigned char bq_rx1[RX_PORT_GAIN_MAX][26];
    unsigned char bq_rx2[RX_PORT_GAIN_MAX][26];
    char rx_dc_cal_flag[2];
    char rx_dc_lut_update;

    char rx_bw_cal_flag[2];
    unsigned char rx_imbalance_cal[2][2];
    unsigned char rx_bw_cal[2][2];

    char rx_qec_flag[2];
    RX_QEC_CFG_REGS rx_qec[2];

    char tx_qec_flag[2];
    TX_QEC_CFG_REGS tx_qec[2][6];

    short tx_atten[2];
    short tx_curr_index;
    unsigned char tx_pa_gain[20];
    unsigned char tx_bb_gain[16];
    
    short rx_lmt_gain[2];
    short rx_lpf_gain[2];
    short rx_dig_gain[2];

    BANDWIDTH_ENUM bandwidth;
    unsigned long long  rxlo;
    unsigned long long  txlo;
    unsigned long long  min_vco;

    unsigned long module_debug;
    char debug_on;
    char loading_lut;
    short error;
    char init_flag;
    FSM_ST_ENUM st;
    int spi;
    rx_qec_tracking_thread_var rxqec_track_thread[TRX_CH_CNT];
    rx_ana_tracking_thread_var rxdc_track_thread[TRX_CH_CNT];
    unsigned char set_trx_lo_sw_cal_flag;
} rf_chip_phy_t;

extern unsigned int hal_fpga_read_reg(rf_chip_phy_t *phy, unsigned int reg);
extern short hal_fpga_write_reg(rf_chip_phy_t *phy, unsigned int reg, unsigned int val);
extern unsigned char hal_spi_read_reg(rf_chip_phy_t *phy, unsigned short reg);
extern short hal_spi_write_reg(rf_chip_phy_t *phy, unsigned short reg, unsigned char val);
extern void HAL_CONFIG_REGS(rf_chip_phy_t *phy, const reg_t *setting, short len);
extern unsigned char HAL_REG_GET_BITS(rf_chip_phy_t *phy, unsigned short reg, REG_BIT bit, char cnt);
extern void  HAL_REG_SET_BITS(rf_chip_phy_t *phy, unsigned short reg, REG_BIT bit, char cnt, unsigned char setval);
extern char HAL_REG_GET_BIT(rf_chip_phy_t *phy, unsigned short reg, REG_BIT bit);
extern void  HAL_REG_SET_BIT(rf_chip_phy_t *phy, unsigned short reg, REG_BIT bit);
extern void  HAL_REG_CLR_BIT(rf_chip_phy_t *phy, unsigned short reg, REG_BIT bit);

extern void wire_control_en(rf_chip_phy_t *phy, short en, WIRE_CTRL_ENUM pulse);
extern int tdd_wait_to_alert(rf_chip_phy_t *phy);
extern int tdd_alert_to_rx(rf_chip_phy_t *phy);
extern int tdd_rx_to_wait(rf_chip_phy_t *phy);
extern int tdd_alert_to_tx(rf_chip_phy_t *phy);
extern int tdd_tx_to_wait(rf_chip_phy_t *phy);
extern int tdd_rx_to_alert_level(rf_chip_phy_t *phy);
extern int tdd_tx_to_alert_level(rf_chip_phy_t *phy);
extern int tdd_alert_to_tx_level(rf_chip_phy_t *phy);
extern int tdd_alert_to_rx_level(rf_chip_phy_t *phy);
extern int fpga_and_chip_reset(rf_chip_phy_t *phy);
extern void chip_reset_withus(rf_chip_phy_t *phy, unsigned int delay);

extern void lvds_delay_cal(rf_chip_phy_t *phy);
extern short tx_lvds_delay_cal(rf_chip_phy_t *phy);
extern short rx_lvds_delay_cal(rf_chip_phy_t *phy);
extern int fpga_tail_set(rf_chip_phy_t *phy, short dir, short chn, short intflag);
extern int fn_fpga_set_if(rf_chip_phy_t *phy, short mode, short dif, short port, short rate, short flag);
extern int valid_channel(TRX_CHN_ENUM chn, CHIP_MODE_ENUM mode);

typedef struct  handle_struct
{
    int fd;
}handle_t;

extern int read_flash(handle_t handle, int addr, unsigned char buffer[], int size);
extern handle_t get_bin_handle(rf_chip_phy_t *phy, short *ok);
extern void close_bin_handle(handle_t handle);

#if (!LOAD_LUT_FROM_BIN_FILE)

#if (CH1_FDD_LUT_ON)
extern const unsigned char lut_D1_CH1_FDD[3752][8];
extern const unsigned char lut_D2F1_CH1_FDD[2448][8];
extern const unsigned char lut_E1_CH1_FDD[3752][8];
extern const unsigned char lut_GSREDA1B1_CH1_FDD[2448][8];
#endif

#if (RX1_TX2_FDD_LUT_ON)
extern const unsigned char lut_D1_RX1TX2_FDD[3752][8];
extern const unsigned char lut_D2F1_RX1TX2_FDD[2448][8];
extern const unsigned char lut_E1_RX1TX2_FDD[3752][8];
extern const unsigned char lut_GSREDA1B1_RX1TX2_FDD[2448][8];
#endif

#if (RX2_TX1_FDD_LUT_ON)
extern const unsigned char lut_D1_RX2TX1_FDD[3752][8];
extern const unsigned char lut_D2F1_RX2TX1_FDD[2448][8];
extern const unsigned char lut_E1_RX2TX1_FDD[3752][8];
extern const unsigned char lut_GSREDA1B1_RX2TX1_FDD[2448][8];
#endif

#if (CH2_FDD_LUT_ON)
extern const unsigned char lut_D1_CH2_FDD[3752][8];
extern const unsigned char lut_D2F1_CH2_FDD[2448][8];
extern const unsigned char lut_E1_CH2_FDD[3752][8];
extern const unsigned char lut_GSREDA1B1_CH2_FDD[2448][8];
#endif

#if (CH1_CH2_FDD_LUT_ON)
extern const unsigned char lut_D1_CH1CH2_FDD[3752][8];
extern const unsigned char lut_D2F1_CH1CH2_FDD[2448][8];
extern const unsigned char lut_E1_CH1CH2_FDD[3752][8];
extern const unsigned char lut_GSREDA1B1_CH1CH2_FDD[2448][8];
#endif

#if (CH1_TDD_LUT_ON)
extern const unsigned char lut_D1_CH1_TDD[3752][8];
extern const unsigned char lut_D2F1_CH1_TDD[2448][8];
extern const unsigned char lut_E1_CH1_TDD[3752][8];
extern const unsigned char lut_GSREDA1B1_CH1_TDD[2448][8];
#endif

#if (CH2_TDD_LUT_ON)
extern const unsigned char lut_D1_CH2_TDD[3752][8];
extern const unsigned char lut_D2F1_CH2_TDD[2448][8];
extern const unsigned char lut_E1_CH2_TDD[3752][8];
extern const unsigned char lut_GSREDA1B1_CH2_TDD[2448][8];
#endif

#if (CH1_CH2_TDD_LUT_ON)
extern const unsigned char lut_D1_CH1CH2_TDD[3752][8];
extern const unsigned char lut_D2F1_CH1CH2_TDD[2448][8];
extern const unsigned char lut_E1_CH1CH2_TDD[3752][8];
extern const unsigned char lut_GSREDA1B1_CH1CH2_TDD[2448][8];
#endif

#if (HYBRID_FDD_CH1CH2_LUT_ON)
extern const unsigned char lut_D1_HYBRID_CH1CH2_FDD[3752][8];
extern const unsigned char lut_D2F1_HYBRID_CH1CH2_FDD[2448][8];
extern const unsigned char lut_E1_HYBRID_CH1CH2_FDD[3752][8];
extern const unsigned char lut_GSREDA1B1_HYBRID_CH1CH2_FDD[2448][8];
#endif

#if (HYBRID_TDD_CH1CH2_LUT_ON)
extern const unsigned char lut_D1_HYBRID_CH1CH2_TDD[3752][8];
extern const unsigned char lut_D2F1_HYBRID_CH1CH2_TDD[2448][8];
extern const unsigned char lut_E1_HYBRID_CH1CH2_TDD[3752][8];
extern const unsigned char lut_GSREDA1B1_HYBRID_CH1CH2_TDD[2448][8];
#endif

#if (HYBRID_FDD_CH1_LUT_ON)
extern const unsigned char lut_D1_HYBRID_CH1_FDD[3752][8];
extern const unsigned char lut_D2F1_HYBRID_CH1_FDD[2448][8];
extern const unsigned char lut_E1_HYBRID_CH1_FDD[3752][8];
extern const unsigned char lut_GSREDA1B1_HYBRID_CH1_FDD[2448][8];
#endif

#if (HYBRID_FDD_CH2_LUT_ON)
extern const unsigned char lut_D1_HYBRID_CH2_FDD[3752][8];
extern const unsigned char lut_D2F1_HYBRID_CH2_FDD[2448][8];
extern const unsigned char lut_E1_HYBRID_CH2_FDD[3752][8];
extern const unsigned char lut_GSREDA1B1_HYBRID_CH2_FDD[2448][8];
#endif

#endif /* LOAD_LUT_FROM_BIN_FILE */


#endif  /* __PLATFORM_H */

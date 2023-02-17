/*
    #include "FCS152_KDU.h"
*/
#ifndef __FCS152_KDU_H__
#define __FCS152_KDU_H__

#define DEBUG 1



#include "userinclude.h"
#include "bsp_delay.h"
#include "lcd.h"

#define  THISCHIP   THISCHIP_ESP32S2

//define the major file to include.
#if     (THISCHIP == THISCHIP_STM32F103RET6)
#include "stm32f1xx.h"
#elif   (THISCHIP == THISCHIP_ESP32S2)
#include <Arduino.h>
#include "esp_timer.h"
#include "esp_rom_sys.h"
#include "driver/timer.h"
#elif   (THISCHIP == THISCHIP_CM32M101A)
#include "cm32m101a.h"
#else
#endif

//define the debug mode how to realize.
#if     (THISCHIP != THISCHIP_ESP32S2)
#ifdef DEBUG
#define D_printf(fmt,args...) \
        do \
        {\
            printf(fmt, ##args);\
        }while(0)
//printf("%s:  %s:  %d\n", __FILE__, __FUNCTION__, __LINE__);
#else
#define D_printf(fmt,args...)
#endif
#else
#ifdef DEBUG
#define D_printf(fmt,args...) \
        do \
        {\
            Serial.printf(fmt, ##args);\
        }while(0)
//printf("%s:  %s:  %d\n", __FILE__, __FUNCTION__, __LINE__);
#else
#define D_printf(fmt,args...)
#endif
#endif
//define some system operate function.
#if     (THISCHIP == THISCHIP_STM32F103RET6)
#define FeedDog()           IWDG->KR = 0XAAAA
#define ResetSystem()       NVIC_SystemReset()
#elif   (THISCHIP == THISCHIP_ESP32S2)
#define FeedDog()           delay_ms(5)
#define ResetSystem()       ESP.restart()
#elif   (THISCHIP == THISCHIP_CM32M101A)
#define FeedDog()           IWDG->KEY = 0xAAAA
#define ResetSystem()       NVIC_SystemReset()
//#define INTX_DISABLE()    __disable_irq();
//#define INTX_ENABLE()     __enable_irq();
#else
#endif

//define the flash address to store the BOOTLOADER, APP and FLAG
#if     (THISCHIP == THISCHIP_ESP32S2)              ////No need to set the receiving address for the time being
#elif   (THISCHIP == THISCHIP_CM32M101A)            ////only KDU
/*
    0x0800 0000 ~ 0x0800 4800: 18K boot
        bootloader  : 18K = 18*1024 = 18432 = 0x4800
    0x0800 4800 ~ 0x0800 5000: 2K  USER FLAG
        USERFLAG    : 2K  = 2*1024  = 2048  = 0x0800

    0x0800 5000 ~ 0x0801 2800: 54K              //App space
        APP RUN     : 54K = 54*1024 = 55296 = 0xD800
    0x0801 2800 ~ 0x0802 0000: 54K              //Receive new programs
        APP RCV     : 54K = 54*1024 = 55296 = 0xD800
*/
#define     APP_PAGE_SIZE       0xD800          //54K   //The space to store the running APP, the space to store the receiving APP, is 54K
#define     USER_PAGE_SIZE      0x0800          //2K    //Space size of each page
#define     KDU_FLAG_ADDR       0x08004800      //Program update information storage address        0X55
#define     KDU_SIZE_ADDR       0x08004900      //KDU Receiving data size storage address
#define     KDU_RUN_ADDR        0x08005000      //KDU Run start address
#define     KDU_RCV_ADDR        0x08012800      //KDU Receiving start address
#elif   (THISCHIP == THISCHIP_STM32F103RET6)        ////Both PRC-152 and KDU need to be set
#define     KDU_RUN_ADDR        0x08010000      //
#define     KDU_RCV_ADDR        0x08020000      //
#define     KDU_FLAG_ADDR       0x08030000      //Program update information storage address        0X55

//Start address for 152 IAP
#if !defined UPBOOT
#define     _152_RUN_ADDR   0x8020000       //128K space to store APPS
#else
#define     _152_RUN_ADDR   0x8000000
#endif
#endif

//deside the device information string
#if     (THISCHIP != THISCHIP_STM32F103RET6)
#define   __NEW__                           //this program base on CM32M101, which is new
#endif


#ifndef   __NEW__
#define  STR_152         "FCS PRC152"
#define  STR_KDU         " FCS  KDU "
#define  VERSION_152     "Rev 1.0.000"
#define  VERSION_KDU     "Rev 1.1.000"          //Modify compatible ESP
#else
#define  STR_152         "FCS PRC152-N"
#define  STR_KDU         "FCS KDU-N "
#define  VERSION_152     "XVF 2.1.001"
#define  VERSION_KDU     "Rev 2.0.000"
#endif

#define  VERSION_UPBOOT     "Upgrade Bootloader0.1" //Used to update the PRC-152 program version of the underlying
#define  VERSION_BOOT       "BOOT Rev 0.0.003"      //Used to upgrade the BOOT version of PRC-152

//////////////////////////////////////////////////////////////////////////////////
#define     USART1_BUF_SIZE         1024+8+1+50     //define Serial Port 1 buffer length
#define     USART2_BUF_SIZE         255             //define Serial port 2 buffer length

#define     NO_OPERATE          0
#define     RELOAD_ARG          1
#define     SAVE_SET            2

#define     CLR2LAST            0
#define     ENT2LAST            1
#define     BACK2MAIN           2

#define     FM_EN               1
#define     LCD12832            0
#define     LCD12864            2

//The flag of appaddr whether the app should run
#define     NEW_APP             0x55
#define     RUN_APP             0xAA
//#define   WRO_APP             0xAA
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//8.4V boot voltage up/down limit - Setting the battery voltage limits. In case of 8.4V battery.
#define  VOLTAGE_ON_8_UPPER         8700
#define  VOLTAGE_ON_8_FLOOR         6400    //Too little power, useless to boot
//12.4V boot voltage up/down limit - Setting the battery voltage limits. In case of 12.4V battery.
#define  VOLTAGE_ON_12_UPPER        13250
#define  VOLTAGE_ON_12_FLOOR        9250

//8.4V Shutdown voltage upper and lower limit
#define  VOLTAGE_OFF_8_UPPER        8680
#define  VOLTAGE_OFF_8_FLOOR        6100    //Put it to a safe voltage as much as possible
//12.4V Shutdown voltage up/down limit
#define  VOLTAGE_OFF_12_UPPER       13250
#define  VOLTAGE_OFF_12_FLOOR       8900
//Calculate the voltage base value
#define  VOLTAGE_8_CAL              6400
#define  VOLTAGE_12_CAL             9600

//Voltage level definition
typedef enum {
    VOLTAGE_NORMAL=0,
    VOLTAGE_LOW,
    VOLTAGE_HIGH,
    VOLTAGE_ERROR
} CheckVoltage;

typedef enum {
    FAILED = 0,
    PASSED = !FAILED
} Status;

//Sound output port definition 
typedef enum {
    IN=0,
    TOP,
    SIDE
} AudioSelect;

typedef enum {
    TONE2K,
    TONE1_5K,
} ToneClass;

typedef struct { //32
    volatile u8 CHAN;
    volatile u8 RS;
    volatile u8 TS;
    volatile u8 POWER;
    volatile u8 GBW;
    volatile u8 SCAN;

    volatile double RX_FREQ;
    volatile double TX_FREQ;
    volatile char NN[8];        //one extra for ending '\0'

}
CHAN_ARV, *CHAN_ARV_P;

#define ARV_MEM_COUNT 4
enum {
    NOW=0,  //current channel parameters
    TMP,    //cache the channel parameters sought
    CHANA,  //channel A parameters in dual watch mode
    CHANB,  //channel B parameters in dual watch mode
};
extern CHAN_ARV chan_arv[ARV_MEM_COUNT];
enum {
    MAIN_MODE = 0,
    BIG_MODE,
    DUAL_MODE
};
enum recv_mess {
    ASKALL=0,       //receive
    _ASKALL,        //send

    ASKCHAN,
    _ASKCHAN,

    ASKA,
    _ASKA,

    ASKB,
    _ASKB,

    RELOAD,
    _RELOAD,

    RELA,
    _RELA,

    RELB,
    _RELB,

    SETCHAN,
    _SETCHAN,

    NORMAL,
    _SETZERO,

    SETHOMEMODE,
    _SETHOMEMODE,
////////////////////////////////////
    SETSTEP,
    _SETSTEP,

    SETSQL,
    _SETSQL,

    SETAUD,
    _SETAUD,

    SETENC,
    _SETENC,

    SETTOT,
    _SETTOT,

    SETOP,
    _SETOP,

    SETVOLU,
    _SETVOLU,           //Volume

    SETTONE,
    _SETTONE,           //Tone

    SETFM,
    _SETFM,             //FM Radio (?)

    SETDUALPOS,
    _SETDUALPOS,
};
//
//PGM Menu definition
typedef enum {
    normal_set_mic_gain,
    normal_set_sql,
    normal_set_step,
    normal_set_tot,
    normal_set_lamptime,
    normal_set_powerout,
    normal_set_ptttone,
} normal_set;

//Data length
#define Length_CHAN             3   //channel
#define Length_RX               8   //Receiving frequency
#define Length_TX               8   //Transmit frequency
#define Length_RS               3   //Receive subsonic - defining filtering subsonic frequencies (?)
#define Length_TS               3   //Emit subsonic - defining filtering subsonic frequencies (?)
#define Length_POWER            1   //Power - defining output power (?)
#define Length_BW               1   //Bandwidth - defining radio bandwidth (?)
#define Length_NN               8   //Alias (?)
#define Length_SCAN             1   //Scan logo - I don't know what it would be!! Must be SCAN option, but radio doesn't have any.

#define Length_CF               1   //Current channel/frequency mode
#define Length_VU               1   //Current V/U segment
#define Length_CHANA            3   //Dual Channel A
#define Length_CHANB            3   //Dual Channel B

#define Length_VOLUME           1   //Volume
#define Length_STEP             1   //Step
#define Length_SQL              1   //Squelch
#define Length_AUDIO            1   //Audio
#define Length_MIC              1   //mic sensitivity (?)
#define Length_ENCRYPTION       1   //Launch encryption
#define Length_TOT              1   //Length of transmission limit
#define Length_OUTPOWER         1   //Six-pin voltage output (at top of radio)
#define Length_PRETONE          1   //Pre-launch tone. Used to beeping before start of transmission. 
#define Length_ENDTONE          1   //Launch end tone. Use to beeping at the end of transmission.
#define Length_FMFREQ           4   //Radio frequency  
//KDU
#define Length_WFM              1   //收音机开关
#define Length_FMCHAN           1   //收音机频道
#define Length_VOLTAGE          3   //电压
#define Length_RSSI             3   //信号强度
#define Length_KEYSQ            1   //接收信号状态
#define Length_KEYSQU           1   //按键静噪状态
#define Length_KEYPTT           1   //按键PTT状态 
#define Length_HOMEMODE         1   //主页模式
#define Length_NOWRCVCHAN       3   //当前接收到信号的信道(双守模式下)
#define Length_NOWSELCHAN       1   //当前选中双守模式的信道//A或B
//MEMORY
#define Length_BACKLIGHTNESS    3   //背光亮度
#define Length_FLAGBACKLIGHT    1   //背光开关
#define Length_LAMPTIME         1   //背光时间
#define Length_SCREENCONTRAST   1   //屏幕对比度

//AT保存数据地址
//100-499 全局变量
//100-199 设置
//排列顺序
#define CHAN_RANK                           0
#define RX_RANK                             (CHAN_RANK          +Length_CHAN      ) //3
#define TX_RANK                             (RX_RANK            +Length_RX        ) //11
#define RS_RANK                             (TX_RANK            +Length_TX        ) //19
#define TS_RANK                             (RS_RANK            +Length_RS        ) //22
#define POWER_RANK                          (TS_RANK            +Length_TS        ) //25
#define BW_RANK                             (POWER_RANK         +Length_POWER     ) //26
#define NN_RANK                             (BW_RANK            +Length_BW        ) //27
#define SCAN_RANK                           (NN_RANK            +Length_NN        ) //35

#define CF_RANK                             (SCAN_RANK          +Length_SCAN      ) //36
#define VU_RANK                             (CF_RANK            +Length_CF        ) //37
#define CHANA_RANK                          (VU_RANK            +Length_VU        ) //38
#define CHANB_RANK                          (CHANA_RANK         +Length_CHANA     ) //41

#define VOLUME_RANK                         (CHANB_RANK         +Length_CHANB     ) //44
#define STEP_RANK                           (VOLUME_RANK        +Length_VOLUME    ) //45
#define SQL_RANK                            (STEP_RANK          +Length_STEP      ) //46
#define AUDIO_RANK                          (SQL_RANK           +Length_SQL       ) //47
#define MIC_RANK                            (AUDIO_RANK         +Length_AUDIO     ) //48
#define ENCRYPTION_RANK                     (MIC_RANK           +Length_MIC       ) //49
#define TOT_RANK                            (ENCRYPTION_RANK    +Length_ENCRYPTION) //50
#define VDO_RANK                            (TOT_RANK           +Length_TOT       ) //51   
#define PRETONE_RANK                        (VDO_RANK           +Length_OUTPOWER  ) //52
#define ENDTONE_RANK                        (PRETONE_RANK       +Length_PRETONE   ) //53
#define FMFREQ_RANK                         (ENDTONE_RANK       +Length_ENDTONE   ) //54

//KDU用
#define WFM_RANK                            (FMFREQ_RANK        +Length_FMFREQ    ) //////58
#define FMCHAN_RANK                         (WFM_RANK           +Length_WFM       ) //59

#define VOLTAGE_RANK                        (FMCHAN_RANK        +Length_FMCHAN    ) //60
#define RSSI_RANK                           (VOLTAGE_RANK       +Length_VOLTAGE   ) //63
#define KEY_SQ_RANK                         (RSSI_RANK          +Length_RSSI      ) //66
#define KEY_SQU_RANK                        (KEY_SQ_RANK        +Length_KEYSQ     ) //67
#define KEY_PTT_RANK                        (KEY_SQU_RANK       +Length_KEYSQU    ) //68

#define HOMEMODE_RANK                       (KEY_PTT_RANK       +Length_KEYPTT    ) //69
#define NOWRCVCHAN_RANK                     (HOMEMODE_RANK      +Length_HOMEMODE  ) //70
#define NOWSELCHAN_RANK                     (NOWRCVCHAN_RANK    +Length_NOWRCVCHAN) //73

//记忆存储用
#define BACKLIGHTNESS_RANK                  (FMFREQ_RANK        +Length_FMFREQ    ) //////58
#define FLAG_BACKLIGHT_RANK                 (BACKLIGHTNESS_RANK +Length_BACKLIGHTNESS)  //61
#define LAMPTIME_RANK                       (FLAG_BACKLIGHT_RANK+Length_FLAGBACKLIGHT)  //62
#define SCREEN_CONTRAST_RANK                (LAMPTIME_RANK      +Length_LAMPTIME  )     //63

//将数据全部发送和接收,一帧数据包含所有信息, 根据命令不同解读即可获取不同内容
#define kdu_start_rank                      16
#define chan_rank                           kdu_start_rank + CHAN_RANK
#define rx_rank                             kdu_start_rank + RX_RANK
#define tx_rank                             kdu_start_rank + TX_RANK
#define rs_rank                             kdu_start_rank + RS_RANK
#define ts_rank                             kdu_start_rank + TS_RANK
#define pw_rank                             kdu_start_rank + POWER_RANK
#define bw_rank                             kdu_start_rank + BW_RANK
#define nn_rank                             kdu_start_rank + NN_RANK
#define scan_rank                           kdu_start_rank + SCAN_RANK

#define cf_rank                             kdu_start_rank + CF_RANK
#define vu_rank                             kdu_start_rank + VU_RANK
#define chana_rank                          kdu_start_rank + CHANA_RANK
#define chanb_rank                          kdu_start_rank + CHANB_RANK

#define volume_rank                         kdu_start_rank + VOLUME_RANK
#define step_rank                           kdu_start_rank + STEP_RANK
#define sql_rank                            kdu_start_rank + SQL_RANK
#define aud_rank                            kdu_start_rank + AUDIO_RANK
#define mic_rank                            kdu_start_rank + MIC_RANK
#define enc_rank                            kdu_start_rank + ENCRYPTION_RANK
#define tot_rank                            kdu_start_rank + TOT_RANK
#define op_rank                             kdu_start_rank + VDO_RANK
#define pre_rank                            kdu_start_rank + PRETONE_RANK
#define end_rank                            kdu_start_rank + ENDTONE_RANK
#define ffreq_rank                          kdu_start_rank + FMFREQ_RANK    //FM_Freq   收音机频率

#define wfm_rank                            kdu_start_rank + WFM_RANK       //收音机开关标志
#define fmchan_rank                         kdu_start_rank + FMCHAN_RANK    //fm频率是否是频道

#define volt_rank                           kdu_start_rank + VOLTAGE_RANK
#define rssi_rank                           kdu_start_rank + RSSI_RANK      //A20信号强度
#define sq_rank                             kdu_start_rank + KEY_SQ_RANK    //信号状态
#define squ_rank                            kdu_start_rank + KEY_SQU_RANK   //静噪状态
#define ptt_rank                            kdu_start_rank + KEY_PTT_RANK   //PTT状态
#define homemode_rank                       kdu_start_rank + HOMEMODE_RANK
#define nowrcvchan_rank                     kdu_start_rank + NOWRCVCHAN_RANK
#define nowselchan_rank                     kdu_start_rank + NOWSELCHAN_RANK

#define BUF_SIZE                            nowselchan_rank + Length_NOWSELCHAN+1
//////////////////////////////////////////////////////////////////////////////////////////
//Enable the eeprom
#if (THISCHIP != THISCHIP_ESP32S2)
#define  EN_EEROOM
#endif

#ifdef EN_EEROOM                            //AT保存数据地址

#define RESETADDR                           99
#define SETADDR                             100                         //基地址
#define CURRENT_CHANNEL_ADDR                SETADDR+CHAN_RANK           //当前信道号
#define FLAG_CF_SWITCH_ADDR                 SETADDR+CF_RANK             //0C,   1F
#define FLAG_VU_SWITCH_ADDR                 SETADDR+VU_RANK             //0V，  1U
#define CHANA_ADDR                          SETADDR+CHANA_RANK
#define CHANB_ADDR                          SETADDR+CHANB_RANK

#define STEP_ADDR                           SETADDR+STEP_RANK           //步进     5k 10k 12.5k
#define SQ_ADDR                             SETADDR+SQL_RANK            //静噪等级 0-8
#define AUDIO_SELECT_ADDR                   SETADDR+AUDIO_RANK          //音频输出选择
#define MIC_LEVEL_ADDR                      SETADDR+MIC_RANK            //咪灵敏度 0-7
#define SCRAM_LEVEL_ADDR                    SETADDR+ENCRYPTION_RANK     //发射加密 0-8
#define TOT_ADDR                            SETADDR+TOT_RANK            //发射限时 0-9min
#define OUTPOWER_ADDR                       SETADDR+VDO_RANK       //6针头输出
#define PRETONE_ADDR                        SETADDR+PRETONE_RANK        //PTT前置提示音
#define ENDTONE_ADDR                        SETADDR+ENDTONE_RANK        //PTT结束提示音
#define OVER_VOLUME_ADDR                    SETADDR+VOLUME_RANK         //全局音量 0-7
//#define FLAG_WFMMOD_ADDR                  SETADDR+WFM_RANK            //收音机   OFF0  ON1
#define FM_RADIO_FREQ_ADDR                  SETADDR+FMFREQ_RANK         //收音机频 4位
#define LAMPTIME_ADDR                       SETADDR+LAMPTIME_RANK       //背光时间 0/1 常亮/10秒
#define BACKLIGHTNESS_ADDR                  SETADDR+BACKLIGHTNESS_RANK  //背光强度 0-100
#define FLAG_BACKLIGHT_ADDR                 SETADDR+FLAG_BACKLIGHT_RANK //背光开关
#define ScreenContrast_ADDR                 SETADDR+SCREEN_CONTRAST_RANK//对比度
#define DATA_ADDR                           896
//#define MEM_LENGTH                          64
#define V_CHANNEL_ADDR                      DATA_ADDR+CHAN_RANK         //V信道info起始地址
#define V_RX_ADDR                           DATA_ADDR+RX_RANK           //V收频
#define V_TX_ADDR                           DATA_ADDR+TX_RANK           //V发频
#define V_RS_ADDR                           DATA_ADDR+RS_RANK           //V收亚音
#define V_TS_ADDR                           DATA_ADDR+TS_RANK           //V发亚音
#define V_TX_POWER_ADDR                     DATA_ADDR+POWER_RANK        //发射功率
#define V_GBW_ADDR                          DATA_ADDR+BW_RANK           //带宽
#define V_NN_ADDR                           DATA_ADDR+NN_RANK           //别名
#define V_SCAN_ADDR                         DATA_ADDR+SCAN_RANK         //扫描标志
#define U_FREQ_CHANNEL                      100
#define U_CHANNEL_ADDR                      (DATA_ADDR+MEM_LENGTH*U_FREQ_CHANNEL)    //U信道info起始地址
#define U_RX_ADDR                           U_CHANNEL_ADDR+RX_RANK      //U收频
#define U_TX_ADDR                           U_CHANNEL_ADDR+TX_RANK      //U发频
#define U_RS_ADDR                           U_CHANNEL_ADDR+RS_RANK      //U收亚音
#define U_TS_ADDR                           U_CHANNEL_ADDR+TS_RANK      //U发亚音
#define U_TX_POWER_ADDR                     U_CHANNEL_ADDR+POWER_RANK   //发射功率
#define U_GBW_ADDR                          U_CHANNEL_ADDR+BW_RANK      //带宽
#define U_NN_ADDR                           U_CHANNEL_ADDR+NN_RANK      //别名
#define U_SCAN_ADDR                         U_CHANNEL_ADDR+SCAN_RANK    //扫描标志
#else //above non-ESP32, but use this enum if it's an ESP32
typedef enum {
    RESETADDR=0, //wasn't explicitly set in OEM code
    FLAG_CF_SWITCH_ADDR,
    FLAG_VU_SWITCH_ADDR,
} FLAG;
#endif

#endif

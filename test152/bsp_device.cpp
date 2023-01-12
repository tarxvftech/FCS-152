#include "bsp_wifi.h"
#include "main.h"

volatile u32 OFF_MS=0, OFF_HUR=0;

void SHUT(void)
{
    D_printf("<<<<<<SHUTING>>>>>>\n");

    LCD_Clear(GLOBAL32); //关屏
    LCD_ShowPICALL(pic_HARRIS);

#if FM_EN
    //关收音机
    if (WFM)
        RDA5807_Init(OFF);
#endif

    //62364关
    M62364_SetSingleChannel(4, 0);
    M62364_SetSingleChannel(6, 0);

    SPK_SWITCH(0, 0);

    A002_PD_CLR; //对讲机睡眠
    VDO_CLR;    //六针头关

    delay_ms(1000);
    LCD_Clear(GLOBAL32);
    BackLight_SetVal(0);
    delay_ms(1000);
    Sys_Enter_Standby();
}

void ClearShut(void)
{
    OFF_MS  = 0;
    OFF_HUR = 0;
    LightBacklight();
}
//
extern void SHUT();
void Cal2Shut(void)
{
    if(OFF_MS++>=36000)
    {
        OFF_MS=0;
        if(OFF_HUR++>=4)
            SHUT();
    }
}



//系统进入待机模式
void Sys_Enter_Standby(void)
{
    // pinMode(WAKE_UP_PIN, INPUT_PULLDOWN);
    // esp_sleep_enable_ext0_wakeup(WAKE_UP_PIN, HIGH); //1 = High, 0 = Low
    // Serial.println("Going to sleep now");
    // esp_deep_sleep_start();
POWER_EN_CLR;

}
//
//初始待机及退出
void Standby_Init()
{
    // Encoder_Init(); //编码器按键初始化
    // delay_ms(5);

    //电源控制引脚首先使能
    ControlGPIO_Init(); //总电源控制引脚初始化
    int t = 0; //记录按下的时间
    while (1)
    {
        FeedDog();//喂狗	
        if (ENCODER_CLICK_READ==0)
        {
            t++; //已经按下了
            delay_ms(20);
            if (t >= 75) //按下超过3秒钟
            {
                POWER_EN_SET;
                return;   //按下3s以上了
            }
               
        }
        else
            Sys_Enter_Standby(); //误触，不是开机，重新进入待机模式
    }
}




#define MENU_UPDATE_NUM 3
const char *MENU_UPDATE[3] = 
{
    "1.CHANNEL SETTING    "     ,
    "2.UPDATE             "		,
	"3.DEVICE ABOUT       "		,
};
//写频界面
int i = 0;
int channelSetting(void)
{
    u8 lock=OFF;
    LCD_Clear(GLOBAL32);
    LCD_ShowString0608(12, 2, "CHANNEL SETTING",     1, 128);
    LCD_ShowString0608(19, 3, VERSION_BOOT,          1, 128);
    memset((char *)rx1_buf, '0', sizeof(char) * (USART1_BUF_SIZE - 1));
    while(1)
    {
        if(lock==OFF)
        {
            switch (Encoder_Switch_Scan(0))
            {
                case key_click:
                    break;
                case key_double:
                    return CLR2LAST;
                case key_long:
                    D_printf("<<<<<<SHUTING>>>>>>\n");
                    SHUT();
                    return CLR2LAST;
                default:
                break;
            }
            if (Matrix_KEY_Scan(0)==MATRIX_RESULT_CLR)
                return CLR2LAST;
        }
        if(Serial.available())
        {
            lock = ON;
            do{
                *(rx1_buf+i) = Serial.read();
                i++;
            }while(Serial.available());
              
            if(i<1024)
            {
                //升级
                if(strstr((char *)rx1_buf, "Line152"))
                {
                    Serial.printf("%cReady", 5);                //准备完毕，可以接收数据
                }
                //写频
                //覆盖某一地址数据
                else if(strstr((char *)rx1_buf, "Cover"))   
                {
                    // W_addr = (rx1_buf[ 5] - '0')*0x10000 + (rx1_buf[ 6] - '0')*0x1000 + (rx1_buf[ 7] - '0')*0x100 + (rx1_buf[ 8] - '0')*0x10 + (rx1_buf[9] - '0');
                    // W_data = rx1_buf[10];               
                    // AT24CXX_WriteOneByte(W_addr, W_data);
                    // Serial.printf("%cRecover", 7);
                }
                //查看某一地址数据
                else if(strstr((char *)rx1_buf, "Seek"))    
                {
                    // W_addr = (rx1_buf[ 5] - '0')*0x10000 + (rx1_buf[ 6] - '0')*0x1000 + (rx1_buf[ 7] - '0')*0x100 + (rx1_buf[ 8] - '0')*0x10 + (rx1_buf[9] - '0');	
                    // W_data = AT24CXX_ReadOneByte(W_addr);
                    // EN_Recv();
                    // printf("%cReSeek%c", 7, W_data);
                }
                //读取所有参数
                else if(strstr((char *)rx1_buf, "Get"))
                {
                    TakeDataFromMem(rx1_buf[3] - '0');
                }
                else if(strstr((char *)rx1_buf, "Write"))
                {
                    if(i<1000)
                        continue;
                    int page = rx1_buf[5] - '0';
                    WriteDataBackMem(page);
                    Serial.printf("%cElse%d", 5, (page+1));
                }
                //升级成功/写频完成均使用这个
                else if(strstr((char *)rx1_buf, "Endfile"))
                {
                    Serial.printf("%cOK", 2);
                    delay_ms(500);
                    SHUT();
                }
                else
                {
                    Serial.printf("%cUNKNOW", 6);
                }
            }
            i = 0;
            memset((char *)rx1_buf, '0', sizeof(char) * (USART1_BUF_SIZE - 1));
            lock = OFF;
        }
    }
}
void TakeDataFromMem(int page)
{
    int send_size  = 1025;
    char data2write[1025] = {0};
    memset (data2write, '0', send_size);	
    sprintf(data2write, "%s%d", "*Get", page);
    
    int pre_size = 5;
    
    if(page == 4)
    {
        data2write[ 0 + pre_size] = load_AudioSelect()  + '0';
        data2write[ 1 + pre_size] = load_MicLevel()     + '0';
                        
        data2write[ 2 + pre_size] = load_Sql()          + '0';
        data2write[ 3 + pre_size] = load_Step()         + '0';
        data2write[ 4 + pre_size] = load_Tot()          + '0';
        data2write[ 5 + pre_size] = load_LampTime()     + '0';
        data2write[ 6 + pre_size] = load_VDO()          + '0';
        data2write[ 7 + pre_size] = load_PreTone()      + '0';
        data2write[ 8 + pre_size] = load_EndTone()      + '0';
                        
        data2write[ 9 + pre_size] = get_Flag(FLAG_CF_SWITCH_ADDR)  + '0';
        data2write[10 + pre_size] = get_Flag(FLAG_VU_SWITCH_ADDR)  + '0';
        
        //信道号
        sprintf(data2write + pre_size + 11, "%03d", load_CurrentChannel());
        
        //VHF
        load_ChannelParameterStr(0,   data2write + pre_size + 14);

        //UHF                                                                                
        load_ChannelParameterStr(100, data2write + pre_size + 14 + 40);
    }
    else
    {
        for(int i = 1; i<26; i++)
        {
            if(page*25+i == 100)
                break;
            load_ChannelParameterStr(page*25+i, data2write + pre_size + (i-1)*40);
        }
    }
    UART1_Send_Message(data2write, send_size);
}
void WriteDataBackMem(int page)
{
    if(page == 4)
    {
        save_AudioSelect(rx1_buf[ 6] - '0') ;
        save_MicLevel   (rx1_buf[ 7] - '0') ;
        
        save_Sql        (rx1_buf[ 8] - '0') ;
        save_Step       (rx1_buf[ 9] - '0') ;
        save_Tot        (rx1_buf[10] - '0') ;
        save_LampTime   (rx1_buf[11] - '0') ;
        save_VDO        (rx1_buf[12] - '0') ;
        save_PreTone    (rx1_buf[13] - '0') ;
        save_EndTone    (rx1_buf[14] - '0') ;
        
        set_Flag(FLAG_CF_SWITCH_ADDR, rx1_buf[15] - '0') ;
        set_Flag(FLAG_VU_SWITCH_ADDR, rx1_buf[16] - '0') ;
        
        //信道号
        uint8_t chan = (rx1_buf[17]-'0')*100+(rx1_buf[18]-'0')*10+(rx1_buf[19]-'0');
        save_CurrentChannel(chan);
        
        //VHF
        save_ChannelParameterStr(0  , (char *)rx1_buf+20);
        //UHF
        save_ChannelParameterStr(100, (char *)rx1_buf+20+40);
        
    }
    else
    {
        for(int i = 1; i<26; i++)
        {
            if(page*25+i == 100)
                break;
            save_ChannelParameterStr(page*25+i, (char *)rx1_buf+6+(i-1)*40);
        }
    }
}
//

int  checkAbout(void)//设备信息查询
{
    LCD_Clear(GLOBAL32);
    LCD_ShowString0608(31, 0, "INFORMATION",     1, 128);
    LCD_ShowString0608(0,  1, "Device :",        0, 128);
    LCD_ShowString0608(0,  2, "Version:",        0, 128);
    LCD_ShowString0608(49, 1, STR_152,           1, 128);
    LCD_ShowString0608(49, 2, VERSION_152,       1, 128);
    LCD_ShowString0608(19, 3, VERSION_BOOT,      1, 128);
    while(1)
    {
        switch (Matrix_KEY_Scan(0))
        {
            case MATRIX_RESULT_ENT:
            case MATRIX_RESULT_CLR:
                return ENT2LAST;
            default:
                break;
        }
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
        case key_double:
            return ENT2LAST;

        case key_long:
            D_printf("<<<<<<SHUTING>>>>>>\n");
            SHUT();
            break;
        }
    }
}

void menuUpdate(void)
{
     TIMES = 0;
    u8 num = 0;
    int ENSURE = 0;
    LCD_Clear(GLOBAL32);
    LCD_ShowMenu41(MENU_UPDATE, MENU_UPDATE_NUM, num);
    while(1)
    {
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_ENT:
            ENSURE = 1;
            break;

        case MATRIX_RESULT_N:
        case MATRIX_RESULT_RIGHT:
            TIMES++;
            break;

        case MATRIX_RESULT_P:
        case MATRIX_RESULT_LEFT:
            TIMES--;
            break;
        }
        if (TIMES > 0)
        {
            num = (num + 1) % MENU_UPDATE_NUM;
            TIMES = 0;
            LCD_ShowMenu41(MENU_UPDATE, MENU_UPDATE_NUM, num);
        }
        else if (TIMES < 0)
        {
            num = (num + MENU_UPDATE_NUM - 1) % MENU_UPDATE_NUM;
            TIMES = 0;
            LCD_ShowMenu41(MENU_UPDATE, MENU_UPDATE_NUM, num);
        }
        // //////////////////////////////////////////////////////////
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            ENSURE = 1;
            break;

        case key_long:
            D_printf("<<<<<<SHUTING>>>>>>\n");
            SHUT();
            break;
        }
        // //////////////////////////////////////////////////////////
        if (ENSURE)
        {
            ENSURE = 0;
            switch (num)
            {
            case 0:
                channelSetting();
                break;

            case 1:
                ConfigureToUpdate();
                break;

            case 2:
                checkAbout();
                break;
            }
            ENSURE = 0;
            TIMES = 0;
            LCD_Clear(GLOBAL32);
            LCD_ShowMenu41(MENU_UPDATE, MENU_UPDATE_NUM, num);
        }
        //
    }
}

//
void enterSecondSystem()
{
    u8 key=0, i = 0;
    while (1) //检测是否要进入更新程序，更新成功重启
    {
        if(SQUELCH_READ == 0)key++;
        else key=0;
        if(key>5)
        {
            key=0;
            delay_ms(1500);
            LCD_Clear(GLOBAL32);
            menuUpdate();
        }
        delay_ms(200);
        D_printf("Waiting for UPGRADE~~~~~~\n");
        if (i++ >= 15)
            break;
    }
    //没有进入bootloader
    //Iap_load(_152_RUN_ADDR); //检测app2是否更新成功，优先进入app2
}
//





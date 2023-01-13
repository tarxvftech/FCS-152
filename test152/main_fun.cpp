#include "main.h"
#define DUAL_SWITCH_TIME        TMR_PERIOD_1S
volatile u8 WFM = OFF;         			//FM开关
volatile u8 KDU_INSERT = OFF;   		//KDU连接标志 OFF:断开  ON连接

extern int TIMES; 						//记录编码器操作
///////////////////////////////用作限制,条件许可内代码允许执行一次
volatile u8 
	FLAG_PTT_ONCE = 0, 					//按下/松开	PTT 时的设置
	FLAG_SQ_ONCE = 0, 					//接收/消失 信号时的设置
	SQUELCH_ONCE=0,						//进入/退出 长静噪的设置,执行一次
	
    SQL_CTL = OFF,                      //启动静噪长按时间计数
    SCAN_CTL = OFF;                     //启动扫描恢复时间计数
//用作计数,达到一定次数/时间可执行代码
u16 scan_cal = 0,						//用作计算 扫描模式下信号消失			的后的时间
	sql_cal  = 0;						//用作计算 按下静噪按键					的时间
	
//模式标志位
volatile char
	Home_Mode = 0,						//主页显示模式	//0:常规, 1:大字 2:双守
	Flag_Main_Page = 1,					//主页显示模式
    SQL_MODE = OFF;       				//长静噪模式

double 	STEP_LEVEL[] = {0.0050, 0.0100, 0.0125}; 	//步进等级

u8	RSSI = 0, SC=3,//SCERRN CONTRAST  CHAN = 0,
	STEP = 2, SQL = 4,  AUD = 0, MIC = 4,  ENC = 0, TOT = 0, BL = 40, VDO = 0, //VDO:输出电源
	VOLUME = 4,	PRE_TONE=0, END_TONE=0, 

	A20_LEVEL[8] = {0, 20, 40,  60,  80,  100, 150, 255},
	WFM_LEVEL[8] = {0,  5, 15,  30,  50,   75, 100, 200},	//{0, 31, 62, 93, 125, 155, 178, 254},//
	MIC_LEVEL[3] = {64, 165, 230};							//	MIC_LEVEL[8] = {0, 32, 64,  96, 160, 170, 195, 230};//{0, 32, 64,  96, 160, 170, 195, 230}
	
int FM_FREQ = 885;	
int LAMP_TIME = 10000;//默认10s
//


void VFO_Load_Data(void)
{
    //重新载入数据
    if (get_Flag(FLAG_CF_SWITCH_ADDR)) //频率模式
    {
        if (get_Flag(FLAG_VU_SWITCH_ADDR))
            chan_arv[NOW].CHAN = 100;
        else
            chan_arv[NOW].CHAN = 0;
    }
    else
        chan_arv[NOW].CHAN = load_CurrentChannel();

    load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
    chan_arv[TMP] = chan_arv[NOW];

    //载入双守模式数据
    chan_arv[CHANA].CHAN = load_ChanA();
    chan_arv[CHANB].CHAN = load_ChanB();
    load_ChannelParameter(chan_arv[CHANA].CHAN, &chan_arv[CHANA]);
    load_ChannelParameter(chan_arv[CHANB].CHAN, &chan_arv[CHANB]);

    //
    STEP = load_Step();       //步进读入
    SQL = load_Sql();         //静噪读入
    AUD = load_AudioSelect(); //音频输出方式加载
    if (AUD)
        MIC = load_MicLevel(); //mic_level读入
    else
        MIC = 1;
    ENC = load_ScramLevel();    //加密等级读入
    TOT = load_Tot();           //超时等级读入
    BL = load_Backlightness();  //背光等级读入
    SC = load_ScreenContrast(); //对比度
    LCD_LightRatio(0x00 + SC * 5);
    LAMP_TIME = load_LampTime() * 10000; //背光时间时间调节 常亮/10S

    VDO = load_VDO();           //输出电源状态读入
    VOLUME = load_OverVolume(); //音量读入
    PRE_TONE = load_PreTone();
    END_TONE = load_EndTone();
    FM_FREQ = load_FMFreq();
    WFM = OFF; //收音机

    VDO_SWITCH(VDO); //六针头电源输出
                     //	printf("STEP:%d, SQL:%d, AUD:%d, MIC:%d, ENC:%d, TOT:%d, BL:%d, VDO:%d, VOLUME:%d, PRE_TONE:%d, END_TONE:%d, WFM:%d\n", STEP, SQL, AUD, MIC, ENC, TOT, BL, VDO, VOLUME, PRE_TONE, END_TONE, WFM);

    delay_ms(250);
    Set_A20(chan_arv[NOW], SQL);
    Set_A20_MIC(MIC * 3 + 1, ENC, TOT);

    M62364Reset();
    BackLight_SetVal(BL);
    bsp_StartAutoTimer(TMR_FLOW, LAMP_TIME);
}

//主界面初始化
void VFO_Clear()
{
    D_printf("***************Main Interface*************\n");
    LCD_Clear(GLOBAL32);
    Flag_Main_Page = 1;

    LCD_ShowString0408(0, 0, "R BAT", 1);
    LCD_ShowBattery(Get_Battery_Vol());
    LCD_ShowString0408(48, 0, "VULOS MOI", 1);
    LCD_ShowString0408(88, 0, "------- PT", 1);
    LCD_ShowVolume(VOLUME);
    //模式3:双守模式
    //DualMode_Clear
    if (Home_Mode == DUAL_MODE)
    {
        LCD_ShowString0608(0, 1, "A:                    ", 1, 128);
        LCD_ShowString0608(0, 2, "B:                    ", 1, 128);
        LCD_ShowString0608(0, 3, "                      ", 1, 128);
        LCD_ShowString0408(0, 3, "DUAL STANDBY MODE.    ", 1);
        bsp_StartAutoTimer(TMR_DUAL_REFRESH, DUAL_SWITCH_TIME); //启动500ms切换一次频率
    }
}
//
//主页界面刷新
char BIG_MODE_buf[12] = {0};
char 
    sele_pos = 0,      //双守双待模式下,选择的信道
    now_chan = 0,      //当前设置的信道
    rcv_chan = 0;      //收到信号的信道

void VFO_Refresh()
{
    // D_printf("%s\n", __FUNCTION__);
    switch (Home_Mode)
    {
    case MAIN_MODE:
        if (chan_arv[NOW].CHAN == 100)
            LCD_ShowString0608(66, 1, "UHF ", 1, 90);
        else if (chan_arv[NOW].CHAN == 0)
            LCD_ShowString0608(66, 1, "VHF ", 1, 90);
        else
            LCD_ShowString0608(66, 1, "CHAN", 1, 90);

        //信道号显示
        LCD_ShowChan(83, 2, chan_arv[NOW].CHAN, 1);

        if (PTT_READ) //空闲时，显示频率、收音机状态
        {
            LCD_ShowFreq(0, 1, chan_arv[NOW].RX_FREQ, 1);
            LCD_ShowString0608(56, 2, WFM ? "WFM" : "FM ", 1, 128);
        }
        else
            LCD_ShowFreq(0, 1, chan_arv[NOW].TX_FREQ, 1);

        //收发不同频标志
        if (chan_arv[NOW].RX_FREQ != chan_arv[NOW].TX_FREQ)
            LCD_ShowPIC0808(92, 1, 0);
        else
            LCD_ShowString0408(92, 1, "  ", 1);

        LCD_ShowString0408(0, 3, "TYPE   TRF    MOD    CHAN  KEY  ", 1);
        LCD_ShowString0608(0, 2, "LOS", 1, 128);

        LCD_ShowString0608(26, 2, TRF_Show[MIC], 1, 128);
        break;

    //BIG_MODE
    case BIG_MODE:
        memset(BIG_MODE_buf, 0, 12);
        sprintf(BIG_MODE_buf, "%02d-%s", chan_arv[NOW].CHAN, chan_arv[NOW].NN);
        LCD_ShowString1016(0, 1, BIG_MODE_buf, 1, 128);
        LCD_ShowString0408(0, 3, "THE NICKNAME OF YOUR CHANNEL.", 1);
        break;

    //双守模式
    case DUAL_MODE:
        sprintf(BIG_MODE_buf, "%02d-%s", chan_arv[CHANA].CHAN, chan_arv[CHANA].NN);
        LCD_ShowString0608(13, 1, BIG_MODE_buf, 1, 82);

        sprintf(BIG_MODE_buf, "%02d-%s", chan_arv[CHANB].CHAN, chan_arv[CHANB].NN);
        LCD_ShowString0608(13, 2, BIG_MODE_buf, 1, 82);

        LCD_ShowString0608(82, 2 - sele_pos, "  ", 1, 128); //勾
        LCD_ShowPIC0808(82, sele_pos + 1, 1);

        break;
    }
    //信号和收发状态显示
    if (PTT_READ)
    {
        LCD_ShowAscii0408(0, 0, 'R');
        LCD_ShowSignal(RSSI); //信号检测
    }
    else
    {
        LCD_ShowAscii0408(0, 0, 'T');
        LCD_ShowSignal(100);
    }
}
//

//主页编码器事件处理 //Homepage Coder Events (dammit google translate!)
void Encoder_process(u8 operate)
{
    // D_printf("%s\n", __FUNCTION__);
    switch (operate) //encoder event handling
    {
    case key_click:
        if (Home_Mode == MAIN_MODE)
        {
            LCD_ShowAscii0608(60, 1, ' ', 1);
            ShortCut_Menu();
        }
        else if (Home_Mode == DUAL_MODE)
            sele_pos = (sele_pos + 1) % 2;
        else
            D_printf("BIG_MODE\n");
        break;

    case key_double:
        break;

    case key_long:
        SHUT();
        break;
    }
}
//matrix button event
u8 Event_Matrix(u8 matrix_key)
{
    if (PTT_READ == 0){
        return NO_OPERATE;
    }

    u8 pre_mode = 0;
    switch (matrix_key) {
    case MATRIX_RESULT_1:
        D_printf("{1}\n");
        if (Home_Mode){
            return NO_OPERATE;
        }

        D_printf("NOW : V_SETTING\n");

        pre_mode = get_Flag(FLAG_VU_SWITCH_ADDR); //当前VU频段,用作保留当前的频段
                                                  //不用作"进入不同频段设置"的理由:
                                                  //减少确认后的判断,保证只有在FREQ模式下才修改VU的记忆

        if (get_Flag(FLAG_CF_SWITCH_ADDR)) //FREQ模式下改变显示
        {
            if (pre_mode) //若当前为U段则改变显示为"VHF"
            {
                set_Flag(FLAG_VU_SWITCH_ADDR, 0); //U段变V段
                LCD_ShowString0608(66, 1, "VHF ", 1, 90);
                chan_arv[NOW].CHAN = 0;
            }
        }
        else //CHANNEL模式则显示箭头
            LCD_ShowPIC0608(60, 1, 1, 1);

        //
        switch (RT_FREQ_Set(0, 1, (double *)&chan_arv[NOW].RX_FREQ, 0)) //无论如何都是进行V段设置
        {
        case ENT2LAST:
            chan_arv[NOW].TX_FREQ = chan_arv[NOW].RX_FREQ;
            return SAVE_SET;

        case CLR2LAST:
        case BACK2MAIN:
            if (pre_mode)
            {
                if (get_Flag(FLAG_CF_SWITCH_ADDR))
                {
                    set_Flag(FLAG_VU_SWITCH_ADDR, 1);
                    LCD_ShowString0608(66, 1, "UHF ", 1, 90);
                    chan_arv[NOW].CHAN = 100;
                }
                return NO_OPERATE;
            }
            break;
        }
        break;

    case MATRIX_RESULT_2:
        D_printf("{2}BackLight Switch\n");
        Light_Mode_Set();

        VFO_Clear();
        return NO_OPERATE;

    case MATRIX_RESULT_3:
        Enter_Radio();
        return NO_OPERATE;

    case MATRIX_RESULT_4:
        D_printf("{4}\n");
        if (Home_Mode){
            return NO_OPERATE;
        }

        pre_mode = get_Flag(FLAG_VU_SWITCH_ADDR);
        D_printf("NOW : U_SETTING\n");
        if (get_Flag(FLAG_CF_SWITCH_ADDR))
        {
            if (pre_mode == 0)
            {
                set_Flag(FLAG_VU_SWITCH_ADDR, 1);
                LCD_ShowString0608(66, 1, "UHF ", 1, 90);
                chan_arv[NOW].CHAN = 100;
            }
        }
        else
            LCD_ShowPIC0608(60, 1, 1, 1);

        switch (RT_FREQ_Set(0, 1, (double *)&chan_arv[NOW].RX_FREQ, 1))
        {
        case ENT2LAST:
            chan_arv[NOW].TX_FREQ = chan_arv[NOW].RX_FREQ;
            return SAVE_SET;

        case CLR2LAST:
        case BACK2MAIN:
            if (pre_mode == 0)
            {
                if (get_Flag(FLAG_CF_SWITCH_ADDR))
                {
                    set_Flag(FLAG_VU_SWITCH_ADDR, 0);
                    LCD_ShowString0608(66, 1, "VHF ", 1, 90);
                    chan_arv[NOW].CHAN = 0;
                }
                return NO_OPERATE;
            }
            break;

        default:
            break;
        }
        break;

    case MATRIX_RESULT_5:
        Zero_Menu();
        VFO_Clear();
        break;

    case MATRIX_RESULT_7:
        if (Home_Mode)
            return NO_OPERATE;
        OPTION_Menu();
        VFO_Clear();
        return NO_OPERATE;

    case MATRIX_RESULT_8:
        PGM_Menu();
        VFO_Clear();
        return NO_OPERATE;

    case MATRIX_RESULT_P:
        D_printf("{+}\n");
        TIMES++;
        break;

    case MATRIX_RESULT_N:
        D_printf("{-}\n");
        TIMES--;
        break;

    case MATRIX_RESULT_CLR:
        D_printf("{CLR}\n");
        if (get_Flag(FLAG_CF_SWITCH_ADDR) == 0 && Home_Mode == 0)
            LCD_ShowAscii0608(60, 1, ' ', 1);
        return NO_OPERATE;

    case MATRIX_RESULT_ENT:
        D_printf("{ENT}\n");
        if (Home_Mode)
            return NO_OPERATE;
        LCD_ShowAscii0608(60, 1, ' ', 1);
        ShortCut_Menu();
        return NO_OPERATE;

    case MATRIX_RESULT_LEFT:
        D_printf("{<}\n");
        if (Home_Mode == BIG_MODE)
            return NO_OPERATE;
        else if (Home_Mode == DUAL_MODE)
        {
            sele_pos = (sele_pos + 1) % 2;
            return NO_OPERATE;
        }
        //
        if (get_Flag(FLAG_CF_SWITCH_ADDR))
        {
            D_printf("FREQ>>>CHAN%d\n", __LINE__);
            LCD_ShowPIC0608(60, 1, 1, 1);
            set_Flag(FLAG_CF_SWITCH_ADDR, 0);
            chan_arv[NOW].CHAN = load_CurrentChannel();
        }
        else
        {
            D_printf("CHAN>>>FREQ%d\n", __LINE__);
            LCD_ShowAscii0608(60, 1, ' ', 1);
            set_Flag(FLAG_CF_SWITCH_ADDR, 1);

            chan_arv[NOW].CHAN = (get_Flag(FLAG_VU_SWITCH_ADDR) ? 100 : 0);
            D_printf("CHAN:%d\n", chan_arv[NOW].CHAN);
        }
        return RELOAD_ARG;

    case MATRIX_RESULT_RIGHT:
        D_printf("{>}\n");
        if (Home_Mode == BIG_MODE)
            return NO_OPERATE;
        else if (Home_Mode == DUAL_MODE)
        {
            sele_pos = (sele_pos + 1) % 2;
            return NO_OPERATE;
        }
        //
        if (get_Flag(FLAG_CF_SWITCH_ADDR))
        {
            u8 vu_mode = get_Flag(FLAG_VU_SWITCH_ADDR);
            vu_mode = !vu_mode;
            set_Flag(FLAG_VU_SWITCH_ADDR, vu_mode);
            chan_arv[NOW].CHAN = vu_mode * 100;
            return RELOAD_ARG;
        }
        else
            LCD_ShowAscii0608(60, 1, ' ', 1);
        return NO_OPERATE;

    case MATRIX_RESULT_0:
        if (get_Flag(FLAG_CF_SWITCH_ADDR))
            return NO_OPERATE;
        Home_Mode = (Home_Mode + 1) % 3;
        LCD_Clear(EDITZONE32);
        LCD_ShowString1016(14, 1, "WAITING...", 1, 128);
        delay_ms(1000);
        if (Home_Mode != DUAL_MODE)
            bsp_StopTimer(TMR_DUAL_REFRESH);    //显示模式切换时停止双守切换
        VFO_Clear();                            //刷新页面时检测当前页面是否为双守模式，是则重启双守切换计时
        if (Home_Mode != BIG_MODE)              //切换后的模式不是BIG_MODE模式，关闭常静噪状态
            SetNowChanSql0(OFF);                //关闭静噪模式
        return NO_OPERATE;

    default:
        break;
    }
    if (TIMES > 0){
        TIMES = 0;
        // D_printf("{+}: %d\n", en_times++);
        if (Home_Mode == DUAL_MODE)
        {
            if (sele_pos)
            {
                if (++chan_arv[CHANB].CHAN > 99)
                    chan_arv[CHANB].CHAN = 1;
                save_ChanB(chan_arv[CHANB].CHAN);
                load_ChannelParameter(chan_arv[CHANB].CHAN, &chan_arv[CHANB]);
            }
            else
            {
                if (++chan_arv[CHANA].CHAN > 99)
                    chan_arv[CHANA].CHAN = 1;
                save_ChanA(chan_arv[CHANA].CHAN);
                load_ChannelParameter(chan_arv[CHANA].CHAN, &chan_arv[CHANA]);
            }
            if (SQL_MODE == ON)
            {
                SetNowChanSql0(OFF);                                    //关闭常静噪状态，设置正常静噪并且恢复切换
            }
            return NO_OPERATE;
        }
        if (get_Flag(FLAG_CF_SWITCH_ADDR)) //频率模式
        {
            if (Home_Mode)
                return NO_OPERATE;
            if (get_Flag(FLAG_VU_SWITCH_ADDR)) //U
            {
                if (chan_arv[NOW].RX_FREQ + STEP_LEVEL[STEP] <= 480.0)
                    chan_arv[NOW].RX_FREQ += STEP_LEVEL[STEP];
                else
                    chan_arv[NOW].RX_FREQ = 400.0;
            }
            else
            {
                if (chan_arv[NOW].RX_FREQ + STEP_LEVEL[STEP] <= 174.0)
                    chan_arv[NOW].RX_FREQ += STEP_LEVEL[STEP];
                else
                    chan_arv[NOW].RX_FREQ = 136.0;
            }
            chan_arv[NOW].TX_FREQ = chan_arv[NOW].RX_FREQ;
            return SAVE_SET;
        }
        else //信道模式
        {
            if (++chan_arv[NOW].CHAN > 99)
                chan_arv[NOW].CHAN = 1;
            load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
            if (Home_Mode == 0)
                LCD_ShowPIC0608(60, 1, 1, 1);
            else if (Home_Mode == 1)
            {
                LCD_ShowString1016(0, 1, "       ", 1, 80);
                sprintf(BIG_MODE_buf, "%02d-%s", chan_arv[NOW].CHAN, chan_arv[NOW].NN);
                LCD_ShowString1016(0, 1, BIG_MODE_buf, 1, 128);
            }
            save_CurrentChannel(chan_arv[NOW].CHAN);
            return RELOAD_ARG;  //
        }
    }
    else if (TIMES < 0)
    {
        TIMES = 0;

        if (Home_Mode == DUAL_MODE)
        {
            if (sele_pos)
            {
                if (--chan_arv[CHANB].CHAN < 1)
                    chan_arv[CHANB].CHAN = 99;
                save_ChanB(chan_arv[CHANB].CHAN);
                load_ChannelParameter(chan_arv[CHANB].CHAN, &chan_arv[CHANB]);
            }
            else
            {
                if (--chan_arv[CHANA].CHAN < 1)
                    chan_arv[CHANA].CHAN = 99;
                save_ChanA(chan_arv[CHANA].CHAN);
                load_ChannelParameter(chan_arv[CHANA].CHAN, &chan_arv[CHANA]);
            }
            if (SQL_MODE == ON)
            {
                SetNowChanSql0(OFF);                                    //关闭常静噪状态，设置正常静噪并且恢复切换
            }
            return NO_OPERATE;
        }
        if (get_Flag(FLAG_CF_SWITCH_ADDR)) //频率模式
        {
            if (Home_Mode)
                return NO_OPERATE;
            if (get_Flag(FLAG_VU_SWITCH_ADDR)) //U
            {
                if (chan_arv[NOW].RX_FREQ - STEP_LEVEL[STEP] >= 400.0)
                    chan_arv[NOW].RX_FREQ -= STEP_LEVEL[STEP];
                else
                    chan_arv[NOW].RX_FREQ = 480.0;
            }
            else //V
            {
                if (chan_arv[NOW].RX_FREQ - STEP_LEVEL[STEP] >= 136.0)
                    chan_arv[NOW].RX_FREQ -= STEP_LEVEL[STEP];
                else
                    chan_arv[NOW].RX_FREQ = 174.0;
            }
            chan_arv[NOW].TX_FREQ = chan_arv[NOW].RX_FREQ;
            return SAVE_SET;
        }
        else //信道模式
        {
            if (--chan_arv[NOW].CHAN < 1)
                chan_arv[NOW].CHAN = 99;
            load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
            if (Home_Mode == 0)
                LCD_ShowPIC0608(60, 1, 1, 1);
            else if (Home_Mode == 1)
            {
                LCD_ShowString1016(0, 1, "       ", 1, 80);
                sprintf(BIG_MODE_buf, "%02d-%s", chan_arv[NOW].CHAN, chan_arv[NOW].NN);
                LCD_ShowString1016(0, 1, BIG_MODE_buf, 1, 128);
            }
            save_CurrentChannel(chan_arv[NOW].CHAN);
            return RELOAD_ARG;  //
        }
    }

    return NO_OPERATE;
}

//收发参数发生改变后返回值处理
void Argument_process(u8 key_pro_ret)
{

    switch (key_pro_ret) //矩阵按键事件处理
    {
    case RELOAD_ARG: //通道号发生改变，重新载入参数
        D_printf("****************Reload Argument************\n");
        load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
        Set_A20(chan_arv[NOW], SQL);
        break;

    case SAVE_SET: //频率等参数发生改变，保存并设置一次
        D_printf("****************Save Argument**************\n");
        save_ChannelParameter(chan_arv[NOW].CHAN, chan_arv[NOW]);
        Set_A20(chan_arv[NOW], SQL);
        break;
    default:
        break;
    }
}
//

void PTT_Control(void) //按下和松开PTT后处理
{
    //PTT按键
    if (PTT_READ) //松开PTT
    {
        if (FLAG_PTT_ONCE) //FLAG_PTT_ONCE标志作用为:让一下程序只执行一次
        {
            FLAG_PTT_ONCE = 0;
            TIMES = 0;
            if (END_TONE)
                Start_Tone(0);
            delay_ms(200);
            A002_PTT_SET; //关闭ptt
            
            MIC_SWITCH(AUD, OFF);

            if (WFM)
                bsp_StartTimer(TMR_FM_CTRL, TMR_PERIOD_8S);
            if (Home_Mode == DUAL_MODE) //松开ptt后等待一定时间后 恢复 切换信道
                bsp_StartAutoTimer(TMR_DUAL_REFRESH, DUAL_SWITCH_TIME);//直接恢复切换吧

            if (KDU_INSERT)
                SendALL();
            else
                LCD_ShowAscii0408(0, 0, 'R');

            bsp_StartAutoTimer(TMR_VOLT_REFRESH, TMR_PERIOD_3S); //恢复定时检测电压
        }
    }
    else //按下PTT
    {
        if (FLAG_PTT_ONCE == 0)
        {
            if (PTT_READ == 0)
            {
                FLAG_PTT_ONCE = 1;

                if (WFM) //如果收音机是开的，关闭FM收音机
                {
                    RDA5807_Init(OFF);
                    bsp_StopTimer(TMR_FM_CTRL);
                }
                // 关闭常静噪状态并设置参数
                SetNowChanSql0(OFF);

                A002_PTT_CLR;
                
                if (KDU_INSERT)
                    SendALL();
                else
                    LCD_ShowAscii0408(0, 0, 'T');

                MIC_SWITCH(AUD, ON); //==>发送
                delay_ms(300);      //至少300ms
                if (PRE_TONE)
                    Start_Tone(1); //矩阵的开关在函数内完成

                /////////////////////////
                bsp_StopTimer(TMR_VOLT_REFRESH); //停止电压检测

                //清零关机倒计时
                ClearShut();

            }
        }
    }
    //
}
void SQ_Read_Control(void) //接收和断开信号处理
{
    A002_CALLBACK();
    if(KDU_INSERT == OFF)
        KDUCheck();
    
    if (LAMP_TIME && bsp_CheckTimer(TMR_FLOW))
        BackLight_SetVal(1);
    if (!PTT_READ)
        return;
    //信号判断
    if (A002_SQ_READ)
    {
        if (FLAG_SQ_ONCE)
        {
            FLAG_SQ_ONCE = 0;
            SPK_SWITCH(AUD, OFF);
            M62364_SetSingleChannel(A20_LINE_CHAN, 0); //闲时关闭对讲机声音
            RSSI = 0;
            if (WFM)
                bsp_StartTimer(TMR_FM_CTRL, TMR_PERIOD_8S);
            if (Home_Mode == DUAL_MODE) //恢复双守模式倒计时设置
            {
                if (SQL_MODE == OFF)    //非常静噪状态下, 无信号后直接恢复信道切换
                    bsp_StartAutoTimer(TMR_DUAL_REFRESH, DUAL_SWITCH_TIME);
            }

            if (KDU_INSERT)
            {
                SendALL();
            }
        }
    }
    else
    {
        SPK_SWITCH(AUD, ON);
        if (FLAG_SQ_ONCE == 0)
        {
            if (A002_SQ_READ == 0)
            {
                if (Home_Mode == DUAL_MODE)
                {
                    bsp_StopTimer(TMR_DUAL_REFRESH); //停止切换
                    rcv_chan = now_chan;
                    if (Flag_Main_Page && KDU_INSERT==0)
                    {
                        LCD_ShowString0608(74, 2 - now_chan, " ", 1, 95);
                        LCD_ShowPIC0608(74, now_chan + 1, 0, 1); //箭头表接收
                    }
                }
                M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]); //开对讲机声音
                FLAG_SQ_ONCE = 1;
                if (WFM)
                {
                    RDA5807_Init(OFF);
                    bsp_StopTimer(TMR_FM_CTRL);
                }

                ClearShut();
                if (KDU_INSERT)
                {
                    SendALL();
                }
            }
        }
        //收到信号后每500ms获取一次信号强度
        if (bsp_CheckTimer(TMR_RSSI_CTRL))
        {
            RSSI = Get_A20_RSSI();
        }

        //
    }
    //
    Switch_Dual_Chan();
}

void SetNowChanSql0(u8 on)
{
     if (SQL_MODE == ON) //关闭静噪模式
    {
        SQUELCH_ONCE = 1;
        SQL_MODE = OFF;
    }
    if (Home_Mode == DUAL_MODE)
    {
        if(on)
            bsp_StopTimer(TMR_DUAL_REFRESH);
        else
            bsp_StartAutoTimer(TMR_DUAL_REFRESH, DUAL_SWITCH_TIME);//恢复双守模式的切换

        now_chan = sele_pos;
        if (now_chan)
            Set_A20(chan_arv[CHANB], on?0:SQL);
        else
            Set_A20(chan_arv[CHANA], on?0:SQL);
    }
    else
        Set_A20(chan_arv[NOW], on?0:SQL);
}

void SQUELCH_Contol(void)   //按下和松开静噪处理
{
    if (!PTT_READ)
        return;

    if (SQUELCH_READ == 0)  //按下静噪按键
    {
        if (SQUELCH_ONCE == 0)  //进入静噪0模式  并启动按压计时
        {
            SQUELCH_ONCE = 1;
            if (WFM) //如果收音机是开的，关闭FM收音机
            {
                RDA5807_Init(OFF);
                bsp_StopTimer(TMR_FM_CTRL);
            }
            //
            if (SQL_MODE == OFF) //非长静噪状态
            {
                SetNowChanSql0(ON);                                             //将当前信道的静噪等级设0
                ///////////////////////////////////////////////////
                SPK_SWITCH(AUD, ON);
                M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);      //开对讲机声音
                SQL_CTL = ON;                                                   //启动计数	  //非长静噪模式下才会使sql_cal>=30, 其余为0;
            }
            ClearShut();
        }
        if (sql_cal == 30)      //计时长达三秒， 进入长静噪模式
        {
            SQL_CTL = 0;                                //停止计数
            sql_cal++;                                  //计数已停止,此处为避免重复进入该处理操作
            Start_ToneSql0();                           //常静噪模式提示音
            SQL_MODE = 1;                               //进入长静噪模式
        }
        else if (sql_cal < 30) //不足三秒
            SQL_MODE = OFF;
    }
    else                    //松开静噪按键
    {
        if (SQUELCH_ONCE)
        {
            SQUELCH_ONCE = 0;
            if (sql_cal < 30)
                SPK_SWITCH(AUD, OFF);

            SQL_CTL = 0;
            sql_cal = 0;

            if (SQL_MODE == OFF)
            {
                SetNowChanSql0(OFF);
                ///////////////////////////////////////////////
                if (WFM)
                    bsp_StartTimer(TMR_FM_CTRL, TMR_PERIOD_8S);
            }
        }
    }
    //
}
//
void Switch_Dual_Chan(void) //双守模式时按时切换信道接收
{
    if (bsp_CheckTimer(TMR_DUAL_REFRESH))
    {
        now_chan = (now_chan + 1) % 2;
        if (now_chan)
            Set_A20(chan_arv[CHANB], SQL);
        else
            Set_A20(chan_arv[CHANA], SQL);
        // Serial.printf("Switch Channel:%d\n", now_chan);
    }
}
//

void LoadAgrv2Buf(char *buf, CHAN_ARV B)
{
    sprintf(buf + chan_rank, "%03d", B.CHAN);
    sprintf(buf + rx_rank, "%3.4f", B.RX_FREQ);
    sprintf(buf + tx_rank, "%3.4f", B.TX_FREQ);
    sprintf(buf + rs_rank, "%03d", B.RS);
    sprintf(buf + ts_rank, "%03d", B.TS);
    sprintf(buf + nn_rank, "%s", B.NN);

    buf[pw_rank] = kdu_send_data(B.POWER);
    buf[bw_rank] = kdu_send_data(B.GBW);
    buf[scan_rank] = 0;

    //	for(int i=0; i<7; i++)
    //		buf[nn_rank+i] = B.NN[i];
}

extern unsigned char FM_CHAN;
void SendALL(void) //发送全部数据
{
    char send_buf[256] = {0};
    strcpy(send_buf, prefix_buf[ASKALL]);

    //	chan_arv[NOW].CHAN = CHAN;
    LoadAgrv2Buf(send_buf, chan_arv[NOW]);

    send_buf[cf_rank] = kdu_send_data(get_Flag(FLAG_CF_SWITCH_ADDR));
    send_buf[vu_rank] = kdu_send_data(get_Flag(FLAG_VU_SWITCH_ADDR));
    sprintf(send_buf + chana_rank, "%03d", chan_arv[CHANA].CHAN);
    sprintf(send_buf + chanb_rank, "%03d", chan_arv[CHANB].CHAN);

    send_buf[volume_rank] = kdu_send_data(VOLUME);
    send_buf[step_rank] = kdu_send_data(STEP);
    send_buf[sql_rank] = kdu_send_data(SQL);
    send_buf[aud_rank] = kdu_send_data(AUD);
    send_buf[mic_rank] = kdu_send_data(MIC);
    send_buf[enc_rank] = kdu_send_data(ENC);
    send_buf[tot_rank] = kdu_send_data(TOT);
    send_buf[op_rank] = kdu_send_data(VDO);

    send_buf[pre_rank] = kdu_send_data(PRE_TONE);
    send_buf[end_rank] = kdu_send_data(END_TONE);
    sprintf(send_buf + ffreq_rank, "%04d", FM_FREQ);

    send_buf[wfm_rank] = kdu_send_data(WFM);
    send_buf[fmchan_rank] = kdu_send_data(FM_CHAN);

    if (PTT_READ == 0)
        RSSI = 100;
    else
    {
        if (A002_SQ_READ)
            RSSI = 0;
        else
            RSSI = Get_A20_RSSI();
    }

    sprintf(send_buf + volt_rank, "%03d", Get_Battery_Vol());
    sprintf(send_buf + rssi_rank, "%03d", RSSI);
    send_buf[sq_rank] = kdu_send_data(A002_SQ_READ);
    send_buf[squ_rank] = kdu_send_data(SQUELCH_READ);
    send_buf[ptt_rank] = kdu_send_data(PTT_READ);

    send_buf[homemode_rank] = kdu_send_data(Home_Mode);
    send_buf[nowrcvchan_rank] = kdu_send_data(rcv_chan);
    send_buf[nowselchan_rank] = kdu_send_data(sele_pos);//用于区分双守模式发射信道

    UART1_Send_Message(send_buf, BUF_SIZE);
}

char send_buf[256] = {0};
static int EnterKDUCal = 0;     //3次进入, 不断增加
static int EXitKDUCal  = 0;     //3次退出, 清空EnterKDUCal值

int KDUCheck(void)              //KDU插入检测
{
    if (!PTT_READ)
        return NO_OPERATE;
    KDU_INSERT = OFF;
    //在规定时间内的 询问 少于2次:  1.若有询问则处理反馈, 并且计数; 2.检测计时是否超过, 若超过时间无询问则清空计数值, 152正常运行
    if(EnterKDUCal<2)       
    {
        if(UART1_getRcvFlag())
        {
            UART1_dataPreProcess();
            if (strstr((char *)rx1_buf, prefix_buf[_ASKALL]))
            {
                SendALL();
                EnterKDUCal++;
                bsp_StartTimer(TMR_WAIT_KDU, TMR_PERIOD_1S*1.5);
            }
            else if (strstr((char *)rx1_buf, prefix_buf[_ASKA])) //回复KDU询问的双守A信道参数
            {
                load_ChannelParameter(load_ChanA(), &chan_arv[TMP]);

                //编写回复信息  //发送数据准备
                strcpy(send_buf, prefix_buf[ASKA]);

                LoadAgrv2Buf(send_buf, chan_arv[TMP]);

                send_buf[cf_rank] = kdu_send_data(kdu_recv_data(rx1_buf[cf_rank]));
                send_buf[vu_rank] = kdu_send_data(kdu_recv_data(rx1_buf[vu_rank]));

                UART1_Send_Message(send_buf, BUF_SIZE);
                EnterKDUCal++;
                bsp_StartTimer(TMR_WAIT_KDU, TMR_PERIOD_1S*1.5);
            }
            else if (strstr((char *)rx1_buf, prefix_buf[_ASKB])) //回复KDU询问的双守B信道参数
            {
                load_ChannelParameter(load_ChanB(), &chan_arv[TMP]);

                //编写回复信息  //发送数据准备
                strcpy(send_buf, prefix_buf[ASKB]);
                LoadAgrv2Buf(send_buf, chan_arv[TMP]);

                send_buf[cf_rank] = kdu_send_data(kdu_recv_data(rx1_buf[cf_rank]));
                send_buf[vu_rank] = kdu_send_data(kdu_recv_data(rx1_buf[vu_rank]));

                UART1_Send_Message(send_buf, BUF_SIZE);
                EnterKDUCal++;
                bsp_StartTimer(TMR_WAIT_KDU, TMR_PERIOD_1S*1.5);
            }
            return NO_OPERATE;
        }
        if(bsp_CheckTimer(TMR_WAIT_KDU))                        //检测到计时超过1.5S, 清空计数值并返回
        {
            EnterKDUCal = 0;
            return NO_OPERATE;
        }
    }
    else
    {
        KDU_INSERT = ON;
        return BACK2MAIN;
    }
    return NO_OPERATE;
}

int KDU_Processor(void)         //KDU插入后处理
{
    if(EnterKDUCal>=2)                    //规定时间内询问数据超过2次, 152设置为KDU插入模式;
    {
        static u8 cf = 0; 	//辅助询问信道参数
        BackLight_SetVal(BL);
        LCD_Clear(GLOBAL32);
        LCD_ShowString0608(26, 2, "SEND AND LOAD", 1, 128);
        delay_ms(500);              
        LCD_ShowString0608(26, 2, " KDU CONTROL ", 1, 128);
        BackLight_SetVal(0);
        D_printf("\nEnter KDU Mode\n");
        KDU_INSERT = ON;
        while(1)
        {
            FeedDog(); //喂狗
            switch (Encoder_Switch_Scan(0))
			{
				case key_long:
					SHUT();
					break;
			}
                

            PTT_Control();
            SQUELCH_Contol();
            SQ_Read_Control();
            if (bsp_CheckTimer(TMR_FM_CTRL) && WFM)
                RDA5807_Init(ON);

            if(UART1_getRcvFlag())
            {
                UART1_dataPreProcess();

                if (strstr((char *)rx1_buf, prefix_buf[_ASKALL])) //回复KDU询问的所有参数
                        SendALL();
                //
                else if (strstr((char *)rx1_buf, prefix_buf[_SETHOMEMODE])) //插个队
                {
                    Home_Mode = kdu_recv_data(rx1_buf[homemode_rank]);
                    strcpy(send_buf, prefix_buf[SETHOMEMODE]);
                    send_buf[homemode_rank] = kdu_send_data(Home_Mode);
                    UART1_Send_Message(send_buf, BUF_SIZE);

                    if (Home_Mode == MAIN_MODE)
                    {
                        bsp_StopTimer(TMR_DUAL_REFRESH); //停止切换计时
                        Set_A20(chan_arv[NOW], SQL);
                    }
                    else if (Home_Mode == DUAL_MODE)
                        bsp_StartAutoTimer(TMR_DUAL_REFRESH, DUAL_SWITCH_TIME); //启动500ms切换一次频率
                }
                //

                else if (strstr((char *)rx1_buf, prefix_buf[_ASKA])) //回复KDU询问的双守A信道参数
                {
                    load_ChannelParameter(load_ChanA(), &chan_arv[TMP]);

                    //编写回复信息  //发送数据准备
                    strcpy(send_buf, prefix_buf[ASKA]);

                    LoadAgrv2Buf(send_buf, chan_arv[TMP]);

                    send_buf[cf_rank] = kdu_send_data(kdu_recv_data(rx1_buf[cf_rank]));
                    send_buf[vu_rank] = kdu_send_data(kdu_recv_data(rx1_buf[vu_rank]));

                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                else if (strstr((char *)rx1_buf, prefix_buf[_ASKB])) //回复KDU询问的双守B信道参数
                {
                    load_ChannelParameter(load_ChanB(), &chan_arv[TMP]);

                    //编写回复信息  //发送数据准备
                    strcpy(send_buf, prefix_buf[ASKB]);
                    LoadAgrv2Buf(send_buf, chan_arv[TMP]);

                    send_buf[cf_rank] = kdu_send_data(kdu_recv_data(rx1_buf[cf_rank]));
                    send_buf[vu_rank] = kdu_send_data(kdu_recv_data(rx1_buf[vu_rank]));

                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                else if (strstr((char *)rx1_buf, prefix_buf[_ASKCHAN])) //回复KDU询问的信道参数
                {

                    cf = kdu_recv_data(rx1_buf[cf_rank]);

                    //获取信道号
                    chan_arv[TMP].CHAN = (rx1_buf[chan_rank + 0] - '0') * 100 + (rx1_buf[chan_rank + 1] - '0') * 10 + (rx1_buf[chan_rank + 2] - '0');

                    //通过信道号获取参数
                    load_ChannelParameter(chan_arv[TMP].CHAN, &chan_arv[TMP]);

                    //编写回复信息  //发送数据准备
                    strcpy(send_buf, prefix_buf[ASKCHAN]);
                    LoadAgrv2Buf(send_buf, chan_arv[TMP]);

                    send_buf[cf_rank] = kdu_send_data(kdu_recv_data(rx1_buf[cf_rank]));
                    send_buf[vu_rank] = kdu_send_data(kdu_recv_data(rx1_buf[vu_rank]));

                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                //

                //向KDU发送重载后的信道参数
                else if (strstr((char *)rx1_buf, prefix_buf[_RELOAD])) //重载KDU设置的当前信道, 并将数据信息重新发给KDU用以确认
                {
                    //确定重载  信道号 还是 信道和频率切换
                    cf = kdu_recv_data(rx1_buf[cf_rank]);
                    chan_arv[NOW].CHAN = (rx1_buf[chan_rank + 0] - '0') * 100 + (rx1_buf[chan_rank + 1] - '0') * 10 + (rx1_buf[chan_rank + 2] - '0');

                    if (cf == get_Flag(FLAG_CF_SWITCH_ADDR)) //cf模式不修改
                    {
                        if (cf) //FREQ模式:  vu切换
                            set_Flag(FLAG_VU_SWITCH_ADDR, chan_arv[NOW].CHAN ? 1 : 0);
                        else //CHAN模式:	信道号切换
                            save_CurrentChannel(chan_arv[NOW].CHAN);
                    }
                    else //信道>><<频率
                    {
                        set_Flag(FLAG_CF_SWITCH_ADDR, cf);

                        if (cf)
                            chan_arv[NOW].CHAN = get_Flag(FLAG_VU_SWITCH_ADDR) * 100;
                        else
                            chan_arv[NOW].CHAN = load_CurrentChannel();
                    }
                    //
                    load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);

                    //编写回复信息  //发送数据准备
                    strcpy(send_buf, prefix_buf[RELOAD]);
                    LoadAgrv2Buf(send_buf, chan_arv[NOW]);

                    send_buf[cf_rank] = kdu_send_data(cf);
                    send_buf[vu_rank] = kdu_send_data(kdu_recv_data(rx1_buf[vu_rank]));

                    Set_A20(chan_arv[NOW], SQL);
                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                else if (strstr((char *)rx1_buf, prefix_buf[_RELA])) //重载KDU设置的双守信道A,并将数据信息重新发给KDU用以确认
                {
                    chan_arv[CHANA].CHAN = (rx1_buf[chan_rank + 0] - '0') * 100 + (rx1_buf[chan_rank + 1] - '0') * 10 + (rx1_buf[chan_rank + 2] - '0');
                    save_ChanA(chan_arv[CHANA].CHAN);
                    load_ChannelParameter(chan_arv[CHANA].CHAN, &chan_arv[CHANA]);

                    strcpy(send_buf, prefix_buf[RELA]);
                    LoadAgrv2Buf(send_buf, chan_arv[CHANA]);

                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                else if (strstr((char *)rx1_buf, prefix_buf[_RELB])) //重载KDU设置的双守信道B,并将数据信息重新发给KDU用以确认
                {
                    chan_arv[CHANB].CHAN = (rx1_buf[chan_rank + 0] - '0') * 100 + (rx1_buf[chan_rank + 1] - '0') * 10 + (rx1_buf[chan_rank + 2] - '0');
                    save_ChanB(chan_arv[CHANB].CHAN);
                    load_ChannelParameter(chan_arv[CHANB].CHAN, &chan_arv[CHANB]);

                    strcpy(send_buf, prefix_buf[RELB]);
                    LoadAgrv2Buf(send_buf, chan_arv[CHANB]);
                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                else if (strstr((char *)rx1_buf, prefix_buf[_SETDUALPOS])) //设置双守模式选中的信道，并将数据信息重新发给KDU用以确认
                {  
                    sele_pos = rx1_buf[nowselchan_rank] - '0';
                    strcpy(send_buf, prefix_buf[SETDUALPOS]);
                    send_buf[nowselchan_rank] = kdu_send_data(sele_pos);
                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                //
                //接收KDU设置的信道参数, 并将数据信息重新发给KDU用以确认, 预防KDU未反应
                else if (strstr((char *)rx1_buf, prefix_buf[_SETCHAN]))
                {
                    chan_arv[NOW].CHAN = (rx1_buf[chan_rank + 0] - '0') * 100 + (rx1_buf[chan_rank + 1] - '0') * 10 + (rx1_buf[chan_rank + 2] - '0');

                    chan_arv[NOW].RX_FREQ = (rx1_buf[rx_rank + 0] - '0') * 100 + (rx1_buf[rx_rank + 1] - '0') * 10 + (rx1_buf[rx_rank + 2] - '0') + (rx1_buf[rx_rank + 4] - '0') * 0.1 +
                                            (rx1_buf[rx_rank + 5] - '0') * 0.01 + (rx1_buf[rx_rank + 6] - '0') * 0.001 + (rx1_buf[rx_rank + 7] - '0') * 0.0001;
                    chan_arv[NOW].TX_FREQ = (rx1_buf[tx_rank + 0] - '0') * 100 + (rx1_buf[tx_rank + 1] - '0') * 10 + (rx1_buf[tx_rank + 2] - '0') + (rx1_buf[tx_rank + 4] - '0') * 0.1 +
                                            (rx1_buf[tx_rank + 5] - '0') * 0.01 + (rx1_buf[tx_rank + 6] - '0') * 0.001 + (rx1_buf[tx_rank + 7] - '0') * 0.0001;

                    chan_arv[NOW].RS = (rx1_buf[rs_rank + 0] - '0') * 100 + (rx1_buf[rs_rank + 1] - '0') * 10 + (rx1_buf[rs_rank + 2] - '0');
                    chan_arv[NOW].TS = (rx1_buf[ts_rank + 0] - '0') * 100 + (rx1_buf[ts_rank + 1] - '0') * 10 + (rx1_buf[ts_rank + 2] - '0');

                    chan_arv[NOW].POWER = kdu_recv_data(rx1_buf[pw_rank]);
                    chan_arv[NOW].GBW = kdu_recv_data(rx1_buf[bw_rank]);

                    for (u8 i = 0; i < 7; i++)
                        chan_arv[NOW].NN[i] = rx1_buf[nn_rank + i];

                    if (chan_arv[NOW].CHAN > 0 && chan_arv[NOW].CHAN < 100)
                        save_CurrentChannel(chan_arv[NOW].CHAN);

                    //回复KDU表示设置成功
                    strcpy(send_buf, prefix_buf[SETCHAN]);
                    LoadAgrv2Buf(send_buf, chan_arv[NOW]);

                    //
                    save_ChannelParameter(chan_arv[NOW].CHAN, chan_arv[NOW]);
                    Set_A20(chan_arv[NOW], SQL); //当KDU按键过快时:   这两放发送前,KDU反应会迟缓;  放后面则无法处理紧接的数据而没返回给KDU造成链接丢失bug
                    //

                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                //
                else if (strstr((const char *)rx1_buf, prefix_buf[_SETZERO]))
                {
                    set_Flag(RESETADDR, ~RESET_VAL);
                    SHUT();
                }
                else if (strstr((const char *)rx1_buf, prefix_buf[_SETSTEP]))
                {
                    STEP = kdu_recv_data(rx1_buf[step_rank]);
                    save_Step(STEP);

                    strcpy(send_buf, prefix_buf[SETSTEP]);
                    send_buf[step_rank] = kdu_send_data(STEP);
                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                else if (strstr((const char *)rx1_buf, prefix_buf[_SETSQL]))
                {
                    if (SQL != kdu_recv_data(rx1_buf[sql_rank]))
                    {
                        SQL = kdu_recv_data(rx1_buf[sql_rank]);
                        save_Sql(SQL);
                        Set_A20(chan_arv[NOW], SQL);
                    }
                    strcpy(send_buf, prefix_buf[SETSQL]);
                    send_buf[sql_rank] = kdu_send_data(SQL);

                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                else if (strstr((const char *)rx1_buf, prefix_buf[_SETAUD]))
                {
                    if (kdu_recv_data(rx1_buf[aud_rank]) != AUD)
                    {
                        SPK_SWITCH(AUD, OFF);
                        AUD = kdu_recv_data(rx1_buf[aud_rank]);

                        if (!A002_SQ_READ) //有信号,直接修改
                            SPK_SWITCH(AUD, ON);
                        else
                        {
                            if (WFM)
                                SPK_SWITCH(AUD, ON);
                        }
                        //
                    }
                    if (AUD == 0)
                        MIC = 1;
                    else
                        MIC = kdu_recv_data(rx1_buf[mic_rank]);

                    save_AudioSelect(AUD);
                    save_MicLevel(MIC);
                    M62364_SetSingleChannel(4, MIC_LEVEL[MIC]);

                    strcpy(send_buf, prefix_buf[SETAUD]);
                    send_buf[aud_rank] = kdu_send_data(AUD);
                    send_buf[mic_rank] = kdu_send_data(MIC);

                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                else if (strstr((const char *)rx1_buf, prefix_buf[_SETENC]))
                {
                    ENC = kdu_recv_data(rx1_buf[enc_rank]);
                    save_ScramLevel(ENC);

                    strcpy(send_buf, prefix_buf[SETENC]);
                    send_buf[enc_rank] = kdu_send_data(ENC);

                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                else if (strstr((const char *)rx1_buf, prefix_buf[_SETTOT]))
                {
                    TOT = kdu_recv_data(rx1_buf[tot_rank]);
                    save_Tot(TOT);

                    strcpy(send_buf, prefix_buf[SETTOT]);
                    send_buf[tot_rank] = kdu_send_data(TOT);

                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                else if (strstr((const char *)rx1_buf, prefix_buf[_SETOP]))
                {
                    if (VDO != kdu_recv_data(rx1_buf[op_rank]))
                    {
                        VDO = kdu_recv_data(rx1_buf[op_rank]);
                        save_VDO(VDO);
                        VDO_SWITCH(VDO);
                    }
                    strcpy(send_buf, prefix_buf[SETOP]);
                    send_buf[op_rank] = kdu_send_data(VDO);

                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                else if (strstr((const char *)rx1_buf, prefix_buf[_SETVOLU]))
                {
                    if (VOLUME != kdu_recv_data(rx1_buf[volume_rank]))
                    {
                        VOLUME = kdu_recv_data(rx1_buf[volume_rank]);
                        save_OverVolume(VOLUME);
                        //需要注意收到信号/FM使用中的音量修改
                        if (WFM) //开着收音机的时候不中断输出，直接修改音量
                        {
                            RDA5807_ResumeImmediately();
                            if (!A002_SQ_READ)
                            {
                                M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[0]);
                                M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);
                            }
                            else
                                M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[VOLUME]);
                        }
                        else
                        {
                            if (A002_SQ_READ)
                                SPK_SWITCH(AUD, 0);
                            M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[0]); //0
                            M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);
                        }
                    }
                    //
                    strcpy(send_buf, prefix_buf[SETVOLU]);
                    send_buf[volume_rank] = kdu_send_data(VOLUME);

                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                else if (strstr((const char *)rx1_buf, prefix_buf[_SETTONE]))
                {
                    PRE_TONE = kdu_recv_data(rx1_buf[pre_rank]);
                    END_TONE = kdu_recv_data(rx1_buf[end_rank]);
                    save_PreTone(PRE_TONE);
                    save_EndTone(END_TONE);

                    strcpy(send_buf, prefix_buf[SETTONE]);
                    send_buf[pre_rank] = kdu_send_data(PRE_TONE);
                    send_buf[end_rank] = kdu_send_data(END_TONE);

                    UART1_Send_Message(send_buf, BUF_SIZE);
                }

                //
                else if (strstr((const char *)rx1_buf, prefix_buf[_SETFM]))
                {
                    int fmfreq = (rx1_buf[ffreq_rank + 0] - '0') * 1000 + (rx1_buf[ffreq_rank + 1] - '0') * 100 + (rx1_buf[ffreq_rank + 2] - '0') * 10 + (rx1_buf[ffreq_rank + 3] - '0');
                    if (WFM != kdu_recv_data(rx1_buf[wfm_rank])) //进行FM的开关
                    {
                        WFM = kdu_recv_data(rx1_buf[wfm_rank]);
                        RDA5807_Init(WFM);
                        if (WFM == OFF && A002_SQ_READ && PTT_READ)
                            SPK_SWITCH(AUD, OFF);
                    }
                    else //切换频率,并对频率进行判断
                    {
                        RDA5807_Set_Freq(fmfreq);
                        if (RDA5807_ReadReg(0xb) & 0x0100)
                        {
                            FM_CHAN = 1;
                            save_FMFreq(fmfreq);
                        }
                        else
                            FM_CHAN = 0;
                    }
                    //                                  SendALL();
                    FM_FREQ = fmfreq;                       //轮询时需要
                    strcpy(send_buf, prefix_buf[SETFM]);
                    sprintf(send_buf + ffreq_rank, "%04d", fmfreq);
                    send_buf[wfm_rank] = kdu_send_data(WFM);
                    send_buf[fmchan_rank] = kdu_send_data(FM_CHAN);
                    UART1_Send_Message(send_buf, BUF_SIZE);
                }

                /////////////////////////////////////////////////////////////////////////////////////////////////
                UART1_EnRCV();
                memset(send_buf, '0', 256);
                ClearShut();
                EnterKDUCal++;
                EXitKDUCal = 0;
                bsp_StartAutoTimer(TMR_WAIT_KDU, TMR_PERIOD_1S*1.2);
            }

            if(bsp_CheckTimer(TMR_WAIT_KDU))                        //检测到计时超过1.5S, 计算KDU退出值
            {
                if(EXitKDUCal ++ >=2)
                {
                    KDU_INSERT = OFF;
                    EnterKDUCal = 0;
                    EXitKDUCal = 0;
                    bsp_StopTimer(TMR_WAIT_KDU);
                    TIMES = 0;
                    VFO_Clear();
                    VFO_Refresh();
                    ClearShut();
                    return BACK2MAIN;
                }
            }

        }
    }
    return NO_OPERATE;
}

void VOL_Reflash(void) {
    //音量设置
    int volume_change = 0;

    switch (VolumeKeyScan(0)) {
    case 1:
        if (VOLUME < 7) {
            volume_change = 1;
            VOLUME++;
        }
        break;
    case 2:
        if (VOLUME > 0) {
            volume_change = 1;
            VOLUME--;
        }
        break;
    default:
        volume_change = 0;
        break;
    };
    if (volume_change) {
        volume_change = 0;
        save_OverVolume(VOLUME);
        LCD_ShowVolume(VOLUME);
        if (WFM) {
            //开着收音机的时候不中断输出，直接修改音量
            RDA5807_ResumeImmediately();
            if (!A002_SQ_READ) {
                M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[0]);
                M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);
            } else {
                M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[VOLUME]);
            }
        } else {
            if (A002_SQ_READ){
                SPK_SWITCH(AUD, 0);
            }
            M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[0]); //0
            M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);
        }
    }

    if (bsp_CheckTimer(TMR_VOLT_REFRESH) && PTT_READ && KDU_INSERT==0){
        LCD_ShowBattery(Get_Battery_Vol());
    }
}
void MY_GLOBAL_FUN(void) //全局功能函数
{
    // D_printf("%s\n", __FUNCTION__);
    FeedDog(); //喂狗
    PTT_Control();

    SQUELCH_Contol();
    SQ_Read_Control();  //接收信号处理，A002设置应答，双守切换处理，KDU检测处理

    VOL_Reflash();

    if (bsp_CheckTimer(TMR_FM_CTRL) && WFM){
        RDA5807_Init(ON);
    }
}

//

//quick settings on the main interface
void ShortCut_Menu(void){
    u8 option_num = 0,
       Inc_select_change = 1,
       ENSURE = 0;

    TIMES = 0;
    //	LCD_ShowPIC0608(16, 3, 0, 1); //show < point sto the first option
    while (1)
    {
        MY_GLOBAL_FUN();
        if (KDU_INSERT)
            return;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_CLR: //退出
            LCD_ShowString0408(0, 3, "TYPE   TRF    MOD    CHAN  KEY  ", 1);
            return;

        case MATRIX_RESULT_ENT:
        case MATRIX_RESULT_0:
            ENSURE = 1;
            break;

        case MATRIX_RESULT_RIGHT:
        case MATRIX_RESULT_P:
            TIMES++;
            break;

        case MATRIX_RESULT_LEFT:
        case MATRIX_RESULT_N:
            TIMES--;
            break;
        };
        if (TIMES > 0)
        {
            option_num = (option_num + 1) % 5;
            TIMES = 0;
            Inc_select_change = 1;
        }
        else if (TIMES < 0)
        {
            option_num = (option_num + 5 - 1) % 5;
            TIMES = 0;
            Inc_select_change = 1;
        }

        if (Inc_select_change) //显示箭头，避免重复刷新
        {
            Inc_select_change = 0;
            LCD_ShowString0408(0, 3, "TYPE   TRF    MOD    CHAN  KEY  ", 1);
            switch (option_num)
            {
            case 0:
                LCD_ShowPIC0608(16, 3, 0, 1);
                break;
            case 1:
                LCD_ShowPIC0608(40, 3, 0, 1);
                break;
            case 2:
                LCD_ShowPIC0608(68, 3, 0, 1);
                break;
            case 3:
                LCD_ShowPIC0608(100, 3, 0, 1);
                break;
            case 4:
                LCD_ShowPIC0608(120, 3, 0, 1);
                break;
            }
        }

        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            ENSURE = 1;
            break;

        case key_double:
            LCD_ShowString0408(0, 3, "TYPE   TRF    MOD    CHAN  KEY  ", 1);
            return;

        case key_long:
            SHUT();
            break;
        }
        if (ENSURE)
        {
            ENSURE = 0;
            switch (option_num)
            {
            case 0: //图形菜单
                TIMES = 0;
                D_printf("TYPE\n");
                RT_Menu();
                VFO_Clear();
                return;

            case 1: //咪模式
                if (AUD == 0)
                    break;
                ShortCut_MICGAIN_Select();
                break;

            case 2: //收音机
#if FM_EN
                ShortCut_FM_Select();
#endif
                break;

            case 3: //channel模式下的通道号选择
                if (!PTT_READ)
                    break;
                ShortCut_CHAN_Select();
                break;
                //

            case 4: //锁屏
                LCD_ShowString0408(0, 3, "TYPE   TRF    MOD    CHAN  KEY  ", 1);
                Lock_Screen_KeyBoard();
                ClearShut();

                while (VOL_ADD_READ == 0 || VOL_SUB_READ == 0)
                    FeedDog(); //喂狗
                break;
            }
            LCD_ShowString0408(0, 3, "TYPE   TRF    MOD    CHAN  KEY  ", 1);
            return;
        }
    }
}
//
void ShortCut_MICGAIN_Select(void) //主界面快捷设置mic灵敏度
{
    TIMES = 0;
    char trf = MIC, trf_old = MIC;
    D_printf("TRF\n");
    LCD_ShowString0608(26, 2, TRF_Show[trf], 0, 128); //显示选中的等级
    trf_old = trf;
    while (1)
    {
        MY_GLOBAL_FUN();
        if (KDU_INSERT)
            return;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_P:
        case MATRIX_RESULT_RIGHT:
            TIMES++;
            break;

        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT:
            TIMES--;
            break;

        case MATRIX_RESULT_CLR:
            LCD_ShowString0608(26, 2, TRF_Show[trf_old], 1, 128);
            return;

        case MATRIX_RESULT_ENT:
            LCD_ShowString0608(26, 2, TRF_Show[trf], 1, 128);
            MIC = trf;
            save_MicLevel(MIC);
            M62364_SetSingleChannel(4, MIC_LEVEL[MIC]);

            return;
        }
        //
        if (TIMES > 0)
        {
            trf = (trf + TIMES) % 3;
            TIMES = 0;
            LCD_ShowString0608(26, 2, TRF_Show[trf], 0, 128);
        }
        else if (TIMES < 0)
        {
            trf = (trf + 3 - ((-TIMES) % 3)) % 3;
            TIMES = 0;
            LCD_ShowString0608(26, 2, TRF_Show[trf], 0, 128);
        }

        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            LCD_ShowString0608(26, 2, TRF_Show[trf], 1, 128);
            MIC = trf;
            save_MicLevel(MIC);
            M62364_SetSingleChannel(4, MIC_LEVEL[MIC]);
            return;

        case key_double:
            LCD_ShowString0608(26, 2, TRF_Show[trf_old], 1, 128);
            return;

        case key_long:
            SHUT();
        }
    }
    //
}

void ShortCut_FM_Select(void) //主界面快捷开关收音机
{
    TIMES = 0;
    char FM_now = WFM, FM_old = WFM;
    LCD_ShowString0608(56, 2, FM_Show[FM_now], 0, 128);
    while (1)
    {
        MY_GLOBAL_FUN();
        if (KDU_INSERT)
            return;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_P:
        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT:
        case MATRIX_RESULT_RIGHT:
            TIMES++;
            break;

        case MATRIX_RESULT_ENT:
            LCD_ShowString0608(56, 2, FM_Show[FM_now], 1, 128);
            if (WFM == FM_now)
                return;
            WFM = FM_now;
            RDA5807_Init(FM_now);
            SPK_SWITCH(AUD, FM_now);
            if (WFM)
                Enter_Radio();
            return;

        case MATRIX_RESULT_CLR:
            LCD_ShowString0608(56, 2, FM_Show[FM_old], 0, 128);
            return;
        }
        if (TIMES != 0)
        {
            FM_now = !FM_now;
            TIMES = 0;
            LCD_ShowString0608(56, 2, FM_Show[FM_now], 0, 128);
        }
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            LCD_ShowString0608(56, 2, FM_Show[FM_now], 1, 128);
            if (WFM == FM_now)
                return;
            WFM = FM_now;
            RDA5807_Init(FM_now);

            SPK_SWITCH(AUD, FM_now);
            if (FM_now)
                Enter_Radio();
            return;

        case key_double:
            LCD_ShowString0608(56, 2, FM_Show[FM_old], 0, 128);
            return;

        case key_long:
            SHUT();
        }
    }
    //
}

//主界面信道切换
void Channel_Info_Show(unsigned char channel) //通道选中信息显示
{
    LCD_ShowChan(83, 2, channel, 0); //显示channel号
    load_ChannelParameter(channel, &chan_arv[TMP]);
    LCD_ShowFreq(0, 1, chan_arv[TMP].RX_FREQ, 1); //显示接收频率
    return;
}
void ShortCut_CHAN_Select(void)
{
    TIMES = 0;
    char chan = chan_arv[NOW].CHAN;
    if (get_Flag(FLAG_CF_SWITCH_ADDR))
        return;

    LCD_ShowChan(83, 2, chan, 0);
    while (1)
    {
        FeedDog(); //喂狗
        if (KDU_INSERT)
            return;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_0:
            chan = 1;
            Channel_Info_Show(chan);
            break;

        case MATRIX_RESULT_P:
        case MATRIX_RESULT_RIGHT:
            TIMES++;
            break;

        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT:
            TIMES--;
            break;

        case MATRIX_RESULT_ENT:
            LCD_ShowAscii0608(60, 1, ' ', 1);
            chan_arv[NOW].CHAN = chan;
            save_CurrentChannel(chan_arv[NOW].CHAN);
            load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
            return;

        case MATRIX_RESULT_CLR:
            LCD_ShowAscii0608(60, 1, ' ', 1);
            return;
        }
        if (TIMES != 0)
        {
            chan += TIMES;
            TIMES = 0;
            if (chan > 99)
                chan = 1;
            if (chan < 1)
                chan = 99;
            Channel_Info_Show(chan);
        }

        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            LCD_ShowAscii0608(60, 1, ' ', 1);
            chan_arv[NOW].CHAN = chan;
            save_CurrentChannel(chan_arv[NOW].CHAN);
            load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
            return;

        case key_double:
            LCD_ShowAscii0608(60, 1, ' ', 1);
            return;

        case key_long:
            SHUT();
            break;
        }
    }
}

//主菜单图形1： 收发设置
void RT_Menu()
{
    TIMES = 0;
    Flag_Main_Page = 0;

    char
        now_mode = 0,           //当前进入的vu模式
        pre_mode = 0,           //进入之前的vu模式
        pos = 0,                //光标应在选项
        change = 0,             //数据修改保存
        FLAG_Modify = 0,        //确认选项，进入修改
        FLAG_FLASH_OPTIONS = 0; //更新光标标志

    pre_mode = (chan_arv[NOW].RX_FREQ > 174);
    now_mode = pre_mode;

    RT_Menu_Clear();
    LCD_ShowMatrixMenu22(matrix_menu1, 8, pos);    //(const char[][2][12])matrix_menu1

    while (1)
    {
        MY_GLOBAL_FUN();
        if (KDU_INSERT)
            return;

        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_1:
            if (!PTT_READ)
                break;
            if (pos == 0)
            {
                pre_mode = (chan_arv[NOW].RX_FREQ > 174); //根据频率保存进入之前属于V段还是U段
                now_mode = 0;                             //当前设置频率范围定为:136-174
                FLAG_Modify = 1;
            }
            else if (pos == 2)
            {
                if (chan_arv[NOW].RX_FREQ > 174) //设置发射频率选项时发现接收频率U段则不允许设置136-174
                    break;
                now_mode = 0;
                FLAG_Modify = 1;
            }
            break;

        case MATRIX_RESULT_4:
            if (!PTT_READ)
                break;
            if (pos == 0)
            {
                pre_mode = (chan_arv[NOW].RX_FREQ > 174); //设置接收频率前保留当前V段还是U段,
                now_mode = 1;                             //当前设置频率范围定为:400-480
                FLAG_Modify = 1;
            }
            else if (pos == 2)
            {
                if (chan_arv[NOW].RX_FREQ < 400) //设置发射频率选项时发现接收频率V段则不允许设置400-480
                    break;
                now_mode = 1;
                FLAG_Modify = 1;
            }
            break;

        case MATRIX_RESULT_ENT:
            if ((pos == 6 && get_Flag(FLAG_CF_SWITCH_ADDR)) || !PTT_READ)
                break;
            FLAG_Modify = 1;
            if (pos == 0 || pos == 2)
            {
                pre_mode = (chan_arv[NOW].RX_FREQ > 174); //获取进入之前的频段
                now_mode = pre_mode;
            }

            break;

        case MATRIX_RESULT_CLR:
            D_printf("Back\n");
            return;

        case MATRIX_RESULT_P:
            D_printf("Turn Up\n");
            TIMES -= 2;
            break;

        case MATRIX_RESULT_N:
            D_printf("Turn Down\n");
            TIMES += 2;
            break;

        case MATRIX_RESULT_LEFT:
            TIMES--;
            break;
        case MATRIX_RESULT_RIGHT:
            TIMES++;
            break;
        };
        if (TIMES != 0)
        {
            pos = (pos + TIMES + 8) % 8;
            TIMES = 0;
            FLAG_FLASH_OPTIONS = 1;
        }

        if (FLAG_Modify)
        {

            switch (pos)
            //根据按下的当前位置进入菜单
            {
            case 0:
                D_printf("\t\t\t\tTX_FREQ Setting\n");
                //先判断当前是频率还是信道模式		  CHAN 模式下 直接设置频率
                if (get_Flag(FLAG_CF_SWITCH_ADDR)) //FREQ 模式先 切换VU段后重装参数再作修改
                {
                    if (now_mode != pre_mode) //VU互切 重载数据
                    {
                        chan_arv[NOW].CHAN = now_mode * 100;
                        load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
                    }
                    //频段不变也是直接设置频率
                }

                if (now_mode)
                    LCD_ShowString0408(0, 3, "FRE SET:400-480MHZ,CLR OR ENT", 1);
                else
                    LCD_ShowString0408(0, 3, "FRE SET:136-174MHZ,CLR OR ENT", 1);

                //设置频率完成后,进入保存和设置A20步骤
                if (RT_FREQ_Set(12, 1, (double *)&chan_arv[NOW].RX_FREQ, now_mode) == ENT2LAST)
                {
                    chan_arv[NOW].TX_FREQ = chan_arv[NOW].RX_FREQ;
                    change = 1;
                    if (get_Flag(FLAG_CF_SWITCH_ADDR))
                    {
                        if (now_mode != pre_mode)
                            set_Flag(FLAG_VU_SWITCH_ADDR, now_mode);
                    }
                }
                else
                {
                    //不保存设置退出  判断CF模式
                    if (get_Flag(FLAG_CF_SWITCH_ADDR))
                    {
                        //频率模式下, 如果切换了频段则重新载入
                        if (now_mode != pre_mode)
                        {
                            chan_arv[NOW].CHAN = pre_mode * 100;
                            load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
                        }
                    }
                }
                break;

            case 1:
                D_printf("\t\t\t\tRX CTCSS Selecting\n");
                chan_arv[NOW].RS = RT_SubVoice_Set(1, chan_arv[NOW].RS);
                change = 1;
                break;

            case 2:
                D_printf("\t\t\t\tTX_FREQ_Setting\n");

                if (now_mode)
                    LCD_ShowString0408(0, 3, "FRE SET:400-480MHZ,CLR OR ENT", 1);
                else
                    LCD_ShowString0408(0, 3, "FRE SET:136-174MHZ,CLR OR ENT", 1);

                if (RT_FREQ_Set(12, 2, (double *)&chan_arv[NOW].TX_FREQ, now_mode) == ENT2LAST)
                    change = 1;
                break;

            case 3:
                D_printf("\t\t\t\tTX CTCSS Selecting\n");
                chan_arv[NOW].TS = RT_SubVoice_Set(2, chan_arv[NOW].TS);
                change = 1;
                break;

            case 4:
                D_printf("\t\t\t\tPower Selecting\n");
                chan_arv[NOW].POWER = RT_TX_POWER_Set(chan_arv[NOW].POWER);
                change = 1;
                break;

            case 5:
                D_printf("\t\t\t\tBand Selecting\n");
                chan_arv[NOW].GBW = RT_GBW_Set(chan_arv[NOW].GBW);
                change = 1;
                break;

            case 6:
                D_printf("\t\t\t\tNickName Setting\n");
                if (chan_arv[NOW].CHAN > 0 && chan_arv[NOW].CHAN < 100)
                {
                    RT_NICKNAME_Set(chan_arv[NOW].CHAN, (u8 *)chan_arv[NOW].NN); //////改了未验证
                    save_ChannelParameter(chan_arv[NOW].CHAN, chan_arv[NOW]);
                    change = 0;
                }
                break;

            case 7:
                D_printf("\t\t\t\tCHAN Select && CF_SWITCH \n");
                RT_CHAN_Switch();
                now_mode = (chan_arv[NOW].RX_FREQ > 174);
                change = 0; //由于在选择通道号的时候，相关参数信息需要跟随改变，故设置A002收发参数放置在此模块中，则change=0即可
                break;
            };

            if (change)
            {
                save_ChannelParameter(chan_arv[NOW].CHAN, chan_arv[NOW]);
                Set_A20(chan_arv[NOW], SQL);
            }

            FLAG_Modify = 0;
            FLAG_FLASH_OPTIONS = 1;
            RT_Menu_Clear();
        }
        if (FLAG_FLASH_OPTIONS) //当前选择选项显示
        {
            FLAG_FLASH_OPTIONS = 0;
            LCD_ShowMatrixMenu22(matrix_menu1, 8, pos);
        }
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            FLAG_Modify = 1;
            break;

        case key_double:
            return;

        case key_long:
            SHUT();
            break;
        }
    }
}

void RT_Menu_Clear() //矩阵数据置位
{
    sprintf(matrix_menu1[0][0], "R:%3.4f", chan_arv[NOW].RX_FREQ);
    sprintf(matrix_menu1[0][1], "%s", menu_subvoice[chan_arv[NOW].RS]);
    sprintf(matrix_menu1[1][0], "T:%3.4f", chan_arv[NOW].TX_FREQ);
    sprintf(matrix_menu1[1][1], "%s", menu_subvoice[chan_arv[NOW].TS]);

    matrix_menu1[3][0][0] = 'C';
    matrix_menu1[3][0][1] = 'N';
    matrix_menu1[3][0][2] = ':';
    if (get_Flag(FLAG_CF_SWITCH_ADDR)) //FREQ
    {
        sprintf((char *)matrix_menu1[3][1], "FREQ    ");
        if (get_Flag(FLAG_VU_SWITCH_ADDR))
            sprintf(matrix_menu1[3][0] + 3, "%s", "UHF    ");
        else
            sprintf(matrix_menu1[3][0] + 3, "%s", "VHF    ");
    }
    else //CHAN
    {
        sprintf((char *)matrix_menu1[3][1], "CH-%02d   ", chan_arv[NOW].CHAN);
        memset(matrix_menu1[3][0] + 3, 32, 7);
        memcpy(matrix_menu1[3][0] + 3, (char *)chan_arv[NOW].NN, 7);
    }

    sprintf((char *)matrix_menu1[2][0], (chan_arv[NOW].POWER == 1) ? "PWR:LOW   " : "PWR:HIGH  ");
    sprintf((char *)matrix_menu1[2][1], (chan_arv[NOW].GBW == 1) ? "BW:WIDE " : "BW:NARR ");
}

//频率校验
//0:频率无需校正
//其他:校正的频率
int checkFreq(int freq_tmp)
{
    int step_temp[3] = {50, 100, 125};
    int mul = 0;
    D_printf("freq_tmp:%d\n", freq_tmp);
    if (get_Flag(FLAG_CF_SWITCH_ADDR))
    {
        if (freq_tmp % step_temp[STEP] == 0)
            return 0;
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            if (freq_tmp % step_temp[i] == 0)
                return 0;
        }
    }
    //默认以设置的步进校正频率
    mul = freq_tmp / step_temp[STEP] + 0.5;
    D_printf("mul:%d, return: %d\n", mul, mul * step_temp[STEP]);
    return (mul * step_temp[STEP]);
}
//				收发设置：频率设置
//	确认返回:ENT2LAST
//	取消返回:CLR2LAST
//	KDU控制:BACK2MAIN
int RT_FREQ_Set(int x, int y, double *result, int vu_mode)
{
    unsigned char
        locate = x + 6,
        bit = 1,
        flag_finish = 0, /*flag_finish 停止输入标志（完成)*/
        key_result = 23,
        num_input = 0,
        freq_buf[8] = {0};

    int int_freq = 0;

    double freq = *result;
    LCD_ShowPIC0608(60, y, 0, 1);

    LCD_ShowString0608(x, y, "        ", 1, 120);
    if (vu_mode)
    {
        freq_buf[0] = 4;
        LCD_ShowAscii0608(x, y, '4', 1); //进入后首位设为4
    }
    else
    {
        freq_buf[0] = 1;
        LCD_ShowAscii0608(x, y, '1', 1); //进入后首位设为4
    }
    //
    bsp_StartAutoTimer(TMR_OUT_CTRL, TMR_PERIOD_8S);
    while (1)
    {
        FeedDog(); //喂狗
        if (KDU_INSERT || bsp_CheckTimer(TMR_OUT_CTRL))
        {
            bsp_StopTimer(TMR_OUT_CTRL);
            return BACK2MAIN;
        }

        if (locate == (18 + x)) //3个单位整数后面加个‘.’
        {
            LCD_ShowAscii0608(locate, y, '.', 1);
            locate += 6;
            bit++;
        }
        if (locate > x && locate < 48 + x) //未设置位设为'_'
            LCD_ShowAscii0608(locate, y, '_', 1);
        if (locate == 48 + x || bit == 8) //8个单位设置完了
            flag_finish = 1;

        key_result = Matrix_KEY_Scan(0);
        if (key_result != MATRIX_RESULT_ERROR)
            ReloadOutCal();
        switch (key_result)
        {
        case MATRIX_RESULT_0:
        case MATRIX_RESULT_1:
        case MATRIX_RESULT_2:
        case MATRIX_RESULT_3:
        case MATRIX_RESULT_4:
        case MATRIX_RESULT_5:
        case MATRIX_RESULT_6:
        case MATRIX_RESULT_7:
        case MATRIX_RESULT_8:
        case MATRIX_RESULT_9:
            D_printf("Press{%d}", key_result);
            num_input = 1;
            break;

        case MATRIX_RESULT_CLR:
            if (locate == x + 6) //exit
            {
                D_printf("Press{CLR} : Exit\n");
                LCD_ShowFreq(x, y, *result, 1);
                return CLR2LAST;
            }
            else //clear
            {
                D_printf("Press{CLR} : %d\n", locate); //A
                LCD_ShowString0608(x, y, "        ", 1, 120);

                while (bit--)
                    freq_buf[bit] = 0;
                if (vu_mode)
                {
                    freq_buf[0] = 4;
                    LCD_ShowAscii0608(x, y, '4', 1);
                }
                else
                {
                    freq_buf[0] = 1;
                    LCD_ShowAscii0608(x, y, '1', 1);
                }
                locate = x + 6;
                bit = 1;
            }
            break;

        case MATRIX_RESULT_ENT:
            D_printf("Press{ENT}\n"); //B
            flag_finish = 1;
            break;
        }
        //

        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            flag_finish = 1;
            break;

        case key_double:
            bsp_StopTimer(TMR_OUT_CTRL);
            LCD_ShowFreq(x, y, *result, 1);
            return CLR2LAST;

        case key_long:
            SHUT();
            break;
        }
        //
        if (num_input) //数字存入
        {
            num_input = 0;
            D_printf("_[%d]_: %d\n", bit, key_result);
            freq_buf[bit] = key_result;
            bit++;
            LCD_ShowAscii0608(locate, y, key_result + '0', 1);
            locate += 6;
        }

        //
        if (flag_finish) //输入完成
        {
            LCD_ShowString0608(x + 60, y, "  ", 1, x + 68);
            if (locate < 48 + x || bit < 7) //按下确定键，未写入完毕补零
            {
                for (; bit < 8; bit++)
                    freq_buf[bit] = 0;
            }
            //
            freq = (vu_mode ? 400 : 100);
            freq = freq + freq_buf[1] * 10 + freq_buf[2] + freq_buf[4] * 0.1 + freq_buf[5] * 0.01 + freq_buf[6] * 0.001 + freq_buf[7] * 0.0001;

            int_freq = freq_buf[0] * 1000000 + freq_buf[1] * 100000 + freq_buf[2] * 10000 +
                       freq_buf[4] * 1000 + freq_buf[5] * 100 + freq_buf[6] * 10 + freq_buf[7];
            //			int_freq = freq*10000;	//无法取整:409.7800
            D_printf("int_freq:%d\n", int_freq);

            if (freq < 136.0)
                freq = 136.0;
            else if ((freq > 174.0 && freq < 400.0) || freq > 480.0)
                freq = *result;
            else
            {
                int res = checkFreq(int_freq);
                if (res > 0)
                    freq = (double)res / 10000;
                D_printf("res:%d, freq:%.4f\n", res, freq);
            }
            *result = freq;
            LCD_ShowFreq(x, y, freq, 0); //频率显示
            bsp_StopTimer(TMR_OUT_CTRL);
            return ENT2LAST;
        }
    }
}

//				收发设置：亚音设置
//	确认返回:确认修改的亚音
//	取消返回:原来的亚音
//	KDU控制:原来的亚音
int RT_SubVoice_Set(int row, int subvoice) //第row行显示第subvoice个亚音
{
    TIMES = 0;
    LCD_ShowPIC0608(116, row, 0, 1);
    LCD_ShowString0408(0, 3, " CTCSS SET,   OR   ,CLR OR ENT  ", 1);
    LCD_ShowPIC0408(48, 3, 0);
    LCD_ShowPIC0408(68, 3, 1);

    int subvoice_temp = 0;
    if (subvoice < 0 || subvoice >= 122)
        subvoice_temp = 0;
    else
        subvoice_temp = subvoice;

    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();

        if (KDU_INSERT)
            return subvoice;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_0:
            subvoice = 0;
            LCD_ShowString0608(68, row, "        ", 1, 128);
            LCD_ShowString0608(68, row, menu_subvoice[0], 0, 128);
            break;

        case MATRIX_RESULT_CLR: //取消返回
            LCD_ShowAscii0608(116, row, ' ', 1);
            return subvoice_temp;

        case MATRIX_RESULT_ENT: //确认
            LCD_ShowAscii0608(116, row, ' ', 1);
            if (subvoice < 0 || subvoice > 121)
                return 0;
            else
                return subvoice;

        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT: //选择前一项亚音
            TIMES--;
            break;

        case MATRIX_RESULT_P:
        case MATRIX_RESULT_RIGHT: //选择后一项亚音
            TIMES++;
            break;

        case MATRIX_RESULT_5:
            subvoice = RT_SubVoice_Matrix_Menu_Select(subvoice);
            return subvoice;
        }

        if (TIMES > 0)
        {
            if (subvoice + TIMES > 121)
                subvoice = 0;
            else
                subvoice += TIMES;
            TIMES = 0;

            LCD_ShowString0608(68, row, "        ", 1, 128);
            LCD_ShowString0608(68, row, menu_subvoice[subvoice], 0, 128);
        }
        else if (TIMES < 0)
        {
            if (subvoice + TIMES < 0)
                subvoice = 121;
            else
                subvoice += TIMES;
            TIMES = 0;

            LCD_ShowString0608(68, row, "        ", 1, 128);
            LCD_ShowString0608(68, row, menu_subvoice[subvoice], 0, 128);
        }
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            LCD_ShowAscii0608(116, row, ' ', 1);
            if (subvoice < 0 || subvoice > 121)
                return 0;
            else
                return subvoice;

        case key_double:
            LCD_ShowAscii0608(116, row, ' ', 1);
            return subvoice_temp;

        case key_long:
            SHUT();
            break;
        }
    }
}

int RT_SubVoice_Matrix_Menu_Select(int subvoice) //亚音设置：矩阵亚音选择
{
    TIMES = 0;
    int subvoice_temp = subvoice;
    u8 change_3 = 0;

    LCD_Clear(EDITZONE32);
    LCD_ShowMatrixMenu33(matrix_menu_subvoice, 122, subvoice);
    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();
        if (KDU_INSERT)
            return subvoice;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_CLR:
            return subvoice_temp;

        case MATRIX_RESULT_ENT:
            if (0 > subvoice || subvoice >= 122)
                return 0;
            return subvoice;

        case MATRIX_RESULT_P:
            TIMES -= 3;
            change_3 = 1;
            break;

        case MATRIX_RESULT_N:
            TIMES += 3;
            change_3 = 1;
            break;

        case MATRIX_RESULT_LEFT:
            TIMES--;
            break;

        case MATRIX_RESULT_RIGHT:
            TIMES++;
            break;

        case MATRIX_RESULT_0:
            subvoice = 0;
            LCD_ShowMatrixMenu33(matrix_menu_subvoice, 122, subvoice);
        }

        if (TIMES > 0)
        {
            if (change_3)
            {
                change_3 = 0;
                switch (subvoice)
                {
                case 119:
                    subvoice = 2;
                    break;
                case 120:
                    subvoice = 0;
                    break;
                case 121:
                    subvoice = 1;
                    break;

                default:
                    subvoice += 3;
                }
            }
            else
            {
                if (subvoice + TIMES > 121)
                    subvoice = 0;
                else
                    subvoice += TIMES;
            }

            TIMES = 0;
            LCD_ShowMatrixMenu33(matrix_menu_subvoice, 122, subvoice);
        }
        else if (TIMES < 0)
        {
            if (change_3)
            {
                change_3 = 0;
                switch (subvoice)
                {
                case 0:
                    subvoice = 120;
                    break;
                case 1:
                    subvoice = 121;
                    break;
                case 2:
                    subvoice = 119;
                    break;

                default:
                    subvoice -= 3;
                    break;
                }
            }
            else
            {
                if (subvoice + TIMES < 0)
                    subvoice = 121;
                else
                    subvoice += TIMES;
            }

            TIMES = 0;
            LCD_ShowMatrixMenu33(matrix_menu_subvoice, 122, subvoice);
        }

        switch (Encoder_Switch_Scan(0))
        {
        case 1:
            if (0 > subvoice || subvoice > 121)
                return 0;
            return subvoice;

        case 2:
            return subvoice_temp;

        case 3:
            SHUT();
            break;
        }
    }
}
//				收发设置：发射功率选择
//	确认返回:确认修改的发射功率
//	取消返回:原来的发射功率
//	KDU控制:原来的发射功率
int RT_TX_POWER_Set(int power_temp)
{
    TIMES = 0;
    unsigned char power = power_temp;
    LCD_ShowPIC0608(60, 1, 0, 1);

    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();
        if (KDU_INSERT)
            return power_temp;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_CLR:
            return power;

        case MATRIX_RESULT_ENT:
            return power_temp;

        case MATRIX_RESULT_LEFT:
        case MATRIX_RESULT_RIGHT:
        case MATRIX_RESULT_P:
        case MATRIX_RESULT_N:
            TIMES++;
            break;
        }
        if (TIMES != 0)
        {
            TIMES = 0;
            power_temp = !power_temp;
            LCD_ShowString0608(24, 1, POWER_SHOW[power_temp], 0, 128);
        }

        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            return power_temp;

        case key_double:
            return power;

        case key_long:
            SHUT();
            break;
        }
    }
}
//				收发设置：带宽选择
//	确认返回:确认修改的带宽
//	取消返回:原来的带宽
//	KDU控制:原来的带宽
int RT_GBW_Set(int gbw_temp)
{
    TIMES = 0;
    u8 gbw_t = gbw_temp;
    LCD_ShowPIC0608(116, 1, 0, 1);
    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();
        if (KDU_INSERT)
            return gbw_temp;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_CLR:
            return gbw_t;

        case MATRIX_RESULT_ENT:
            return gbw_temp;

        case MATRIX_RESULT_LEFT:
        case MATRIX_RESULT_RIGHT:
        case MATRIX_RESULT_P:
        case MATRIX_RESULT_N:
            TIMES++;
            break;
        }
        if (TIMES != 0)
        {
            TIMES = 0;
            gbw_temp = (gbw_temp + 1) % 2;
            LCD_ShowString0608(86, 1, gbw_temp ? "WIDE" : "NARR", 0, 128);
        }
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            return gbw_temp;

        case key_double:
            return gbw_t;

        case key_long:
            SHUT();
            break;
        }
    }
}

//				收发设置：频道别名
void RT_NICKNAME_Set(unsigned char current_channel, unsigned char nn_temp[7])
{
    TIMES = 0;
    unsigned char
        nn[7] = {32},
        key_old = MATRIX_RESULT_ERROR,
        result_matrix = MATRIX_RESULT_ERROR, //当前触发按键
        nn_locate = 0,                       //当前光标位置
        nn_locate_change = 0,                //光标位置改变,用作刷新press_times
        select_bit_flag = 0,                 //编码器使用：当前模式为调节位置/设置字符
        press_times = 0,                     //当前按键按压次数
        locate_change = 1,                   //刷新选中的位置
        clear = 0;                           //编辑栏清空标志，0未清空，1为清空
                                             //	int i;

    memset(nn, 32, 7);
    sprintf((char *)nn, "%s", nn_temp);

    LCD_ShowString0608(0, 2, "CN:", 1, 18);
    LCD_ShowPIC0608(60, 2, 0, 1);

    while (1)
    {
        MY_GLOBAL_FUN();
        if (KDU_INSERT)
            return;
        switch (Encoder_Switch_Scan(0))
        {
        case key_click: //编码器确认始终为：确认当前位置的字符设置
            select_bit_flag = !select_bit_flag;
            break;

        case key_double:
            return;

        case key_long:
            SHUT();
            break;
        }

        if (select_bit_flag == 0) //编码器切换为调节位置
        {
            if (TIMES != 0)
            {
                nn_locate = (nn_locate + 7 - ((-TIMES) % 7)) % 7;
                TIMES = 0;
                locate_change = 1;
            }
        }
        else //编码器设置别名
        {
            if (TIMES > 0)
            {
                nn[nn_locate] = (nn[nn_locate] - 32 + TIMES) % 95 + 32;
                TIMES = 0;
                clear = 0;
                LCD_ShowAscii0608(18 + nn_locate * 6, 2, nn[nn_locate], 0);
            }
            else if (TIMES < 0)
            {
                nn[nn_locate] = nn[nn_locate] + 95 - (-TIMES) % 95 > 127 ? nn[nn_locate] - (-TIMES) % 95 : nn[nn_locate] + 95 - (-TIMES) % 95;
                TIMES = 0;
                clear = 0;
                LCD_ShowAscii0608(18 + nn_locate * 6, 2, nn[nn_locate], 0);
            }
        }

        result_matrix = Matrix_KEY_Scan(0);
        switch (result_matrix)
        {
        case MATRIX_RESULT_CLR:
            if (clear) //已清空，返回初值
                return;
            else //未清空，清空编辑栏
            {
                memset(nn, ' ', 7);
                locate_change = 1;
                nn_locate = 0;
                clear = 1;
            }
            break;

        case MATRIX_RESULT_ENT:
            sprintf((char *)nn_temp, "%s", nn);
            D_printf("%s\n", nn_temp);
            return;

        case MATRIX_RESULT_0:
        case MATRIX_RESULT_1:
        case MATRIX_RESULT_2:
        case MATRIX_RESULT_3:
        case MATRIX_RESULT_4:
        case MATRIX_RESULT_5:
        case MATRIX_RESULT_6:
        case MATRIX_RESULT_7:
        case MATRIX_RESULT_8:
        case MATRIX_RESULT_9:
            clear = 0;
            press_times++;
            press_times %= 9;
            if (key_old != result_matrix || nn_locate_change)
            {
                nn_locate_change = 0;
                key_old = result_matrix;
                press_times = 0;
            }
            nn[nn_locate] = square_9[result_matrix][press_times];
            LCD_ShowAscii0608(18 + nn_locate * 6, 2, nn[nn_locate], 0);
            break;

        case MATRIX_RESULT_LEFT:
            nn_locate--;
            locate_change = 1;
            select_bit_flag = 0; //按键切换位置后，编码器旋转功能改为切换位置
            if (nn_locate > 6)
                nn_locate = 6;
            break;

        case MATRIX_RESULT_RIGHT:
            nn_locate++;
            locate_change = 1;
            select_bit_flag = 0; //按键切换位置后，编码器旋转功能改为切换位置
            if (nn_locate > 6)
                nn_locate = 0;
            break;
        };
        if (locate_change) //光标位置修改
        {
            locate_change = 0;
            nn_locate_change = 1;
            LCD_ShowString0608(18, 2, "       ", 1, 128);
            LCD_ShowString0608(18, 2, (char *)nn, 1, 128);
            if (nn[nn_locate])
                LCD_ShowAscii0608(18 + nn_locate * 6, 2, nn[nn_locate], 0); //显示当前选中
            else
                LCD_ShowAscii0608(18 + nn_locate * 6, 2, ' ', 0);
        }
    }
}
//				收发设置：信道号选择
void RT_CHAN_Switch(void)
{
    TIMES = 0;
    u8
        chan_temp = chan_arv[NOW].CHAN,
        name[9] = "",
        reload = 0,
        pre_cf = get_Flag(FLAG_CF_SWITCH_ADDR),
        pre_vu = get_Flag(FLAG_VU_SWITCH_ADDR),
        now_cf = pre_cf,
        now_vu = pre_vu;

    chan_arv[TMP] = chan_arv[NOW];

    LCD_ShowPIC0608(116, 2, 0, 1);
    load_ChannelParameter(chan_temp, &chan_arv[TMP]);

    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();
        if (KDU_INSERT)
            return;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_0:
            if (now_cf)
                break;
            chan_temp = 1;
            reload = 1;
            break;

        case MATRIX_RESULT_LEFT:
            now_cf = !now_cf;
            if (now_cf)
                chan_temp = (now_vu ? 100 : 0);
            else
                chan_temp = load_CurrentChannel();
            D_printf("CHAN:%d\n", chan_temp);
            reload = 1;
            break;

        case MATRIX_RESULT_RIGHT:
            if (now_cf)
            {
                now_vu = !now_vu;
                chan_temp = now_vu * 100;
                reload = 1;
            }
            break;

        case MATRIX_RESULT_P:
            if (now_cf)
                break;
            TIMES++;
            break;

        case MATRIX_RESULT_N:
            if (now_cf)
                break;
            TIMES--;
            break;

        case MATRIX_RESULT_CLR:
            return;

        case MATRIX_RESULT_ENT:
            set_Flag(FLAG_CF_SWITCH_ADDR, now_cf);
            set_Flag(FLAG_VU_SWITCH_ADDR, now_vu);
            chan_arv[NOW].CHAN = chan_temp;
            load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
            if (now_cf == 0)
                save_CurrentChannel(chan_arv[NOW].CHAN);
            Set_A20(chan_arv[NOW], SQL);
            return;
        }
        if (TIMES != 0)
        {
            if (now_cf)
                TIMES = 0;
            if (TIMES < 0)
            {
                TIMES++;
                if (chan_temp > 1)
                {
                    reload = 1;
                    chan_temp--;
                }
            }
            else if (TIMES > 0)
            {
                TIMES--;
                if (chan_temp < 99)
                {
                    reload = 1;
                    chan_temp++;
                }
            }
        }
        if (reload)
        {
            reload = 0;
            load_ChannelParameter(chan_temp, &chan_arv[TMP]);
            LCD_ShowString0608(86, 1, chan_arv[TMP].GBW ? "WIDE" : "NARR", 1, 128);
            LCD_ShowString0608(24, 1, chan_arv[TMP].POWER ? "LOW " : "HIGH", 1, 128);

            if (now_cf)
                strcpy((char *)name, "FREQ    ");
            else
                sprintf((char *)name, "CH-%02d  ", chan_temp);

            LCD_ShowString0608(18, 2, "       ", 1, 128);
            LCD_ShowString0608(68, 2, "       ", 1, 128);
            LCD_ShowString0608(18, 2, (char *)chan_arv[TMP].NN, 1, 128);
            LCD_ShowString0608(68, 2, (char *)name, 0, 128);
        }
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            set_Flag(FLAG_CF_SWITCH_ADDR, now_cf);
            set_Flag(FLAG_VU_SWITCH_ADDR, now_vu);
            chan_arv[NOW].CHAN = chan_temp;
            load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
            if (now_cf == 0)
                save_CurrentChannel(chan_arv[NOW].CHAN);
            Set_A20(chan_arv[NOW], SQL);
            return;

        case key_double:
            return;

        case key_long:
            SHUT();
            break;
        }
    }
    //
}
//

//按键2进入背光及亮度设置
//CLR2LAST: 返回前一级
//ENT2LAST: 设置成功退出
//BACK2MAIN:按键2/kdu控制退出
u8 Screen_Contrast_Set(void)
{
    LCD_ShowString0608(19, 1, "SCREEN CONTRAST", 1, 128);
    LCD_ShowContrast(SC);
    while (1)
    {
        MY_GLOBAL_FUN();
        if (KDU_INSERT)
            return BACK2MAIN;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_2:
            return BACK2MAIN;

        case MATRIX_RESULT_CLR:
            return CLR2LAST;

        case MATRIX_RESULT_ENT:
            return ENT2LAST;

        case MATRIX_RESULT_RIGHT:
        case MATRIX_RESULT_P:
            TIMES++;
            break;

        case MATRIX_RESULT_LEFT:
        case MATRIX_RESULT_N:
            TIMES--;
            break;
        }
        if (TIMES)
        {
            if (SC + TIMES > 6)
                SC = 6;
            else if (SC + TIMES < 0)
                SC = 0;
            else
                SC += TIMES;
            TIMES = 0;
            LCD_LightRatio(0x00 + SC * 5);
            save_ScreenContrast(SC);
            LCD_ShowContrast(SC);
        }
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            return ENT2LAST;

        case key_double:
            return CLR2LAST;

        case key_long:
            SHUT();
            break;
        }
    }
}

//CLR2LAST:返回前一级
//BACK2MAIN:按键2/kdu控制退出/进入对比度设置后设置完成
u8 Light_Intensity_set(void)
{
    u8 ENTER = 0;
    LCD_ShowString0608(19, 1, "LIGHT INTENSITY", 1, 128);
    LCD_ShowString0608(0, 2, "                      ", 1, 128);
    LCD_ShowBackLight(BL);
    while (1)
    {
        MY_GLOBAL_FUN();
        if (KDU_INSERT)
            return BACK2MAIN;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_2:
            return BACK2MAIN;

        case MATRIX_RESULT_CLR:
            return CLR2LAST;

        case MATRIX_RESULT_ENT:
            ENTER = 1;
            break;

        case MATRIX_RESULT_RIGHT:
        case MATRIX_RESULT_P:
            TIMES++;
            break;

        case MATRIX_RESULT_LEFT:
        case MATRIX_RESULT_N:
            TIMES--;
            break;
        }
        if (TIMES)
        {
            BL += (TIMES * 10);
            TIMES = 0;
            if (BL > 127)
                BL = 0;
            if (BL > 100)
                BL = 100;
            LCD_ShowBackLight(BL);
            BackLight_SetVal(BL);
        }
        //
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            ENTER = 1;
            break;

        case key_double:
            return CLR2LAST;

        case key_long:
            SHUT();
            break;
        }
        if (ENTER)
        {
            ENTER = 0;
            save_Backlightness(BL);
            if (Screen_Contrast_Set())
                return BACK2MAIN;
            LCD_ShowString0608(19, 1, "LIGHT INTENSITY", 1, 128);
            LCD_ShowBackLight(BL);
        }
    }

    //	return 0;
}

void Light_Mode_Clear(u8 sel_pos)
{

    LCD_Clear(EDITZONE32);
    LCD_ShowString0608(34, 1, "LIGHT MODE", 1, 128);
    LCD_ShowOption(37, 2, LIGHT_SHOW, 2, sel_pos);
    LCD_ShowString0408(20, 3, " TO CHANGE / ENT TO CONT", 1);

    LCD_ShowPIC0408(10, 3, 0);
    LCD_ShowPIC0408(15, 3, 1);
}

void Light_Mode_Set(void) //
{
    TIMES = 0;
    u8 BL_temp = BL;
    static u8 pos = 0;
    u8 ENTER = 0;
    Light_Mode_Clear(pos);

    while (1)
    {
        MY_GLOBAL_FUN();
        if (KDU_INSERT)
            return;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_2:
        case MATRIX_RESULT_CLR:
            BackLight_SetVal(BL_temp);
            return;

        case MATRIX_RESULT_ENT:
            ENTER = 1;
            break;

        case MATRIX_RESULT_P:
            TIMES++;
            break;

        case MATRIX_RESULT_N:
            TIMES--;
            break;
        }
        if (TIMES)
        {
            pos = !pos;
            TIMES = 0;
            BL = pos ? 0 : load_Backlightness();
            LCD_ShowString0608(0, 2, "                      ", 1, 128);
            BackLight_SetVal(BL);
            Light_Mode_Clear(pos);
        }
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            ENTER = 1;
            break;

        case key_double:
            BackLight_SetVal(BL_temp);
            return;

        case key_long:
            SHUT();
            break;
        }

        //
        if (ENTER)
        {
            ENTER = 0;
            if (pos) //确认关闭亮灯后--->进对比度设置
            {
                if (Screen_Contrast_Set())
                    return;
            }
            else //确认亮灯后--->亮度调节--->对比度设置
            {
                if (Light_Intensity_set())
                    return;
            }
            Light_Mode_Clear(pos);
        }
    }
    //
}
//

//按键5 初始化
void Zero_Menu(void)
{
    Flag_Main_Page = 0;
    TIMES = 0;
    u8 pos = 0;
    int ENSURE = 0;
    LCD_Clear(EDITZONE32);
    LCD_ShowMenu31(MENU_ZERO, MENU_ZERO_NUM, pos);

    while (1)
    {
        MY_GLOBAL_FUN();
        if (KDU_INSERT)
            return;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_5:
        case MATRIX_RESULT_CLR:
            return;

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
            pos = (pos + TIMES) % MENU_ZERO_NUM;
            TIMES = 0;
            LCD_ShowMenu31(MENU_ZERO, MENU_ZERO_NUM, pos);
        }
        else if (TIMES < 0)
        {
            pos = (pos + MENU_ZERO_NUM - ((-TIMES) % MENU_ZERO_NUM)) % MENU_ZERO_NUM;
            TIMES = 0;
            LCD_ShowMenu31(MENU_ZERO, MENU_ZERO_NUM, pos);
        }
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            ENSURE = 1;
            break;

        case key_double:
            return;

        case key_long:
            D_printf("<<<<<<SHUTING>>>>>>\n");
            SHUT();
            break;
        }
        ///////////////////////////////////////////////////////
        if (ENSURE)
        {
            ENSURE = 0;
            switch (pos)
            {
            case 0:
                D_printf("ZEROIZE ALL\n");
                ENSURE = Zeroize_All();
                break;
            }
            if (ENSURE == 2)
                return;

            ENSURE = 0;
            LCD_ShowMenu31(MENU_ZERO, MENU_ZERO_NUM, pos);
        }
    }
}

//返回值无特殊意义
int Zeroize_All(void)
{
    TIMES = 0;
    u8 pos = 0;

    LCD_Clear(EDITZONE32);
    LCD_ShowString0608(25, 1, "ZEROIZE RADIO", 1, 128);
    LCD_ShowOption(55, 2, CONFIRM_OPT, 2, pos);
    LCD_ShowString0408(18, 3, "TO SCROLL / ENT TO CONT", 1);

    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();
        if (KDU_INSERT)
            return BACK2MAIN;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_LEFT:
        case MATRIX_RESULT_RIGHT:
        case MATRIX_RESULT_P:
        case MATRIX_RESULT_N:
            TIMES++;
            break;

        case MATRIX_RESULT_5:
            return BACK2MAIN;

        case MATRIX_RESULT_CLR:
            return CLR2LAST;

        case MATRIX_RESULT_ENT:
            if (pos)
            {
                set_Flag(RESETADDR, RESET_VAL + 1);
                SPK_SWITCH(0, 0);
                DATA_Init();
                SHUT();
            }
            else
                return ENT2LAST;
        }
        if (TIMES)
        {
            TIMES = 0;
            pos = !pos;
            LCD_ShowOption(55, 2, CONFIRM_OPT, 2, pos);
        }
        //
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            if (pos)
            {
                set_Flag(RESETADDR, ~RESET_VAL);
                SPK_SWITCH(0, 0);
                DATA_Init();
                SHUT();
            }
            break;

        case key_double:
            return CLR2LAST;

        case key_long:
            SHUT();
            break;
        }
        //
    }
}
//

//按键7 OPTION菜单
void OPTION_Menu(void)
{
    Flag_Main_Page = 0;
    TIMES = 0;
    u8 num = 0;
    int ENSURE = 0;

    LCD_Clear(EDITZONE32);
    LCD_ShowMenu31(MENU_OPT, MENU_OPT_NUM, num);
    while (1)
    {
        MY_GLOBAL_FUN();
        if (KDU_INSERT)
            return;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_7:
        case MATRIX_RESULT_CLR:
            return;

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
        ///////////////////////////////////////////////////////////////////////////////////////
        if (TIMES > 0)
        {
            num = (num + TIMES) % MENU_OPT_NUM;
            TIMES = 0;
            LCD_ShowMenu31(MENU_OPT, MENU_OPT_NUM, num);
        }
        else if (TIMES < 0)
        {
            num = (num + MENU_OPT_NUM - ((-TIMES) % MENU_OPT_NUM)) % MENU_OPT_NUM;
            TIMES = 0;
            LCD_ShowMenu31(MENU_OPT, MENU_OPT_NUM, num);
        }
        ////////////////////////////////////////////////////////////////////////////////////////
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            ENSURE = 1;
            break;

        case key_double:
            return;

        case key_long:
            D_printf("<<<<<<SHUTING>>>>>>\n");
            SHUT();
            break;
        }
        //////////////////////////////////////////////////////////////////////////////////////
        if (ENSURE)
        {
            ENSURE = 0;
            switch (num)
            {
            case 0:
                D_printf("LOCK KEY PAD\n");
                ENSURE = Lock_Screen_KeyBoard();
                ClearShut();
                while (VOL_ADD_READ == 0 || VOL_SUB_READ == 0)
                    FeedDog(); //喂狗;//防止长按退出时执行一次音量的加减
                if (ENSURE == 2)
                {
                    Flag_Main_Page = 1;
                    return;
                }
                break;

            case 1:
                D_printf("KEY TEST\n");
                Key_Test();
                break;
            }
            ENSURE = 0;
            LCD_ShowMenu31(MENU_OPT, MENU_OPT_NUM, num);
        }
        //
    }
}
//
//测试按键
void Key_Test()
{
    TIMES = 0;
    int delay_f1_f2 = 0, result = 0;

    LCD_ShowString0608(0, 1, "     KEY TESTING      ", 1, 128);
    LCD_ShowString0608(0, 2, "                      ", 1, 128);
    LCD_ShowString0608(0, 3, "VALUE:                ", 1, 128);
    bsp_StartAutoTimer(TMR_OUT_CTRL, TMR_PERIOD_8S);
    while (1)
    {
        FeedDog(); //喂狗
        if (KDU_INSERT || bsp_CheckTimer(TMR_OUT_CTRL) || delay_f1_f2 >= 100000)
        {
            bsp_StopTimer(TMR_OUT_CTRL);
            return;
        }

        result = Matrix_KEY_Scan(0);
        if (result != 17)
        {
            D_printf("%d\n", result);
            ReloadOutCal();
        }
        switch (result)
        {
        case MATRIX_RESULT_0:
            LCD_ShowString0608(36, 3, "MATRIX KEY 0  ", 1, 128);
            break;
        case MATRIX_RESULT_1:
            LCD_ShowString0608(36, 3, "MATRIX KEY 1  ", 1, 128);
            break;
        case MATRIX_RESULT_2:
            LCD_ShowString0608(36, 3, "MATRIX KEY 2  ", 1, 128);
            break;
        case MATRIX_RESULT_3:
            LCD_ShowString0608(36, 3, "MATRIX KEY 3  ", 1, 128);
            break;
        case MATRIX_RESULT_4:
            LCD_ShowString0608(36, 3, "MATRIX KEY 4  ", 1, 128);
            break;
        case MATRIX_RESULT_5:
            LCD_ShowString0608(36, 3, "MATRIX KEY 5  ", 1, 128);
            break;
        case MATRIX_RESULT_6:
            LCD_ShowString0608(36, 3, "MATRIX KEY 6  ", 1, 128);
            break;
        case MATRIX_RESULT_7:
            LCD_ShowString0608(36, 3, "MATRIX KEY 7  ", 1, 128);
            break;
        case MATRIX_RESULT_8:
            LCD_ShowString0608(36, 3, "MATRIX KEY 8  ", 1, 128);
            break;
        case MATRIX_RESULT_9:
            LCD_ShowString0608(36, 3, "MATRIX KEY 9  ", 1, 128);
            break;
        case MATRIX_RESULT_CLR:
            LCD_ShowString0608(36, 3, "MATRIX KEY CLR", 1, 128);
            break;
        case MATRIX_RESULT_ENT:
            LCD_ShowString0608(36, 3, "MATRIX KEY ENT", 1, 128);
            break;
        case MATRIX_RESULT_P:
            LCD_ShowString0608(36, 3, "MATRIX KEY +  ", 1, 128);
            break;
        case MATRIX_RESULT_N:
            LCD_ShowString0608(36, 3, "MATRIX KEY -  ", 1, 128);
            break;
        case MATRIX_RESULT_LEFT:
            LCD_ShowString0608(36, 3, "MATRIX KEY L  ", 1, 128);
            break;
        case MATRIX_RESULT_RIGHT:
            LCD_ShowString0608(36, 3, "MATRIX KEY R  ", 1, 128);
            break;
        }
        switch (VolumeKeyScan(0))
        {
        case 1:
            LCD_ShowString0608(36, 3, "VOLUME +      ", 1, 128);
            delay_f1_f2++;
            ReloadOutCal();
            break;
        case 2:
            LCD_ShowString0608(36, 3, "VOLUME -      ", 1, 128);
            delay_f1_f2++;
            ReloadOutCal();
            break;
        }
        if (VOL_ADD_READ == 0 || VOL_SUB_READ == 0)
        {
            delay_f1_f2++;
        }
        else
        {
            delay_f1_f2 = 0;
        }

        switch (Encoder_Switch_Scan(0))
        {
        case 1:
            ReloadOutCal();
            LCD_ShowString0608(36, 3, "ENCODER CLICK ", 1, 128);
            break;

        case 2:
            ReloadOutCal();
            LCD_ShowString0608(36, 3, "ENCODER DOUBLE", 1, 128);
            break;

        case 3:
            ReloadOutCal();
            LCD_ShowString0608(36, 3, "ENCODER LONG  ", 1, 128);
            break;
        }
        //		if(A002_SQ_READ == 0)
        //		{
        //			if(bsp_CheckTimer(TMR_RSSI_CTRL))
        //				LCD_ShowSignal(Get_A20_RSSI());
        //		}
        if (!PTT_READ)
        {
            LCD_ShowString0608(36, 3, " PUSH TO TALK ", 1, 128);
            ReloadOutCal();
        }
        if (!SQUELCH_READ)
        {
            LCD_ShowString0608(36, 3, " SQUELCH_READ ", 1, 128);
            ReloadOutCal();
        }

        if (TIMES < 0)
        {
            TIMES = 0;
            LCD_ShowString0608(36, 3, "ENCODER   -   ", 1, 128);
            ReloadOutCal();
        }
        else if (TIMES > 0)
        {
            TIMES = 0;
            LCD_ShowString0608(36, 3, "ENCODER   +   ", 1, 128);
            ReloadOutCal();
        }
    }
}
//功能0：锁屏锁盘
int Lock_Screen_KeyBoard()
{
    int f1 = 0, f2 = 0, i = 0;
    int volume_change = 0;
    if (Flag_Main_Page == 0)
    {
        VFO_Clear();
        VFO_Refresh();
    }
    LCD_ShowString0408(108, 3, "KEY", 0);
    while (1)
    {

        FeedDog(); //喂狗
        PTT_Control();
        SQUELCH_Contol();
        SQ_Read_Control();
        A002_CALLBACK(); //A002设置应答
        if (bsp_CheckTimer(TMR_FM_CTRL) && WFM)
            RDA5807_Init(ON);

        if (PTT_READ){
            LCD_ShowAscii0408(0, 0, 'R');
            LCD_ShowSignal(RSSI); //信号检测
        } else {
            LCD_ShowAscii0408(0, 0, 'T');
            LCD_ShowSignal(100);
        }
        if(EncoderClickValidate()){
            if (i++ > 150){
                SHUT();
            }
        }

        if (KDU_INSERT){
            TIMES = 0;
            return BACK2MAIN;
        }

        delay_ms(15);
        if (VOL_ADD_READ == 0){
            f1++;
        } else {
            if (f1 > 0) {
                f1 = 0;
                if (VOLUME < 7) {
                    volume_change = 1;
                    VOLUME++;
                }
            }
        }
        //

        if (VOL_SUB_READ == 0){
            f2++;
        } else {
            if (f2 > 0) {
                f2 = 0;
                if (VOLUME > 0) {
                    volume_change = 1;
                    VOLUME--;
                }
            }
        }
        if (volume_change) {
            volume_change = 0;
            save_OverVolume(VOLUME);
            LCD_ShowVolume(VOLUME);
            if (WFM) //开着收音机的时候不中断输出，直接修改音量
            {
                RDA5807_ResumeImmediately();
                if (!A002_SQ_READ)
                {
                    M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[0]);
                    M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);
                }
                else
                    M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[VOLUME]);
            }
            else
            {
                if (A002_SQ_READ)
                    SPK_SWITCH(AUD, 0);
                M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[0]); //0
                M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);
            }
        }

        if (f1 > 100 || f2 > 100)
        {
            LCD_ShowString0408(108, 3, "KEY", 1);
            TIMES = 0;
            return BACK2MAIN;
        }
    }
}
//

//按键8 PGM菜单
void PGM_Menu()
{
    Flag_Main_Page = 0;
    TIMES = 0;
    u8 num = 0;
    int ENSURE = 0;
    LCD_Clear(GLOBAL32);

    LCD_ShowString0408(0, 0, "PGM", 1);
    LCD_ShowMenu31(MENU_PGM, MENU_PGM_NUM, 0);
    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();
        if (KDU_INSERT)
            return;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_8:
        case MATRIX_RESULT_CLR:
            return;

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
        ///////////////////////////////////////////////////////////////////////////////////////
        if (TIMES > 0)
        {
            num = (num + 1) % MENU_PGM_NUM;
            TIMES = 0;
            LCD_ShowMenu31(MENU_PGM, MENU_PGM_NUM, num);
        }
        else if (TIMES < 0)
        {
            num = (num - 1 + MENU_PGM_NUM) % MENU_PGM_NUM;
            TIMES = 0;
            LCD_ShowMenu31(MENU_PGM, MENU_PGM_NUM, num);
        }
        ////////////////////////////////////////////////////////////////////////////////////////
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            ENSURE = 1;
            break;

        case key_double:
            return;

        case key_long:
            D_printf("<<<<<<SHUTING>>>>>>\n");
            SHUT();
            break;
        }
        //////////////////////////////////////////////////////////////////////////////////////
        if (ENSURE)
        {
            ENSURE = 0;
            switch (num)
            {
            case normal_set_mic_gain:
                D_printf("MIC Selecting\n");
                ENSURE = PGM_AUDIO_Select(num % 3 + 1);
                break;

            case normal_set_sql:
                D_printf("SQL Selecting\n");
                ENSURE = PGM_SQL_Set(num % 3 + 1);
                break;

            case normal_set_step:
                D_printf("Step Selecting\n");
                ENSURE = PGM_STEP_Set(num % 3 + 1);
                break;

                //					case normal_set_encryp:
                //						D_printf("ENCRYP Setting\n");
                //						ENSURE = PGM_ENCRPY_Set(num%3+1);
                //						break;

            case normal_set_tot:
                D_printf("TOT Setting\n");
                ENSURE = PGM_TOT_Set(num % 3 + 1);
                break;

            case normal_set_lamptime:
                D_printf("LampTime Setting\n");
                ENSURE = PGM_LAMP_TIME_Set(num % 3 + 1);
                break;

            case normal_set_powerout:
                D_printf("Power Out\n");
                ENSURE = PGM_POWEROUT_Set(num % 3 + 1);
                break;

            case normal_set_ptttone:
                D_printf("PTT TONE\n");
                ENSURE = PGM_TONE_Select(num % 3 + 1);
                break;
            default:
                break;
            }
            if (ENSURE == BACK2MAIN)
                return;
            ENSURE = 0;
            LCD_ShowMenu31(MENU_PGM, MENU_PGM_NUM, num);
            while (VOL_ADD_READ == 0 || VOL_SUB_READ == 0)
                FeedDog(); //喂狗;//防止长按退出时执行一次音量的加减
        }
        //
    }
}
//
//	CLR2LAST:不修改,重新选择需要选通的音频通道
//	ENT2LAST:确认修改并退出音频设置返回PDM菜单
//	BACK2MAIN:按键8/KDU控制直接退出
//								相关设置1：音频选通并设置咪灵敏度
int AUDIO_SET(u8 _audio)
{
    TIMES = 0;

    u8 temp = MIC;

    LCD_ShowString0608(60, _audio + 1, ":", 0, 66);
    LCD_ShowString0608(66, _audio + 1, TRF_Show[temp], 0, 90);

    LCD_ShowPIC0608(116, _audio + 1, 0, 0);
    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();
        if (KDU_INSERT)
            return BACK2MAIN;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_8:
            return BACK2MAIN;

        case MATRIX_RESULT_CLR:
            return CLR2LAST;

        case MATRIX_RESULT_ENT:
            if (_audio != AUD)
                SPK_SWITCH(AUD, OFF);

            if (!A002_SQ_READ) //有信号,直接修改
                SPK_SWITCH(_audio, ON);
            else
            {
                if (WFM)
                    SPK_SWITCH(_audio, ON);
            }
            //

            MIC = temp;
            AUD = _audio;
            save_MicLevel(MIC);
            save_AudioSelect(AUD);
            M62364_SetSingleChannel(4, MIC_LEVEL[MIC]);
            return ENT2LAST;

        case MATRIX_RESULT_0:
            temp = 0;
            LCD_ShowString0608(66, _audio + 1, TRF_Show[temp], 0, 128);
            break;

        case MATRIX_RESULT_P:
        case MATRIX_RESULT_RIGHT:
            TIMES++;

            break;

        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT:
            TIMES--;
            break;
        }
        /////////////////////////////////////////////////////////////
        if (TIMES > 0)
        {
            temp = (temp + 1) % 3;
            TIMES = 0;
            LCD_ShowString0608(66, _audio + 1, TRF_Show[temp], 0, 128);
        }
        else if (TIMES < 0)
        {
            temp = (temp - 1 + 3) % 3;
            TIMES = 0;
            LCD_ShowString0608(66, _audio + 1, TRF_Show[temp], 0, 128);
        }
        ///////////////////////////////////////////////////////////
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            if (_audio != AUD)
                SPK_SWITCH(AUD, OFF);

            if (!A002_SQ_READ) //有信号,直接修改
                SPK_SWITCH(_audio, ON);
            else
            {
                if (WFM)
                    SPK_SWITCH(_audio, ON);
            }

            MIC = temp;
            AUD = _audio;
            save_MicLevel(MIC);
            save_AudioSelect(AUD);
            M62364_SetSingleChannel(4, MIC_LEVEL[MIC]);
            return ENT2LAST;

        case key_double:
            return CLR2LAST;

        case key_long:
            SHUT();
            break;
        }
    }
}
//	CLR2LAST:不修改返回PGM菜单
//	ENT2LAST:确认修改然后返回PGM菜单
//	BACK2MAIN:按键8/KDU控制直接退出
int PGM_AUDIO_Select(u8 row)
{
    TIMES = 0;
    u8 index = AUD, ENTER = 0;
    LCD_ShowMenu31(menu_audio, 3, index);
    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();
        if (KDU_INSERT)
            return BACK2MAIN;
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            ENTER = 1;
            break;

        case key_double:
            return CLR2LAST;

        case key_long:
            SHUT();
            break;
        }
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_8:
            return BACK2MAIN;

        case MATRIX_RESULT_P:
        case MATRIX_RESULT_LEFT:
            TIMES--;
            break;

        case MATRIX_RESULT_N:
        case MATRIX_RESULT_RIGHT:
            TIMES++;
            break;

        case MATRIX_RESULT_ENT:
            ENTER = 1;
            break;

        case MATRIX_RESULT_CLR:
            return CLR2LAST;
        }
        if (TIMES > 0) //
        {
            TIMES = 0;
            index = (index + 1) % 3;
            LCD_ShowMenu31(menu_audio, 3, index);
        }
        else if (TIMES < 0)
        {
            TIMES = 0;
            index = (index + 2) % 3;
            LCD_ShowMenu31(menu_audio, 3, index);
        }

        if (ENTER)
        {
            if (index == 0)
            {
                if (index != AUD)
                    SPK_SWITCH(AUD, OFF);

                if (!A002_SQ_READ) //有信号,直接修改
                    SPK_SWITCH(index, ON);
                else
                {
                    if (WFM)
                        SPK_SWITCH(index, ON);
                }
                //

                AUD = 0;
                MIC = 1;
                save_AudioSelect(AUD);
                M62364_SetSingleChannel(4, MIC_LEVEL[MIC]);
                return ENT2LAST;
            }
            ENTER = AUDIO_SET(index);
            if (ENTER == CLR2LAST)
                LCD_ShowMenu31(menu_audio, 3, index);
            else
                return ENTER; //可以是返回主界面,也可以是返回菜单
        }
    }
}
//								相关设置2：静噪等级
int PGM_SQL_Set(u8 row)
{
    TIMES = 0;
    u8 sql_temp = SQL;
    LCD_ShowString0608(30, row, ":LEVEL", 0, 66);
    LCD_ShowAscii0608(66, row, sql_temp + '0', 0);
    LCD_ShowPIC0608(116, row, 0, 0);
    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();
        if (KDU_INSERT)
            return BACK2MAIN;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_8:
            return BACK2MAIN;

        case MATRIX_RESULT_CLR:
            return CLR2LAST;

        case MATRIX_RESULT_ENT:
            save_Sql(sql_temp);
            SQL = sql_temp;
            Set_A20(chan_arv[NOW], SQL);
            return ENT2LAST;

        case MATRIX_RESULT_0:
            sql_temp = 0;
            LCD_ShowAscii0608(66, row, sql_temp + '0', 0);
            break;

        case MATRIX_RESULT_P:
        case MATRIX_RESULT_RIGHT:
            TIMES++;
            break;

        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT:
            TIMES--;
            break;
        }
        if (TIMES != 0)
        {
            sql_temp += TIMES;
            if (sql_temp > 127)
                sql_temp = 0;
            else if (sql_temp > 8)
                sql_temp = 8;

            LCD_ShowAscii0608(66, row, sql_temp + '0', 0);
            TIMES = 0;
        }

        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            save_Sql(sql_temp);
            SQL = sql_temp;
            Set_A20(chan_arv[NOW], SQL);
            return ENT2LAST;

        case key_double:
            return CLR2LAST;

        case key_long:
            SHUT();
            break;
        }
    }
}

//								相关设置3：步进
int PGM_STEP_Set(u8 row)
{
    TIMES = 0;
    u8 step_temp = STEP;
    LCD_ShowAscii0608(36, row, ':', 0);
    LCD_ShowString0608(42, row, STEP_SHOW[STEP], 0, 72);
    LCD_ShowPIC0608(116, row, 0, 0);

    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();
        if (KDU_INSERT)
            return BACK2MAIN;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_8:
            return BACK2MAIN;

        case MATRIX_RESULT_CLR:
            return CLR2LAST;

        case MATRIX_RESULT_ENT:
            save_Step(step_temp);
            STEP = step_temp;
            return ENT2LAST;

        case MATRIX_RESULT_0:
            step_temp = 0;
            LCD_ShowString0608(42, row, STEP_SHOW[step_temp], 0, 128);
            break;

        case MATRIX_RESULT_P:
        case MATRIX_RESULT_RIGHT:
            TIMES++;
            break;

        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT:
            TIMES--;
            break;
        }
        if (TIMES != 0)
        {
            step_temp += TIMES;
            if (step_temp > 127)
                step_temp = 0;
            else if (step_temp > 2)
                step_temp = 2;
            LCD_ShowString0608(42, row, STEP_SHOW[step_temp], 0, 128);
            TIMES = 0;
        }

        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            save_Step(step_temp);
            STEP = step_temp;
            return ENT2LAST;

        case key_double:
            return CLR2LAST;

        case key_long:
            SHUT();
            break;
        }
    }
}
//								相关设置4：加密
int PGM_ENCRPY_Set(u8 row)
{
    TIMES = 0;
    u8 SCRAM_LEVEL_temp = ENC;
    LCD_ShowAscii0608(48, row, ':', 0);
    LCD_ShowAscii0608(54, row, SCRAM_LEVEL_temp + '0', 0);
    LCD_ShowPIC0608(116, row, 0, 0);
    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();
        if (KDU_INSERT)
            return BACK2MAIN;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_8:
            return BACK2MAIN;

        case MATRIX_RESULT_CLR:
            return CLR2LAST;

        case MATRIX_RESULT_ENT:
            save_ScramLevel(SCRAM_LEVEL_temp);
            ENC = SCRAM_LEVEL_temp;
            Set_A20_MIC(MIC * 3 + 1, ENC, TOT);
            return ENT2LAST;

        case MATRIX_RESULT_0:
            SCRAM_LEVEL_temp = 0;
            LCD_ShowAscii0608(54, row, SCRAM_LEVEL_temp + '0', 0);
            break;

        case MATRIX_RESULT_P:
        case MATRIX_RESULT_RIGHT:
            TIMES++;
            break;

        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT:
            TIMES--;
            break;
        }
        if (TIMES != 0)
        {
            SCRAM_LEVEL_temp += TIMES;
            if (SCRAM_LEVEL_temp > 127)
                SCRAM_LEVEL_temp = 0;
            else if (SCRAM_LEVEL_temp > 9)
                SCRAM_LEVEL_temp = 9;
            LCD_ShowAscii0608(54, row, SCRAM_LEVEL_temp + '0', 0);
            TIMES = 0;
        }
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            save_ScramLevel(SCRAM_LEVEL_temp);
            ENC = SCRAM_LEVEL_temp;
            Set_A20_MIC(MIC * 3 + 1, ENC, TOT);
            return ENT2LAST;

        case key_double:
            return CLR2LAST;

        case key_long:
            SHUT();
            break;
        }
    }
}
//								相关设置5：发射限时
int PGM_TOT_Set(u8 row)
{
    TIMES = 0;
    u8 tot_temp = load_Tot();
    LCD_ShowString0608(30, row, ": MINUTE", 0, 128);
    LCD_ShowAscii0608(36, row, tot_temp + '0', 0);
    if (tot_temp == 0)
        LCD_ShowString0608(30, row, ":NO TOT ", 0, 78);
    LCD_ShowPIC0608(116, row, 0, 0);
    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();
        if (KDU_INSERT)
            return BACK2MAIN;
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_8:
            return BACK2MAIN;

        case MATRIX_RESULT_CLR:
            return CLR2LAST;

        case MATRIX_RESULT_ENT:
            TOT = tot_temp;
            save_Tot(TOT);
            Set_A20_MIC(MIC * 3 + 1, ENC, TOT);
            return ENT2LAST;

        case MATRIX_RESULT_0:
            tot_temp = 0;
            LCD_ShowString0608(30, row, ":NO TOT ", 0, 128);
            break;

        case MATRIX_RESULT_P:
        case MATRIX_RESULT_RIGHT:
            TIMES++;
            break;

        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT:
            TIMES--;
            break;
        }
        //
        if (TIMES != 0)
        {
            tot_temp += TIMES;
            TIMES = 0;
            if (tot_temp > 127)
                tot_temp = 0;
            if (tot_temp > 8)
                tot_temp = 9;
            if (!tot_temp)
                LCD_ShowString0608(30, row, ":NO TOT ", 0, 128);
            else
            {
                LCD_ShowString0608(30, row, ": MINUTE", 0, 128);
                LCD_ShowAscii0608(36, row, tot_temp + '0', 0);
            }
        }

        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            TOT = tot_temp;
            save_Tot(TOT);
            Set_A20_MIC(MIC * 3 + 1, ENC, TOT);
            return ENT2LAST;

        case key_double:
            return CLR2LAST;

        case key_long:
            SHUT();
            break;
        }
    }
}
//								相关设置6：背光灯亮起时长
int PGM_LAMP_TIME_Set(u8 row)
{

    u8 LT = LAMP_TIME / 10000;

    LCD_ShowAscii0608(60, row, ':', 0); //

    LCD_ShowString0608(72, row, LAMP_SHOW[LT], 0, 128); //

    LCD_ShowPIC0608(116, row, 0, 0); //箭头

    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();
        if (KDU_INSERT)
            return BACK2MAIN;

        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            LAMP_TIME = LT * 10000;
            save_LampTime(LT);
            return ENT2LAST;

        case key_double:
            return ENT2LAST;

        case key_long:
            SHUT();
            break;
        }
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_8:
            return BACK2MAIN;

        case MATRIX_RESULT_CLR:
            return ENT2LAST;

        case MATRIX_RESULT_ENT:
            save_LampTime(LT);
            LAMP_TIME = LT * 10000;
            return ENT2LAST;

        case MATRIX_RESULT_0:
            LT = 0;
            LCD_ShowString0608(72, row, LAMP_SHOW[LT], 0, 128);
            break;

        case MATRIX_RESULT_P:
        case MATRIX_RESULT_RIGHT:
        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT:
            TIMES++;
            break;
        }
        if (TIMES != 0)
        {
            TIMES = 0;
            LT = !LT;
            LCD_ShowString0608(72, row, LAMP_SHOW[LT], 0, 128);
        }
        //
    }
}

//								相关设置7：六针头电源输出
int PGM_POWEROUT_Set(u8 row)
{
    TIMES = 0;
    char power = VDO;
    LCD_ShowAscii0608(60, row, ':', 0);
    LCD_ShowString0608(66, row, opt_state[power], 0, 90);
    LCD_ShowPIC0608(116, row, 0, 0);

    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();
        if (KDU_INSERT)
            return BACK2MAIN;

        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            VDO = power;
            save_VDO(power);
            VDO_SWITCH(power);
            return ENT2LAST;

        case key_double:
            return CLR2LAST;

        case key_long:
            SHUT();
            break;
        }
        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_8:
            return BACK2MAIN;

        case MATRIX_RESULT_P:
        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT:
        case MATRIX_RESULT_RIGHT:
            TIMES++;
            break;

        case MATRIX_RESULT_ENT:
            VDO = power;
            save_VDO(VDO);
            VDO_SWITCH(VDO);
            return ENT2LAST;

        case MATRIX_RESULT_CLR:
            return CLR2LAST;
        }
        if (TIMES != 0)
        {
            TIMES = 0;
            power = !power;
            LCD_ShowString0608(66, row, opt_state[power], 0, 128);
        }
    }
}

//								相关设置8:PTT按键提示音设置
int TONE_SET(u8 _tone) //_tone:要进行设置的Tone;
{
    char t_sta[2] = {PRE_TONE, END_TONE}; //前置信令和后置信令的状态

    TIMES = 0;
    LCD_ShowAscii0608(84, _tone + 1, ':', 0);
    LCD_ShowString0608(90, _tone + 1, opt_state[t_sta[_tone]], 0, 114); //显示开关状态
    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();
        if (KDU_INSERT)
            return BACK2MAIN;

        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_8:
            return BACK2MAIN;
        case MATRIX_RESULT_P:
        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT:
        case MATRIX_RESULT_RIGHT:
            TIMES--;
            break;

        case MATRIX_RESULT_ENT:
            if (_tone)
            {
                END_TONE = t_sta[_tone];
                save_EndTone(t_sta[_tone]);
            }
            else
            {
                PRE_TONE = t_sta[_tone];
                save_PreTone(t_sta[_tone]);
            }
            return ENT2LAST;

        case MATRIX_RESULT_CLR:
            return CLR2LAST;
        }
        if (TIMES != 0)
        {
            TIMES = 0;
            t_sta[_tone] = !t_sta[_tone];
            LCD_ShowString0608(90, _tone + 1, opt_state[t_sta[_tone]], 0, 128);
        }
        switch (Encoder_Switch_Scan(0))
        {
        case key_click: //确认当前设置
            if (_tone)
            {
                END_TONE = t_sta[_tone];
                save_EndTone(t_sta[_tone]);
            }
            else
            {
                PRE_TONE = t_sta[_tone];
                save_PreTone(t_sta[_tone]);
            }
            return ENT2LAST;

        case key_double:
            return CLR2LAST;

        case key_long:
            SHUT();
            break;
        }
    }
}
int PGM_TONE_Select(u8 row)
{
    TIMES = 0;
    u8 index = 0;
    LCD_ShowMenu31(menu_tone, 2, index);
    while (1)
    {
        FeedDog(); //喂狗
        SQ_Read_Control();
        if (KDU_INSERT)
            return BACK2MAIN;

        switch (Matrix_KEY_Scan(0))
        {
        case MATRIX_RESULT_8:
            return BACK2MAIN;

        case MATRIX_RESULT_P:
        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT:
        case MATRIX_RESULT_RIGHT:
            TIMES--;
            break;

        case MATRIX_RESULT_ENT:
            if (TONE_SET(index) == BACK2MAIN)
                return BACK2MAIN;
            LCD_ShowMenu31(menu_tone, 2, index);
            break;

        case MATRIX_RESULT_CLR:
            return CLR2LAST;
        }
        if (TIMES != 0) //
        {
            TIMES = 0;
            index = !index;
            LCD_ShowMenu31(menu_tone, 2, index);
        }
        switch (Encoder_Switch_Scan(0))
        {
        case key_click:
            if (TONE_SET(index) == BACK2MAIN)
                return BACK2MAIN;
            PRE_TONE = load_PreTone();
            END_TONE = load_EndTone();
            LCD_ShowMenu31(menu_tone, 2, index);
            break;

        case key_double:
            return CLR2LAST;

        case key_long:
            SHUT();
            break;
        }
    }
}
//
//


extern volatile u8 key_timer_cnt1, key_timer_cnt2;
void disposePer100ms(void) //100ms
{

    //清除中断标志位
    if (SQL_CTL)
        sql_cal++;
    if (SCAN_CTL)
        scan_cal++;

    Cal2Shut();
    key_timer_cnt1++;
    key_timer_cnt2++;
}
//

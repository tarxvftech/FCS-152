#include "main.h"
#define DUAL_SWITCH_TIME        TMR_PERIOD_1S
volatile u8 WFM = OFF;                          //FM switch
volatile u8 KDU_INSERT = OFF;                   //KDU connection flag OFF: Disconnect, ON: connection

extern int TIMES;                               //Record encoder operation
///////////////////////////////Used as a restriction, the code in the conditional license is allowed to be executed once
volatile u8 FLAG_PTT_ONCE = 0;                  //Settings when PTT is pressed/released
volatile u8 FLAG_SQ_ONCE  = 0;                  //Settings when receiving/disappearing signals
volatile u8 SQUELCH_ONCE  = 0;                  //Enter/exit the long squelch setting, execute it once
volatile u8 SQL_CTL  = OFF;                     //Start squelch and count by time for a long time
volatile u8 SCAN_CTL = OFF;                     //Start scan recovery time count
//Used as a count, executable code that reaches a certain number of times/time
u16 scan_cal = 0;                               //Used to calculate the time after the signal disappears in scanning mode
u16 sql_cal  = 0;                               //Used to calculate the time to press the squelch button
//Mode flag
volatile char Home_Mode = 0;                    //Home page display mode //0: normal, 1: large characters 2: dual chan watch
volatile char Flag_Main_Page = 1;               //Home display mode (?)
volatile char SQL_MODE = OFF;                   //Long squelch mode (?)

double STEP_LEVEL[] = {0.0050, 0.0100, 0.0125};    //Step level - they missed a 6,25 kHz step, but fixed this in firmware v.2.1.1226

u8 RSSI = 0;
u8 SC=3;//screen contrast
//CHAN = 0,
u8 STEP = 2;
u8 SQL = 4;
u8 AUD = 0;
u8 MIC = 4;
u8 ENC = 0;
u8 TOT = 0;
u8 BL = 40;
u8 VDO = 0;                                     //VDO: Output power
u8 VOLUME = 4;
u8 PRE_TONE=0;
u8 END_TONE=0;
u8 A20_LEVEL[8] = {0, 20, 40,  60,  80,  100, 150, 255};
u8 WFM_LEVEL[8] = {0,  5, 15,  30,  50,   75, 100, 200};    //{0, 31, 62, 93, 125, 155, 178, 254},//
u8 MIC_LEVEL[3] = {64, 165, 230};               //  MIC_LEVEL[8] = {0, 32, 64,  96, 160, 170, 195, 230};//{0, 32, 64,  96, 160, 170, 195, 230}

int FM_FREQ = 885;
int LAMP_TIME = 10000;                          //Default 10 sec



void VFO_Load_Data(void) {
    //Reload data
    if (get_Flag(FLAG_CF_SWITCH_ADDR)) {
        //Frequency mode
        if (get_Flag(FLAG_VU_SWITCH_ADDR)) {
            chan_arv[NOW].CHAN = 100;
        } else {
            chan_arv[NOW].CHAN = 0;
        }
    } else {
        chan_arv[NOW].CHAN = load_CurrentChannel();
    }

    load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
    chan_arv[TMP] = chan_arv[NOW];

    //Load dual-guard mode data - which is where it switches between two channels, listening for activity on each, and stopping on whichever has traffic.
    chan_arv[CHANA].CHAN = load_ChanA();
    chan_arv[CHANB].CHAN = load_ChanB();
    load_ChannelParameter(chan_arv[CHANA].CHAN, &chan_arv[CHANA]);
    load_ChannelParameter(chan_arv[CHANB].CHAN, &chan_arv[CHANB]);


    STEP = load_Step();                         //Step read in
    SQL = load_Sql();                           //Squelch read in
    AUD = load_AudioSelect();                   //Audio output mode loading
    if (AUD) {
        MIC = load_MicLevel();                  //mic_level read in
    } else {
        MIC = 1;
    }
    ENC = load_ScramLevel();                    //Encryption level read in
    TOT = load_Tot();                           //Timeout level read in
    BL = load_Backlightness();                  //Backlight level read in
    SC = load_ScreenContrast();                 //Contrast level read in
    LCD_LightRatio(0x00 + SC * 5);
    LAMP_TIME = load_LampTime() * 10000;        //Backlight time, time adjustment. Two modes: "constant light" / "10 Sec" - 
                                                //as a PRC-152 radio user I think 2 sec, 5 sec modes also needed.

    VDO = load_VDO();                           //Output power status read in
    VOLUME = load_OverVolume();                 //Volume read in
    PRE_TONE = load_PreTone();                  //Pre-tone read in
    END_TONE = load_EndTone();                  //End-tone read in
    FM_FREQ = load_FMFreq();
    WFM = OFF;                                  //FM radio

    VDO_SWITCH(VDO);                            //Six-pin power output
    //printf("STEP:%d, SQL:%d, AUD:%d, MIC:%d, ENC:%d, TOT:%d, BL:%d, VDO:%d, VOLUME:%d, PRE_TONE:%d, END_TONE:%d, WFM:%d\n", STEP, SQL, AUD, MIC, ENC, TOT, BL, VDO, VOLUME, PRE_TONE, END_TONE, WFM);

    delay_ms(250);
    Set_A20(chan_arv[NOW], SQL);
    Set_A20_MIC(MIC * 3 + 1, ENC, TOT);

    M62364Reset();
    BackLight_SetVal(BL);
    bsp_StartAutoTimer(TMR_FLOW, LAMP_TIME);
}

//Main interface initialization
void VFO_Clear() {
    D_printf("***************Main Interface*************\n");
    LCD_Clear(GLOBAL32);
    Flag_Main_Page = 1;

    LCD_ShowString0408(0, 0, "R BAT", 1);
    LCD_ShowBattery(Get_Battery_Vol());
    LCD_ShowString0408(48, 0, "VULOS MOI", 1);
    LCD_ShowString0408(88, 0, "------- PT", 1);
    LCD_ShowVolume(VOLUME);
    //Mode 3: Dual guard mode (?)
    //DualMode_Clear
    if (Home_Mode == DUAL_MODE) {
        LCD_ShowString0608(0, 1, "A:                    ", 1, 128);
        LCD_ShowString0608(0, 2, "B:                    ", 1, 128);
        LCD_ShowString0608(0, 3, "                      ", 1, 128);
        LCD_ShowString0408(0, 3, "DUAL STANDBY MODE.    ", 1);
        bsp_StartAutoTimer(TMR_DUAL_REFRESH, DUAL_SWITCH_TIME);         //Start 500 ms to switch the frequency once
    }
}

//Homepage interface refresh
char BIG_MODE_buf[12] = {0};
char sele_pos = 0;                      //In dual-guard and dual-standby mode, the selected channel
char now_chan = 0;                      //Currently set channel
char rcv_chan = 0;                      //The channel where the signal is received

void VFO_Refresh() {
    // D_printf("%s\n", __FUNCTION__);
    switch (Home_Mode) {
    case MAIN_MODE:
        if (chan_arv[NOW].CHAN == 100) {
            LCD_ShowString0608(66, 1, "UHF ", 1, 90);
        } else if (chan_arv[NOW].CHAN == 0) {
            LCD_ShowString0608(66, 1, "VHF ", 1, 90);
        } else {
            LCD_ShowString0608(66, 1, "CHAN", 1, 90);
        }

        //Channel number display
        LCD_ShowChan(83, 2, chan_arv[NOW].CHAN, 1);

        if (PTT_READ) {
            //When idle, display frequency and radio status
            LCD_ShowFreq(0, 1, chan_arv[NOW].RX_FREQ, 1);
            LCD_ShowString0608(56, 2, WFM ? "WFM" : "FM ", 1, 128);
        } else {
            LCD_ShowFreq(0, 1, chan_arv[NOW].TX_FREQ, 1);
        }

        //Send and receive different frequency signs
        if (chan_arv[NOW].RX_FREQ != chan_arv[NOW].TX_FREQ) {
            LCD_ShowPIC0808(92, 1, 0);
        } else {
            LCD_ShowString0408(92, 1, "  ", 1);
        }

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

    //Dual guard mode
    case DUAL_MODE:
        sprintf(BIG_MODE_buf, "%02d-%s", chan_arv[CHANA].CHAN, chan_arv[CHANA].NN);
        LCD_ShowString0608(13, 1, BIG_MODE_buf, 1, 82);

        sprintf(BIG_MODE_buf, "%02d-%s", chan_arv[CHANB].CHAN, chan_arv[CHANB].NN);
        LCD_ShowString0608(13, 2, BIG_MODE_buf, 1, 82);

        LCD_ShowString0608(82, 2 - sele_pos, "  ", 1, 128);         //hook
        LCD_ShowPIC0808(82, sele_pos + 1, 1);

        break;
    }
    //Signal and transceiver status display
    if (PTT_READ) {
        LCD_ShowAscii0408(0, 0, 'R');
        LCD_ShowSignal(RSSI);               //Signal detection
    } else {
        LCD_ShowAscii0408(0, 0, 'T');
        LCD_ShowSignal(100);
    }
}
//

//Homepage encoder event handling //Homepage Coder Events (dammit google translate!)
void Encoder_process(u8 operate) {
    // D_printf("%s\n", __FUNCTION__);
    switch (operate) {                      //encoder event handling
    case key_click:
        if (Home_Mode == MAIN_MODE) {
            LCD_ShowAscii0608(60, 1, ' ', 1);
            ShortCut_Menu();
        } else if (Home_Mode == DUAL_MODE) {
            sele_pos = (sele_pos + 1) % 2;
        } else {
            D_printf("BIG_MODE\n");
        }
        break;

    case key_double:
        break;

    case key_long:
        SHUT();
        break;
    }
}
//matrix button event
u8 Event_Matrix(u8 matrix_key) {
    if (PTT_READ == 0) {
        return NO_OPERATE;
    }

    u8 pre_mode = 0;
    switch (matrix_key) {
    case MATRIX_RESULT_1:
        D_printf("{1}\n");
        if (Home_Mode) {
            return NO_OPERATE;
        }

        D_printf("NOW : V_SETTING\n");

        pre_mode = get_Flag(FLAG_VU_SWITCH_ADDR);       //The current VU frequency band is used to retain the current frequency band
        //Not used as a reason to "enter different frequency band settings":
        //Reduce the judgment after confirmation, and ensure that the memory of VU is only modified in FREQ mode

        if (get_Flag(FLAG_CF_SWITCH_ADDR)) {            //Change the display in FREQ mode
            if (pre_mode) {                             //If it is currently a U segment, change the display to "VHF"
                set_Flag(FLAG_VU_SWITCH_ADDR, 0);       //U segment becomes V segment
                LCD_ShowString0608(66, 1, "VHF ", 1, 90);
                chan_arv[NOW].CHAN = 0;
            }
        } else {                                        //Arrows are displayed in CHANNEL mode
            LCD_ShowPIC0608(60, 1, 1, 1);
        }

        switch (RT_FREQ_Set(0, 1, (double *)&chan_arv[NOW].RX_FREQ, 0)) {       //In any case, the V segment is set
        case ENT2LAST:
            chan_arv[NOW].TX_FREQ = chan_arv[NOW].RX_FREQ;
            return SAVE_SET;

        case CLR2LAST:
        case BACK2MAIN:
            if (pre_mode) {
                if (get_Flag(FLAG_CF_SWITCH_ADDR)) {
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
        if (Home_Mode) {
            return NO_OPERATE;
        }

        pre_mode = get_Flag(FLAG_VU_SWITCH_ADDR);
        D_printf("NOW : U_SETTING\n");
        if (get_Flag(FLAG_CF_SWITCH_ADDR)) {
            if (pre_mode == 0) {
                set_Flag(FLAG_VU_SWITCH_ADDR, 1);
                LCD_ShowString0608(66, 1, "UHF ", 1, 90);
                chan_arv[NOW].CHAN = 100;
            }
        } else {
            LCD_ShowPIC0608(60, 1, 1, 1);
        }

        switch (RT_FREQ_Set(0, 1, (double *)&chan_arv[NOW].RX_FREQ, 1)) {
        case ENT2LAST:
            chan_arv[NOW].TX_FREQ = chan_arv[NOW].RX_FREQ;
            return SAVE_SET;

        case CLR2LAST:
        case BACK2MAIN:
            if (pre_mode == 0) {
                if (get_Flag(FLAG_CF_SWITCH_ADDR)) {
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
        if (Home_Mode) {
            return NO_OPERATE;
        }
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
        if (get_Flag(FLAG_CF_SWITCH_ADDR) == 0 && Home_Mode == 0) {
            LCD_ShowAscii0608(60, 1, ' ', 1);
        }
        return NO_OPERATE;

    case MATRIX_RESULT_ENT:
        D_printf("{ENT}\n");
        if (Home_Mode) {
            return NO_OPERATE;
        }
        LCD_ShowAscii0608(60, 1, ' ', 1);
        ShortCut_Menu();
        return NO_OPERATE;

    case MATRIX_RESULT_LEFT:
        D_printf("{<}\n");
        if (Home_Mode == BIG_MODE) {
            return NO_OPERATE;
        } else if (Home_Mode == DUAL_MODE) {
            sele_pos = (sele_pos + 1) % 2;
            return NO_OPERATE;
        }
        //
        if (get_Flag(FLAG_CF_SWITCH_ADDR)) {
            D_printf("FREQ>>>CHAN%d\n", __LINE__);
            LCD_ShowPIC0608(60, 1, 1, 1);
            set_Flag(FLAG_CF_SWITCH_ADDR, 0);
            chan_arv[NOW].CHAN = load_CurrentChannel();
        } else {
            D_printf("CHAN>>>FREQ%d\n", __LINE__);
            LCD_ShowAscii0608(60, 1, ' ', 1);
            set_Flag(FLAG_CF_SWITCH_ADDR, 1);

            chan_arv[NOW].CHAN = (get_Flag(FLAG_VU_SWITCH_ADDR) ? 100 : 0);
            D_printf("CHAN:%d\n", chan_arv[NOW].CHAN);
        }
        return RELOAD_ARG;

    case MATRIX_RESULT_RIGHT:
        D_printf("{>}\n");
        if (Home_Mode == BIG_MODE) {
            return NO_OPERATE;
        } else if (Home_Mode == DUAL_MODE) {
            sele_pos = (sele_pos + 1) % 2;
            return NO_OPERATE;
        }
        //
        if (get_Flag(FLAG_CF_SWITCH_ADDR)) {
            u8 vu_mode = get_Flag(FLAG_VU_SWITCH_ADDR);
            vu_mode = !vu_mode;
            set_Flag(FLAG_VU_SWITCH_ADDR, vu_mode);
            chan_arv[NOW].CHAN = vu_mode * 100;
            return RELOAD_ARG;
        } else {
            LCD_ShowAscii0608(60, 1, ' ', 1);
        }
        return NO_OPERATE;

    case MATRIX_RESULT_0:
        if (get_Flag(FLAG_CF_SWITCH_ADDR)) {
            return NO_OPERATE;
        }
        Home_Mode = (Home_Mode + 1) % 3;
        LCD_Clear(EDITZONE32);
        LCD_ShowString1016(14, 1, "WAITING...", 1, 128);
        delay_ms(1000);
        if (Home_Mode != DUAL_MODE) {
            bsp_StopTimer(TMR_DUAL_REFRESH);    //Stop dual-guard switching when the display mode is switched
        }
        VFO_Clear();                            //When refreshing the page, it detects whether the current page is in dual-guard mode, and if so, restarts the dual-guard switching timing.
        if (Home_Mode != BIG_MODE) {            //The switched mode is not BIG_MODE mode, and the normal squelch state is turned off
            SetNowChanSql0(OFF);                //Turn off squelch mode
        }
        return NO_OPERATE;

    default:
        break;
    }
    if (TIMES > 0) {
        TIMES = 0;
        // D_printf("{+}: %d\n", en_times++);
        if (Home_Mode == DUAL_MODE) {
            if (sele_pos) {
                if (++chan_arv[CHANB].CHAN > 99) {
                    chan_arv[CHANB].CHAN = 1;
                }
                save_ChanB(chan_arv[CHANB].CHAN);
                load_ChannelParameter(chan_arv[CHANB].CHAN, &chan_arv[CHANB]);
            } else {
                if (++chan_arv[CHANA].CHAN > 99) {
                    chan_arv[CHANA].CHAN = 1;
                }
                save_ChanA(chan_arv[CHANA].CHAN);
                load_ChannelParameter(chan_arv[CHANA].CHAN, &chan_arv[CHANA]);
            }
            if (SQL_MODE == ON) {
                SetNowChanSql0(OFF);                //Turn off the normal squelch state, set the normal squelch and resume switching
            }
            return NO_OPERATE;
        }
        if (get_Flag(FLAG_CF_SWITCH_ADDR)) {        //Frequency mode
            if (Home_Mode) {
                return NO_OPERATE;
            }
            if (get_Flag(FLAG_VU_SWITCH_ADDR)) {    //U
                if (chan_arv[NOW].RX_FREQ + STEP_LEVEL[STEP] <= 480.0) {
                    chan_arv[NOW].RX_FREQ += STEP_LEVEL[STEP];
                } else {
                    chan_arv[NOW].RX_FREQ = 400.0;
                }
            } else {
                if (chan_arv[NOW].RX_FREQ + STEP_LEVEL[STEP] <= 174.0) {
                    chan_arv[NOW].RX_FREQ += STEP_LEVEL[STEP];
                } else {
                    chan_arv[NOW].RX_FREQ = 136.0;
                }
            }
            chan_arv[NOW].TX_FREQ = chan_arv[NOW].RX_FREQ;
            return SAVE_SET;
        } else {                                    //Channel mode
            if (++chan_arv[NOW].CHAN > 99) {
                chan_arv[NOW].CHAN = 1;
            }
            load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
            if (Home_Mode == 0) {
                LCD_ShowPIC0608(60, 1, 1, 1);
            } else if (Home_Mode == 1) {
                LCD_ShowString1016(0, 1, "       ", 1, 80);
                sprintf(BIG_MODE_buf, "%02d-%s", chan_arv[NOW].CHAN, chan_arv[NOW].NN);
                LCD_ShowString1016(0, 1, BIG_MODE_buf, 1, 128);
            }
            save_CurrentChannel(chan_arv[NOW].CHAN);
            return RELOAD_ARG;  //
        }
    } else if (TIMES < 0) {
        TIMES = 0;

        if (Home_Mode == DUAL_MODE) {
            if (sele_pos) {
                if (--chan_arv[CHANB].CHAN < 1) {
                    chan_arv[CHANB].CHAN = 99;
                }
                save_ChanB(chan_arv[CHANB].CHAN);
                load_ChannelParameter(chan_arv[CHANB].CHAN, &chan_arv[CHANB]);
            } else {
                if (--chan_arv[CHANA].CHAN < 1) {
                    chan_arv[CHANA].CHAN = 99;
                }
                save_ChanA(chan_arv[CHANA].CHAN);
                load_ChannelParameter(chan_arv[CHANA].CHAN, &chan_arv[CHANA]);
            }
            if (SQL_MODE == ON) {
                SetNowChanSql0(OFF);                  //Turn off the normal squelch state, set the normal squelch and resume switching
            }
            return NO_OPERATE;
        }
        if (get_Flag(FLAG_CF_SWITCH_ADDR)) {          //Frequency mode
            if (Home_Mode) {
                return NO_OPERATE;
            }
            if (get_Flag(FLAG_VU_SWITCH_ADDR)) {      //U
                if (chan_arv[NOW].RX_FREQ - STEP_LEVEL[STEP] >= 400.0) {
                    chan_arv[NOW].RX_FREQ -= STEP_LEVEL[STEP];
                } else {
                    chan_arv[NOW].RX_FREQ = 480.0;
                }
            } else {                                  //V
                if (chan_arv[NOW].RX_FREQ - STEP_LEVEL[STEP] >= 136.0) {
                    chan_arv[NOW].RX_FREQ -= STEP_LEVEL[STEP];
                } else {
                    chan_arv[NOW].RX_FREQ = 174.0;
                }
            }
            chan_arv[NOW].TX_FREQ = chan_arv[NOW].RX_FREQ;
            return SAVE_SET;
        } else {                                       //Channel mode
            if (--chan_arv[NOW].CHAN < 1) {
                chan_arv[NOW].CHAN = 99;
            }
            load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
            if (Home_Mode == 0) {
                LCD_ShowPIC0608(60, 1, 1, 1);
            } else if (Home_Mode == 1) {
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

//Processing of the return value after the sending and receiving parameters have changed
void Argument_process(u8 key_pro_ret) {

    switch (key_pro_ret) {                             //Matrix key event handling
    case RELOAD_ARG:                                   //The channel number has changed, reload the parameters
        D_printf("****************Reload Argument************\n");
        load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
        Set_A20(chan_arv[NOW], SQL);
        break;

    case SAVE_SET:                                     //Frequency and other parameters change, save and set once
        D_printf("****************Save Argument**************\n");
        save_ChannelParameter(chan_arv[NOW].CHAN, chan_arv[NOW]);
        Set_A20(chan_arv[NOW], SQL);
        break;
    default:
        break;
    }
}
//

void PTT_Control(void) {                               //Post-processing after pressing and releasing PTT
    //PTT button
    if (PTT_READ) {                                    //Release PTT
        if (FLAG_PTT_ONCE) {                           //The function of the "FLAG_PTT_ONCE" flag is: Let the program execute only once
            FLAG_PTT_ONCE = 0;
            TIMES = 0;
            if (END_TONE) {
                Start_Tone(0);
            }
            delay_ms(200);
            A002_PTT_SET;                              //Turn off PTT

            MIC_SWITCH(AUD, OFF);

            if (WFM) {
                bsp_StartTimer(TMR_FM_CTRL, TMR_PERIOD_8S);
            }
            if (Home_Mode == DUAL_MODE) {               //After releasing the PTT, wait for a certain period of time before resuming switching channels
                bsp_StartAutoTimer(TMR_DUAL_REFRESH, DUAL_SWITCH_TIME);    //Just resume the switch
            }

            if (KDU_INSERT) {
                SendALL();
            } else {
                LCD_ShowAscii0408(0, 0, 'R');
            }

            bsp_StartAutoTimer(TMR_VOLT_REFRESH, TMR_PERIOD_3S);            //Restore timing to detect voltage
        }
    } else {                                                                //Press PTT
        if (FLAG_PTT_ONCE == 0) {
            if (PTT_READ == 0) {
                FLAG_PTT_ONCE = 1;

                if (WFM) {                                                  //If the radio is on, turn off the FM radio
                    RDA5807_Init(OFF);
                    bsp_StopTimer(TMR_FM_CTRL);
                }
                //Turn off the normal squelch state and set the parameters
                SetNowChanSql0(OFF);

                A002_PTT_CLR;

                if (KDU_INSERT) {
                    SendALL();
                } else {
                    LCD_ShowAscii0408(0, 0, 'T');
                }

                MIC_SWITCH(AUD, ON);                            //==> send
                delay_ms(300);                                  //At least 300 ms
                if (PRE_TONE) {
                    Start_Tone(1);                              //The switch of the matrix is completed within the function
                }

                /////////////////////////
                bsp_StopTimer(TMR_VOLT_REFRESH);                //Stop voltage detection

                //Clear shutdown countdown
                ClearShut();

            }
        }
    }
    //
}
void SQ_Read_Control(void) {                                    //Receive and disconnect signal processing
    A002_CALLBACK();
    if (KDU_INSERT == OFF) {
        KDUCheck();
    }

    if (LAMP_TIME && bsp_CheckTimer(TMR_FLOW)) {
        BackLight_SetVal(1);
    }
    if (!PTT_READ) {
        return;
    }
    //Signal judgment (?) 
    if (A002_SQ_READ) {
        if (FLAG_SQ_ONCE) {
            FLAG_SQ_ONCE = 0;
            SPK_SWITCH(AUD, OFF);
            M62364_SetSingleChannel(A20_LINE_CHAN, 0);          //Turn off the walkie-talkie sound in your spare time
            RSSI = 0;
            if (WFM) {
                bsp_StartTimer(TMR_FM_CTRL, TMR_PERIOD_8S);
            }
            if (Home_Mode == DUAL_MODE) {                       //Restore the countdown setting of dual guard mode
                if (SQL_MODE == OFF) {                          //In a very squelch state, channel switching is restored directly after there is no signal
                    bsp_StartAutoTimer(TMR_DUAL_REFRESH, DUAL_SWITCH_TIME);
                }
            }

            if (KDU_INSERT) {
                SendALL();
            }
        }
    } else {
        SPK_SWITCH(AUD, ON);
        if (FLAG_SQ_ONCE == 0) {
            if (A002_SQ_READ == 0) {
                if (Home_Mode == DUAL_MODE) {
                    bsp_StopTimer(TMR_DUAL_REFRESH);                            //Stop switching
                    rcv_chan = now_chan;
                    if (Flag_Main_Page && KDU_INSERT==0) {
                        LCD_ShowString0608(74, 2 - now_chan, " ", 1, 95);
                        LCD_ShowPIC0608(74, now_chan + 1, 0, 1);                //Arrow table reception
                    }
                }
                M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);      //Turn on the walkie-talkie sound
                FLAG_SQ_ONCE = 1;
                if (WFM) {
                    RDA5807_Init(OFF);
                    bsp_StopTimer(TMR_FM_CTRL);
                }

                ClearShut();
                if (KDU_INSERT) {
                    SendALL();
                }
            }
        }
        //Get the signal strength every 500 ms after receiving the signal
        if (bsp_CheckTimer(TMR_RSSI_CTRL)) {
            RSSI = Get_A20_RSSI();
        }

        //
    }
    //
    Switch_Dual_Chan();
}

void SetNowChanSql0(u8 on) {
    if (SQL_MODE == ON) {                                               //Turn off squelch mode
        SQUELCH_ONCE = 1;
        SQL_MODE = OFF;
    }
    if (Home_Mode == DUAL_MODE) {
        if (on) {
            bsp_StopTimer(TMR_DUAL_REFRESH);
        } else {
            bsp_StartAutoTimer(TMR_DUAL_REFRESH, DUAL_SWITCH_TIME);     //Restore the switch of dual guard mode
        }

        now_chan = sele_pos;
        if (now_chan) {
            Set_A20(chan_arv[CHANB], on?0:SQL);
        } else {
            Set_A20(chan_arv[CHANA], on?0:SQL);
        }
    } else {
        Set_A20(chan_arv[NOW], on?0:SQL);
    }
}

void SQUELCH_Control(void) {                            //Press and release squelch processing
    if (!PTT_READ) {
        return;
    }

    if (SQUELCH_READ == 0) {                            //Press the squelch button
        if (SQUELCH_ONCE == 0) {                        //Enter squelch 0 mode and start the press timing
            SQUELCH_ONCE = 1;
            if (WFM) {                                  //If the radio is on, turn off the FM radio
                RDA5807_Init(OFF);
                bsp_StopTimer(TMR_FM_CTRL);
            }
            //
            if (SQL_MODE == OFF) {                      //Non-long squelch state
                SetNowChanSql0(ON);                     //Set the squelch level of the current channel to 0
                ///////////////////////////////////////////////////
                SPK_SWITCH(AUD, ON);
                M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);      //Turn on the walkie-talkie sound
                SQL_CTL = ON;                                                   //Start counting 
                                                                                //Only in non-long squelch mode will sql_cal>=30 be made, the rest is 0;
            }
            ClearShut();
        }
        if (sql_cal == 30) {                            //Timed for up to three seconds, enter long squelch mode
            SQL_CTL = 0;                                //Stop counting
            sql_cal++;                                  //The counting has stopped, here to avoid repeated entry into the processing operation
            Start_ToneSql0();                           //Normally squelch mode tone
            SQL_MODE = 1;                               //Enter long squelch mode
        } else if (sql_cal < 30) {                      //Less than three seconds
            SQL_MODE = OFF;
        }
    } else {                                            //Release the squelch button
        if (SQUELCH_ONCE) {
            SQUELCH_ONCE = 0;
            if (sql_cal < 30) {
                SPK_SWITCH(AUD, OFF);
            }

            SQL_CTL = 0;
            sql_cal = 0;

            if (SQL_MODE == OFF) {
                SetNowChanSql0(OFF);
                ///////////////////////////////////////////////
                if (WFM) {
                    bsp_StartTimer(TMR_FM_CTRL, TMR_PERIOD_8S);
                }
            }
        }
    }
    //
}
//
void Switch_Dual_Chan(void) {                           //Switch channel reception on time in dual-guard mode
    if (bsp_CheckTimer(TMR_DUAL_REFRESH)) {
        now_chan = (now_chan + 1) % 2;
        if (now_chan) {
            Set_A20(chan_arv[CHANB], SQL);
        } else {
            Set_A20(chan_arv[CHANA], SQL);
        }
        // Serial.printf("Switch Channel:%d\n", now_chan);
    }
}
//

void LoadAgrv2Buf(char * buf, CHAN_ARV B) {
    sprintf(buf + chan_rank, "%03d", B.CHAN);
    sprintf(buf + rx_rank, "%3.4f", B.RX_FREQ);
    sprintf(buf + tx_rank, "%3.4f", B.TX_FREQ);
    sprintf(buf + rs_rank, "%03d", B.RS);
    sprintf(buf + ts_rank, "%03d", B.TS);
    sprintf(buf + nn_rank, "%s", B.NN);

    buf[pw_rank] = kdu_send_data(B.POWER);
    buf[bw_rank] = kdu_send_data(B.GBW);
    buf[scan_rank] = 0;

    //  for(int i=0; i<7; i++)
    //      buf[nn_rank+i] = B.NN[i];
}

extern unsigned char FM_CHAN;
void SendALL(void) {                                    //Send all data
    char send_buf[256] = {0};
    strcpy(send_buf, prefix_buf[ASKALL]);

    //  chan_arv[NOW].CHAN = CHAN;
    LoadAgrv2Buf(send_buf, chan_arv[NOW]);

    send_buf[cf_rank] = kdu_send_data(get_Flag(FLAG_CF_SWITCH_ADDR));
    send_buf[vu_rank] = kdu_send_data(get_Flag(FLAG_VU_SWITCH_ADDR));
    sprintf(send_buf + chana_rank, "%03d", chan_arv[CHANA].CHAN);
    sprintf(send_buf + chanb_rank, "%03d", chan_arv[CHANB].CHAN);

    send_buf[volume_rank] = kdu_send_data(VOLUME);
    send_buf[step_rank]   = kdu_send_data(STEP);
    send_buf[sql_rank]    = kdu_send_data(SQL);
    send_buf[aud_rank]    = kdu_send_data(AUD);
    send_buf[mic_rank]    = kdu_send_data(MIC);
    send_buf[enc_rank]    = kdu_send_data(ENC);
    send_buf[tot_rank]    = kdu_send_data(TOT);
    send_buf[op_rank]     = kdu_send_data(VDO);

    send_buf[pre_rank] = kdu_send_data(PRE_TONE);
    send_buf[end_rank] = kdu_send_data(END_TONE);
    sprintf(send_buf + ffreq_rank, "%04d", FM_FREQ);

    send_buf[wfm_rank] = kdu_send_data(WFM);
    send_buf[fmchan_rank] = kdu_send_data(FM_CHAN);

    if (PTT_READ == 0) {
        RSSI = 100;
    } else {
        if (A002_SQ_READ) {
            RSSI = 0;
        } else {
            RSSI = Get_A20_RSSI();
        }
    }

    sprintf(send_buf + volt_rank, "%03d", Get_Battery_Vol());
    sprintf(send_buf + rssi_rank, "%03d", RSSI);
    send_buf[sq_rank] = kdu_send_data(A002_SQ_READ);
    send_buf[squ_rank] = kdu_send_data(SQUELCH_READ);
    send_buf[ptt_rank] = kdu_send_data(PTT_READ);

    send_buf[homemode_rank] = kdu_send_data(Home_Mode);
    send_buf[nowrcvchan_rank] = kdu_send_data(rcv_chan);
    send_buf[nowselchan_rank] = kdu_send_data(sele_pos);            //Used to distinguish the transmission channel of the dual-guard mode

    UART1_Send_Message(send_buf, BUF_SIZE);
}

char send_buf[256] = {0};
static int EnterKDUCal = 0;                         //3 entries, increasing continuously
static int EXitKDUCal  = 0;                         //Exit 3 times, clear the EnterKDUCal value

int KDUCheck(void) {                                //KDU connection detection
    if (!PTT_READ) {
        return NO_OPERATE;
    }
    KDU_INSERT = OFF;
    //Less than 2 inquiries within the specified time: 
    //1. If there is an inquiry, the feedback is processed and counted; 
    //2. Detect whether the timing is exceeded, if the time is exceeded without inquiry, the count value will be cleared, and 152 will operate normally.
    if (EnterKDUCal<2) {
        if (UART1_getRcvFlag()) {
            UART1_dataPreProcess();
            if (strstr((char *)rx1_buf, prefix_buf[_ASKALL])) {
                SendALL();
                EnterKDUCal++;
                bsp_StartTimer(TMR_WAIT_KDU, TMR_PERIOD_1S*1.5);
            } else if (strstr((char *)rx1_buf, prefix_buf[_ASKA])) {            //Reply to KDU's inquiry about the dual-guard A channel parameters
                load_ChannelParameter(load_ChanA(), &chan_arv[TMP]);
                
                //Channel A
                //Write a reply message 
                //Send data preparation
                strcpy(send_buf, prefix_buf[ASKA]);

                LoadAgrv2Buf(send_buf, chan_arv[TMP]);

                send_buf[cf_rank] = kdu_send_data(kdu_recv_data(rx1_buf[cf_rank]));
                send_buf[vu_rank] = kdu_send_data(kdu_recv_data(rx1_buf[vu_rank]));

                UART1_Send_Message(send_buf, BUF_SIZE);
                EnterKDUCal++;
                bsp_StartTimer(TMR_WAIT_KDU, TMR_PERIOD_1S*1.5);
            } else if (strstr((char *)rx1_buf, prefix_buf[_ASKB])) {            //Reply to the dual-guard B channel parameters asked by KDU
                load_ChannelParameter(load_ChanB(), &chan_arv[TMP]);

                //Channel B
                //Write a reply message 
                //Send data preparation
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
        if (bsp_CheckTimer(TMR_WAIT_KDU)) {                     //Detects that the timing exceeds 1.5 sec, clear the count value and return
            EnterKDUCal = 0;
            return NO_OPERATE;
        }
    } else {
        KDU_INSERT = ON;
        return BACK2MAIN;
    }
    return NO_OPERATE;
}

int KDU_Processor(void) {                   //KDU post-insertion processing
    if (EnterKDUCal>=2) {                   //Ask for data more than 2 times within the specified time, 152 is set to KDU connection mode;
        static u8 cf = 0;                   //Auxiliary inquiry channel parameters
        BackLight_SetVal(BL);
        LCD_Clear(GLOBAL32);
        LCD_ShowString0608(26, 2, "SEND AND LOAD", 1, 128);
        delay_ms(500);
        LCD_ShowString0608(26, 2, " KDU CONTROL ", 1, 128);
        BackLight_SetVal(0);
        D_printf("\nEnter KDU Mode\n");
        KDU_INSERT = ON;
        while (1) {
            FeedDog();                      //Feed the dog - again about a watchdog timer
            switch (Encoder_Switch_Scan(0)) {
            case key_long:
                SHUT();
                break;
            }


            PTT_Control();
            SQUELCH_Control();
            SQ_Read_Control();
            if (bsp_CheckTimer(TMR_FM_CTRL) && WFM) {
                RDA5807_Init(ON);
            }

            if (UART1_getRcvFlag()) {
                UART1_dataPreProcess();

                if (strstr((char *)rx1_buf, prefix_buf[_ASKALL])) {             //Reply to all parameters asked by KDU
                    SendALL();
                }
                //
                else if (strstr((char *)rx1_buf, prefix_buf[_SETHOMEMODE])) {   //Insert a team / Cut in line - Yandex translate vs. Google translate. Think what you want!! ;)
                    Home_Mode = kdu_recv_data(rx1_buf[homemode_rank]);
                    strcpy(send_buf, prefix_buf[SETHOMEMODE]);
                    send_buf[homemode_rank] = kdu_send_data(Home_Mode);
                    UART1_Send_Message(send_buf, BUF_SIZE);

                    if (Home_Mode == MAIN_MODE) {
                        bsp_StopTimer(TMR_DUAL_REFRESH);                        //Stop switching timing
                        Set_A20(chan_arv[NOW], SQL);
                    } else if (Home_Mode == DUAL_MODE) {
                        bsp_StartAutoTimer(TMR_DUAL_REFRESH, DUAL_SWITCH_TIME);    //Start 500ms to switch the frequency once
                    }
                }
                //

                else if (strstr((char *)rx1_buf, prefix_buf[_ASKA])) {          //Reply to KDU's inquiry about the dual-guard A channel parameters
                    load_ChannelParameter(load_ChanA(), &chan_arv[TMP]);

                    //Channel A
                    //Write a reply message
                    //Send data preparation
                    strcpy(send_buf, prefix_buf[ASKA]);

                    LoadAgrv2Buf(send_buf, chan_arv[TMP]);

                    send_buf[cf_rank] = kdu_send_data(kdu_recv_data(rx1_buf[cf_rank]));
                    send_buf[vu_rank] = kdu_send_data(kdu_recv_data(rx1_buf[vu_rank]));

                    UART1_Send_Message(send_buf, BUF_SIZE);
                } else if (strstr((char *)rx1_buf, prefix_buf[_ASKB])) {        //Reply to the dual-guard B channel parameters asked by KDU
                    load_ChannelParameter(load_ChanB(), &chan_arv[TMP]);

                    //Channel B
                    //Write a reply message
                    //Send data preparation
                    strcpy(send_buf, prefix_buf[ASKB]);
                    LoadAgrv2Buf(send_buf, chan_arv[TMP]);

                    send_buf[cf_rank] = kdu_send_data(kdu_recv_data(rx1_buf[cf_rank]));
                    send_buf[vu_rank] = kdu_send_data(kdu_recv_data(rx1_buf[vu_rank]));

                    UART1_Send_Message(send_buf, BUF_SIZE);
                } else if (strstr((char *)rx1_buf, prefix_buf[_ASKCHAN])) {     //Reply to the channel parameters asked by KDU

                    cf = kdu_recv_data(rx1_buf[cf_rank]);

                    //Get channel number
                    chan_arv[TMP].CHAN = (rx1_buf[chan_rank + 0] - '0') * 100 + (rx1_buf[chan_rank + 1] - '0') * 10 + (rx1_buf[chan_rank + 2] - '0');

                    //Get parameters by channel number
                    load_ChannelParameter(chan_arv[TMP].CHAN, &chan_arv[TMP]);

                    //Write a reply message
                    //Send data preparation
                    strcpy(send_buf, prefix_buf[ASKCHAN]);
                    LoadAgrv2Buf(send_buf, chan_arv[TMP]);

                    send_buf[cf_rank] = kdu_send_data(kdu_recv_data(rx1_buf[cf_rank]));
                    send_buf[vu_rank] = kdu_send_data(kdu_recv_data(rx1_buf[vu_rank]));

                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                //

                //Send the overloaded channel parameters to the KDU - maybe Preloaded Channel (?)
                else if (strstr((char *)rx1_buf, prefix_buf[_RELOAD])) { //Overload (Preload ?) the current channel set by the KDU, and resend the data information to the KDU for confirmation
                    //Determine whether to overload (Preload ?) the channel number or channel and frequency switching
                    cf = kdu_recv_data(rx1_buf[cf_rank]);
                    chan_arv[NOW].CHAN = (rx1_buf[chan_rank + 0] - '0') * 100 + (rx1_buf[chan_rank + 1] - '0') * 10 + (rx1_buf[chan_rank + 2] - '0');

                    if (cf == get_Flag(FLAG_CF_SWITCH_ADDR)) {          //cf mode is not modified
                        if (cf) {                                       //FREQ mode: vu switch
                            set_Flag(FLAG_VU_SWITCH_ADDR, chan_arv[NOW].CHAN ? 1 : 0);
                        } else {                                        //CHAN mode: Channel number switching
                            save_CurrentChannel(chan_arv[NOW].CHAN);
                        }
                    } else {                                            //Channel>><<frequency
                        set_Flag(FLAG_CF_SWITCH_ADDR, cf);

                        if (cf) {
                            chan_arv[NOW].CHAN = get_Flag(FLAG_VU_SWITCH_ADDR) * 100;
                        } else {
                            chan_arv[NOW].CHAN = load_CurrentChannel();
                        }
                    }
                    //
                    load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);

                    //Write a reply message
                    //Send data preparation
                    strcpy(send_buf, prefix_buf[RELOAD]);
                    LoadAgrv2Buf(send_buf, chan_arv[NOW]);

                    send_buf[cf_rank] = kdu_send_data(cf);
                    send_buf[vu_rank] = kdu_send_data(kdu_recv_data(rx1_buf[vu_rank]));

                    Set_A20(chan_arv[NOW], SQL);
                    UART1_Send_Message(send_buf, BUF_SIZE);
                } else if (strstr((char *)rx1_buf, prefix_buf[_RELA])) { //Overload (Preload ?) the double-guard channel A set by KDU, and resend the data information to KDU for confirmation
                    chan_arv[CHANA].CHAN = (rx1_buf[chan_rank + 0] - '0') * 100 + (rx1_buf[chan_rank + 1] - '0') * 10 + (rx1_buf[chan_rank + 2] - '0');
                    save_ChanA(chan_arv[CHANA].CHAN);
                    load_ChannelParameter(chan_arv[CHANA].CHAN, &chan_arv[CHANA]);

                    strcpy(send_buf, prefix_buf[RELA]);
                    LoadAgrv2Buf(send_buf, chan_arv[CHANA]);

                    UART1_Send_Message(send_buf, BUF_SIZE);
                } else if (strstr((char *)rx1_buf, prefix_buf[_RELB])) { //Overload (Preload ?) the double-guard channel B set by KDU, and resend the data information to KDU for confirmation
                    chan_arv[CHANB].CHAN = (rx1_buf[chan_rank + 0] - '0') * 100 + (rx1_buf[chan_rank + 1] - '0') * 10 + (rx1_buf[chan_rank + 2] - '0');
                    save_ChanB(chan_arv[CHANB].CHAN);
                    load_ChannelParameter(chan_arv[CHANB].CHAN, &chan_arv[CHANB]);

                    strcpy(send_buf, prefix_buf[RELB]);
                    LoadAgrv2Buf(send_buf, chan_arv[CHANB]);
                    UART1_Send_Message(send_buf, BUF_SIZE);
                } else if (strstr((char *)rx1_buf, prefix_buf[_SETDUALPOS])) { //Set the channel selected in the dual-guard mode, and resend the data information to the KDU for confirmation
                    sele_pos = rx1_buf[nowselchan_rank] - '0';
                    strcpy(send_buf, prefix_buf[SETDUALPOS]);
                    send_buf[nowselchan_rank] = kdu_send_data(sele_pos);
                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                //
                //Receive the channel parameters set by the KDU, and resend the data information to the KDU for confirmation to prevent the KDU from unresponsive
                else if (strstr((char *)rx1_buf, prefix_buf[_SETCHAN])) {
                    chan_arv[NOW].CHAN = (rx1_buf[chan_rank + 0] - '0') * 100 + (rx1_buf[chan_rank + 1] - '0') * 10 + (rx1_buf[chan_rank + 2] - '0');

                    chan_arv[NOW].RX_FREQ = (rx1_buf[rx_rank + 0] - '0') * 100 + (rx1_buf[rx_rank + 1] - '0') * 10 + (rx1_buf[rx_rank + 2] - '0') + (rx1_buf[rx_rank + 4] - '0') * 0.1 +
                                            (rx1_buf[rx_rank + 5] - '0') * 0.01 + (rx1_buf[rx_rank + 6] - '0') * 0.001 + (rx1_buf[rx_rank + 7] - '0') * 0.0001;
                    chan_arv[NOW].TX_FREQ = (rx1_buf[tx_rank + 0] - '0') * 100 + (rx1_buf[tx_rank + 1] - '0') * 10 + (rx1_buf[tx_rank + 2] - '0') + (rx1_buf[tx_rank + 4] - '0') * 0.1 +
                                            (rx1_buf[tx_rank + 5] - '0') * 0.01 + (rx1_buf[tx_rank + 6] - '0') * 0.001 + (rx1_buf[tx_rank + 7] - '0') * 0.0001;

                    chan_arv[NOW].RS = (rx1_buf[rs_rank + 0] - '0') * 100 + (rx1_buf[rs_rank + 1] - '0') * 10 + (rx1_buf[rs_rank + 2] - '0');
                    chan_arv[NOW].TS = (rx1_buf[ts_rank + 0] - '0') * 100 + (rx1_buf[ts_rank + 1] - '0') * 10 + (rx1_buf[ts_rank + 2] - '0');

                    chan_arv[NOW].POWER = kdu_recv_data(rx1_buf[pw_rank]);
                    chan_arv[NOW].GBW = kdu_recv_data(rx1_buf[bw_rank]);

                    for (u8 i = 0; i < 7; i++) {
                        chan_arv[NOW].NN[i] = rx1_buf[nn_rank + i];
                    }

                    if (chan_arv[NOW].CHAN > 0 && chan_arv[NOW].CHAN < 100) {
                        save_CurrentChannel(chan_arv[NOW].CHAN);
                    }

                    //Reply to KDU to indicate that the setting is successful
                    strcpy(send_buf, prefix_buf[SETCHAN]);
                    LoadAgrv2Buf(send_buf, chan_arv[NOW]);

                    //
                    save_ChannelParameter(chan_arv[NOW].CHAN, chan_arv[NOW]);
                    Set_A20(chan_arv[NOW], SQL); //When the KDU button is too fast: before the two are sent, the KDU will react slowly; 
                    //after the two are sent, the subsequent data cannot be processed and the link is not returned to the KDU, causing the link to be lost. Bug (WTF!!??)

                    UART1_Send_Message(send_buf, BUF_SIZE);
                }
                //
                else if (strstr((const char *)rx1_buf, prefix_buf[_SETZERO])) {
                    set_Flag(RESETADDR, ~RESET_VAL);
                    SHUT();
                } else if (strstr((const char *)rx1_buf, prefix_buf[_SETSTEP])) {
                    STEP = kdu_recv_data(rx1_buf[step_rank]);
                    save_Step(STEP);

                    strcpy(send_buf, prefix_buf[SETSTEP]);
                    send_buf[step_rank] = kdu_send_data(STEP);
                    UART1_Send_Message(send_buf, BUF_SIZE);
                } else if (strstr((const char *)rx1_buf, prefix_buf[_SETSQL])) {
                    if (SQL != kdu_recv_data(rx1_buf[sql_rank])) {
                        SQL = kdu_recv_data(rx1_buf[sql_rank]);
                        save_Sql(SQL);
                        Set_A20(chan_arv[NOW], SQL);
                    }
                    strcpy(send_buf, prefix_buf[SETSQL]);
                    send_buf[sql_rank] = kdu_send_data(SQL);

                    UART1_Send_Message(send_buf, BUF_SIZE);
                } else if (strstr((const char *)rx1_buf, prefix_buf[_SETAUD])) {
                    if (kdu_recv_data(rx1_buf[aud_rank]) != AUD) {
                        SPK_SWITCH(AUD, OFF);
                        AUD = kdu_recv_data(rx1_buf[aud_rank]);

                        if (!A002_SQ_READ) {                //If there is a signal, modify it directly (?)
                            SPK_SWITCH(AUD, ON);
                        } else {
                            if (WFM) {
                                SPK_SWITCH(AUD, ON);
                            }
                        }
                        //
                    }
                    if (AUD == 0) {
                        MIC = 1;
                    } else {
                        MIC = kdu_recv_data(rx1_buf[mic_rank]);
                    }

                    save_AudioSelect(AUD);
                    save_MicLevel(MIC);
                    M62364_SetSingleChannel(4, MIC_LEVEL[MIC]);

                    strcpy(send_buf, prefix_buf[SETAUD]);
                    send_buf[aud_rank] = kdu_send_data(AUD);
                    send_buf[mic_rank] = kdu_send_data(MIC);

                    UART1_Send_Message(send_buf, BUF_SIZE);
                } else if (strstr((const char *)rx1_buf, prefix_buf[_SETENC])) {
                    ENC = kdu_recv_data(rx1_buf[enc_rank]);
                    save_ScramLevel(ENC);

                    strcpy(send_buf, prefix_buf[SETENC]);
                    send_buf[enc_rank] = kdu_send_data(ENC);

                    UART1_Send_Message(send_buf, BUF_SIZE);
                } else if (strstr((const char *)rx1_buf, prefix_buf[_SETTOT])) {
                    TOT = kdu_recv_data(rx1_buf[tot_rank]);
                    save_Tot(TOT);

                    strcpy(send_buf, prefix_buf[SETTOT]);
                    send_buf[tot_rank] = kdu_send_data(TOT);

                    UART1_Send_Message(send_buf, BUF_SIZE);
                } else if (strstr((const char *)rx1_buf, prefix_buf[_SETOP])) {
                    if (VDO != kdu_recv_data(rx1_buf[op_rank])) {
                        VDO = kdu_recv_data(rx1_buf[op_rank]);
                        save_VDO(VDO);
                        VDO_SWITCH(VDO);
                    }
                    strcpy(send_buf, prefix_buf[SETOP]);
                    send_buf[op_rank] = kdu_send_data(VDO);

                    UART1_Send_Message(send_buf, BUF_SIZE);
                } else if (strstr((const char *)rx1_buf, prefix_buf[_SETVOLU])) {
                    if (VOLUME != kdu_recv_data(rx1_buf[volume_rank])) {
                        VOLUME = kdu_recv_data(rx1_buf[volume_rank]);
                        save_OverVolume(VOLUME);
                        //Need to pay attention to the volume modification in the use of the received signal/FM
                        if (WFM) { //Do not interrupt the output when the radio is on, directly modify the volume
                            RDA5807_ResumeImmediately();
                            if (!A002_SQ_READ) {
                                M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[0]);
                                M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);
                            } else {
                                M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[VOLUME]);
                            }
                        } else {
                            if (A002_SQ_READ) {
                                SPK_SWITCH(AUD, 0);
                            }
                            M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[0]);       //0
                            M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);
                        }
                    }
                    //
                    strcpy(send_buf, prefix_buf[SETVOLU]);
                    send_buf[volume_rank] = kdu_send_data(VOLUME);

                    UART1_Send_Message(send_buf, BUF_SIZE);
                } else if (strstr((const char *)rx1_buf, prefix_buf[_SETTONE])) {
                    PRE_TONE = kdu_recv_data(rx1_buf[pre_rank]);
                    END_TONE = kdu_recv_data(rx1_buf[end_rank]);
                    save_PreTone(PRE_TONE);
                    save_EndTone(END_TONE);

                    strcpy(send_buf, prefix_buf[SETTONE]);
                    send_buf[pre_rank] = kdu_send_data(PRE_TONE);
                    send_buf[end_rank] = kdu_send_data(END_TONE);

                    UART1_Send_Message(send_buf, BUF_SIZE);
                } else if (strstr((const char *)rx1_buf, prefix_buf[_SETFM])) {
                    int fmfreq = (rx1_buf[ffreq_rank + 0] - '0') * 1000 + (rx1_buf[ffreq_rank + 1] - '0') * 100 + (rx1_buf[ffreq_rank + 2] - '0') * 10 + (rx1_buf[ffreq_rank + 3] - '0');
                    if (WFM != kdu_recv_data(rx1_buf[wfm_rank])) {          //Perform FM switch
                        WFM = kdu_recv_data(rx1_buf[wfm_rank]);
                        RDA5807_Init(WFM);
                        if (WFM == OFF && A002_SQ_READ && PTT_READ) {
                            SPK_SWITCH(AUD, OFF);
                        }
                    } else {                            //Switch the frequency and judge the frequency
                        RDA5807_Set_Freq(fmfreq);
                        if (RDA5807_ReadReg(0xb) & 0x0100) {
                            FM_CHAN = 1;
                            save_FMFreq(fmfreq);
                        } else {
                            FM_CHAN = 0;
                        }
                    }
                    //SendALL();
                    FM_FREQ = fmfreq;                       //Required when polling
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

            if (bsp_CheckTimer(TMR_WAIT_KDU)) {                     //The timing is detected to exceed 1.5 sec, and the KDU exit value is calculated
                if (EXitKDUCal ++ >=2) {
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
    //Volume setting
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
            //Do not interrupt the output when the radio is on, directly modify the volume
            RDA5807_ResumeImmediately();
            if (!A002_SQ_READ) {
                M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[0]);
                M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);
            } else {
                M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[VOLUME]);
            }
        } else {
            if (A002_SQ_READ) {
                SPK_SWITCH(AUD, 0);
            }
            M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[0]);       //0
            M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);
        }
    }

    if (bsp_CheckTimer(TMR_VOLT_REFRESH) && PTT_READ && KDU_INSERT==0) {
        LCD_ShowBattery(Get_Battery_Vol());
    }
}
void MY_GLOBAL_FUN(void) {              //Global function function
    // D_printf("%s\n", __FUNCTION__);
    FeedDog(); //喂狗
    //PTT_Control();

    //SQUELCH_Control();
    //SQ_Read_Control();                  //Receiving signal processing, A002 setting response, dual-guard switching processing, KDU detection processing

    //VOL_Reflash();

    //if (bsp_CheckTimer(TMR_FM_CTRL) && WFM) {
        //RDA5807_Init(ON);
    //}
}

//

//quick settings on the main interface
void ShortCut_Menu(void) {
    u8 option_num = 0,
       Inc_select_change = 1,
       ENSURE = 0;

    TIMES = 0;
    //  LCD_ShowPIC0608(16, 3, 0, 1); //show < point sto the first option
    while (1) {
        MY_GLOBAL_FUN();
        if (KDU_INSERT) {
            return;
        }
        switch (Matrix_KEY_Scan(0)) {
        case MATRIX_RESULT_CLR:             //exit
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
        if (TIMES > 0) {
            option_num = (option_num + 1) % 5;
            TIMES = 0;
            Inc_select_change = 1;
        } else if (TIMES < 0) {
            option_num = (option_num + 5 - 1) % 5;
            TIMES = 0;
            Inc_select_change = 1;
        }

        if (Inc_select_change) {            //Display arrows to avoid repeated refreshes
            Inc_select_change = 0;
            LCD_ShowString0408(0, 3, "TYPE   TRF    MOD    CHAN  KEY  ", 1);
            switch (option_num) {
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

        switch (Encoder_Switch_Scan(0)) {
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
        if (ENSURE) {
            ENSURE = 0;
            switch (option_num) {
            case 0:                     //Graphic menu
                TIMES = 0;
                D_printf("TYPE\n");
                RT_Menu();
                VFO_Clear();
                return;

            case 1:                     //Microphone mode
                if (AUD == 0) {
                    break;
                }
                ShortCut_MICGAIN_Select();
                break;

            case 2:                     //FM radio
#if FM_EN
                ShortCut_FM_Select();
#endif
                break;

            case 3:                     //Channel number selection in channel mode
                if (!PTT_READ) {
                    break;
                }
                ShortCut_CHAN_Select();
                break;
            //

            case 4:                     //Lock screen
                LCD_ShowString0408(0, 3, "TYPE   TRF    MOD    CHAN  KEY  ", 1);
                Lock_Screen_KeyBoard();
                ClearShut();

                while (VOL_ADD_READ == 0 || VOL_SUB_READ == 0) {
                    FeedDog();          //Feed the dog - watchdog timer initialization (?)
                }
                break;
            }
            LCD_ShowString0408(0, 3, "TYPE   TRF    MOD    CHAN  KEY  ", 1);
            return;
        }
    }
}
//
void ShortCut_MICGAIN_Select(void) {    //The main interface quickly sets the mic sensitivity
    TIMES = 0;
    char trf = MIC, trf_old = MIC;
    D_printf("TRF\n");
    LCD_ShowString0608(26, 2, TRF_Show[trf], 0, 128);       //Display the selected level
    trf_old = trf;
    while (1) {
        MY_GLOBAL_FUN();
        if (KDU_INSERT) {
            return;
        }
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES > 0) {
            trf = (trf + TIMES) % 3;
            TIMES = 0;
            LCD_ShowString0608(26, 2, TRF_Show[trf], 0, 128);
        } else if (TIMES < 0) {
            trf = (trf + 3 - ((-TIMES) % 3)) % 3;
            TIMES = 0;
            LCD_ShowString0608(26, 2, TRF_Show[trf], 0, 128);
        }

        switch (Encoder_Switch_Scan(0)) {
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

void ShortCut_FM_Select(void) {             //The main interface quickly switches the radio
    TIMES = 0;
    char FM_now = WFM, FM_old = WFM;
    LCD_ShowString0608(56, 2, FM_Show[FM_now], 0, 128);
    while (1) {
        MY_GLOBAL_FUN();
        if (KDU_INSERT) {
            return;
        }
        switch (Matrix_KEY_Scan(0)) {
        case MATRIX_RESULT_P:
        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT:
        case MATRIX_RESULT_RIGHT:
            TIMES++;
            break;

        case MATRIX_RESULT_ENT:
            LCD_ShowString0608(56, 2, FM_Show[FM_now], 1, 128);
            if (WFM == FM_now) {
                return;
            }
            WFM = FM_now;
            RDA5807_Init(FM_now);
            SPK_SWITCH(AUD, FM_now);
            if (WFM) {
                Enter_Radio();
            }
            return;

        case MATRIX_RESULT_CLR:
            LCD_ShowString0608(56, 2, FM_Show[FM_old], 0, 128);
            return;
        }
        if (TIMES != 0) {
            FM_now = !FM_now;
            TIMES = 0;
            LCD_ShowString0608(56, 2, FM_Show[FM_now], 0, 128);
        }
        switch (Encoder_Switch_Scan(0)) {
        case key_click:
            LCD_ShowString0608(56, 2, FM_Show[FM_now], 1, 128);
            if (WFM == FM_now) {
                return;
            }
            WFM = FM_now;
            RDA5807_Init(FM_now);

            SPK_SWITCH(AUD, FM_now);
            if (FM_now) {
                Enter_Radio();
            }
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

//Main interface channel switching
void Channel_Info_Show(unsigned char channel) {         //Channel selection information display
    LCD_ShowChan(83, 2, channel, 0);                    //Display channel number
    load_ChannelParameter(channel, &chan_arv[TMP]);
    LCD_ShowFreq(0, 1, chan_arv[TMP].RX_FREQ, 1);       //Display reception frequency
    return;
}
void ShortCut_CHAN_Select(void) {
    TIMES = 0;
    char chan = chan_arv[NOW].CHAN;
    if (get_Flag(FLAG_CF_SWITCH_ADDR)) {
        return;
    }

    LCD_ShowChan(83, 2, chan, 0);
    while (1) {
        FeedDog();                   //Feed the dog - watchdog timer initialization (?)
        if (KDU_INSERT) {
            return;
        }
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES != 0) {
            chan += TIMES;
            TIMES = 0;
            if (chan > 99) {
                chan = 1;
            }
            if (chan < 1) {
                chan = 99;
            }
            Channel_Info_Show(chan);
        }

        switch (Encoder_Switch_Scan(0)) {
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

//Main menu Figure 1: Send and receive settings
void RT_Menu() {
    TIMES = 0;
    Flag_Main_Page = 0;

    char
    now_mode = 0,           //Currently entering vu mode
    pre_mode = 0,           //Enter the previous vu mode
    pos = 0,                //The cursor should be in the option
    change = 0,             //Data modification and preservation
    FLAG_Modify = 0,        //Confirm the option and enter the modification
    FLAG_FLASH_OPTIONS = 0; //Update cursor logo

    pre_mode = (chan_arv[NOW].RX_FREQ > 174);
    now_mode = pre_mode;

    RT_Menu_Clear();
    LCD_ShowMatrixMenu22(matrix_menu1, 8, pos);    //(const char[][2][12])matrix_menu1

    while (1) {
        MY_GLOBAL_FUN();
        if (KDU_INSERT) {
            return;
        }

        switch (Matrix_KEY_Scan(0)) {
        case MATRIX_RESULT_1:
            if (!PTT_READ) {
                break;
            }
            if (pos == 0) {
                pre_mode = (chan_arv[NOW].RX_FREQ > 174); //Save whether it belongs to the V segment or the U segment before entering according to the frequency
                now_mode = 0;                             //The current setting frequency range is set to: 136-174
                FLAG_Modify = 1;
            } else if (pos == 2) {
                if (chan_arv[NOW].RX_FREQ > 174) {        //When setting the transmit frequency option, it is found that the receive frequency U segment is not allowed to set 136-174
                    break;
                }
                now_mode = 0;
                FLAG_Modify = 1;
            }
            break;

        case MATRIX_RESULT_4:
            if (!PTT_READ) {
                break;
            }
            if (pos == 0) {
                pre_mode = (chan_arv[NOW].RX_FREQ > 174); //Keep the current V segment or U segment before setting the receiving frequency,
                now_mode = 1;                             //The current setting frequency range is set to:400-480
                FLAG_Modify = 1;
            } else if (pos == 2) {
                if (chan_arv[NOW].RX_FREQ < 400) {        //When setting the transmit frequency option, it is found that the receive frequency V segment is not allowed to set 400-480
                    break;
                }
                now_mode = 1;
                FLAG_Modify = 1;
            }
            break;

        case MATRIX_RESULT_ENT:
            if ((pos == 6 && get_Flag(FLAG_CF_SWITCH_ADDR)) || !PTT_READ) {
                break;
            }
            FLAG_Modify = 1;
            if (pos == 0 || pos == 2) {
                pre_mode = (chan_arv[NOW].RX_FREQ > 174); //Get the frequency band before entering
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
        if (TIMES != 0) {
            pos = (pos + TIMES + 8) % 8;
            TIMES = 0;
            FLAG_FLASH_OPTIONS = 1;
        }

        if (FLAG_Modify) {

            switch (pos)
                //Enter the menu according to the current position pressed
            {
            case 0:
                D_printf("\t\t\t\tTX_FREQ Setting\n");
                //First determine whether it is the current frequency or channel mode          Set the frequency directly in CHAN mode
                if (get_Flag(FLAG_CF_SWITCH_ADDR)) {    //FREQ mode first switch the VU segment, then reinstall the parameters and then modify
                    if (now_mode != pre_mode) {         //VU cut each other and overload data
                        chan_arv[NOW].CHAN = now_mode * 100;
                        load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
                    }
                    //The frequency band remains unchanged and the frequency is directly set
                }

                if (now_mode) {
                    LCD_ShowString0408(0, 3, "FRE SET:400-480MHZ,CLR OR ENT", 1);
                } else {
                    LCD_ShowString0408(0, 3, "FRE SET:136-174MHZ,CLR OR ENT", 1);
                }

                //After setting the frequency, enter the save and set A20 step
                if (RT_FREQ_Set(12, 1, (double *)&chan_arv[NOW].RX_FREQ, now_mode) == ENT2LAST) {
                    chan_arv[NOW].TX_FREQ = chan_arv[NOW].RX_FREQ;
                    change = 1;
                    if (get_Flag(FLAG_CF_SWITCH_ADDR)) {
                        if (now_mode != pre_mode) {
                            set_Flag(FLAG_VU_SWITCH_ADDR, now_mode);
                        }
                    }
                } else {
                    //Do not save the settings, exit the judgment CF mode
                    if (get_Flag(FLAG_CF_SWITCH_ADDR)) {
                        //In frequency mode, reload if the frequency band is switched
                        if (now_mode != pre_mode) {
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

                if (now_mode) {
                    LCD_ShowString0408(0, 3, "FRE SET:400-480MHZ,CLR OR ENT", 1);
                } else {
                    LCD_ShowString0408(0, 3, "FRE SET:136-174MHZ,CLR OR ENT", 1);
                }

                if (RT_FREQ_Set(12, 2, (double *)&chan_arv[NOW].TX_FREQ, now_mode) == ENT2LAST) {
                    change = 1;
                }
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
                if (chan_arv[NOW].CHAN > 0 && chan_arv[NOW].CHAN < 100) {
                    RT_NICKNAME_Set(chan_arv[NOW].CHAN, (u8 *)chan_arv[NOW].NN);    //Changed unverified
                    save_ChannelParameter(chan_arv[NOW].CHAN, chan_arv[NOW]);
                    change = 0;
                }
                break;

            case 7:
                D_printf("\t\t\t\tCHAN Select && CF_SWITCH \n");
                RT_CHAN_Switch();
                now_mode = (chan_arv[NOW].RX_FREQ > 174);
                change = 0;             //Since the relevant parameter information needs to be changed when selecting the channel number, 
                                        //set the A002 transceiver parameter to be placed in this module, then change=0 can be used.
                break;
            };

            if (change) {
                save_ChannelParameter(chan_arv[NOW].CHAN, chan_arv[NOW]);
                Set_A20(chan_arv[NOW], SQL);
            }

            FLAG_Modify = 0;
            FLAG_FLASH_OPTIONS = 1;
            RT_Menu_Clear();
        }
        if (FLAG_FLASH_OPTIONS) {       //The currently selected option is displayed
            FLAG_FLASH_OPTIONS = 0;
            LCD_ShowMatrixMenu22(matrix_menu1, 8, pos);
        }
        switch (Encoder_Switch_Scan(0)) {
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

void RT_Menu_Clear() {                                          //Matrix data setting
    sprintf(matrix_menu1[0][0], "R:%3.4f", chan_arv[NOW].RX_FREQ);
    sprintf(matrix_menu1[0][1], "%s", menu_subvoice[chan_arv[NOW].RS]);
    sprintf(matrix_menu1[1][0], "T:%3.4f", chan_arv[NOW].TX_FREQ);
    sprintf(matrix_menu1[1][1], "%s", menu_subvoice[chan_arv[NOW].TS]);

    matrix_menu1[3][0][0] = 'C';
    matrix_menu1[3][0][1] = 'N';
    matrix_menu1[3][0][2] = ':';
    if (get_Flag(FLAG_CF_SWITCH_ADDR)) {                        //FREQ
        sprintf((char *)matrix_menu1[3][1], "FREQ    ");
        if (get_Flag(FLAG_VU_SWITCH_ADDR)) {
            sprintf(matrix_menu1[3][0] + 3, "%s", "UHF    ");
        } else {
            sprintf(matrix_menu1[3][0] + 3, "%s", "VHF    ");
        }
    } else {                                                    //CHAN
        sprintf((char *)matrix_menu1[3][1], "CH-%02d   ", chan_arv[NOW].CHAN);
        memset(matrix_menu1[3][0] + 3, 32, 7);
        memcpy(matrix_menu1[3][0] + 3, (char *)chan_arv[NOW].NN, 7);
    }

    sprintf((char *)matrix_menu1[2][0], (chan_arv[NOW].POWER == 1) ? "PWR:LOW   " : "PWR:HIGH  ");
    sprintf((char *)matrix_menu1[2][1], (chan_arv[NOW].GBW == 1) ? "BW:WIDE " : "BW:NARR ");
}

//Frequency verification
//0: Frequency does not need to be corrected
//Other: Corrected frequency
int checkFreq(int freq_tmp) {
    int step_temp[3] = {50, 100, 125};
    int mul = 0;
    D_printf("freq_tmp:%d\n", freq_tmp);
    if (get_Flag(FLAG_CF_SWITCH_ADDR)) {
        if (freq_tmp % step_temp[STEP] == 0) {
            return 0;
        }
    } else {
        for (int i = 0; i < 3; i++) {
            if (freq_tmp % step_temp[i] == 0) {
                return 0;
            }
        }
    }
    //The default is to set the step correction frequency
    mul = freq_tmp / step_temp[STEP] + 0.5;
    D_printf("mul:%d, return: %d\n", mul, mul * step_temp[STEP]);
    return (mul * step_temp[STEP]);
}
//  Transceiver settings: frequency settings
//  Confirm return: ENT2LAST
//  Cancel return: CLR2LAST
//  KDU control: BACK2MAIN
int RT_FREQ_Set(int x, int y, double * result, int vu_mode) {
    unsigned char
    locate = x + 6,
    bit = 1,
    flag_finish = 0, /*flag_finish Stop entering the flag (complete)*/
    key_result = 23,
    num_input = 0,
    freq_buf[8] = {0};

    int int_freq = 0;

    double freq = *result;
    LCD_ShowPIC0608(60, y, 0, 1);

    LCD_ShowString0608(x, y, "        ", 1, 120);
    if (vu_mode) {
        freq_buf[0] = 4;
        LCD_ShowAscii0608(x, y, '4', 1);    //After entering, the first place is set to 4
    } else {
        freq_buf[0] = 1;
        LCD_ShowAscii0608(x, y, '1', 1);    //After entering, the first place is set to 4
    }
    //
    bsp_StartAutoTimer(TMR_OUT_CTRL, TMR_PERIOD_8S);
    while (1) {
        FeedDog();                          //Feed the dog - watchdog timer initialization (?)
        if (KDU_INSERT || bsp_CheckTimer(TMR_OUT_CTRL)) {
            bsp_StopTimer(TMR_OUT_CTRL);
            return BACK2MAIN;
        }

        if (locate == (18 + x)) {       //Add a‘after the 3 unit integer.’ - or maybe this correct "3 unit integers followed by one"
            LCD_ShowAscii0608(locate, y, '.', 1);
            locate += 6;
            bit++;
        }
        if (locate > x && locate < 48 + x) { //The unset bit is set to '_'
            LCD_ShowAscii0608(locate, y, '_', 1);
        }
        if (locate == 48 + x || bit == 8) { //8 units are set up
            flag_finish = 1;
        }

        key_result = Matrix_KEY_Scan(0);
        if (key_result != MATRIX_RESULT_ERROR) {
            ReloadOutCal();
        }
        switch (key_result) {
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
            if (locate == x + 6) {                      //exit
                D_printf("Press{CLR} : Exit\n");
                LCD_ShowFreq(x, y, *result, 1);
                return CLR2LAST;
            } else {                                    //clear
                D_printf("Press{CLR} : %d\n", locate);  //A
                LCD_ShowString0608(x, y, "        ", 1, 120);

                while (bit--) {
                    freq_buf[bit] = 0;
                }
                if (vu_mode) {
                    freq_buf[0] = 4;
                    LCD_ShowAscii0608(x, y, '4', 1);
                } else {
                    freq_buf[0] = 1;
                    LCD_ShowAscii0608(x, y, '1', 1);
                }
                locate = x + 6;
                bit = 1;
            }
            break;

        case MATRIX_RESULT_ENT:
            D_printf("Press{ENT}\n");                   //B
            flag_finish = 1;
            break;
        }
        //

        switch (Encoder_Switch_Scan(0)) {
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
        if (num_input) {                                //Digital deposit (?)
            num_input = 0;
            D_printf("_[%d]_: %d\n", bit, key_result);
            freq_buf[bit] = key_result;
            bit++;
            LCD_ShowAscii0608(locate, y, key_result + '0', 1);
            locate += 6;
        }

        //
        if (flag_finish) {                              //Input is complete
            LCD_ShowString0608(x + 60, y, "  ", 1, x + 68);
            if (locate < 48 + x || bit < 7) {           //Press the OK button, the zero is not written and the zero is filled
                for (; bit < 8; bit++) {
                    freq_buf[bit] = 0;
                }
            }
            //
            freq = (vu_mode ? 400 : 100);
            freq = freq + freq_buf[1] * 10 + freq_buf[2] + freq_buf[4] * 0.1 + freq_buf[5] * 0.01 + freq_buf[6] * 0.001 + freq_buf[7] * 0.0001;

            int_freq = freq_buf[0] * 1000000 + freq_buf[1] * 100000 + freq_buf[2] * 10000 +
                       freq_buf[4] * 1000 + freq_buf[5] * 100 + freq_buf[6] * 10 + freq_buf[7];
            //          int_freq = freq*10000;  //Cannot be rounded: 409.7800
            D_printf("int_freq:%d\n", int_freq);

            if (freq < 136.0) {
                freq = 136.0;
            } else if ((freq > 174.0 && freq < 400.0) || freq > 480.0) {
                freq = *result;
            } else {
                int res = checkFreq(int_freq);
                if (res > 0) {
                    freq = (double)res / 10000;
                }
                D_printf("res:%d, freq:%.4f\n", res, freq);
            }
            *result = freq;
            LCD_ShowFreq(x, y, freq, 0);        //Frequency display
            bsp_StopTimer(TMR_OUT_CTRL);
            return ENT2LAST;
        }
    }
}

//  Transceiver settings: sub-tone settings
//  Confirm return: confirm the modified sub-tone
//  Cancel back: original sub-tone
//  KDU control: original sub-tone
int RT_SubVoice_Set(int row, int subvoice) { //The first row shows the first subvoice (sub-tone)
    TIMES = 0;
    LCD_ShowPIC0608(116, row, 0, 1);
    LCD_ShowString0408(0, 3, " CTCSS SET,   OR   ,CLR OR ENT  ", 1);
    LCD_ShowPIC0408(48, 3, 0);
    LCD_ShowPIC0408(68, 3, 1);

    int subvoice_temp = 0;
    if (subvoice < 0 || subvoice >= 122) {
        subvoice_temp = 0;
    } else {
        subvoice_temp = subvoice;
    }

    while (1) {
        FeedDog();              //Feed the dog - watchdog timer initialization (?)
        SQ_Read_Control();

        if (KDU_INSERT) {
            return subvoice;
        }
        switch (Matrix_KEY_Scan(0)) {
        case MATRIX_RESULT_0:
            subvoice = 0;
            LCD_ShowString0608(68, row, "        ", 1, 128);
            LCD_ShowString0608(68, row, menu_subvoice[0], 0, 128);
            break;

        case MATRIX_RESULT_CLR: //Cancel return
            LCD_ShowAscii0608(116, row, ' ', 1);
            return subvoice_temp;

        case MATRIX_RESULT_ENT: //confirm
            LCD_ShowAscii0608(116, row, ' ', 1);
            if (subvoice < 0 || subvoice > 121) {
                return 0;
            } else {
                return subvoice;
            }

        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT: //Select the previous sub-tone
            TIMES--;
            break;

        case MATRIX_RESULT_P:
        case MATRIX_RESULT_RIGHT: //Select the latter sub-tone
            TIMES++;
            break;

        case MATRIX_RESULT_5:
            subvoice = RT_SubVoice_Matrix_Menu_Select(subvoice);
            return subvoice;
        }

        if (TIMES > 0) {
            if (subvoice + TIMES > 121) {
                subvoice = 0;
            } else {
                subvoice += TIMES;
            }
            TIMES = 0;

            LCD_ShowString0608(68, row, "        ", 1, 128);
            LCD_ShowString0608(68, row, menu_subvoice[subvoice], 0, 128);
        } else if (TIMES < 0) {
            if (subvoice + TIMES < 0) {
                subvoice = 121;
            } else {
                subvoice += TIMES;
            }
            TIMES = 0;

            LCD_ShowString0608(68, row, "        ", 1, 128);
            LCD_ShowString0608(68, row, menu_subvoice[subvoice], 0, 128);
        }
        switch (Encoder_Switch_Scan(0)) {
        case key_click:
            LCD_ShowAscii0608(116, row, ' ', 1);
            if (subvoice < 0 || subvoice > 121) {
                return 0;
            } else {
                return subvoice;
            }

        case key_double:
            LCD_ShowAscii0608(116, row, ' ', 1);
            return subvoice_temp;

        case key_long:
            SHUT();
            break;
        }
    }
}

int RT_SubVoice_Matrix_Menu_Select(int subvoice) { //Sub-tone setting: matrix sub-tone selection
    TIMES = 0;
    int subvoice_temp = subvoice;
    u8 change_3 = 0;

    LCD_Clear(EDITZONE32);
    LCD_ShowMatrixMenu33(matrix_menu_subvoice, 122, subvoice);
    while (1) {
        FeedDog();                  //Feed the dog - watchdog timer initialization (?)
        SQ_Read_Control();
        if (KDU_INSERT) {
            return subvoice;
        }
        switch (Matrix_KEY_Scan(0)) {
        case MATRIX_RESULT_CLR:
            return subvoice_temp;

        case MATRIX_RESULT_ENT:
            if (0 > subvoice || subvoice >= 122) {
                return 0;
            }
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

        if (TIMES > 0) {
            if (change_3) {
                change_3 = 0;
                switch (subvoice) {
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
            } else {
                if (subvoice + TIMES > 121) {
                    subvoice = 0;
                } else {
                    subvoice += TIMES;
                }
            }

            TIMES = 0;
            LCD_ShowMatrixMenu33(matrix_menu_subvoice, 122, subvoice);
        } else if (TIMES < 0) {
            if (change_3) {
                change_3 = 0;
                switch (subvoice) {
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
            } else {
                if (subvoice + TIMES < 0) {
                    subvoice = 121;
                } else {
                    subvoice += TIMES;
                }
            }

            TIMES = 0;
            LCD_ShowMatrixMenu33(matrix_menu_subvoice, 122, subvoice);
        }

        switch (Encoder_Switch_Scan(0)) {
        case 1:
            if (0 > subvoice || subvoice > 121) {
                return 0;
            }
            return subvoice;

        case 2:
            return subvoice_temp;

        case 3:
            SHUT();
            break;
        }
    }
}
//  Transceiver settings: transmit power selection
//  Confirm return: Confirm the modified transmit power
//  Cancel return: original transmit power
//  KDU control: original transmit power
int RT_TX_POWER_Set(int power_temp) {
    TIMES = 0;
    unsigned char power = power_temp;
    LCD_ShowPIC0608(60, 1, 0, 1);

    while (1) {
        FeedDog();                  //Feed the dog - watchdog timer initialization (?)
        SQ_Read_Control();
        if (KDU_INSERT) {
            return power_temp;
        }
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES != 0) {
            TIMES = 0;
            power_temp = !power_temp;
            LCD_ShowString0608(24, 1, POWER_SHOW[power_temp], 0, 128);
        }

        switch (Encoder_Switch_Scan(0)) {
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
//  Transceiver settings: Bandwidth selection
//  Confirm return: Confirm the modified bandwidth
//  Cancel back: original bandwidth
//  KDU control: original bandwidth
int RT_GBW_Set(int gbw_temp) {
    TIMES = 0;
    u8 gbw_t = gbw_temp;
    LCD_ShowPIC0608(116, 1, 0, 1);
    while (1) {
        FeedDog();                  //Feed the dog - watchdog timer initialization (?)
        SQ_Read_Control();
        if (KDU_INSERT) {
            return gbw_temp;
        }
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES != 0) {
            TIMES = 0;
            gbw_temp = (gbw_temp + 1) % 2;
            LCD_ShowString0608(86, 1, gbw_temp ? "WIDE" : "NARR", 0, 128);
        }
        switch (Encoder_Switch_Scan(0)) {
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

//Sending and receiving settings: channel alias
void RT_NICKNAME_Set(unsigned char current_channel, unsigned char nn_temp[7]) {
    TIMES = 0;
    unsigned char
    nn[7] = {32},
            key_old = MATRIX_RESULT_ERROR,
            result_matrix = MATRIX_RESULT_ERROR, //Current trigger button
            nn_locate = 0,                       //Current cursor position
            nn_locate_change = 0,                //The cursor position changes, used to refresh press_times
            select_bit_flag = 0,                 //Encoder use: the current mode is to adjust the position/set the character
            press_times = 0,                     //Current number of button presses
            locate_change = 1,                   //Refresh the selected location
            clear = 0;                           //The edit bar is cleared: 0 - is not cleared, 1 - is cleared
    // int i;

    memset(nn, 32, 7);
    sprintf((char *)nn, "%s", nn_temp);

    LCD_ShowString0608(0, 2, "CN:", 1, 18);
    LCD_ShowPIC0608(60, 2, 0, 1);

    while (1) {
        MY_GLOBAL_FUN();
        if (KDU_INSERT) {
            return;
        }
        switch (Encoder_Switch_Scan(0)) {
        case key_click:                             //Encoder confirmation is always: confirm the character setting of the current position
            select_bit_flag = !select_bit_flag;
            break;

        case key_double:
            return;

        case key_long:
            SHUT();
            break;
        }

        if (select_bit_flag == 0) {                 //The encoder is switched to the adjustment position
            if (TIMES != 0) {
                nn_locate = (nn_locate + 7 - ((-TIMES) % 7)) % 7;
                TIMES = 0;
                locate_change = 1;
            }
        } else {                                    //Encoder setting alias
            if (TIMES > 0) {
                nn[nn_locate] = (nn[nn_locate] - 32 + TIMES) % 95 + 32;
                TIMES = 0;
                clear = 0;
                LCD_ShowAscii0608(18 + nn_locate * 6, 2, nn[nn_locate], 0);
            } else if (TIMES < 0) {
                nn[nn_locate] = nn[nn_locate] + 95 - (-TIMES) % 95 > 127 ? nn[nn_locate] - (-TIMES) % 95 : nn[nn_locate] + 95 - (-TIMES) % 95;
                TIMES = 0;
                clear = 0;
                LCD_ShowAscii0608(18 + nn_locate * 6, 2, nn[nn_locate], 0);
            }
        }

        result_matrix = Matrix_KEY_Scan(0);
        switch (result_matrix) {
        case MATRIX_RESULT_CLR:
            if (clear) {                            //Cleared, returns the initial value
                return;
            } else {                                //Not cleared, clear the edit bar
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
            if (key_old != result_matrix || nn_locate_change) {
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
            select_bit_flag = 0; //After pressing the button to switch the position, the encoder rotation function is changed to switch the position
            if (nn_locate > 6) {
                nn_locate = 6;
            }
            break;

        case MATRIX_RESULT_RIGHT:
            nn_locate++;
            locate_change = 1;
            select_bit_flag = 0; //After pressing the button to switch the position, the encoder rotation function is changed to switch the position
            if (nn_locate > 6) {
                nn_locate = 0;
            }
            break;
        };
        if (locate_change) { //Cursor position modification
            locate_change = 0;
            nn_locate_change = 1;
            LCD_ShowString0608(18, 2, "       ", 1, 128);
            LCD_ShowString0608(18, 2, (char *)nn, 1, 128);
            if (nn[nn_locate]) {
                LCD_ShowAscii0608(18 + nn_locate * 6, 2, nn[nn_locate], 0);    //Show currently selected
            } else {
                LCD_ShowAscii0608(18 + nn_locate * 6, 2, ' ', 0);
            }
        }
    }
}
//Transceiver settings: channel number selection
void RT_CHAN_Switch(void) {
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

    while (1) {
        FeedDog();                  //Feed the dog - watchdog timer initialization (?)
        SQ_Read_Control();
        if (KDU_INSERT) {
            return;
        }
        switch (Matrix_KEY_Scan(0)) {
        case MATRIX_RESULT_0:
            if (now_cf) {
                break;
            }
            chan_temp = 1;
            reload = 1;
            break;

        case MATRIX_RESULT_LEFT:
            now_cf = !now_cf;
            if (now_cf) {
                chan_temp = (now_vu ? 100 : 0);
            } else {
                chan_temp = load_CurrentChannel();
            }
            D_printf("CHAN:%d\n", chan_temp);
            reload = 1;
            break;

        case MATRIX_RESULT_RIGHT:
            if (now_cf) {
                now_vu = !now_vu;
                chan_temp = now_vu * 100;
                reload = 1;
            }
            break;

        case MATRIX_RESULT_P:
            if (now_cf) {
                break;
            }
            TIMES++;
            break;

        case MATRIX_RESULT_N:
            if (now_cf) {
                break;
            }
            TIMES--;
            break;

        case MATRIX_RESULT_CLR:
            return;

        case MATRIX_RESULT_ENT:
            set_Flag(FLAG_CF_SWITCH_ADDR, now_cf);
            set_Flag(FLAG_VU_SWITCH_ADDR, now_vu);
            chan_arv[NOW].CHAN = chan_temp;
            load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
            if (now_cf == 0) {
                save_CurrentChannel(chan_arv[NOW].CHAN);
            }
            Set_A20(chan_arv[NOW], SQL);
            return;
        }
        if (TIMES != 0) {
            if (now_cf) {
                TIMES = 0;
            }
            if (TIMES < 0) {
                TIMES++;
                if (chan_temp > 1) {
                    reload = 1;
                    chan_temp--;
                }
            } else if (TIMES > 0) {
                TIMES--;
                if (chan_temp < 99) {
                    reload = 1;
                    chan_temp++;
                }
            }
        }
        if (reload) {
            reload = 0;
            load_ChannelParameter(chan_temp, &chan_arv[TMP]);
            LCD_ShowString0608(86, 1, chan_arv[TMP].GBW ? "WIDE" : "NARR", 1, 128);
            LCD_ShowString0608(24, 1, chan_arv[TMP].POWER ? "LOW " : "HIGH", 1, 128);

            if (now_cf) {
                strcpy((char *)name, "FREQ    ");
            } else {
                sprintf((char *)name, "CH-%02d  ", chan_temp);
            }

            LCD_ShowString0608(18, 2, "       ", 1, 128);
            LCD_ShowString0608(68, 2, "       ", 1, 128);
            LCD_ShowString0608(18, 2, (char *)chan_arv[TMP].NN, 1, 128);
            LCD_ShowString0608(68, 2, (char *)name, 0, 128);
        }
        switch (Encoder_Switch_Scan(0)) {
        case key_click:
            set_Flag(FLAG_CF_SWITCH_ADDR, now_cf);
            set_Flag(FLAG_VU_SWITCH_ADDR, now_vu);
            chan_arv[NOW].CHAN = chan_temp;
            load_ChannelParameter(chan_arv[NOW].CHAN, &chan_arv[NOW]);
            if (now_cf == 0) {
                save_CurrentChannel(chan_arv[NOW].CHAN);
            }
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

//Button 2 to enter the backlight and brightness settings
//CLR2LAST: Return to the previous level
//ENT2LAST: Set to exit successfully
//BACK2MAIN: Button 2/kdu control exit
u8 Screen_Contrast_Set(void) {
    LCD_ShowString0608(19, 1, "SCREEN CONTRAST", 1, 128);
    LCD_ShowContrast(SC);
    while (1) {
        MY_GLOBAL_FUN();
        if (KDU_INSERT) {
            return BACK2MAIN;
        }
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES) {
            if (SC + TIMES > 6) {
                SC = 6;
            } else if (SC + TIMES < 0) {
                SC = 0;
            } else {
                SC += TIMES;
            }
            TIMES = 0;
            LCD_LightRatio(0x00 + SC * 5);
            save_ScreenContrast(SC);
            LCD_ShowContrast(SC);
        }
        switch (Encoder_Switch_Scan(0)) {
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

//CLR2LAST: Return to the previous level
//BACK2MAIN: Button *2(LT)*/KDU control exit/enter the contrast setting after the setting is complete
u8 Light_Intensity_set(void) {
    u8 ENTER = 0;
    LCD_ShowString0608(19, 1, "LIGHT INTENSITY", 1, 128);
    LCD_ShowString0608(0, 2, "                      ", 1, 128);
    LCD_ShowBackLight(BL);
    while (1) {
        MY_GLOBAL_FUN();
        if (KDU_INSERT) {
            return BACK2MAIN;
        }
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES) {
            BL += (TIMES * 10);
            TIMES = 0;
            if (BL > 127) {
                BL = 0;
            }
            if (BL > 100) {
                BL = 100;
            }
            LCD_ShowBackLight(BL);
            BackLight_SetVal(BL);
        }
        //
        switch (Encoder_Switch_Scan(0)) {
        case key_click:
            ENTER = 1;
            break;

        case key_double:
            return CLR2LAST;

        case key_long:
            SHUT();
            break;
        }
        if (ENTER) {
            ENTER = 0;
            save_Backlightness(BL);
            if (Screen_Contrast_Set()) {
                return BACK2MAIN;
            }
            LCD_ShowString0608(19, 1, "LIGHT INTENSITY", 1, 128);
            LCD_ShowBackLight(BL);
        }
    }

    //  return 0;
}

void Light_Mode_Clear(u8 sel_pos) {

    LCD_Clear(EDITZONE32);
    LCD_ShowString0608(34, 1, "LIGHT MODE", 1, 128);
    LCD_ShowOption(37, 2, LIGHT_SHOW, 2, sel_pos);
    LCD_ShowString0408(20, 3, " TO CHANGE / ENT TO CONT", 1);

    LCD_ShowPIC0408(10, 3, 0);
    LCD_ShowPIC0408(15, 3, 1);
}

void Light_Mode_Set(void) { //
    TIMES = 0;
    u8 BL_temp = BL;
    static u8 pos = 0;
    u8 ENTER = 0;
    Light_Mode_Clear(pos);

    while (1) {
        MY_GLOBAL_FUN();
        if (KDU_INSERT) {
            return;
        }
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES) {
            pos = !pos;
            TIMES = 0;
            BL = pos ? 0 : load_Backlightness();
            LCD_ShowString0608(0, 2, "                      ", 1, 128);
            BackLight_SetVal(BL);
            Light_Mode_Clear(pos);
        }
        switch (Encoder_Switch_Scan(0)) {
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
        if (ENTER) {
            ENTER = 0;
            if (pos) { //After confirming that the light is turned off--->Enter contrast settings
                if (Screen_Contrast_Set()) {
                    return;
                }
            } else { //After confirming that the light is on--->Brightness adjustment--->contrast setting
                if (Light_Intensity_set()) {
                    return;
                }
            }
            Light_Mode_Clear(pos);
        }
    }
    //
}
//

//Button *5(Zero)* initialization
void Zero_Menu(void) {
    Flag_Main_Page = 0;
    TIMES = 0;
    u8 pos = 0;
    int ENSURE = 0;
    LCD_Clear(EDITZONE32);
    LCD_ShowMenu31(MENU_ZERO, MENU_ZERO_NUM, pos);

    while (1) {
        MY_GLOBAL_FUN();
        if (KDU_INSERT) {
            return;
        }
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES > 0) {
            pos = (pos + TIMES) % MENU_ZERO_NUM;
            TIMES = 0;
            LCD_ShowMenu31(MENU_ZERO, MENU_ZERO_NUM, pos);
        } else if (TIMES < 0) {
            pos = (pos + MENU_ZERO_NUM - ((-TIMES) % MENU_ZERO_NUM)) % MENU_ZERO_NUM;
            TIMES = 0;
            LCD_ShowMenu31(MENU_ZERO, MENU_ZERO_NUM, pos);
        }
        switch (Encoder_Switch_Scan(0)) {
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
        if (ENSURE) {
            ENSURE = 0;
            switch (pos) {
            case 0:
                D_printf("ZEROIZE ALL\n");
                ENSURE = Zeroize_All();
                break;
            }
            if (ENSURE == 2) {
                return;
            }

            ENSURE = 0;
            LCD_ShowMenu31(MENU_ZERO, MENU_ZERO_NUM, pos);
        }
    }
}

//The return value has no special meaning
int Zeroize_All(void) {
    TIMES = 0;
    u8 pos = 0;

    LCD_Clear(EDITZONE32);
    LCD_ShowString0608(25, 1, "ZEROIZE RADIO", 1, 128);
    LCD_ShowOption(55, 2, CONFIRM_OPT, 2, pos);
    LCD_ShowString0408(18, 3, "TO SCROLL / ENT TO CONT", 1);

    while (1) {
        FeedDog();                  //Feed the dog - watchdog timer initialization (?)
        SQ_Read_Control();
        if (KDU_INSERT) {
            return BACK2MAIN;
        }
        switch (Matrix_KEY_Scan(0)) {
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
            if (pos) {
                set_Flag(RESETADDR, RESET_VAL + 1);
                SPK_SWITCH(0, 0);
                DATA_Init();
                SHUT();
            } else {
                return ENT2LAST;
            }
        }
        if (TIMES) {
            TIMES = 0;
            pos = !pos;
            LCD_ShowOption(55, 2, CONFIRM_OPT, 2, pos);
        }
        //
        switch (Encoder_Switch_Scan(0)) {
        case key_click:
            if (pos) {
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

//Button *7(OPT)* menu
void OPTION_Menu(void) {
    Flag_Main_Page = 0;
    TIMES = 0;
    u8 num = 0;
    int ENSURE = 0;

    LCD_Clear(EDITZONE32);
    LCD_ShowMenu31(MENU_OPT, MENU_OPT_NUM, num);
    while (1) {
        MY_GLOBAL_FUN();
        if (KDU_INSERT) {
            return;
        }
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES > 0) {
            num = (num + TIMES) % MENU_OPT_NUM;
            TIMES = 0;
            LCD_ShowMenu31(MENU_OPT, MENU_OPT_NUM, num);
        } else if (TIMES < 0) {
            num = (num + MENU_OPT_NUM - ((-TIMES) % MENU_OPT_NUM)) % MENU_OPT_NUM;
            TIMES = 0;
            LCD_ShowMenu31(MENU_OPT, MENU_OPT_NUM, num);
        }
        ////////////////////////////////////////////////////////////////////////////////////////
        switch (Encoder_Switch_Scan(0)) {
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
        if (ENSURE) {
            ENSURE = 0;
            switch (num) {
            case 0:
                D_printf("LOCK KEY PAD\n");
                ENSURE = Lock_Screen_KeyBoard();
                ClearShut();
                while (VOL_ADD_READ == 0 || VOL_SUB_READ == 0) {
                    FeedDog();      //Feed the dog - watch dog timer initialization (?)
                                    //Prevent a volume increase or decrease from being performed when long-pressing to exit
                }
                if (ENSURE == 2) {
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
//Test button menu
void Key_Test() {
    TIMES = 0;
    int delay_f1_f2 = 0, result = 0;

    LCD_ShowString0608(0, 1, "     KEY TESTING      ", 1, 128);
    LCD_ShowString0608(0, 2, "                      ", 1, 128);
    LCD_ShowString0608(0, 3, "VALUE:                ", 1, 128);
    bsp_StartAutoTimer(TMR_OUT_CTRL, TMR_PERIOD_8S);
    while (1) {
        FeedDog();                  //Feed the dog - watch dog timer initialization (?)
        if (KDU_INSERT || bsp_CheckTimer(TMR_OUT_CTRL) || delay_f1_f2 >= 100000) {
            bsp_StopTimer(TMR_OUT_CTRL);
            return;
        }

        result = Matrix_KEY_Scan(0);
        if (result != 17) {
            D_printf("%d\n", result);
            ReloadOutCal();
        }
        switch (result) {
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
        switch (VolumeKeyScan(0)) {
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
        if (VOL_ADD_READ == 0 || VOL_SUB_READ == 0) {
            delay_f1_f2++;
        } else {
            delay_f1_f2 = 0;
        }

        switch (Encoder_Switch_Scan(0)) {
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
        //      if(A002_SQ_READ == 0)
        //      {
        //          if(bsp_CheckTimer(TMR_RSSI_CTRL))
        //              LCD_ShowSignal(Get_A20_RSSI());
        //      }
        if (!PTT_READ) {
            LCD_ShowString0608(36, 3, " PUSH TO TALK ", 1, 128);
            ReloadOutCal();
        }
        if (!SQUELCH_READ) {
            LCD_ShowString0608(36, 3, " SQUELCH_READ ", 1, 128);
            ReloadOutCal();
        }

        if (TIMES < 0) {
            TIMES = 0;
            LCD_ShowString0608(36, 3, "ENCODER   -   ", 1, 128);
            ReloadOutCal();
        } else if (TIMES > 0) {
            TIMES = 0;
            LCD_ShowString0608(36, 3, "ENCODER   +   ", 1, 128);
            ReloadOutCal();
        }
    }
}
//Function 0: Lock screen, lock disk
int Lock_Screen_KeyBoard() {
    int f1 = 0, f2 = 0, i = 0;
    int volume_change = 0;
    if (Flag_Main_Page == 0) {
        VFO_Clear();
        VFO_Refresh();
    }
    LCD_ShowString0408(108, 3, "KEY", 0);
    while (1) {

        FeedDog();              //Feed the dog - watch dog timer initialization (?)
        PTT_Control();
        SQUELCH_Control();
        SQ_Read_Control();
        A002_CALLBACK();        //A002 set response
        if (bsp_CheckTimer(TMR_FM_CTRL) && WFM) {
            RDA5807_Init(ON);
        }

        if (PTT_READ) {
            LCD_ShowAscii0408(0, 0, 'R');
            LCD_ShowSignal(RSSI); //Signal detection
        } else {
            LCD_ShowAscii0408(0, 0, 'T');
            LCD_ShowSignal(100);
        }
        if (EncoderClickValidate()) {
            if (i++ > 150) {
                SHUT();
            }
        }

        if (KDU_INSERT) {
            TIMES = 0;
            return BACK2MAIN;
        }

        delay_ms(15);
        if (VOL_ADD_READ == 0) {
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

        if (VOL_SUB_READ == 0) {
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
            if (WFM) {                  //Do not interrupt the output when the radio is on, directly modify the volume
                RDA5807_ResumeImmediately();
                if (!A002_SQ_READ) {
                    M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[0]);
                    M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);
                } else {
                    M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[VOLUME]);
                }
            } else {
                if (A002_SQ_READ) {
                    SPK_SWITCH(AUD, 0);
                }
                M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[0]); //0
                M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);
            }
        }

        if (f1 > 100 || f2 > 100) {
            LCD_ShowString0408(108, 3, "KEY", 1);
            TIMES = 0;
            return BACK2MAIN;
        }
    }
}
//

//Button *8(PGM)* menu
void PGM_Menu() {
    Flag_Main_Page = 0;
    TIMES = 0;
    u8 num = 0;
    int ENSURE = 0;
    LCD_Clear(GLOBAL32);

    LCD_ShowString0408(0, 0, "PGM", 1);
    LCD_ShowMenu31(MENU_PGM, MENU_PGM_NUM, 0);
    while (1) {
        FeedDog();                  //Feed the dog - watch dog timer initialization (?)
        SQ_Read_Control();
        if (KDU_INSERT) {
            return;
        }
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES > 0) {
            num = (num + 1) % MENU_PGM_NUM;
            TIMES = 0;
            LCD_ShowMenu31(MENU_PGM, MENU_PGM_NUM, num);
        } else if (TIMES < 0) {
            num = (num - 1 + MENU_PGM_NUM) % MENU_PGM_NUM;
            TIMES = 0;
            LCD_ShowMenu31(MENU_PGM, MENU_PGM_NUM, num);
        }
        ////////////////////////////////////////////////////////////////////////////////////////
        switch (Encoder_Switch_Scan(0)) {
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
        if (ENSURE) {
            ENSURE = 0;
            switch (num) {
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

            //                  case normal_set_encryp:
            //                      D_printf("ENCRYP Setting\n");
            //                      ENSURE = PGM_ENCRPY_Set(num%3+1);
            //                      break;

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
            if (ENSURE == BACK2MAIN) {
                return;
            }
            ENSURE = 0;
            LCD_ShowMenu31(MENU_PGM, MENU_PGM_NUM, num);
            while (VOL_ADD_READ == 0 || VOL_SUB_READ == 0) {
                FeedDog();      //Feed the dog - watch dog timer initialization (?)
                                //Prevent a volume increase or decrease from being performed when long-pressing to exit
            }
        }
        //
    }
}

//  Related settings 1: Audio strobe and set microphone sensitivity
//  CLR2LAST: Do not modify, re-select the audio channel that needs to be strobe
//  ENT2LAST: Confirm the modification and exit the audio settings to return to the PDM menu
//  BACK2MAIN: Button 8/KDU control to exit directly
int AUDIO_SET(u8 _audio) {
    TIMES = 0;

    u8 temp = MIC;

    LCD_ShowString0608(60, _audio + 1, ":", 0, 66);
    LCD_ShowString0608(66, _audio + 1, TRF_Show[temp], 0, 90);

    LCD_ShowPIC0608(116, _audio + 1, 0, 0);
    while (1) {
        FeedDog();                  //Feed the dog - watch dog timer initialization (?)
        SQ_Read_Control();
        if (KDU_INSERT) {
            return BACK2MAIN;
        }
        switch (Matrix_KEY_Scan(0)) {
        case MATRIX_RESULT_8:
            return BACK2MAIN;

        case MATRIX_RESULT_CLR:
            return CLR2LAST;

        case MATRIX_RESULT_ENT:
            if (_audio != AUD) {
                SPK_SWITCH(AUD, OFF);
            }

            if (!A002_SQ_READ) {     //If there is a signal, modify it directly
                SPK_SWITCH(_audio, ON);
            } else {
                if (WFM) {
                    SPK_SWITCH(_audio, ON);
                }
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
        if (TIMES > 0) {
            temp = (temp + 1) % 3;
            TIMES = 0;
            LCD_ShowString0608(66, _audio + 1, TRF_Show[temp], 0, 128);
        } else if (TIMES < 0) {
            temp = (temp - 1 + 3) % 3;
            TIMES = 0;
            LCD_ShowString0608(66, _audio + 1, TRF_Show[temp], 0, 128);
        }
        ///////////////////////////////////////////////////////////
        switch (Encoder_Switch_Scan(0)) {
        case key_click:
            if (_audio != AUD) {
                SPK_SWITCH(AUD, OFF);
            }

            if (!A002_SQ_READ) {        //If there is a signal, modify it directly
                SPK_SWITCH(_audio, ON);
            } else {
                if (WFM) {
                    SPK_SWITCH(_audio, ON);
                }
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
//  CLR2LAST: Return to PGM menu without modification
//  ENT2LAST: Confirm the modification and return to the PGM menu
//  BACK2MAIN: Button 8/KDU control to exit directly
int PGM_AUDIO_Select(u8 row) {
    TIMES = 0;
    u8 index = AUD, ENTER = 0;
    LCD_ShowMenu31(menu_audio, 3, index);
    while (1) {
        FeedDog();              //Feed the dog - watch dog timer initialization (?)
        SQ_Read_Control();
        if (KDU_INSERT) {
            return BACK2MAIN;
        }
        switch (Encoder_Switch_Scan(0)) {
        case key_click:
            ENTER = 1;
            break;

        case key_double:
            return CLR2LAST;

        case key_long:
            SHUT();
            break;
        }
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES > 0) { //
            TIMES = 0;
            index = (index + 1) % 3;
            LCD_ShowMenu31(menu_audio, 3, index);
        } else if (TIMES < 0) {
            TIMES = 0;
            index = (index + 2) % 3;
            LCD_ShowMenu31(menu_audio, 3, index);
        }

        if (ENTER) {
            if (index == 0) {
                if (index != AUD) {
                    SPK_SWITCH(AUD, OFF);
                }

                if (!A002_SQ_READ) {            //If there is a signal, modify it directly
                    SPK_SWITCH(index, ON);
                } else {
                    if (WFM) {
                        SPK_SWITCH(index, ON);
                    }
                }
                //

                AUD = 0;
                MIC = 1;
                save_AudioSelect(AUD);
                M62364_SetSingleChannel(4, MIC_LEVEL[MIC]);
                return ENT2LAST;
            }
            ENTER = AUDIO_SET(index);
            if (ENTER == CLR2LAST) {
                LCD_ShowMenu31(menu_audio, 3, index);
            } else {
                return ENTER;    //It can be to return to the main interface or to the menu
            }
        }
    }
}
//Related setting 2: squelch level
int PGM_SQL_Set(u8 row) {
    TIMES = 0;
    u8 sql_temp = SQL;
    LCD_ShowString0608(30, row, ":LEVEL", 0, 66);
    LCD_ShowAscii0608(66, row, sql_temp + '0', 0);
    LCD_ShowPIC0608(116, row, 0, 0);
    while (1) {
        FeedDog();                  //Feed the dog - watch dog timer initialization (?)
        SQ_Read_Control();
        if (KDU_INSERT) {
            return BACK2MAIN;
        }
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES != 0) {
            sql_temp += TIMES;
            if (sql_temp > 127) {
                sql_temp = 0;
            } else if (sql_temp > 8) {
                sql_temp = 8;
            }

            LCD_ShowAscii0608(66, row, sql_temp + '0', 0);
            TIMES = 0;
        }

        switch (Encoder_Switch_Scan(0)) {
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

//Related settings 3: Step
int PGM_STEP_Set(u8 row) {
    TIMES = 0;
    u8 step_temp = STEP;
    LCD_ShowAscii0608(36, row, ':', 0);
    LCD_ShowString0608(42, row, STEP_SHOW[STEP], 0, 72);
    LCD_ShowPIC0608(116, row, 0, 0);

    while (1) {
        FeedDog();                  //Feed the dog - watch dog timer initialization (?)
        SQ_Read_Control();
        if (KDU_INSERT) {
            return BACK2MAIN;
        }
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES != 0) {
            step_temp += TIMES;
            if (step_temp > 127) {
                step_temp = 0;
            } else if (step_temp > 2) {
                step_temp = 2;
            }
            LCD_ShowString0608(42, row, STEP_SHOW[step_temp], 0, 128);
            TIMES = 0;
        }

        switch (Encoder_Switch_Scan(0)) {
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
//Related settings 4: Encryption
int PGM_ENCRPY_Set(u8 row) {
    TIMES = 0;
    u8 SCRAM_LEVEL_temp = ENC;
    LCD_ShowAscii0608(48, row, ':', 0);
    LCD_ShowAscii0608(54, row, SCRAM_LEVEL_temp + '0', 0);
    LCD_ShowPIC0608(116, row, 0, 0);
    while (1) {
        FeedDog();                  //Feed the dog - watch dog timer initialization (?)
        SQ_Read_Control();
        if (KDU_INSERT) {
            return BACK2MAIN;
        }
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES != 0) {
            SCRAM_LEVEL_temp += TIMES;
            if (SCRAM_LEVEL_temp > 127) {
                SCRAM_LEVEL_temp = 0;
            } else if (SCRAM_LEVEL_temp > 9) {
                SCRAM_LEVEL_temp = 9;
            }
            LCD_ShowAscii0608(54, row, SCRAM_LEVEL_temp + '0', 0);
            TIMES = 0;
        }
        switch (Encoder_Switch_Scan(0)) {
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
//Related setting 5: Launch time limit
int PGM_TOT_Set(u8 row) {
    TIMES = 0;
    u8 tot_temp = load_Tot();
    LCD_ShowString0608(30, row, ": MINUTE", 0, 128);
    LCD_ShowAscii0608(36, row, tot_temp + '0', 0);
    if (tot_temp == 0) {
        LCD_ShowString0608(30, row, ":NO TOT ", 0, 78);
    }
    LCD_ShowPIC0608(116, row, 0, 0);
    while (1) {
        FeedDog();                      //Feed the dog - watch dog timer initialization (?)
        SQ_Read_Control();
        if (KDU_INSERT) {
            return BACK2MAIN;
        }
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES != 0) {
            tot_temp += TIMES;
            TIMES = 0;
            if (tot_temp > 127) {
                tot_temp = 0;
            }
            if (tot_temp > 8) {
                tot_temp = 9;
            }
            if (!tot_temp) {
                LCD_ShowString0608(30, row, ":NO TOT ", 0, 128);
            } else {
                LCD_ShowString0608(30, row, ": MINUTE", 0, 128);
                LCD_ShowAscii0608(36, row, tot_temp + '0', 0);
            }
        }

        switch (Encoder_Switch_Scan(0)) {
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
//Related setting 6: The length of time the backlight is on 
int PGM_LAMP_TIME_Set(u8 row) {

    u8 LT = LAMP_TIME / 10000;

    LCD_ShowAscii0608(60, row, ':', 0); //

    LCD_ShowString0608(72, row, LAMP_SHOW[LT], 0, 128); //

    LCD_ShowPIC0608(116, row, 0, 0); //arrow

    while (1) {
        FeedDog();                  //Feed the dog - watch dog timer initialization (?)
        SQ_Read_Control();
        if (KDU_INSERT) {
            return BACK2MAIN;
        }

        switch (Encoder_Switch_Scan(0)) {
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
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES != 0) {
            TIMES = 0;
            LT = !LT;
            LCD_ShowString0608(72, row, LAMP_SHOW[LT], 0, 128);
        }
        //
    }
}

//Related setting 7: Six-pin power output
int PGM_POWEROUT_Set(u8 row) {
    TIMES = 0;
    char power = VDO;
    LCD_ShowAscii0608(60, row, ':', 0);
    LCD_ShowString0608(66, row, opt_state[power], 0, 90);
    LCD_ShowPIC0608(116, row, 0, 0);

    while (1) {
        FeedDog();                  //Feed the dog - watch dog timer initialization (?)
        SQ_Read_Control();
        if (KDU_INSERT) {
            return BACK2MAIN;
        }

        switch (Encoder_Switch_Scan(0)) {
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
        switch (Matrix_KEY_Scan(0)) {
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
        if (TIMES != 0) {
            TIMES = 0;
            power = !power;
            LCD_ShowString0608(66, row, opt_state[power], 0, 128);
        }
    }
}

//Related settings 8: PTT button tone setting
int TONE_SET(u8 _tone) {                            //_tone: Tone to be set;
    char t_sta[2] = {PRE_TONE, END_TONE};           //The status of the pre-confidence signal and the post-confidence signal

    TIMES = 0;
    LCD_ShowAscii0608(84, _tone + 1, ':', 0);
    LCD_ShowString0608(90, _tone + 1, opt_state[t_sta[_tone]], 0, 114);         //Display switch status
    while (1) {
        FeedDog();                  //Feed the dog - watch dog timer initialization (?)
        SQ_Read_Control();
        if (KDU_INSERT) {
            return BACK2MAIN;
        }

        switch (Matrix_KEY_Scan(0)) {
        case MATRIX_RESULT_8:
            return BACK2MAIN;
        case MATRIX_RESULT_P:
        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT:
        case MATRIX_RESULT_RIGHT:
            TIMES--;
            break;

        case MATRIX_RESULT_ENT:
            if (_tone) {
                END_TONE = t_sta[_tone];
                save_EndTone(t_sta[_tone]);
            } else {
                PRE_TONE = t_sta[_tone];
                save_PreTone(t_sta[_tone]);
            }
            return ENT2LAST;

        case MATRIX_RESULT_CLR:
            return CLR2LAST;
        }
        if (TIMES != 0) {
            TIMES = 0;
            t_sta[_tone] = !t_sta[_tone];
            LCD_ShowString0608(90, _tone + 1, opt_state[t_sta[_tone]], 0, 128);
        }
        switch (Encoder_Switch_Scan(0)) {
        case key_click:                         //Confirm the current settings
            if (_tone) {
                END_TONE = t_sta[_tone];
                save_EndTone(t_sta[_tone]);
            } else {
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
int PGM_TONE_Select(u8 row) {
    TIMES = 0;
    u8 index = 0;
    LCD_ShowMenu31(menu_tone, 2, index);
    while (1) {
        FeedDog();                  //Feed the dog - watch dog timer initialization (?)
        SQ_Read_Control();
        if (KDU_INSERT) {
            return BACK2MAIN;
        }

        switch (Matrix_KEY_Scan(0)) {
        case MATRIX_RESULT_8:
            return BACK2MAIN;

        case MATRIX_RESULT_P:
        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT:
        case MATRIX_RESULT_RIGHT:
            TIMES--;
            break;

        case MATRIX_RESULT_ENT:
            if (TONE_SET(index) == BACK2MAIN) {
                return BACK2MAIN;
            }
            LCD_ShowMenu31(menu_tone, 2, index);
            break;

        case MATRIX_RESULT_CLR:
            return CLR2LAST;
        }
        if (TIMES != 0) {
            TIMES = 0;
            index = !index;
            LCD_ShowMenu31(menu_tone, 2, index);
        }
        switch (Encoder_Switch_Scan(0)) {
        case key_click:
            if (TONE_SET(index) == BACK2MAIN) {
                return BACK2MAIN;
            }
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

extern volatile u8 key_timer_cnt1, key_timer_cnt2;
void disposePer100ms(void) { //100ms

    //Clear interrupt flag
    if (SQL_CTL) {
        sql_cal++;
    }
    if (SCAN_CTL) {
        scan_cal++;
    }

    Cal2Shut();
    key_timer_cnt1++;
    key_timer_cnt2++;
}

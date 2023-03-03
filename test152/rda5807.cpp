#include "rda5807.h"

#include "tim_int.h"        //Radio recovery
#include "key.h"            //Volume UP/DOWN buttons
#include "encoder.h"        //Encoder
#include "bsp_m62364.h"     //Audio control
#include "FCS152_KDU.h"     //PRC-152_KDU functions definition

#include "bsp_iic.h"                //I2C bus
#include "bsp_timer.h"              //Internal timer/watchdog timer
#include "bsp_MatrixKeyBoard.h"     //Radio "main" keyboard

#include "bsp_storage.h"            //Storage frequency
#include "controller.h"             //Enable/control

extern volatile u8 WFM;
extern volatile u8 KDU_INSERT;
extern int TIMES;                   //Record the number of encoder rotations
extern u8 WFM_LEVEL[8], VOLUME, AUD;
extern void SHUT(void);
extern void VOL_Reflash(void);
extern void MY_GLOBAL_FUN(void);
extern void VFO_Clear(void);

short RDA5807_ReadReg(unsigned char addr) {
    short buf;
    IIC_Start();
    IIC_Send_Byte(RDA5807_WRITE);
    IIC_Wait_Ack();
    IIC_Send_Byte(addr);
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(RDA5807_READ);
    IIC_Wait_Ack();
    buf = IIC_Read_Byte(1);
    buf = buf<<8;
    buf = buf|IIC_Read_Byte(0);
    IIC_Stop();
    delay_ms(5);
    return buf;
}
void RDA5807_WriteReg(unsigned char addr,short val) {
    IIC_Start();
    IIC_Send_Byte(RDA5807_WRITE);
    IIC_Wait_Ack();
    IIC_Send_Byte(addr);
    IIC_Wait_Ack();
    IIC_Send_Byte(val>>8);
    IIC_Wait_Ack();
    IIC_Send_Byte(val&0xFF);
    IIC_Wait_Ack();
    IIC_Stop();
}
//

//Radio ON/OFF switch
void RDA_Power(unsigned char off_on) {
    if (off_on) {
        FM_AMP_EN_SET;
        FM_S_EN_SET;
        return;
    }
    FM_AMP_EN_CLR;
    FM_S_EN_CLR;

}
//

//Volume setting 0 ~ 15
void RDA5807_Set_Volume(unsigned char vol) {
    short temp=0;
    temp=RDA5807_ReadReg(RDA5807_R05);
    temp&=0xfff0;
    RDA5807_WriteReg(RDA5807_R05, temp|vol);

}

//Signal strength acquisition 0 ~ 127
short RDA5807_RSSI() {
    return (RDA5807_ReadReg(RDA5807_R0B)>>9);
}

//Set the frequency in units of 100 K, which is 870 ~ 1000 kHz
void RDA5807_Set_Freq(short freq) {
    uint16_t timeout=0;
    RDA5807_WriteReg(RDA5807_R03, ((freq-870)<<6) + 0x0010);
    while ((RDA5807_ReadReg(RDA5807_R0B)&0x80)==0 && timeout++);
    delay_ms(50);
}

//Radio switch, used when setting radio data
void RDA5807_Init(char off_on) {
    if (off_on) {
        RDA_Power(ON);
        delay_ms(2);
        RDA5807_WriteReg(RDA5807_R02,0xc001);
        delay_ms(700);
        RDA5807_WriteReg(RDA5807_R04,0x0400);
        delay_ms(60);
        RDA5807_WriteReg(RDA5807_R05,0xc5Ac);
        delay_ms(60);
        RDA5807_WriteReg(RDA5807_R02,0xf281);
        delay_ms(70);                           //delay_ms(700);

        RDA5807_Set_Freq(load_FMFreq());
        M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[VOLUME]);
        SPK_SWITCH(AUD, ON);
    } else {
        M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[0]);
        RDA5807_WriteReg(RDA5807_R02,0x0000);
        delay_ms(50);                        //printf("Close FM: %d\n", WFM);
        RDA_Power(OFF);
    }

}
//

//The radio is paused, the flag is not changed, and it is used when the signal is detected.
void RDA5807_MUTE() {
    RDA5807_WriteReg(RDA5807_R02, 0x0000);
    delay_ms(50);
}
void RDA5807_NMUTE() {
    RDA5807_WriteReg(RDA5807_R02, 0xc001);
    delay_ms(500);                           //Wake up
}

//
int RDA5807_Switch(void) {
    char FM_NOW = WFM;              //Enabling FM broadcast radio 
    TIMES=0;

    LCD_ShowPIC0608(104, 1, 1, 1);
    LCD_ShowString0608(110, 1, opt_state[WFM], 0, 128);
    while (1) {
        if (KDU_INSERT) {
            return 1;
        }

        MY_GLOBAL_FUN();
        if (PTT_READ == 0) {
            continue;
        }
        if (TIMES!=0) {
            TIMES = 0;
            FM_NOW = !FM_NOW;
            LCD_ShowString0608(110, 1, opt_state[FM_NOW], 0, 128);
        }
        //////////////////////////////////////////////////////////
        switch (Encoder_Switch_Scan(0)) {
        case key_click:
            LCD_ShowAscii0608(104, 1, ' ', 1);
            LCD_ShowString0608(110, 1, opt_state[FM_NOW], 1, 128);
            if (FM_NOW == WFM) {
                return 1;
            }

            WFM = FM_NOW;
            if (A002_SQ_READ==0) {
                RDA5807_Init(OFF);
            } else {
                RDA5807_Init(WFM);
            }

            if (WFM == OFF && A002_SQ_READ && PTT_READ) {
                SPK_SWITCH(AUD, OFF);
            }

            //              WFM?RDA5807_Resume():RDA5807_Stop();
            if (RDA5807_ReadReg(0xb)&0x0100) {
                LCD_ShowString0608(0,1, "TRUE       ",1,100);
            } else {
                LCD_ShowString0608(0,1, "FALSE      ",1,100);
            }
            return 1;

        case key_double:
            LCD_ShowAscii0608(104, 1, ' ', 1);
            LCD_ShowString0608(110, 1, opt_state[WFM], 1, 128);
            return 1;

        case key_long:
            SHUT();
            return 0;
        }
        //////////////////////////////////////////////////////////
        switch (Matrix_KEY_Scan(0)) {
        case MATRIX_RESULT_P:
        case MATRIX_RESULT_N:
        case MATRIX_RESULT_LEFT:
        case MATRIX_RESULT_RIGHT:
            TIMES++;
            break;

        case MATRIX_RESULT_ENT:
            LCD_ShowAscii0608(104, 1, ' ', 1);
            LCD_ShowString0608(110, 1, opt_state[FM_NOW], 1, 128);
            if (FM_NOW == WFM) {
                return 0;
            }

            WFM = FM_NOW;
            if (A002_SQ_READ==0) {
                RDA5807_Init(OFF);
            } else {
                RDA5807_Init(WFM);
            }
            if (WFM == 0 && A002_SQ_READ && PTT_READ) {
                SPK_SWITCH(AUD, OFF);
            }
            //              WFM?RDA5807_Resume():RDA5807_Stop();
            if (RDA5807_ReadReg(0xb)&0x0100) {
                LCD_ShowString0608(0,1, "TRUE       ",1,100);
            } else {
                LCD_ShowString0608(0,1, "FALSE      ",1,100);
            }
            return 1;

        case MATRIX_RESULT_CLR:
            LCD_ShowAscii0608(104, 1, ' ', 1);
            LCD_ShowString0608(110, 1, opt_state[FM_NOW], 1, 128);
            return 1;

        case MATRIX_RESULT_3:
            return 3;
        }
        //////////////////////////////////////////////////////////
    }
}


void Radio_Freq_Show(int fm_freq, int mode) {
    LCD_ShowAscii1016(0,  2, fm_freq/1000%10 + '0', mode);
    LCD_ShowAscii1016(10, 2, fm_freq/100%10  + '0', mode);
    LCD_ShowAscii1016(20, 2, fm_freq/10%10   + '0', mode);
    LCD_ShowAscii1016(30, 2, '.', mode);
    LCD_ShowAscii1016(40, 2, fm_freq%10      + '0', mode);
}

//
extern volatile char Home_Mode, Flag_Main_Page;
extern char now_chan;
extern int  FM_FREQ;
unsigned char FM_CHAN=0;

int fm_locate=0, fm_bit=0, fm_freq_static;
unsigned char fm_freq_buf[4]= {0,0,0,1}, flag_clear=0, fm_finish=0,
                                         val_in=0, key_press=17, first_press=1;

int FM_Freq_Set_Show(int x,int y,int * result) {
    if (A002_SQ_READ==0 || PTT_READ==0) {
        fm_freq_buf[0]=0,fm_freq_buf[1]=0,fm_freq_buf[2]=0,fm_freq_buf[3]=0;
        first_press=1;
        fm_locate=x;
        fm_bit=0;
        Radio_Freq_Show(FM_FREQ, 1);
        return 0;
    }
    if (fm_locate==30+x) {
        LCD_ShowAscii1016(fm_locate, y, '.', 1);
        fm_locate+=10;
    }
    if (fm_locate==50+x||fm_bit==4) {
        fm_finish=1;
    }
    if (fm_locate>x) {
        LCD_ShowAscii1016(fm_locate, y, '_',1);
    }

    key_press=Matrix_KEY_Scan(0);
    switch (key_press) {
    case MATRIX_RESULT_0:
        if (fm_bit!=0
                || (fm_freq_buf[0]==0 && ((fm_freq_buf[1]==8&&fm_freq_buf[2]>=7) || fm_freq_buf[1]==9))
                || fm_freq_buf[0]==1) {
            val_in=1;
        }
        break;

    case MATRIX_RESULT_1:
        if (WFM==OFF) {
            return 0;
        }
        if (fm_bit==0
                || (fm_freq_buf[0]==0 && ((fm_freq_buf[1]==8&&fm_freq_buf[2]>=7) || fm_freq_buf[1]==9))
                || (fm_freq_buf[0]==1 && fm_freq_buf[1]==0 && fm_freq_buf[2]<8 && fm_bit>1)) {              //fm_freq_buf Cleared to zero
            val_in=1;
        }
        break;

    case MATRIX_RESULT_2:
    case MATRIX_RESULT_4:
    case MATRIX_RESULT_5:
    case MATRIX_RESULT_6:
        D_printf("Press{%d}", key_press);
        if ((fm_freq_buf[0]==0 && ((fm_freq_buf[1]==8&&fm_freq_buf[2]>=7) || fm_freq_buf[1]==9))
                ||(fm_freq_buf[0]==1 && fm_freq_buf[1]==0 && fm_freq_buf[2]<8 && fm_bit>1)) {
            val_in=1;
        }
        break;

    case MATRIX_RESULT_3:
        if (fm_bit == 0) {
            return 8;
        }

        if ((fm_freq_buf[0]==0 && ((fm_freq_buf[1]==8&&fm_freq_buf[2]>=7) || fm_freq_buf[1]==9))
                ||(fm_freq_buf[0]==1 && fm_freq_buf[1]==0 && fm_freq_buf[2]<8 && fm_bit>1)) {
            val_in=1;
        }
        break;


    case MATRIX_RESULT_7:
        D_printf("Press{7}");
        if ((fm_freq_buf[0]==0 && (fm_freq_buf[1]==8 || fm_freq_buf[1]==9))
                ||(fm_freq_buf[0]==1 && fm_freq_buf[1]==0 && (fm_bit==2 || (fm_freq_buf[2]<8&&fm_bit>1)))) {
            val_in=1;
        }
        break;

    case MATRIX_RESULT_8:
        D_printf("Press{8}");
        if (WFM==OFF) {
            return 0;
        }
        if (fm_bit==0) {
            val_in=2;
        } else if ((fm_freq_buf[0]==0 && (fm_freq_buf[1]==8 || fm_freq_buf[1]==9))
                   ||(fm_freq_buf[0]==1 && fm_freq_buf[1]==0 && (fm_bit==2 || (fm_freq_buf[2]<8&&fm_bit>1)))) {
            val_in=1;
        }
        break;

    case MATRIX_RESULT_9:
        D_printf("Press{9}");
        if (WFM==OFF) {
            return 0;
        }
        if (fm_bit==0) {
            val_in=2;
        } else if ((fm_freq_buf[0]==0 && (fm_freq_buf[1]==8 || fm_freq_buf[1]==9))
                   ||(fm_freq_buf[0]==1 && fm_freq_buf[1]==0 && (fm_freq_buf[2]<8&&fm_bit>2))) {
            val_in=1;
        }
        break;

    case MATRIX_RESULT_CLR:
        D_printf("Press{CLR}, CLEAR\n");
        if ((fm_freq_buf[0]==0&&fm_freq_buf[1]==0&&fm_freq_buf[2]==0&&fm_freq_buf[3]==0 && flag_clear)||WFM==OFF) {
            flag_clear=0;
            return 8;           //Exit
        }
        LCD_ShowAscii1016(x+0,  y, ' ', 1);
        LCD_ShowAscii1016(x+10, y, ' ', 1);
        LCD_ShowAscii1016(x+20, y, ' ', 1);
        LCD_ShowAscii1016(x+30, y, ' ', 1);
        LCD_ShowAscii1016(x+40, y, ' ', 1);
        LCD_ShowAscii1016(x+50, y, '}'+1, 1);   //Arrow - I think it's picture of double arrow inside square. 
        fm_freq_buf[0]=0,fm_freq_buf[1]=0,fm_freq_buf[2]=0,fm_freq_buf[3]=0;
        fm_locate=x;
        fm_bit=0;
        flag_clear=1;       //EXIT required
        first_press=1;      //Empty the need
        break;

    case MATRIX_RESULT_ENT:
        D_printf("Press{ENT}, ENTER\n");
        if (fm_bit>0||(fm_bit==0&&flag_clear==1)) {
            if (fm_bit<3 || fm_locate<50+x)
                for (; fm_bit<4; fm_bit++) {        //Make up zero
                    fm_freq_buf[fm_bit]=0;
                }
            fm_finish=1;
            flag_clear=0;
        } else if (fm_bit==0) {
            LCD_ShowAscii0608(98, 1, ' ', 1);
            if (RDA5807_Switch()==3) {
                return 8;
            }
        }
        break;

    case MATRIX_RESULT_P:
        if (WFM==OFF) {
            return 0;
        }
        D_printf("Press{+}\n");
        fm_freq_buf[0]=0,fm_freq_buf[1]=0,fm_freq_buf[2]=0,fm_freq_buf[3]=0;
        first_press=1;
        fm_locate=x;
        fm_bit=0;
        return 2;

    case MATRIX_RESULT_N:
        if (WFM==OFF) {
            return 0;
        }
        D_printf("Press{-}\n");
        fm_freq_buf[0]=0,fm_freq_buf[1]=0,fm_freq_buf[2]=0,fm_freq_buf[3]=0;
        first_press=1;
        fm_locate=x;
        fm_bit=0;
        return 3;

    case MATRIX_RESULT_LEFT:
        if (WFM==OFF) {
            return 0;
        }
        if (fm_bit!=0) {
            for (; fm_bit<4; fm_bit++) {
                fm_freq_buf[fm_bit]=0;
                LCD_ShowAscii1016(fm_locate, y, '0', 1);
                if (fm_bit==2) {
                    fm_locate+=20;
                } else {
                    fm_locate+=10;
                }
            }
            first_press=1;
            fm_locate=x;
            fm_bit=0;
        }
        D_printf("Press{*}, SEEKING FORWARD\n");
        return 4;

    case MATRIX_RESULT_RIGHT:
        if (WFM==OFF) {
            return 0;
        }
        if (fm_bit!=0) {
            for (; fm_bit<4; fm_bit++) {
                fm_freq_buf[fm_bit]=0;
                LCD_ShowAscii1016(fm_locate, y, '0', 1);
                if (fm_bit==2) {
                    fm_locate+=20;
                } else {
                    fm_locate+=10;
                }
            }
            first_press=1;
            fm_locate=x;
            fm_bit=0;
        }
        D_printf("Press[#], SEEKING BACKWARD\n");
        return 5;
    }
    //

    //
    if (val_in==1) {
        flag_clear=0;
        val_in=0;
        if (first_press) {
            first_press=0;
            LCD_ShowAscii1016(x+0, y,   ' ', 1);
            LCD_ShowAscii1016(x+10,y,   ' ', 1);
            LCD_ShowAscii1016(x+20,y,   ' ', 1);
            LCD_ShowAscii1016(x+30,y,   ' ', 1);
            LCD_ShowAscii1016(x+40,y,   ' ', 1);
            LCD_ShowAscii1016(x+50,y, '}'+1, 1);        //Arrow - I think it's picture of double arrow inside square. 
        }
        D_printf("[%d] : %d\n", fm_bit, key_press);
        fm_freq_buf[fm_bit]=key_press;
        fm_bit++;
        LCD_ShowAscii1016(fm_locate, y, key_press + '0', 1);
        fm_locate+=10;
    } else if (val_in==2) { //8x, 9x
        flag_clear=0;
        val_in=0;
        if (first_press) {
            first_press=0;
            LCD_ShowAscii1016(x+0,  y, ' ', 1);
            LCD_ShowAscii1016(x+10, y, ' ', 1);
            LCD_ShowAscii1016(x+20, y, ' ', 1);
            LCD_ShowAscii1016(x+30, y, ' ', 1);
            LCD_ShowAscii1016(x+40, y, ' ', 1);
            LCD_ShowAscii1016(x+50, y, '}'+1, 1);       //Arrow - I think it's picture of double arrow inside square. 
        }
        D_printf("[2] : %d", key_press);
        fm_freq_buf[0]=0,fm_freq_buf[1]=key_press;
        fm_bit+=2;
        LCD_ShowAscii1016(fm_locate,   y, ' ',1);
        LCD_ShowAscii1016(fm_locate+10,y, key_press + '0',1);
        fm_locate+=20;
    }
    //

    //
    if (fm_finish) {
        D_printf("Writing Freq Completed\n");
        LCD_ShowAscii1016(x+50, y, ' ', 1);         //' '

        fm_freq_static=fm_freq_buf[0]*1000+fm_freq_buf[1]*100+fm_freq_buf[2]*10+fm_freq_buf[3];

        if (fm_freq_static<870 || fm_freq_static>1090) {
            fm_freq_static=870;
        }

        D_printf("Right Freq : %d*100K\n", fm_freq_static);
        *result=fm_freq_static;
        Radio_Freq_Show(fm_freq_static, 1);

        //fm_freq_buf[0]=0,fm_freq_buf[1]=0,fm_freq_buf[2]=0,fm_freq_buf[3]=0;
        memset(fm_freq_buf, 0, 4);
        fm_freq_static=0;
        fm_locate=x;
        fm_bit=0;
        first_press=1;
        fm_finish=0;
        return 1;
    }
    //

    return 0;
}


void Enter_Radio() {
    Flag_Main_Page = 0;

    unsigned char   key_encoder,     //Encoder return value
             key_matrix,             //Keyboard operation return value
             locate_encoder=0,       //After the encoder is pressed for the first time, the position of the cursor
             flag_return=0,          //The encoder exits the operation and clears the arrow cursor
             fm_change=1;            //A sign for channel judgment after operation

    D_printf("#######%s: %d#######\n", __FUNCTION__, __LINE__);
    //FM_FREQ=load_FMFreq();
    D_printf("#######%s: %d#######\n", __FUNCTION__, __LINE__);

    LCD_Clear(EDITZONE32);

    Radio_Freq_Show(FM_FREQ, 1);
    LCD_ShowString0608(110, 1, opt_state[WFM], 1, 128);
    LCD_ShowPIC1616(60,2,  11,1);  //<<
    LCD_ShowPIC1616(86,2,  12,1);  //>>
    LCD_ShowPIC1616(112,2, 13,1);  //Return flag
    while (1) {
        MY_GLOBAL_FUN();
        if (A002_SQ_READ==0) {
            LCD_ShowString0608(0, 1, "R:", 1, 12);
            if (Home_Mode == DUAL_MODE) {
                LCD_ShowFreq(12, 1, now_chan?chan_arv[CHANB].RX_FREQ:chan_arv[CHANA].RX_FREQ, 1);
            } else {
                LCD_ShowFreq(12, 1, chan_arv[NOW].RX_FREQ, 1);
            }
        }
        if (PTT_READ==0) {
            LCD_ShowString0608(0, 1, "T:", 1, 12);
            if (Home_Mode == DUAL_MODE) {
                LCD_ShowFreq(12, 1, now_chan?chan_arv[CHANB].TX_FREQ:chan_arv[CHANA].TX_FREQ, 1);
            } else {
                LCD_ShowFreq(12, 1, chan_arv[NOW].TX_FREQ, 1);
            }

            continue;
        }

        D_printf("#######%s: %d#######\n", __FUNCTION__, __LINE__);
        if (KDU_INSERT) {
            return;
        }

        key_encoder = Encoder_Switch_Scan(0);
        switch (key_encoder) {
        case key_click:             //Press the encoder for the first time
            TIMES=0;

            if (flag_clear || !first_press) {
                if (flag_clear) {
                    flag_clear=0;
                } else {
                    first_press = 1;
                    fm_freq_buf[0]=0,fm_freq_buf[1]=0,fm_freq_buf[2]=0,fm_freq_buf[3]=0;
                    first_press=1;
                    fm_locate=0;
                    fm_bit=0;
                }

                Radio_Freq_Show(FM_FREQ, 1);
                LCD_ShowAscii1016(50, 2, ' ', 1);
                break;
            }


            LCD_ShowString0608(98, 1, " ", 1, 128);
            LCD_ShowPIC0608(104, 1, 1, 1);
            LCD_ShowString0608(110, 1, opt_state[WFM], 1, 128);

            while (1) {
                MY_GLOBAL_FUN();
                if (KDU_INSERT) {
                    return;
                }

                key_matrix = Matrix_KEY_Scan(0);
                key_encoder = Encoder_Switch_Scan(0);

                if (key_matrix == MATRIX_RESULT_P || key_matrix == MATRIX_RESULT_RIGHT) {
                    TIMES++;
                }
                if (key_matrix == MATRIX_RESULT_N || key_matrix == MATRIX_RESULT_LEFT) {
                    TIMES--;
                }

                if (TIMES!=0) {             //Select option
                    if (TIMES>0) {          //+
                        TIMES=0;
                        locate_encoder=(locate_encoder+1)%5;
                    } else if (TIMES<0) { //-
                        TIMES=0;
                        locate_encoder=(locate_encoder-1+5)%5;
                    }
                    LCD_ShowAscii0608(104, 1, ' ', 1);
                    LCD_ShowAscii1016(50, 2, ' ', 1);
                    LCD_ShowAscii1016(77, 2, ' ', 1);
                    LCD_ShowAscii1016(103, 2, ' ', 1);
                    switch (locate_encoder) {           //Arrow display - perhaps this is the activation of the indication of the search and switching of radio 
                                                        //frequencies in the broadcast range.
                    case 0:
                        LCD_ShowPIC0608(104, 1, 1, 1);
                        break;
                    case 1:
                        LCD_ShowAscii1016(50,  2,'}'+1,1);
                        break;
                    case 2:
                        LCD_ShowAscii1016(77,  2,'}'+1,1);
                        break;
                    case 3:
                        LCD_ShowAscii1016(103, 2,'}'+1,1);
                        break;
                    case 4:
                        LCD_ShowAscii1016(103, 2,'}'+2,1);
                        break;
                    }
                }
                //
                if (key_encoder == key_click || key_matrix == MATRIX_RESULT_ENT) {
                    TIMES=0;
                    switch (locate_encoder) {
                    case 0:
                        LCD_ShowAscii0608(98, 1, ' ', 1);
                        LCD_ShowString0608(110, 1, opt_state[WFM], 0, 128);
                        flag_return=RDA5807_Switch();
                        if (flag_return == 3) {
                            VFO_Clear();
                            return;
                        }
                        break;

                    case 1:                                         //Step by step FM
                        if (A002_SQ_READ==0 || WFM==OFF) {
                            flag_return=1;
                            break;
                        }
                        RDA5807_ResumeImmediately();
                        Radio_Freq_Show(FM_FREQ, 0);
                        while (1) {
                            MY_GLOBAL_FUN();
                            if (TIMES!=0) {
                                if (TIMES>0) {
                                    TIMES--;
                                    if (FM_FREQ<1080) {
                                        FM_FREQ++;
                                    }
                                } else if (TIMES<0) {
                                    TIMES++;
                                    if (FM_FREQ>870) {
                                        FM_FREQ--;
                                    }
                                }
                                RDA5807_Set_Freq(FM_FREQ);
                                Radio_Freq_Show(FM_FREQ, 0);
                                if (RDA5807_ReadReg(0xb)&0x0100) {
                                    D_printf("fm_freq_temp=%d\n",FM_FREQ);
                                    LCD_ShowString0608(0,1, "TRUE       ", 1, 100);
                                } else {
                                    LCD_ShowString0608(0,1, "FALSE      ", 1, 100);
                                }
                            }
                            //
                            key_encoder= Encoder_Switch_Scan(0);
                            key_matrix = Matrix_KEY_Scan(0);
                            if (key_encoder==key_click || key_encoder==key_double || KDU_INSERT || key_matrix == MATRIX_RESULT_CLR || PTT_READ==0 || A002_SQ_READ==0) { //Confirm to return to the previous level
                                Radio_Freq_Show(FM_FREQ, 1);
                                save_FMFreq(FM_FREQ);
                                flag_return=1;
                                break;
                            } else if (key_encoder==key_long) {
                                SHUT();
                            }
                        }
                        break;

                    case 2:                                         //Sweep down
                        if (A002_SQ_READ==0 || WFM==OFF) {
                            flag_return=1;
                            break;
                        }
                        RDA5807_ResumeImmediately();
                        LCD_ShowPIC1616(60,2,11,0);
                        LCD_ShowString0608(0, 1, "SEEKING...   ", 1, 120);
                        while (1) {
                            MY_GLOBAL_FUN();

                            FM_FREQ--;
                            if (FM_FREQ<870) {
                                FM_FREQ=1080;
                            }
                            RDA5807_Set_Freq(FM_FREQ);
                            Radio_Freq_Show(FM_FREQ, 1);

                            if (RDA5807_ReadReg(0xb)&0x0100) {      //Find the station and return to the previous level
                                LCD_ShowString0608(0,1, "TRUE         ", 1, 120);
                                break;
                            }
                            key_encoder=Encoder_Switch_Scan(0);
                            key_matrix = Matrix_KEY_Scan(0);
                            if (key_encoder==key_click|key_encoder==key_double|| key_matrix == MATRIX_RESULT_CLR || KDU_INSERT || PTT_READ==0 || A002_SQ_READ==0) {
                                if (RDA5807_ReadReg(0xb)&0x0100) {          //Find the station and return to the previous level
                                    LCD_ShowString0608(0, 1, "TRUE         ", 1, 120);
                                } else {
                                    LCD_ShowString0608(0, 1, "FALSE        ", 1, 120);
                                }
                                break;
                            } else if (key_encoder==key_long) {
                                SHUT();
                            }
                        }
                        save_FMFreq(FM_FREQ);
                        LCD_ShowPIC1616(60,2,11,1);
                        break;

                    case 3:                                         //Sweep up
                        if (A002_SQ_READ==0 || WFM==OFF) {
                            flag_return=1;
                            break;
                        }
                        RDA5807_ResumeImmediately();
                        LCD_ShowPIC1616(86,2,12,0);
                        LCD_ShowString0608(0, 1, "SEEKING...   ", 1, 120);
                        while (1) {

                            MY_GLOBAL_FUN();

                            FM_FREQ++;
                            if (FM_FREQ>1080) {
                                FM_FREQ=870;
                            }
                            RDA5807_Set_Freq(FM_FREQ);
                            Radio_Freq_Show(FM_FREQ, 1);

                            if (RDA5807_ReadReg(0xb)&0x0100) {      //Find the station and return to the previous level
                                LCD_ShowString0608(0, 1, "TRUE         ", 1, 120);
                                break;
                            }
                            key_encoder=Encoder_Switch_Scan(0);
                            key_matrix = Matrix_KEY_Scan(0);
                            if (key_encoder==key_click || key_encoder==key_double || key_matrix==MATRIX_RESULT_CLR || KDU_INSERT || A002_SQ_READ==0 || PTT_READ ==0) {
                                if (RDA5807_ReadReg(0xb)&0x0100) {  //Find the station and return to the previous level
                                    LCD_ShowString0608(0, 1, "TRUE         ", 1, 120);
                                } else {
                                    LCD_ShowString0608(0, 1, "FALSE        ", 1, 120);
                                }
                                break;
                            } else if (key_encoder==key_long) {
                                SHUT();
                            }
                        }
                        save_FMFreq(FM_FREQ);
                        LCD_ShowPIC1616(86,2,12,1);
                        break;

                    case 4:                                         //Exit the radio
                        VFO_Clear();
                        return;
                    }
                    TIMES=0;
                } else if (key_encoder == key_double || key_matrix == MATRIX_RESULT_CLR || PTT_READ==0 || A002_SQ_READ==0) {
                    flag_return=1;
                } else if (key_encoder == key_long) {
                    SHUT();
                }
                if (flag_return) {                                  //Exit encoder selection
                    LCD_ShowAscii0608(104, 1, ' ', 1);
                    LCD_ShowAscii1016(50,  2, ' ', 1);
                    LCD_ShowAscii1016(77,  2, ' ', 1);
                    LCD_ShowAscii1016(103, 2, ' ', 1);
                    flag_return=0;
                    break;
                }
            }
            locate_encoder=0;
            break;

        case key_double:
            if (flag_clear || !first_press) {
                if (flag_clear) {
                    flag_clear=0;
                } else {
                    first_press = 1;
                    fm_freq_buf[0]=0,fm_freq_buf[1]=0,fm_freq_buf[2]=0,fm_freq_buf[3]=0;
                    first_press=1;
                    fm_locate=0;
                    fm_bit=0;
                }

                Radio_Freq_Show(FM_FREQ, 1);
                LCD_ShowAscii1016(50, 2, ' ', 1);
                break;
            }
            VFO_Clear();
            return;

        case key_long:
            SHUT();
            return;
        }

        D_printf("#######%s: %d#######\n", __FUNCTION__, __LINE__);
        key_matrix=FM_Freq_Set_Show(0, 2, &FM_FREQ);
        if (key_matrix==1) {                                        //Finished
            fm_change=1;
            RDA5807_Set_Freq(FM_FREQ);
        } else if (key_matrix==2 || TIMES>0) {                      //Up plus P key (?)
            TIMES=0;
            if (WFM==OFF) {
                continue;
            }
            RDA5807_ResumeImmediately();
            fm_change=1;
            if (FM_FREQ<1080) {
                FM_FREQ++;
            }

            RDA5807_Set_Freq(FM_FREQ);
            Radio_Freq_Show(FM_FREQ, 1);

        } else if (key_matrix==3 || TIMES<0) {                      //Down minus N key (?)
            TIMES=0;
            if (WFM==OFF) {
                continue;
            }
            RDA5807_ResumeImmediately();
            fm_change=1;
            if (FM_FREQ>870) {
                FM_FREQ--;
            }

            RDA5807_Set_Freq(FM_FREQ);
            Radio_Freq_Show(FM_FREQ, 1);

        } else if (key_matrix==4) {                                 //Swipe down the left button - Button "4" on matrix keyboard hasn't any labels - what does it mean?
            TIMES=0;
            RDA5807_ResumeImmediately();
            LCD_ShowPIC1616(60,2,11,0);
            LCD_ShowString0608(0, 1, "SEEKING...   ", 1, 120);
            while (1) {
                FeedDog();                                          //Feed the dog - initializing watchdog timer
                FM_FREQ--;
                if (FM_FREQ<870) {
                    FM_FREQ=1080;
                }
                RDA5807_Set_Freq(FM_FREQ);
                Radio_Freq_Show(FM_FREQ, 1);

                if (RDA5807_ReadReg(0xb)&0x0100) {
                    save_FMFreq(FM_FREQ);
                    FM_CHAN = 1;
                    LCD_ShowString0608(0, 1, (FM_CHAN?"TRUE       ":"FALSE      "), 1, 100);
                    break;
                }
                key_encoder = Encoder_Switch_Scan(0);
                key_matrix  = Matrix_KEY_Scan(0);
                if (key_encoder==key_click || key_encoder==key_double || key_matrix==MATRIX_RESULT_CLR || KDU_INSERT || A002_SQ_READ==0 || PTT_READ ==0) {
                    fm_change=1;
                    break;
                } else if (key_encoder == key_long) {
                    SHUT();
                }
            }
            //
            TIMES = 0;
            LCD_ShowPIC1616(60,2,11,1);
        } else if (key_matrix==5) {                                 //Swipe up right button (?) - Button "5" on matrix keyboard is "Zero" - what does it mean?
            TIMES=0;
            RDA5807_ResumeImmediately();
            LCD_ShowPIC1616(86,2,12,0);
            LCD_ShowString0608(0, 1, "SEEKING...   ", 1, 120);
            while (1) {
                FeedDog();                                          //Feed the dog - initializing watchdog timer
                FM_FREQ++;
                if (FM_FREQ>1080) {
                    FM_FREQ=870;
                }
                RDA5807_Set_Freq(FM_FREQ);
                Radio_Freq_Show(FM_FREQ, 1);

                if (RDA5807_ReadReg(0xb)&0x0100) {
                    save_FMFreq(FM_FREQ);
                    FM_CHAN = 1;
                    LCD_ShowString0608(0, 1, (FM_CHAN?"TRUE       ":"FALSE      "), 1, 100);
                    break;
                }
                key_encoder = Encoder_Switch_Scan(0);
                key_matrix  = Matrix_KEY_Scan(0);
                if (key_encoder==key_click || key_encoder==key_double || key_matrix==MATRIX_RESULT_CLR || KDU_INSERT || A002_SQ_READ==0 || PTT_READ ==0) {
                    fm_change=1;
                    break;
                } else if (key_encoder == key_long) {
                    SHUT();
                }
            }
            TIMES=0;
            LCD_ShowPIC1616(86,2,12,1);
        } else if (key_matrix==8) {                                 //Exit CLR key - (?)
            VFO_Clear();
            return;
        }
        D_printf("#######%s: %d#######\n", __FUNCTION__, __LINE__);

        //Judge whether the real station is or not (?)
        if (fm_change) {
            fm_change=0;
            D_printf("#######%s: %d#######\n", __FUNCTION__, __LINE__);
            save_FMFreq(FM_FREQ);
            D_printf("#######%s: %d#######\n", __FUNCTION__, __LINE__);
            FM_CHAN = RDA5807_ReadReg(0xb)&0x0100;
            LCD_ShowString0608(0, 1, FM_CHAN?"TRUE       ":"FALSE      ", 1, 100);

        }
    }
}
//
#include "bsp_m62364.h"
void RDA5807_Stop(void) {
    //printf("Stop FM\n");
    RDA_Power(OFF);
    M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[0]);
}
void RDA5807_Resume(void) {
    //printf("ReOpen FM\n");
    RDA_Power(ON);
    delay_ms(2);
    RDA5807_Set_Freq(load_FMFreq());
    M62364_SetSingleChannel(WFM_LINE_CHAN, WFM_LEVEL[VOLUME]);
    SPK_SWITCH(AUD, ON);
}

//




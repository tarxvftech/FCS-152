#include "analog.h"
#include "tim_int.h"
#include "controller.h"

#include "bsp_m62364.h"
#include "bsp_delay.h"
#include "bsp_dac.h"
#include "bsp_uart.h"
#include "bsp_device.h"

extern int LAMP_TIME;
extern u8 BL;
extern u8 A20_LEVEL[8], VOLUME, AUD;
extern volatile u8 KDU_INSERT;
u8 POWER_SELECT_FLAG= 1;

void PWM_Init(void) {
    bsp_PWM_Init();
    BackLight_SetVal(50);
}

void ADC_Init(void) {
    bsp_ADC_Init();
}

void DAC_Init(void) {
    SineWave_Data();    //生成数据
    bsp_DAC_Init();
}

void LightBacklight(void) {
// Serial.printf("LAMP_TIME :%d && KDU_INSERT = %d\n", LAMP_TIME, KDU_INSERT);
// Serial.printf(" \n@@@@@%04d  KDU_INSERT = %d@@@@@%\n", __LINE__, KDU_INSERT);
    if(LAMP_TIME > 0 && KDU_INSERT == OFF) {
        // D_printf("*****%s\n", __FUNCTION__);
        bsp_StartAutoTimer(TMR_FLOW, LAMP_TIME);
        BackLight_SetVal(BL);
    }
}


int  Get_Battery_Vol(void) {
    int voltage  = Use_ADC();
    static u8 lowBatteryCal = 0;    //检测电池电压低次数
    if(POWER_SELECT_FLAG) {         //12V
        if(voltage < VOLTAGE_OFF_12_FLOOR || voltage > VOLTAGE_OFF_12_UPPER) {
            lowBatteryCal++;
        }
        voltage = (voltage - VOLTAGE_12_CAL) / 30;
    } else {                        //8V
        if(voltage < VOLTAGE_OFF_8_FLOOR || voltage > VOLTAGE_OFF_8_UPPER) {
            lowBatteryCal++;
        }
        voltage = (voltage - VOLTAGE_8_CAL) / 20;
    }
    if(lowBatteryCal >= 5) {
        D_printf("Low Power to Shut...\n");
        SHUT();
    }


    if(voltage < 0) {
        voltage = 0;
    } else if(voltage > 100) {
        voltage = 100;
    }

    return voltage;
}

//PTT press: start and end transmission beeps
void Start_Tone(unsigned char STOP_START) {
    SPK_SWITCH(AUD, ON);//==>响 发射提示

    if(VOLUME>0) {
        M62364_SetSingleChannel(A20_LINE_CHAN, 5);
        M62364_SetSingleChannel(8, 50);
    } else {
        M62364_SetSingleChannel(A20_LINE_CHAN, 0);
        M62364_SetSingleChannel(8, 0);
    }
    M62364_SetSingleChannel(TONE_OUT_CHAN, 100);    //输出到A20发射

    delay_ms(200);//必需的延时,否则缺失第一声

    pinMode(18, OUTPUT);
    ESP_ERROR_CHECK(dac_output_enable(DAC_CHAN));
    if(STOP_START==1) { //开始
        //前置提示音
        RingTone(TONE2K, ON);
        delay_ms(100);//delay_ms(80);
        RingTone(TONE2K, OFF);
        delay_ms(110);
        RingTone(TONE2K, ON);
        delay_ms(160);
        RingTone(TONE2K, OFF);
    } else {
        //结束提示音
        RingTone(TONE1_5K, ON);
        delay_ms(100);
        RingTone(TONE1_5K, OFF);
    }
//////////////////////////////////////////////////////////////////
    dac_output_voltage(DAC_CHAN, 0);
    ESP_ERROR_CHECK(dac_output_disable(DAC_CHAN));
    pinMode(18, INPUT_PULLUP);

    SPK_SWITCH(AUD, OFF);//==>发射提示
    M62364_SetSingleChannel(A20_LINE_CHAN, 0);
    M62364_SetSingleChannel(TONE_OUT_CHAN, 0);
    M62364_SetSingleChannel(8, 0);
}

//长按静噪按键进入常静噪模式的提示音
void Start_ToneSql0(void) {
////////////////////////////////////////////////////////////////////////////////////////////////////////
    M62364_SetSingleChannel(A20_LINE_CHAN, 5);                  //修改增益输出"Di"
    M62364_SetSingleChannel(8, 50);                             //toneout输出打开
//////////////////////////////////////////////////////////////////
// configure DAC
    pinMode(18, OUTPUT);                            //Set the pin to output mode in order to start the DAC
    ESP_ERROR_CHECK(dac_output_enable(DAC_CHAN));
    //////////////////////////////////////////////////
    RingTone(TONE1_5K, ON);
    delay_ms(60);
    RingTone(TONE1_5K, OFF);
//////////////////////////////////////////////////////
//end DAC, reconfigure A002
    dac_output_voltage(DAC_CHAN, 0);
    ESP_ERROR_CHECK(dac_output_disable(DAC_CHAN));
    pinMode(18, INPUT_PULLUP);                      //Pull high level to restore serial port 2 RX communication
//////////////////////////////////////////////////////////////////
    M62364_SetSingleChannel(8, 0);                              //toneout输出关闭
    M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);  //恢复当前增益大小输出声音
////////////////////////////////////////////////////////////////////////////////////////////////////////
}

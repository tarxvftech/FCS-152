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
    SineWave_Data();    //Generate sinewave data
    bsp_DAC_Init();
}

void LightBacklight(void) {
// Serial.printf("LAMP_TIME :%d && KDU_INSERT = %d\n", LAMP_TIME, KDU_INSERT);
// Serial.printf(" \n@@@@@%04d  KDU_INSERT = %d@@@@@%\n", __LINE__, KDU_INSERT);
    if (LAMP_TIME > 0 && KDU_INSERT == OFF) {
        // D_printf("*****%s\n", __FUNCTION__);
        bsp_StartAutoTimer(TMR_FLOW, LAMP_TIME);
        BackLight_SetVal(BL);
    }
}


int Get_Battery_Vol(void) {
    int voltage  = Use_ADC();
    static u8 lowBatteryCal = 0;    //Number of times to detect low battery voltage
    if (POWER_SELECT_FLAG) {        //12V
        if (voltage < VOLTAGE_OFF_12_FLOOR || voltage > VOLTAGE_OFF_12_UPPER) {
            lowBatteryCal++;
        }
        voltage = (voltage - VOLTAGE_12_CAL) / 30;
    } else {                        //8V
        if (voltage < VOLTAGE_OFF_8_FLOOR || voltage > VOLTAGE_OFF_8_UPPER) {
            lowBatteryCal++;
        }
        voltage = (voltage - VOLTAGE_8_CAL) / 20;
    }
    if (lowBatteryCal >= 5) {
        D_printf("Low Power to Shut...\n");
        SHUT();
    }


    if (voltage < 0) {
        voltage = 0;
    } else if (voltage > 100) {
        voltage = 100;
    }

    return voltage;
}

//PTT press: start and end transmission beeps
void Start_Tone(unsigned char STOP_START) {
    SPK_SWITCH(AUD, ON);                            //==>Sound launch prompt

    if (VOLUME>0) {
        M62364_SetSingleChannel(A20_LINE_CHAN, 5);
        M62364_SetSingleChannel(8, 50);
    } else {
        M62364_SetSingleChannel(A20_LINE_CHAN, 0);
        M62364_SetSingleChannel(8, 0);
    }
    M62364_SetSingleChannel(TONE_OUT_CHAN, 100);    //Output to A20 transmitter - what is A20? what it does?

    delay_ms(200);                                  //Necessary delay, otherwise the first sound is missing

    pinMode(18, OUTPUT);
    ESP_ERROR_CHECK(dac_output_enable(DAC_CHAN));   //Sounding an error
    if (STOP_START==1) { //start 
        //Front tone
        RingTone(TONE2K, ON);
        delay_ms(100);//delay_ms(80);
        RingTone(TONE2K, OFF);
        delay_ms(110);
        RingTone(TONE2K, ON);
        delay_ms(160);
        RingTone(TONE2K, OFF);
    } else {
        //End tone
        RingTone(TONE1_5K, ON);
        delay_ms(100);
        RingTone(TONE1_5K, OFF);
    }
//////////////////////////////////////////////////////////////////
    dac_output_voltage(DAC_CHAN, 0);
    ESP_ERROR_CHECK(dac_output_disable(DAC_CHAN));
    pinMode(18, INPUT_PULLUP);

    SPK_SWITCH(AUD, OFF);                             //==>Sound launch off
    M62364_SetSingleChannel(A20_LINE_CHAN, 0);
    M62364_SetSingleChannel(TONE_OUT_CHAN, 0);
    M62364_SetSingleChannel(8, 0);
}

//Press and hold the squelch button to enter the tone of normal squelch mode
void Start_ToneSql0(void) {
////////////////////////////////////////////////////////////////////////////////////////////////////////
    M62364_SetSingleChannel(A20_LINE_CHAN, 5);                  //Modify the DAC gain output "Di - Serial data input" (?)
    M62364_SetSingleChannel(8, 50);                             //toneout output is on (?)
//////////////////////////////////////////////////////////////////
                                                    //Configure DAC
    pinMode(18, OUTPUT);                            //Set the pin to output mode in order to start the DAC
    ESP_ERROR_CHECK(dac_output_enable(DAC_CHAN));
    //////////////////////////////////////////////////
    RingTone(TONE1_5K, ON);
    delay_ms(60);
    RingTone(TONE1_5K, OFF);
//////////////////////////////////////////////////////
                                                    //End DAC reconfigure A002
    dac_output_voltage(DAC_CHAN, 0);
    ESP_ERROR_CHECK(dac_output_disable(DAC_CHAN));
    pinMode(18, INPUT_PULLUP);                      //Pull high level to restore serial port 2 RX communication (?)
//////////////////////////////////////////////////////////////////
    M62364_SetSingleChannel(8, 0);                              //toneout output is off (?)
    M62364_SetSingleChannel(A20_LINE_CHAN, A20_LEVEL[VOLUME]);  //Restore the current gain size and output sound
////////////////////////////////////////////////////////////////////////////////////////////////////////
}

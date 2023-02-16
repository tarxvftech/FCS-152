#define FREERTOS_CONFIG_XTENSA_H
#include "main.h"  
#include "input.h"
#include "view.h"

void setup(){  
    CH423_Init();       //IIC initialization-->CH423 initialization-->CH423 pin initialization

    SPK_SWITCH(IN, OFF); //limit startup noise

    Key_Init();         //Initialize button: initialize encoder button; detect whether to enter BOOT mode
    Standby_Init();     //Confirm whether the encoder is a normal long press; long press to enable the 3.3V control pin ////PO WER_EN_SET;//

    UART1_Init();       //init serial port
    ADC_Init();         //检测电压使用 'detection voltage use', didn't translate well
    Timer_Init();       //Start timer processing ADC detection voltage program DAC output voltage timer interrupt

    //Initialize the control pin and set the state to avoid wrong IO state
    M62364_Init();      //m62364 initialization-->prohibit sound output/turn off FM power

    LCD_Init();
    PWM_Init();
    LCD_ShowPICALL(pic_HARRIS);
    Init_Storage();
    enterSecondSystem(); 

    EN_GPIO_Init();  //POWER_EN_8_SET;

    SineWave_Data();
    VFO_Load_Data();
    A002_Init();
    VFO_Clear();
    /*input_init();*/
    /*view_init();*/

}
void loop(){
    MY_GLOBAL_FUN();
    VFO_Refresh();
    Encoder_process(Encoder_Switch_Scan(0));
    Argument_process(Event_Matrix(Matrix_KEY_Scan(0)));	//matrix button event
    /*KDU_Processor();*/
    /*input_scan();*/
    /*ui_draw(&root);*/
}

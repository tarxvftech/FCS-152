#define FREERTOS_CONFIG_XTENSA_H
#include "main.h"  
#include "input.h"
#include "view.h"

void VFO_Refresh_Task(void * parameter){
    // A002_Init();
    // DAC_Init();

    delay_ms(1000);
    // Serial.printf("Create page refresh task\n");
    Serial.printf("创建写入任务成功\n");
    unsigned char a002_send_buff[20]="AT+DMOSETVOLUME=5\r\n";
    while(1){
        FeedDog();
        Serial1.begin(9600);
        for(int i=0;i<19;i++)
        {
            UART1_Put_Char(a002_send_buff[i]);
            UART2_Put_Char(a002_send_buff[i]);
        }
        delay_ms(1000);
        Serial1.end();
        // delay_ms(500);

        // Start_Tone(1);
        // delay_ms(500);
        // Start_Tone(0);
        // delay_ms(1000);
        //VFO_Refresh();
    }
}
extern void menuUpdate(void);
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
    enterSecondSystem(); //menuUpdate();

    EN_GPIO_Init();  //POWER_EN_8_SET;

    SineWave_Data();
    VFO_Load_Data();
    A002_Init();
    VFO_Clear();
    input_init();
    view_init();

}
void loop(){
    MY_GLOBAL_FUN();
    /*VFO_Refresh();*/
    Encoder_process(Encoder_Switch_Scan(0));
    /*Argument_process(Event_Matrix(Matrix_KEY_Scan(0)));	//matrix button event*/
    /*KDU_Processor();*/
    /*input_scan_task();*/
    ui_draw(&root);
}

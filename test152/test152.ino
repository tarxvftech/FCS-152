#define FREERTOS_CONFIG_XTENSA_H
#include "main.h"  

void VFO_Refresh_Task(void * parameter)
{
    // A002_Init();
    // DAC_Init();

     delay_ms(1000);
    // Serial.printf("创建页面刷新任务\n");
    Serial.printf("创建写入任务成功\n");
    unsigned char a002_send_buff[20]="AT+DMOSETVOLUME=5\r\n";
    while(1)
    {
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
void setup()
{  
    CH423_Init();       //IIC初始化-->CH423初始化-->CH423控制的引脚初始化

    SPK_SWITCH(IN, OFF); //限制开机的杂音

    Key_Init();         //初始化按键:初始化编码器按键; 检测是否需要进入BOOT模式
    Standby_Init();     //确认编码器是否为正常长按;长按则使能3.3V控制引脚  ////POWER_EN_SET;// 

    UART1_Init();       //初始化串口
    ADC_Init();         //检测电压使用
    Timer_Init();       //启动定时器处理 ADC检测电压程序 DAC输出电压定时器中断

    //将控制引脚初始化,并设置状态, 避免出现错误IO状态
    M62364_Init();      //m62364初始化-->禁止声音输出/关闭FM电源

    LCD_Init();
    PWM_Init();
    LCD_ShowPICALL(pic_HARRIS);
    Init_Storage();
    enterSecondSystem(); //menuUpdate();    //

    EN_GPIO_Init();  //POWER_EN_8_SET;//

    SineWave_Data();
    VFO_Load_Data();
    A002_Init();
    VFO_Clear();

}
void loop()
{
    MY_GLOBAL_FUN();
    VFO_Refresh();
    Encoder_process(Encoder_Switch_Scan(0));
    Argument_process(Event_Matrix(Matrix_KEY_Scan(0)));	//矩阵按键事件处理
    KDU_Processor();//KDU处理
    
}






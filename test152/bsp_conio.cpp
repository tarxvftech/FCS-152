#include "bsp_conio.h"
#include "bsp_uart.h"  

void ControlGPIO_Init(void)
{
    pinMode(POWER_EN_PIN, OUTPUT);
    POWER_EN_CLR;
}

void bsp_A002_Init(void)
{
    pinMode(18, INPUT);
    bsp_UART2_Init(9600);

    pinMode(A002_SQ_PIN,  INPUT_PULLUP);
    pinMode(A002_PD_PIN,  OUTPUT);
    pinMode(A002_PTT_PIN, OUTPUT);

    A002_PD_CLR;
    A002_PTT_SET;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int L_LAST=0, R_LAST=0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;  //声明一个portMUX_TYPE类型的变量，利用其对主代码和中断之间的同步进行处理
void Encoder_Click_Init(void)
{
    pinMode(ENCODER_CLICK_PIN, INPUT_PULLUP);
}

void EncoderPinInterrupt();
void Encoder_Spin_init(void)
{
    pinMode(ENCODER_SPIN_R_PIN, INPUT_PULLUP);   
    pinMode(ENCODER_SPIN_L_PIN, INPUT_PULLUP);  //这个我们设置为下拉 INPUT_PULLDOWN
    //我们通过调用attachInterrupt函数将中断附加到引脚
    //handleInterrupt 是中断触发后的触发函数

    attachInterrupt(digitalPinToInterrupt(ENCODER_SPIN_L_PIN), EncoderPinInterrupt, CHANGE);
    //attachInterrupt(digitalPinToInterrupt(ENCODER_SPIN_R_PIN), EncoderPinInterrupt, CHANGE); //add when time to fix the encoder (fix all input handling first lol)

}
extern void disposeEncoderSpined(void);
void EncoderPinInterrupt()
{
    // portENTER_CRITICAL_ISR(&mux);
    
    disposeEncoderSpined();
    
    // portEXIT_CRITICAL_ISR(&mux);
}

void Function_Key_Init(void)
{
    pinMode(VOL_ADD_PIN, INPUT_PULLUP);
    pinMode(VOL_SUB_PIN, INPUT_PULLUP);
    pinMode(PTT_PIN,     INPUT_PULLUP);
    pinMode(SQUELCH_PIN, INPUT_PULLUP);
}




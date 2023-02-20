#include "bsp_conio.h"
#include "bsp_uart.h"

void ControlGPIO_Init(void) {
    pinMode(POWER_EN_PIN, OUTPUT);
    POWER_EN_CLR;
}

void bsp_A002_Init(void) {
    pinMode(18, INPUT); // what is this? compare against analog.cpp.
    bsp_UART2_Init(9600); //AT commands get sent over this to control frequency and such.

    pinMode(A002_SQ_PIN,  INPUT_PULLUP);
    pinMode(A002_PD_PIN,  OUTPUT);
    pinMode(A002_PTT_PIN, OUTPUT);

    A002_PD_CLR;
    A002_PTT_SET;
}

int L_LAST=0, R_LAST=0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;  //Declare a variable of type portMUX_TYPE and use it to process the synchronization between the main code and the interrupt
void Encoder_Click_Init(void) {
    pinMode(ENCODER_CLICK_PIN, INPUT_PULLUP);
}

void EncoderPinInterrupt();
void Encoder_Spin_init(void) {
    pinMode(ENCODER_SPIN_R_PIN, INPUT_PULLUP);  //xvf: is this accurate? why would we pull it up?
    pinMode(ENCODER_SPIN_L_PIN, INPUT_PULLUP);  //We set this as a drop-down INPUT_PULLDOWN (?)
    //We attach the interrupt to the pin by calling the attachInterrupt function
    //handleInterrupt Is the trigger function after the interrupt is triggered

    attachInterrupt(digitalPinToInterrupt(ENCODER_SPIN_L_PIN), EncoderPinInterrupt, CHANGE);
    //attachInterrupt(digitalPinToInterrupt(ENCODER_SPIN_R_PIN), EncoderPinInterrupt, CHANGE); //add when time to fix the encoder (fix all input handling first lol)

}
extern void disposeEncoderSpined(void);
void EncoderPinInterrupt() {
    // portENTER_CRITICAL_ISR(&mux);
    disposeEncoderSpined();
    // portEXIT_CRITICAL_ISR(&mux);
}

void Function_Key_Init(void) {
    pinMode(VOL_ADD_PIN, INPUT_PULLUP);
    pinMode(VOL_SUB_PIN, INPUT_PULLUP);
    pinMode(PTT_PIN,     INPUT_PULLUP);
    pinMode(SQUELCH_PIN, INPUT_PULLUP);
}

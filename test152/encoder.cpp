#include "encoder.h"
#include "bsp_delay.h"
#include "bsp_conio.h"

volatile int TIMES = 0; //Encoder operand value, positive value for addition, negative value for subtraction
volatile u8 key_timer_cnt1 = 0;
volatile u8 key_timer_cnt2 = 0;
volatile int spin_cal = 0;
volatile bool encoder_rotated = false;
static volatile int spin_old = 0;

extern void ClearShut(void);
extern int L_LAST, R_LAST;


void Encoder_Init() {
    Encoder_Click_Init();
    Encoder_Spin_init();

    L_LAST = ENCODER_SPIN_L_READ;
    R_LAST = ENCODER_SPIN_R_READ;
}

int EncoderClickValidate(void) {
    return (ENCODER_CLICK_READ==0);
}
static unsigned char key_driver(void) {
    static unsigned char key_state_buffer1 = key_state_0;

    unsigned char key_return = key_idle;
    unsigned char key = 0;

    if(ENCODER_CLICK_READ==0) {
        delay_ms(5);
        if(ENCODER_CLICK_READ==0) {
            key=1;
        }
    }

    switch(key_state_buffer1) {
    case key_state_0:
        if(key) {
            key_state_buffer1 = key_state_1;
        }
        //The button is pressed, and the state transitions to the button debounce and confirmation state
        break;

    case key_state_1:
        if(key) {
            key_timer_cnt1 = 0;
            key_state_buffer1 = key_state_2;
            //the button is still pressed
            //debounce is completed, key_timer starts to prepare for timing
            //the state is switched to the state of pressing the time and timing
        } else {
            key_state_buffer1 = key_state_0;
            //button released, back to initial state
        }
        break;  //end of software debounce

    case key_state_2:
        if(!key) {
            key_return = key_click;  //button released, so it's a click
            key_state_buffer1 = key_state_0;  //back to initial state
        } else if(key && key_timer_cnt1 >= 15) {
            //continue to press the button, timing exceeds 1000ms
            ClearShut();
            key_return = key_long;  //send the long press event
            key_state_buffer1 = key_state_3;  //transition to wait for key release state
        }
        break;

    case key_state_3:  //wait for key release
        if(!key) { //key release
            key_state_buffer1 = key_state_0;  //back to initial state
        }
        break;
    }
    return key_return;
}

/***************************************************************************
    Function function: the middle layer button processing function, call the bottom layer function once, handle the judgment of the double click event,
                                        Return to the correct four states of no key, single click, double click and long press on the upper layer
    This function is called by the upper loop with an interval of 10ms
***************************************************************************/
u8 Encoder_Switch_Scan(u8 mode) {
    static unsigned char key_state_buffer2 = key_state_0;

    unsigned char key_return = key_idle;
    unsigned char key;
    //D_printf("A:%d, B:%d\r\n", ENCODER_SPIN_L_READ, ENCODER_SPIN_R_READ);

    key = key_driver();

    switch(key_state_buffer2) {
    case key_state_0:
        if(key == key_click) {
            key_timer_cnt2 = 0;  //Click for the first time, do not return, go to the next state to judge whether there will be double-click
            key_state_buffer2 = key_state_1;
            ClearShut();
        } else {
            key_return = key;  //For no key and long press, return to the original event
        }
        break;

    case key_state_1:
        if(key == key_click)  {
            //another click within 500ms
            key_return = key_double;  //return to the double click event
            key_state_buffer2 = key_state_0; //and return to initial state
        } else if(key_timer_cnt2 > 3) {  //4
            //Here, all the keyless events must be read within 500ms, because the long press is greater than 1000ms
            //Before 1s, the bottom layer returns no key

            key_return = key_click;  //If there is no click event again within 500ms, return to the click event
            key_state_buffer2 = key_state_0;  //return to initial state
        }
        break;
    }

    return key_return;
}

void disposeEncoderSpined(void) {
    encoder_rotated = true;
    //TODO:implement https://www.pinteric.com/rotary.html
    //to handle this well requires replacing all uses of TIMES. oh well.
    //delete the rest of this function after all input handling is fixed

    if(L_LAST != ENCODER_SPIN_L_READ && spin_cal - spin_old > 100) {
        ClearShut();
        L_LAST = ENCODER_SPIN_L_READ;
        TIMES = (ENCODER_SPIN_L_READ != ENCODER_SPIN_R_READ ? 1 : -1);
        spin_old = spin_cal;
    }
}

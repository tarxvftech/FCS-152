#include "key.h"
#include "encoder.h"
#include "bsp_delay.h"

#include "bsp_conio.h"
#include "bsp_MatrixKeyBoard.h"

void Key_Init(void) {
    // Encoder_Click_Init();   //Encoder press initialize
    // Encoder_Spin_init();    //Encoder rotation initialization
    Encoder_Init();
    Function_Key_Init();       //Independent function button initialization

    bsp_Matrix_Init();         //Matrix keyboard initialization

}


extern void ClearShut(void);
unsigned char VolumeKeyScan(unsigned char mode) { //Volume up and down button scan
    static unsigned char key_up=1;
    if (mode) {
        key_up=1;
    }
    if (key_up&&(VOL_ADD_READ==0||VOL_SUB_READ==0)) {
        ClearShut();
        delay_ms(10);
        key_up=0;
        if (VOL_ADD_READ==0) {
            return 1;
        } else if (VOL_SUB_READ==0) {
            return 2;
        }
    } else if (VOL_ADD_READ==1&&VOL_SUB_READ==1) {
        key_up=1;
    }
    return 0;
}
/////////////////////////////////






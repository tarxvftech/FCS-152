#include "key.h"
#include "encoder.h"
#include "bsp_delay.h"

#include "bsp_conio.h"
#include "bsp_MatrixKeyBoard.h"

void Key_Init(void)
{
    // Encoder_Click_Init();   //编码器按下初始化
    // Encoder_Spin_init();    //编码器旋转初始化
    Encoder_Init();
    Function_Key_Init();    //独立功能按键初始化

    bsp_Matrix_Init();      //矩阵键盘初始化

}


extern void ClearShut(void);
unsigned char VolumeKeyScan(unsigned char mode) //音量加减按键扫描
{
    static unsigned char key_up=1;
    if(mode) {
        key_up=1;
    }
    if(key_up&&(VOL_ADD_READ==0||VOL_SUB_READ==0)) {
        ClearShut();
        delay_ms(10);
        key_up=0;
        if(VOL_ADD_READ==0) {
            return 1;
        } else if(VOL_SUB_READ==0) {
            return 2;
        }
    } else if(VOL_ADD_READ==1&&VOL_SUB_READ==1) {
        key_up=1;
    }
    return 0;
}
/////////////////////////////////






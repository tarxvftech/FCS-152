#include "controller.h"
#include "bsp_uart.h"
#include "bsp_analog.h"
#include "analog.h"
#include "bsp_m62364.h"

extern void SHUT(void);
extern uint8_t SQL;
void EN_GPIO_Init(void) {       //使能引脚初始化
    switch(Select_Power()) {
    case VOLTAGE_NORMAL:
        D_printf("Normal voltage startup\n");
        return;

    case VOLTAGE_ERROR:
        D_printf("电压错误\n");
        LCD_Clear(GLOBAL32);
        LCD_ShowString0608(25, 2,  "Wrong Power", 1, 128);
        delay_ms(2000);
        break;

    default:
        break;

    }
    SHUT();
}
void A002_Init(void) {          //A20控制引脚初始化
    bsp_A002_Init();
    A002_PD_SET;
    delay_ms(2);

    A002_PTT_SET;
    delay_ms(2);

    unsigned char a002_send_buff[20]="AT+DMOSETVOLUME=5\r\n";
    for(int i=0; i<19; i++) {
        UART2_Put_Char(a002_send_buff[i]);
    }

    Set_A20(chan_arv[NOW], SQL);
    // D_printf("A002 Transmition Moudle Initial Successfully!\n");

}


u8 Select_Power(void) {
    // for(u8 i=0; i<5; i++)
    //     Use_ADC();
    uint32_t adc_val = Use_ADC();
    Serial.printf("\n当前电压:%d\n", adc_val);

    if(adc_val>VOLTAGE_ON_8_FLOOR && adc_val<VOLTAGE_ON_8_UPPER) {
        POWER_EN_12_SET;
        delay_ms(100);
        POWER_EN_12_CLR;//硬件错误导致:8V电源电流被12V电源电路瞬间拉低导致电源关
        POWER_EN_8_SET;
        POWER_SELECT_FLAG=0;
        return VOLTAGE_NORMAL;
    } else if(adc_val>VOLTAGE_ON_12_FLOOR && adc_val<VOLTAGE_ON_12_UPPER) {
        POWER_EN_12_SET;
        POWER_SELECT_FLAG=1;
        return VOLTAGE_NORMAL;
    } else {
        return VOLTAGE_ERROR;
    }
}


extern u8 MIC_LEVEL[3], MIC;

void MIC_SWITCH(char mic_temp, char on_off) {
    D_printf("\nMIC:%d, sta:%d\n", mic_temp, on_off);
    //开运放
    //设置增益
    if(on_off) {
        switch(mic_temp) {
        case IN:            //内mic
            MIC_IN_SET;
            M62364_SetSingleChannel(MIC_IN_CHAN, MIC_LEVEL[1]);
            break;


        case TOP:           //顶mic
        case SIDE:          //侧mic
            MIC_OUT_SET;
            M62364_SetSingleChannel(MIC_OUT_CHAN, MIC_LEVEL[MIC]);
            break;
        }
    } else {
        //关运放
        MIC_IN_CLR;
        MIC_OUT_CLR;
        //关矩阵增益
        M62364_SetSingleChannel(MIC_IN_CHAN,  0);
        M62364_SetSingleChannel(MIC_OUT_CHAN, 0);
    }
}
//
void SPK_SWITCH(char spk_temp, char on_off) {
    D_printf("\nSPK:%d, sta:%d\n", spk_temp, on_off);
    if(on_off) {
        //开功放
        switch(spk_temp) {
        case IN:            //内spk
            SPK_IN_SET;
            break;

        case TOP:           //顶spk
        case SIDE:          //边spk
            SPK_OUT_SET;
            break;
        }
    } else {
        //关功放
        SPK_IN_CLR;
        SPK_OUT_CLR;
    }
    delay_ms(1);
}
void VDO_SWITCH(unsigned char on_off) { //6针头电源输出
    if(on_off) {
        VDO_SET;
        return;
    }
    VDO_CLR;
}
//



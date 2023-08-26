#include "bsp_analog.h"
#include "bsp_conio.h"

#define PWM_CHAN        1
#define ADC_SAMPLE_NUM  10
volatile uint16_t MY_ADC_VAL[ADC_SAMPLE_NUM]; // ADC value buffer

void bsp_PWM_Init(void) {
    ledcSetup(PWM_CHAN, 24000, 8);  //0~255
    ledcAttachPin(PWM_PIN, PWM_CHAN);
    ledcWrite(PWM_CHAN, 0);
}
void bsp_ADC_Init(void) {
    pinMode(ADC_PIN,INPUT);
    adcAttachPin(ADC_PIN);//Connect pins to ADC
    analogReadResolution(13);//Set the resolution of the value returned by analogRead
}

void BackLight_SetVal(u8 val) {
    val = val*255/100;
    ledcWrite(PWM_CHAN, val);
}

void refreshADCVal(void) {
    static int i=0;
    MY_ADC_VAL[i++] = analogRead(ADC_PIN);
    i%=10;
    //MY_ADC_VAL[i] = analogRead(ADC_PIN);
    //D_printf("MY_ADC_VAL[%d]: %d\n", i, MY_ADC_VAL[i]);
    //i = (i+1) % 10;
    MY_ADC_VAL[i++] = analogRead(ADC_PIN);
    i%=10;
}

uint32_t Use_ADC(void) {
    uint32_t real_val = MY_ADC_VAL[0],
             v0       = MY_ADC_VAL[0],
             max_v0   = MY_ADC_VAL[0],
             min_v0   = MY_ADC_VAL[0];

    for (int i=1; i<ADC_SAMPLE_NUM; i++) {
        max_v0 = (max_v0 > MY_ADC_VAL[i]) ? max_v0 : MY_ADC_VAL[i];
        min_v0 = (min_v0 < MY_ADC_VAL[i]) ? min_v0 : MY_ADC_VAL[i];
        v0  += MY_ADC_VAL[i];
    }
    //memset((void *)MY_ADC_VAL, 0, ADC_SAMPLE_NUM);
    v0   = (v0   - max_v0  - min_v0) /(ADC_SAMPLE_NUM-2);

    real_val = v0 * 2563/8191 *321/51;
    return real_val;
}

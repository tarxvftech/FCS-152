#include "bsp_dac.h"



volatile int ToneState = OFF;
ToneClass    Tone = TONE2K;
static volatile int i1=0, i2=0;
int SineWave_Value_2K[DAC_SAMPLE_2K];         //dac的正弦波数据   开始两次2K
int SineWave_Value_nK[DAC_SAMPLE_1_5K];       //dac的正弦波数据   结束一次1.5K

static int test_val[200]= {
    0x7F,0x8A,0x96,0xA1,0xAC,0xB6,0xC0,0xCA,0xD3,0xDB,0xE3,0xE9,0xEF,0xF4,0xF8,0xFB
    ,0xFD,0xFE,0xFE,0xFD,0xFB,0xF8,0xF4,0xEF,0xE9,0xE3,0xDB,0xD3,0xCA,0xC0,0xB6,0xAC
    ,0xA1,0x96,0x8A,0x7F,0x74,0x68,0x5D,0x52,0x48,0x3E,0x34,0x2B,0x23,0x1B,0x15,0x0F
    ,0x0A,0x06,0x03,0x01,0x00,0x00,0x01,0x03,0x06,0x0A,0x0F,0x15,0x1B,0x23,0x2B,0x34
    ,0x3E,0x48,0x52,0x5D,0x68,0x74
};

void prepare_data(int array[], int pnt_num)
{
    for(int i = 0; i < pnt_num; i ++) {
        array[i] = (int)((sin(i * CONST_PERIOD_2_PI / pnt_num) + 1) * (double)(AMP_DAC)/2 + 0.5); //2 * PI
        // array[i] =(int) ((1.5*sin(( 2.0*i/pnt_num)*CONST_PERIOD_2_PI)+1.5)*255/3.3);
        // D_printf("array[%d]:%d\n", i, array[i]);
    }
    D_printf("\n");
}
//DAC-DMA正弦波数据表
void SineWave_Data()
{
    // u16 i;
    // for(i=0; i<DAC_SAMPLE; i++)
    //     //SineWave_Value_2K[i] =(u16) ((1.5*sin(( 2.0*i/(DAC_SAMPLE-1))*2*PI)+1.5)*128/3.3);  //1.5  4095
    //     SineWave_Value_2K[i] = (int)((     sin(  2  *i * PI / DAC_SAMPLE)   + 1 )* (double)(255.0)/2 + 0.5);
    // for(i=0; i<667; i++)
    //     SineWave_Value_nK[i] =(u16) ((1.5*sin(( 1.5*i/666           )*2*PI)+1.5)*128/3.3);
    prepare_data(SineWave_Value_2K, DAC_SAMPLE_2K);
    prepare_data(SineWave_Value_nK, DAC_SAMPLE_1_5K);
}
////////////////////////////////////////////////////////////////////////////////////////
extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
void bsp_DAC_Init(void)
{
    // gpio_num_t gpio_num;
    // dac_pad_get_io_num(DAC_CHAN, &gpio_num);
    // D_printf("DACIO:%d\n", gpio_num);

    ESP_ERROR_CHECK(dac_output_enable(DAC_CHAN));
    // dac_output_voltage(DAC_CHAN, 0);
    ESP_ERROR_CHECK(dac_output_disable(DAC_CHAN));
}

void RingTone(ToneClass tone, int state)
{
    if(state) {
        // A002_PD_CLR;
        // Serial1.end();
        // pinMode(18, OUTPUT);
        // ESP_ERROR_CHECK( dac_output_enable(DAC_CHAN) );
        timer_start(TIMER_GROUP_0, TIMER_0);
        Tone      = tone;
        ToneState = ON;
    } else {
        ToneState = OFF;
        timer_pause(TIMER_GROUP_0, TIMER_0);

        // ESP_ERROR_CHECK( dac_output_disable(DAC_CHAN) );
        // A002_Init();
    }

}

void RingArray_Intr(void)
{
    if(ToneState) {
        switch(Tone) {
        case TONE2K:
            if(i1 >= DAC_SAMPLE_2K) {
                i1 = 0;
            }
            dac_output_voltage(DAC_CHAN, *(SineWave_Value_2K + i1));
            //D_printf("wave[%d]:%d\n", i1, SineWave_Value_2K[i1]);
            // dac_output_voltage(DAC_CHAN, i1%2?255:0);
            // dac_output_voltage(DAC_CHAN, i1);
            // dac_output_voltage(DAC_CHAN, *(test_val + i1));
            i1++;
            break;

        case TONE1_5K:
            if(i2 >= DAC_SAMPLE_1_5K) {
                i2 = 0;
            }
            dac_output_voltage(DAC_CHAN, *(SineWave_Value_nK + i2));
            i2++;
            break;

        default:
            break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////


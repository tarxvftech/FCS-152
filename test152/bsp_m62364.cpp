#include "bsp_m62364.h"
#include "bsp_delay.h"

void bsp_M62364_Init(void)
{
    pinMode(M62364_LD_PIN,   OUTPUT);
    pinMode(M62364_CLK_PIN,  OUTPUT);
    pinMode(M62364_DATA_PIN, OUTPUT);

    M62364_LD_SET;
    M62364_CLK_SET;
    M62364_DATA_SET;
}
void M62364Reset(void)
{
    M62364_SetSingleChannel(TONE_OUT_CHAN, 0);
    M62364_SetSingleChannel(FM_S_EN_CHAN,  0);
    M62364_SetSingleChannel(MIC_OUT_CHAN,  0);
    M62364_SetSingleChannel(MIC_IN_CHAN,   0);
    M62364_SetSingleChannel(WFM_LINE_CHAN, 0);
    M62364_SetSingleChannel(A20_LINE_CHAN, 0);
    M62364_SetSingleChannel(8, 0);

}
void M62364_Init(void)
{
    bsp_M62364_Init();
}

void M62364_sendData(short iSdata)
{
    unsigned char	iIndex;

    M62364_LD_CLR;
    delay_us(10);
    for(iIndex = 0; iIndex < 12; iIndex++)
    {
        M62364_CLK_CLR;
        
        if(iSdata << iIndex & 0x8000)
            M62364_DATA_SET;
        else 
            M62364_DATA_CLR;
        delay_us(10);
        M62364_CLK_SET;
        delay_us(10);
    }	
    M62364_LD_SET;
    delay_us(10);
    M62364_DATA_SET;
}
	
void M62364_SetSingleChannel(unsigned char chan, unsigned char data)
{
    D_printf("\n@@@@@chan:%d, value:%03d@@@@@\n", chan, data);
    unsigned short data_send=0;
    if(0<chan && chan<9)
    {
        data_send = ((chan & 0x0a) >> 1) |((chan & 0x05) << 1);
        data_send = ((data_send & 0x03) << 2)|((data_send & 0x0c) >>2);
        data_send = ((data_send << 8) | data) << 4;
        M62364_sendData(data_send);	
    }
}
	


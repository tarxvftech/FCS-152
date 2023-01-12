#include "bsp_ch423.h"
#include "bsp_iic.h"
#include "bsp_delay.h"

static unsigned char SaveCurrentIO  = 0x00;
static unsigned char SaveCurrentOCL = 0x00;
static unsigned char SaveCurrentOCH = 0x00;

void CH423_Init()
{
    bsp_IIC_Init(); //IIC 引脚初始化
    //置低电平
    CH423_WriteByte( CH423_OC_L_CMD);
    CLR_POWER_EN_8_CHAN;
    CLR_POWER_EN_12_CHAN;
    
    CH423_WriteByte( CH423_SYS_CMD | BIT_IO_OE );
    //置高电平  
    CH423_WriteByte( CH423_SET_IO_CMD | BIT_IO0_DAT | BIT_IO1_DAT | BIT_IO7_DAT);
    CH423_WriteByte( CH423_OC_H_CMD   | BIT_OC8_L_DAT);
}

void CH423_Write( unsigned short cmd )    // 写命令
{
    IIC_Start();    // 启动总线
    IIC_Send_Byte( ( ( unsigned char )( cmd>>7 ) & CH423_I2C_MASK ) | CH423_I2C_ADDR1 );
    IIC_Wait_Ack();
    IIC_Send_Byte( ( unsigned char ) cmd );    // 发送数据
    IIC_Wait_Ack();
    IIC_Stop();    // 结束总线 
}

void CH423_WriteByte( unsigned short cmd )    // 写出数据
{
    IIC_Start();    // 启动总线
    IIC_Send_Byte( ( unsigned char )( cmd>>8 ) );
    IIC_Wait_Ack();
    IIC_Send_Byte( ( unsigned char ) cmd );    // 发送数据
    IIC_Wait_Ack();
    IIC_Stop();    // 结束总线  
}

unsigned char CH423_ReadByte()    // 读取数据
{
    unsigned char din=0;
    IIC_Start();    // 启动总线
    IIC_Send_Byte( CH423_RD_IO_CMD );    // 此值为0x4D
    IIC_Wait_Ack();
    din=IIC_Read_Byte(0);               // 读取数据
    IIC_Stop();                         // 结束总线
    return( din );
}

/////////////////////////////////////////////////////////////////////////////
void SetIOChannel(unsigned char IOChannel)
{
    if ( IOChannel > 7 ) return;
    SaveCurrentIO |= ( 0x01 <<IOChannel);
    CH423_WriteByte( CH423_SET_IO_CMD | SaveCurrentIO );
}

void ClrIOChannel(unsigned char IOChannel)
{
    if ( IOChannel > 7 ) return;
    SaveCurrentIO &= ( ~(0x01<<IOChannel) ) ;
    CH423_WriteByte( CH423_SET_IO_CMD | SaveCurrentIO );
}

void SetOCChannel(unsigned char OCChannel)
{
    if ( OCChannel > 15 ) return;
    if ( OCChannel > 7  )
    {
        SaveCurrentOCH |= ( 0x01 << (OCChannel-8) );
        CH423_WriteByte( CH423_OC_H_CMD | SaveCurrentOCH );
    }
    else
    {
        SaveCurrentOCL |= ( 0x01 << OCChannel);
        CH423_WriteByte( CH423_OC_L_CMD | SaveCurrentOCL );
    }

}

void ClrOCChannel(unsigned char OCChannel)
{
    if ( OCChannel > 15 ) return;
    if ( OCChannel > 7  )
    {
        SaveCurrentOCH &= ( ~( 0x01 << (OCChannel-8) ));
        CH423_WriteByte( CH423_OC_H_CMD | SaveCurrentOCH );
    }
    else
    {
        SaveCurrentOCL &= ( ~( 0x01 << OCChannel) );
        CH423_WriteByte( CH423_OC_L_CMD | SaveCurrentOCL );
    }
}






















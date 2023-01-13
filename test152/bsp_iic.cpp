#include "bsp_iic.h"
#include "bsp_delay.h"

#define I2C_DELAY 1

//IIC初始化
void bsp_IIC_Init(void) {
    pinMode(SDA_PIN, OUTPUT);
    pinMode(SCL_PIN, OUTPUT);
    IIC_SDA1;
    IIC_SCL1;
}

//产生IIC起始信号
void IIC_Start(void) {
    SDA_OUT();     //sda线输出
    IIC_SDA1;
    IIC_SCL1;
    delay_us(I2C_DELAY);
    IIC_SDA0;//START:when CLK is high,DATA change form high to low
    delay_us(I2C_DELAY);
    IIC_SCL0;//钳住I2C总线，准备发送或接收数据
}
//产生IIC停止信号
void IIC_Stop(void) {
    SDA_OUT();//sda线输出
    IIC_SCL0;
    IIC_SDA0;//STOP:when CLK is high DATA change form low to high
    delay_us(I2C_DELAY);
    IIC_SCL1;
    delay_us(I2C_DELAY);
    IIC_SDA1;//发送I2C总线结束信号
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void) {
    uint8_t ucErrTime=0;
    SDA_IN();      //SDA设置为输入
    IIC_SDA1;
    delay_us(I2C_DELAY);
    IIC_SCL1;
    delay_us(I2C_DELAY);
    while (READ_SDA) {
        ucErrTime++;
        if (ucErrTime>250) {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL0;//时钟输出0
    return 0;
}
//产生ACK应答
void IIC_Ack(void) {
    IIC_SCL0;
    SDA_OUT();
    IIC_SDA0;
    delay_us(I2C_DELAY);
    IIC_SCL1;
    delay_us(I2C_DELAY);
    IIC_SCL0;
}
//不产生ACK应答
void IIC_NAck(void) {
    IIC_SCL0;
    SDA_OUT();
    IIC_SDA1;
    delay_us(I2C_DELAY);
    IIC_SCL1;
    delay_us(I2C_DELAY);
    IIC_SCL0;
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC_Send_Byte(uint8_t txd) {
    uint8_t t;
    SDA_OUT();
    IIC_SCL0;//拉低时钟开始数据传输
    for (t=0; t<8; t++) {
        //IIC_SDA=(txd&0x80)>>7;
        if ((txd&0x80)>>7) {
            IIC_SDA1;
        } else {
            IIC_SDA0;
        }
        txd<<=1;
        delay_us(I2C_DELAY);   //对TEA5767这三个延时都是必须的
        IIC_SCL1;
        delay_us(I2C_DELAY);
        IIC_SCL0;
        delay_us(I2C_DELAY);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
uint8_t IIC_Read_Byte(uint8_t ack) {
    uint8_t i,receive=0;
    SDA_IN();//SDA设置为输入
    for (i=0; i<8; i++) {
        IIC_SCL0;
        delay_us(I2C_DELAY);
        IIC_SCL1;
        receive<<=1;
        if (READ_SDA) {
            receive++;
        }
        delay_us(I2C_DELAY);
    }
    if (!ack) {
        IIC_NAck();    //发送nACK
    } else {
        IIC_Ack();    //发送ACK
    }
    return receive;
}



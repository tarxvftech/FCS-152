#ifndef _BSP_IIC_H
#define _BSP_IIC_H
#include "FCS152_KDU.h"

#define SCL_PIN             GPIO_NUM_9
#define SDA_PIN             GPIO_NUM_8

//F103： CRL：0-7  CRH：8-15
//0011：Maximum speed output  1000：  10（random）00：input mode
#define SDA_IN()            {digitalWrite(SDA_PIN, HIGH); pinMode(SDA_PIN, INPUT);}
#define SDA_OUT()           pinMode(SDA_PIN, OUTPUT)

//IO operation
#define IIC_SCL0            digitalWrite(SCL_PIN, LOW)
#define IIC_SCL1            digitalWrite(SCL_PIN, HIGH)
#define IIC_SDA0            digitalWrite(SDA_PIN, LOW)
#define IIC_SDA1            digitalWrite(SDA_PIN, HIGH)
#define READ_SDA            digitalRead(SDA_PIN)    //Read the input SDA

//I2C All operation functions
void bsp_IIC_Init(void);                    //Initialize I2C's IO port
void IIC_Start(void);                       //Send the begin signal of I2C
void IIC_Stop(void);                        //Send the stop  signal of I2C
void IIC_Send_Byte(uint8_t txd);            //I2C send a byte
uint8_t IIC_Read_Byte(unsigned char ack);   //I2C read a byte
uint8_t IIC_Wait_Ack(void);                 //I2C wait for ACK
void IIC_Ack(void);                         //I2C send an  ACK
void IIC_NAck(void);                        //I2C send an  NACK


#endif


#ifndef _BSP_IIC_H
#define _BSP_IIC_H
#include "FCS152_KDU.h" 

#define SCL_PIN        		GPIO_NUM_9 
#define SDA_PIN        		GPIO_NUM_8 

//F103： CRL：0-7  CRH：8-15
//0011：最大速度输出  1000：  10（随意）00：输入模式
#define SDA_IN()            {digitalWrite(SDA_PIN, HIGH); pinMode(SDA_PIN, INPUT);}
#define SDA_OUT()           pinMode(SDA_PIN, OUTPUT) 

//IO操作
#define IIC_SCL0	        digitalWrite(SCL_PIN, LOW) 
#define IIC_SCL1	        digitalWrite(SCL_PIN, HIGH) 
#define IIC_SDA0	        digitalWrite(SDA_PIN, LOW) 
#define IIC_SDA1	        digitalWrite(SDA_PIN, HIGH) 
#define READ_SDA            digitalRead(SDA_PIN)  	//读取输入的SDA

//IIC所有操作函数
void bsp_IIC_Init(void);                    //Initialize I2C's IO port				 
void IIC_Start(void);                       //Send the begin signal of I2C
void IIC_Stop(void);                        //Send the stop  signal of I2C
void IIC_Send_Byte(uint8_t txd);            //IIC send a byte
uint8_t IIC_Read_Byte(unsigned char ack);   //IIC read a byte
uint8_t IIC_Wait_Ack(void);                 //IIC wait for ACK
void IIC_Ack(void);                         //IIC send an  ACK
void IIC_NAck(void);                        //IIC send an  NACK


#endif


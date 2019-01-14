#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"

// 6 scl 7 sda

#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF; GPIOB->CRL|=4<<28;}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF; GPIOB->CRL|=3<<28;}
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA 
#define READ_SDA   PBin(7)   

  
// 8 sda 9 scl
//IO
/*
#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}
#define IIC_SCL    PBout(9) 
#define IIC_SDA    PBout(8)
#define READ_SDA   PBin(8)
	*/
void IIC_Init(void); 
int IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(u8 txd);
u8 IIC_Read_Byte(unsigned char ack);
int IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
int i2cWrite(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len);
int i2cRead(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len);

#endif

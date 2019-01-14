#ifndef _I2C_H
#define _I2C_H
#include "stm32f10x.h"
#define PORT_I2C_SCL  GPIOB
#define PORT_I2C_SDA  GPIOB
#define PIN_I2C_SCL  GPIO_Pin_6
#define PIN_I2C_SDA  GPIO_Pin_7
#define SCL_L (PORT_I2C_SCL->BRR = PIN_I2C_SCL)
#define SCL_H (PORT_I2C_SCL->BSRR = PIN_I2C_SCL)
#define SDA_L (PORT_I2C_SDA->BRR = PIN_I2C_SDA)
#define SDA_H (PORT_I2C_SDA->BSRR = PIN_I2C_SDA)

#define SDA_read (PORT_I2C_SDA->IDR & PIN_I2C_SDA)

#define I2C_ACK   0
#define I2C_NOACK  1
#endif

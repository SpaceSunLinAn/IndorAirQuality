#include "stm32f10x.h"
#include "ec.h"
//=============================================================================== 
//GPIO 模拟I2C ，操作EP2ROM IC(AT24C02) 
#define  E2PROM_SDA  GPIO_Pin_7
#define  E2PROM_SCL  GPIO_Pin_6
#define  E2PROM_CMD_WRITE  0xa0
#define  E2PROM_CMD_READ   0xa1
#define        I2C_DELAY                    254                                                        // 

 

static void DelayMs(uint8_t uc)
{
        uint8_t        i, j;

        for (i=0; i<uc; i++) {
                for (j=0; j<I2C_DELAY; j++);
        }
}
/************************************************************* 
*函数名称：I2CStart 
*函数功能：I2C开始信号 
*输入参数： 
*输出参数： 
*备           注：时钟线高时，数据线由高到低的跳变，表示I2C开始信号 
**************************************************************/ 
void I2CStart( void ) 
{ 
  GPIO_SetBits( GPIOB, E2PROM_SDA ); 
  DelayMs(1); 
  GPIO_SetBits( GPIOB, E2PROM_SCL ); 
  DelayMs(1); 
  GPIO_ResetBits( GPIOB, E2PROM_SDA ); 
  DelayMs(1); 

  GPIO_ResetBits( GPIOB, E2PROM_SCL ); 
} 

/************************************************************* 
*函数名称：I2CStop 
*函数功能：I2C停止信号 
*输入参数： 
*输出参数： 
*备           注：时钟线高时，数据线由低到高的跳变，表示I2C停止信号 
**************************************************************/ 
void I2CStop( void ) 
{ 
  GPIO_ResetBits( GPIOB, E2PROM_SDA ); 
  DelayMs(1); 
  GPIO_SetBits( GPIOB, E2PROM_SCL ); 
  DelayMs(1); 
  GPIO_SetBits( GPIOB, E2PROM_SDA ); 
  DelayMs(1); 

  GPIO_ResetBits( GPIOB, E2PROM_SCL ); 
} 

/************************************************************* 
*函数名称：I2CSlaveAck 
*函数功能：I2C从机设备应答查询 
*输入参数： 
*输出参数： 
*备           注： 
**************************************************************/ 
unsigned char I2CSlaveAck( void ) 
{ 
  GPIO_InitTypeDef GPIO_InitStruct; 
  unsigned int TimeOut; 
  unsigned char RetValue; 

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  /*这里一定要设成输入上拉，否则不能读出数据*/ 
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStruct.GPIO_Pin = E2PROM_SDA; 
  GPIO_Init( GPIOB, &GPIO_InitStruct ); 

  GPIO_SetBits( GPIOB, E2PROM_SCL ); 
  TimeOut = 10000; 
  while( TimeOut-- > 0 ) 
  { 
    if( SET == GPIO_ReadInputDataBit( GPIOB, E2PROM_SDA ) ) 
    { 
      RetValue = RESET; 
      break; 
    } 
    else 
    { 
      RetValue = SET; 
    } 
  } 
  GPIO_ResetBits( GPIOB, E2PROM_SCL ); 
   
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init( GPIOB, &GPIO_InitStruct ); 
  return RetValue; 
} 

/************************************************************* 
*函数名称：I2CWriteByte 
*函数功能：I2C写一字节数据 
*输入参数： 
*输出参数： 
*备           注： 
**************************************************************/ 
void I2CWriteByte( unsigned char byte ) 
{ 
  unsigned char i; 

  for( i=0; i<8; i++ ) 
  { 
    if( 0X80 & byte ) 
      GPIO_SetBits( GPIOB, E2PROM_SDA ); 
    else 
      GPIO_ResetBits( GPIOB, E2PROM_SDA ); 
    byte <<= 1; 
    DelayMs(1); 

    GPIO_SetBits( GPIOB, E2PROM_SCL ); 
    DelayMs(1); 
    GPIO_ResetBits( GPIOB, E2PROM_SCL ); 
    DelayMs(1); 
  } 
} 

/************************************************************* 
*函数名称：I2CReadByte 
*函数功能：I2C读一字节数据 
*输入参数： 
*输出参数： 
*备           注： 
**************************************************************/ 
unsigned char I2CReadByte( void ) 
{ 
  unsigned char i; 
  unsigned char ReadValue = 0; 
  GPIO_InitTypeDef GPIO_InitStruct; 
  unsigned char bit; 

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  /*这里一定要设成输入上拉，否则不能读出数据*/ 
  GPIO_InitStruct.GPIO_Pin = E2PROM_SDA; 
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init( GPIOB, &GPIO_InitStruct ); 
  for( i=0; i<8; i++ ) 
  { 
    GPIO_SetBits( GPIOB, E2PROM_SCL ); 
    DelayMs(1); 
    if( SET == GPIO_ReadInputDataBit( GPIOB, E2PROM_SDA ) ) 
      bit = 0X01; 
    else 
      bit = 0x00; 
       
    ReadValue = (ReadValue<<1)|bit; 
    GPIO_ResetBits( GPIOB, E2PROM_SCL ); 
    DelayMs(1); 
  } 

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init( GPIOB, &GPIO_InitStruct ); 
  return ReadValue; 
} 

/************************************************************* 
*函数名称：E2promWriteByte 
*函数功能：E2PROM指定地址写一字节数据 
*输入参数：addr  E2PROM地址 
                             data  写入的数据 
*输出参数：SET: 写入正常；RESET:写入错误 
*备           注： 
**************************************************************/ 
uint8_t E2promWriteByte( uint16_t addr, uint8_t data ) 
{ 
  //asm("CPSID I");  //关中断 
  I2CStart(); 

  I2CWriteByte( E2PROM_CMD_WRITE ); 
  if( RESET == I2CSlaveAck() ) 
  { 
    return RESET; 
  }/*
  I2CWriteByte( (addr >> 8) & 0xFF ); 
  if( RESET == I2CSlaveAck() ) 
  { 
    return RESET; 
  } */
  I2CWriteByte( addr & 0xFF); 
  if( RESET == I2CSlaveAck() ) 
  { 
    return RESET; 
  } 
  I2CWriteByte( data ); 
  if( RESET == I2CSlaveAck() ) 
  { 
    return RESET; 
  } 
  I2CStop(); 
  //asm("CPSIE I");  //关中断 

  return SET; 
} 

/************************************************************* 
*函数名称：E2promReadByte 
*函数功能：E2PROM指定地址读一字节数据 
*输入参数：addr  E2PROM地址 
*输出参数：返回读出的数据 
*备           注： 
**************************************************************/  
uint8_t E2promReadByte( unsigned short int addr ) 
{ 
  unsigned char ReadValue; 

  I2CStart(); 

  I2CWriteByte( E2PROM_CMD_WRITE ); 
  if( RESET == I2CSlaveAck() ) 
  { 
    return RESET; 
  }/*
  I2CWriteByte( (addr >> 8) & 0xFF ); 
  if( RESET == I2CSlaveAck() ) 
  { 
    return RESET; 
  }  */
  I2CWriteByte( addr & 0xFF ); 
  if( RESET == I2CSlaveAck() ) 
  { 
    return RESET; 
  } 
  I2CStart(); 
  I2CWriteByte( E2PROM_CMD_READ ); 
  if( RESET == I2CSlaveAck() ) 
  { 
    return RESET; 
  } 
  ReadValue = I2CReadByte(); 
  I2CStop(); 

  return ReadValue;   
} 
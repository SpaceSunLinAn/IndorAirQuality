#include "stm32f10x.h"
#include "ec.h"
//=============================================================================== 
//GPIO ģ��I2C ������EP2ROM IC(AT24C02) 
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
*�������ƣ�I2CStart 
*�������ܣ�I2C��ʼ�ź� 
*��������� 
*��������� 
*��           ע��ʱ���߸�ʱ���������ɸߵ��͵����䣬��ʾI2C��ʼ�ź� 
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
*�������ƣ�I2CStop 
*�������ܣ�I2Cֹͣ�ź� 
*��������� 
*��������� 
*��           ע��ʱ���߸�ʱ���������ɵ͵��ߵ����䣬��ʾI2Cֹͣ�ź� 
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
*�������ƣ�I2CSlaveAck 
*�������ܣ�I2C�ӻ��豸Ӧ���ѯ 
*��������� 
*��������� 
*��           ע�� 
**************************************************************/ 
unsigned char I2CSlaveAck( void ) 
{ 
  GPIO_InitTypeDef GPIO_InitStruct; 
  unsigned int TimeOut; 
  unsigned char RetValue; 

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  /*����һ��Ҫ������������������ܶ�������*/ 
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
*�������ƣ�I2CWriteByte 
*�������ܣ�I2Cдһ�ֽ����� 
*��������� 
*��������� 
*��           ע�� 
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
*�������ƣ�I2CReadByte 
*�������ܣ�I2C��һ�ֽ����� 
*��������� 
*��������� 
*��           ע�� 
**************************************************************/ 
unsigned char I2CReadByte( void ) 
{ 
  unsigned char i; 
  unsigned char ReadValue = 0; 
  GPIO_InitTypeDef GPIO_InitStruct; 
  unsigned char bit; 

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  /*����һ��Ҫ������������������ܶ�������*/ 
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
*�������ƣ�E2promWriteByte 
*�������ܣ�E2PROMָ����ַдһ�ֽ����� 
*���������addr  E2PROM��ַ 
                             data  д������� 
*���������SET: д��������RESET:д����� 
*��           ע�� 
**************************************************************/ 
uint8_t E2promWriteByte( uint16_t addr, uint8_t data ) 
{ 
  //asm("CPSID I");  //���ж� 
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
  //asm("CPSIE I");  //���ж� 

  return SET; 
} 

/************************************************************* 
*�������ƣ�E2promReadByte 
*�������ܣ�E2PROMָ����ַ��һ�ֽ����� 
*���������addr  E2PROM��ַ 
*������������ض��������� 
*��           ע�� 
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
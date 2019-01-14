#include "s_i2c.h"

int I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack();
void I2C_NoAck();
uint8_t I2C_GetAck(void);
void I2C_SendByte(uint8_t Data);
uint8_t I2C_ReadByte(uint8_t ack);


int I2C_Start(void)
{
	I2C_SDA_OUT();	//配置SDA为推挽输出
	
	SDA_H;
	SCL_H;//高电平有效
	I2C_delay();//延时
	//查看此时SDA是否就绪（高电平）
	if(!SDA_read)
	{
		printf("\r\nSDA线为低电平，总线忙，退出\r\n");
		return DISABLE;//SDA总线忙，退出
	}
	//制造一个下降沿，下降沿是开始的标志
	SDA_L;
	I2C_delay();
	//查看此时SDA已经变为低电平
	if(SDA_read)
	{
		printf("\r\nSDA线为高电平，总线出错，退出\r\n");
		return DISABLE;//SDA总线忙，退出
	}
	SCL_L;
	return ENABLE;
}

void I2C_Stop(void)
{
	I2C_SDA_OUT();	//配置SDA为推挽输出
	
	SCL_L;
	//制造一个上升沿，上升沿是结束的标志
	SDA_L;	
	SCL_H;//高电平有效
	I2C_delay();//延时
	SDA_H;
	I2C_delay();
}
//主机的应答信号,主机把第九位置高，从机将其拉低表示应答
static void I2C_Ack()
{
	SCL_L;
	I2C_SDA_OUT();	//配置SDA为推挽输出
	
	SDA_L;//置低
	I2C_delay();   //注意延时时间应该大于4微秒，其他位置也是如此
	SCL_H;
	I2C_delay();
	SCL_L;
}
 
//主机的非应答信号,从机把第九位置高，主机将其拉低表示非应答
static void I2C_NoAck()
{
	SCL_L;
	I2C_SDA_OUT();	//配置SDA为推挽输出
	
	I2C_delay();
	SDA_H;//置高
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
}
uint8_t I2C_GetAck(void)
{
  uint8_t time = 0;
	I2C_SDA_IN();	//配置SDA为上拉输入
	
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	while(SDA_read)//从机未应答，若应答，会拉低第九位
	{
		time++;
		if(time > 250)
		{
			//不应答时不可以发出终止信号，否则，复合读写模式下不可以进行第二阶段
			//SCCB_Stop();
			
			SCL_L;
			return DISABLE;
		}
	}
	SCL_L;
	return ENABLE;
}
//I2C写一个字节
void I2C_SendByte(uint8_t Data)
{
  uint8_t cnt;
  I2C_SDA_OUT();	//配置SDA为推挽输出
 
  for(cnt=0; cnt<8; cnt++)
  {
    SCL_L;                                 //SCL低(SCL低时，变化SDA)
    I2C_delay();
 
    if(Data & 0x80)
    {
      SDA_H;                              //SDA高，从最低位开始写起
    }
    else
    {
      SDA_L;                               //SDA低
    }
    Data <<= 1;
    SCL_H;                                //SCL高(发送数据)
    I2C_delay();
  }
  SCL_L;                                   //SCL低(等待应答信号)
  I2C_delay();
}
 
//I2C读取一个字节
uint8_t I2C_ReadByte(uint8_t ack)
{
  uint8_t cnt;
  uint8_t data;
  I2C_SDA_IN();	//配置SDA为上拉输入
	
  for(cnt=0; cnt<8; cnt++)
  {
    SCL_L;                                 //SCL低
    I2C_delay();
		
    SCL_H;                                //SCL高(读取数据)
    data <<= 1;
    if(SDA_read)
    {
      data |= 0x01;                              //SDA高(数据有效)
    }
    I2C_delay();
  }
  //发送应答信号，为低代表应答，高代表非应答
  if(ack == 1)
  {
     I2C_NoAck();
  }
  else
  {
     I2C_Ack();
  }
  return data;                                   //返回数据
}
//GPIO配置函数
void I2C_GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
 
  GPIO_InitStructure.GPIO_Pin = PIN_I2C_SCL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
  GPIO_Init(PORT_I2C_SCL, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = PIN_I2C_SDA;
  GPIO_Init(PORT_I2C_SDA, &GPIO_InitStructure);
}
 
 
//重新设置SDA为上拉输入模式
void I2C_SDA_IN()
{
   GPIO_InitTypeDef  GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Pin = PIN_I2C_SDA;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //上拉输入，使得板外部不需要接上拉电阻
   GPIO_Init(PORT_I2C_SDA, &GPIO_InitStructure);	
}
 
//重新设置SDA为推挽输出模式
void I2C_SDA_OUT()
{
   GPIO_InitTypeDef  GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Pin = PIN_I2C_SDA;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
   GPIO_Init(PORT_I2C_SDA, &GPIO_InitStructure);	
}
//I2C初始化
//I2C初始化
void I2C_Initializes(void)
{
  I2C_GPIO_Configuration();
  SCL_H;                                  //置位状态
  SDA_H;
}


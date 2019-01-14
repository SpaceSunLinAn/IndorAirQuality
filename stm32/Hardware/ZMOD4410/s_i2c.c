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
	I2C_SDA_OUT();	//����SDAΪ�������
	
	SDA_H;
	SCL_H;//�ߵ�ƽ��Ч
	I2C_delay();//��ʱ
	//�鿴��ʱSDA�Ƿ�������ߵ�ƽ��
	if(!SDA_read)
	{
		printf("\r\nSDA��Ϊ�͵�ƽ������æ���˳�\r\n");
		return DISABLE;//SDA����æ���˳�
	}
	//����һ���½��أ��½����ǿ�ʼ�ı�־
	SDA_L;
	I2C_delay();
	//�鿴��ʱSDA�Ѿ���Ϊ�͵�ƽ
	if(SDA_read)
	{
		printf("\r\nSDA��Ϊ�ߵ�ƽ�����߳����˳�\r\n");
		return DISABLE;//SDA����æ���˳�
	}
	SCL_L;
	return ENABLE;
}

void I2C_Stop(void)
{
	I2C_SDA_OUT();	//����SDAΪ�������
	
	SCL_L;
	//����һ�������أ��������ǽ����ı�־
	SDA_L;	
	SCL_H;//�ߵ�ƽ��Ч
	I2C_delay();//��ʱ
	SDA_H;
	I2C_delay();
}
//������Ӧ���ź�,�����ѵھ�λ�øߣ��ӻ��������ͱ�ʾӦ��
static void I2C_Ack()
{
	SCL_L;
	I2C_SDA_OUT();	//����SDAΪ�������
	
	SDA_L;//�õ�
	I2C_delay();   //ע����ʱʱ��Ӧ�ô���4΢�룬����λ��Ҳ�����
	SCL_H;
	I2C_delay();
	SCL_L;
}
 
//�����ķ�Ӧ���ź�,�ӻ��ѵھ�λ�øߣ������������ͱ�ʾ��Ӧ��
static void I2C_NoAck()
{
	SCL_L;
	I2C_SDA_OUT();	//����SDAΪ�������
	
	I2C_delay();
	SDA_H;//�ø�
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
}
uint8_t I2C_GetAck(void)
{
  uint8_t time = 0;
	I2C_SDA_IN();	//����SDAΪ��������
	
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	while(SDA_read)//�ӻ�δӦ����Ӧ�𣬻����͵ھ�λ
	{
		time++;
		if(time > 250)
		{
			//��Ӧ��ʱ�����Է�����ֹ�źţ����򣬸��϶�дģʽ�²����Խ��еڶ��׶�
			//SCCB_Stop();
			
			SCL_L;
			return DISABLE;
		}
	}
	SCL_L;
	return ENABLE;
}
//I2Cдһ���ֽ�
void I2C_SendByte(uint8_t Data)
{
  uint8_t cnt;
  I2C_SDA_OUT();	//����SDAΪ�������
 
  for(cnt=0; cnt<8; cnt++)
  {
    SCL_L;                                 //SCL��(SCL��ʱ���仯SDA)
    I2C_delay();
 
    if(Data & 0x80)
    {
      SDA_H;                              //SDA�ߣ������λ��ʼд��
    }
    else
    {
      SDA_L;                               //SDA��
    }
    Data <<= 1;
    SCL_H;                                //SCL��(��������)
    I2C_delay();
  }
  SCL_L;                                   //SCL��(�ȴ�Ӧ���ź�)
  I2C_delay();
}
 
//I2C��ȡһ���ֽ�
uint8_t I2C_ReadByte(uint8_t ack)
{
  uint8_t cnt;
  uint8_t data;
  I2C_SDA_IN();	//����SDAΪ��������
	
  for(cnt=0; cnt<8; cnt++)
  {
    SCL_L;                                 //SCL��
    I2C_delay();
		
    SCL_H;                                //SCL��(��ȡ����)
    data <<= 1;
    if(SDA_read)
    {
      data |= 0x01;                              //SDA��(������Ч)
    }
    I2C_delay();
  }
  //����Ӧ���źţ�Ϊ�ʹ���Ӧ�𣬸ߴ����Ӧ��
  if(ack == 1)
  {
     I2C_NoAck();
  }
  else
  {
     I2C_Ack();
  }
  return data;                                   //��������
}
//GPIO���ú���
void I2C_GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
 
  GPIO_InitStructure.GPIO_Pin = PIN_I2C_SCL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
  GPIO_Init(PORT_I2C_SCL, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = PIN_I2C_SDA;
  GPIO_Init(PORT_I2C_SDA, &GPIO_InitStructure);
}
 
 
//��������SDAΪ��������ģʽ
void I2C_SDA_IN()
{
   GPIO_InitTypeDef  GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Pin = PIN_I2C_SDA;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //�������룬ʹ�ð��ⲿ����Ҫ����������
   GPIO_Init(PORT_I2C_SDA, &GPIO_InitStructure);	
}
 
//��������SDAΪ�������ģʽ
void I2C_SDA_OUT()
{
   GPIO_InitTypeDef  GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Pin = PIN_I2C_SDA;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
   GPIO_Init(PORT_I2C_SDA, &GPIO_InitStructure);	
}
//I2C��ʼ��
//I2C��ʼ��
void I2C_Initializes(void)
{
  I2C_GPIO_Configuration();
  SCL_H;                                  //��λ״̬
  SDA_H;
}


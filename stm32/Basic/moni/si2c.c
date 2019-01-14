#include "si2c.h"
//#include "delay.h"
#define NOK  1  
 

static void i2cDelay()
{  
	delay_us(5);
/*
    volatile int i = 50;
    while (i)
    i--;	 */
}

void IIC_Init(void)
{			
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
//	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);
    IIC_SCL = 1;
    IIC_SDA = 1;
}

int IIC_Start(void)
{
    SDA_OUT(); 
    IIC_SDA=1;
	IIC_SCL=1;
	
    i2cDelay();
    IIC_SDA=0; // START:when CLK is high,DATA change form high to low 
    if(READ_SDA)  return 0; 
    i2cDelay();
    IIC_SCL=0;
    return 1;	   	
}
void IIC_Stop(void)
{
    SDA_OUT(); // sda ���
    IIC_SCL=0;
    IIC_SDA=0; // STOP:when CLK is high DATA change form low to high
    i2cDelay();;
    IIC_SCL=1; 
    IIC_SDA=1;
    i2cDelay();							   	
}
int IIC_Wait_Ack(void)
{
    u8 ucErrTime=0;
	IIC_SDA = 1;
	i2cDelay();
    SDA_IN(); 
  //  IIC_SDA=1; delay_us(1);	   
  //  IIC_SCL=1; delay_us(1);	 
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime > 50)
        {
            IIC_Stop();
            return 0;
			delay_us(1);
        }
    }
    IIC_SCL=0;
    return 1;  
}
void IIC_Ack(void)
{
    IIC_SCL=0;
    SDA_OUT();
    IIC_SDA=0;
    i2cDelay();
    IIC_SCL=1;
    i2cDelay();
    IIC_SCL=0;
}
void IIC_NAck(void)
{
    IIC_SCL=0;
    SDA_OUT();
    IIC_SDA=1;
    i2cDelay();
    IIC_SCL=1;
    i2cDelay();
    IIC_SCL=0;
}
void IIC_Send_Byte(uint8_t txd)
{
  uint8_t cnt;
  SDA_OUT();	
  for(cnt=0; cnt<8; cnt++)
  {
     IIC_SCL = 0;                              
    i2cDelay();
    if(txd & 0x80)
    {
      IIC_SDA = 1;                         
    }
    else
    {
      IIC_SDA = 0;                         
    }
   txd <<= 1;
     IIC_SCL = 1;                              
    i2cDelay();
  }
   IIC_SCL = 0;                                   
  i2cDelay();
}

/*
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;
	SDA_OUT(); 	  
    IIC_SCL = 0;
    for(t = 0; t < 8; t++)
    {     
		if((txd&0x80) >> 7)
			IIC_SDA = 1;
		else
			IIC_SDA = 0;  
		txd <<= 1;        
		i2cDelay();   
		IIC_SCL = 1;    
		i2cDelay();       
		IIC_SCL = 0;	
		i2cDelay();
    }
	}*/	 
	  
/*
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
    SDA_OUT(); 	    
    IIC_SCL=0;
	for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
        delay_us(2);   
        IIC_SCL=1;
        delay_us(2); 
        IIC_SCL=0;	
        delay_us(2);
    }	 
} 	  */

u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, receive = 0;
	SDA_IN();  
    for(i = 0; i < 8; i++ )
	{
        IIC_SCL = 0; 
        i2cDelay();
		IIC_SCL = 1;
        receive = (receive << 1) | READ_SDA;
		i2cDelay(); 
    }
	return receive;
}
/*
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN(); 
    for(i=0;i<8;i++ )
    {
        IIC_SCL=0; 
        delay_us(2);
        IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
        delay_us(1); 
    }					 
    if (ack)
        IIC_Ack();  
    else
        IIC_NAck();  
    return receive;
}	  */

int i2cWrite(u8 addr, u8 reg, u8* data, u8 len){

u8 count = 0;
IIC_Start();
IIC_Send_Byte( addr);	   //·¢ËÍÐ´ÃüÁî
IIC_Wait_Ack();
IIC_Send_Byte(reg);   //·¢ËÍµØÖ·
IIC_Wait_Ack();	  
for(count=0;count<len;count++){
IIC_Send_Byte(data[count]); 
IIC_Wait_Ack(); 
}
IIC_Stop();//²úÉúÒ»¸öÍ£Ö¹Ìõ¼þ
return 0; //status == 0;
} 

int i2cRead(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
u8 count = 0;

IIC_Start();
IIC_Send_Byte(addr);	   //·¢ËÍÐ´ÃüÁî
IIC_Wait_Ack();
IIC_Send_Byte(reg);   //·¢ËÍµØÖ·
IIC_Wait_Ack();	  
IIC_Start();
IIC_Send_Byte(addr+1);  //½øÈë½ÓÊÕÄ£Ê½	
IIC_Wait_Ack();

for(count=0;count<len;count++){

if(count!=len-1)buf[count]=IIC_Read_Byte(1);  //´øACKµÄ¶ÁÊý¾Ý
else  buf[count]=IIC_Read_Byte(0);	 //×îºóÒ»¸ö×Ö½ÚNACK
}
IIC_Stop();//²úÉúÒ»¸öÍ£Ö¹Ìõ¼þ
return count;
}
 /*
int i2cWrite(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len)
{
    int i;
    if (!IIC_Start()) {
        printf("iic start signal create failed\n");
        return -1;
    }
   // IIC_Send_Byte(addr << 1 );
    IIC_Send_Byte(addr );
    if (!IIC_Wait_Ack()) {
        IIC_Stop();
        printf("ack failed\n");
        return -1;
    }
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
    for (i = 0; i < len; i++) {
        IIC_Send_Byte(data[i]);
        if (!IIC_Wait_Ack()) {
            IIC_Stop();
            return -1;
        }
    }
    IIC_Stop();
    return 0;
}			 
int i2cRead(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len)
{ 
    if (!IIC_Start())
        return 1;
 //   IIC_Send_Byte(addr << 1);
	IIC_Send_Byte(addr );
   if (!IIC_Wait_Ack()) {
        printf("ACK return failed\n");
        IIC_Stop();
        return -1;
    } 
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();		
    IIC_Start();
 //   IIC_Send_Byte((addr << 1)+1);
 	IIC_Send_Byte(addr +1);
    IIC_Wait_Ack();
    while (len) {
        if (len == 1)
            *buf = IIC_Read_Byte(0);
        else
            *buf = IIC_Read_Byte(1);
        buf++;
        len--;
    }
    IIC_Stop();
    return 0;
}	 */
/*
#define  I2C_Direction_Transmitter      ((uint8_t)0x00)

uint16_t Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data) 
{
   if(!IIC_Start())return -1;
    IIC_SendByte(SlaveAddress<<1|I2C_Direction_Transmitter);  
    if(!I2C_WaitAck()){I2C_Stop(); return -1;}
    IIC_SendByte(REG_Address );   
    IIC_WaitAck();
    IIC_SendByte(REG_data);
    IIC_WaitAck();  
    IIC_Stop();
    delay_ms(5);
    return 0;
}
#define  I2C_Direction_Receiver         ((uint8_t)0x01)
u8 Single_Read(unsigned char SlaveAddress,unsigned char REG_Address)
{   
        unsigned char REG_data;      
        if(!IIC_Start())
        return -1;
    IIC_SendByte(SlaveAddress<<1|I2C_Direction_Transmitter);
        if(!I2C_WaitAck())
        {
            I2C_Stop();
            return -1;
         }
    IIC_SendByte((u8) REG_Address);
    IIC_WaitAck();
    IIC_Start();
    IIC_SendByte(SlaveAddress<<1|I2C_Direction_Receiver);
    IIC_WaitAck();
    REG_data= IIC_ReadByte();
    IIC_NoAck();
    IIC_Stop();
    //return TRUE;
    return  REG_data;
}
   */
/* 
   unsigned char I2C_ReadOneByte(unsigned char I2C_Addr,unsigned char addr)
   {
   unsigned char res=0;

   IIC_Start();	
   IIC_Send_Byte(I2C_Addr);	   //·¢ËÍÐ´ÃüÁî
   res++;
   IIC_Wait_Ack();
   IIC_Send_Byte(addr); res++;  //·¢ËÍµØÖ·
   IIC_Wait_Ack();	  
//IIC_Stop();//²úÉúÒ»¸öÍ£Ö¹Ìõ¼þ	
IIC_Start();
IIC_Send_Byte(I2C_Addr+1); res++;          //½øÈë½ÓÊÕÄ£Ê½			   
IIC_Wait_Ack();
res=IIC_Read_Byte(0);	   
IIC_Stop();//²úÉúÒ»¸öÍ£Ö¹Ìõ¼þ
return res;
}

u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data){
u8 count = 0;

IIC_Start();
IIC_Send_Byte(dev);	   //·¢ËÍÐ´ÃüÁî
IIC_Wait_Ack();
IIC_Send_Byte(reg);   //·¢ËÍµØÖ·
IIC_Wait_Ack();	  
IIC_Start();
IIC_Send_Byte(dev+1);  //½øÈë½ÓÊÕÄ£Ê½	
IIC_Wait_Ack();

for(count=0;count<length;count++){

if(count!=length-1)data[count]=IIC_Read_Byte(1);  //´øACKµÄ¶ÁÊý¾Ý
else  data[count]=IIC_Read_Byte(0);	 //×îºóÒ»¸ö×Ö½ÚNACK
}
IIC_Stop();//²úÉúÒ»¸öÍ£Ö¹Ìõ¼þ
return count;
}

u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data){

u8 count = 0;
IIC_Start();
IIC_Send_Byte(dev);	   //·¢ËÍÐ´ÃüÁî
IIC_Wait_Ack();
IIC_Send_Byte(reg);   //·¢ËÍµØÖ·
IIC_Wait_Ack();	  
for(count=0;count<length;count++){
IIC_Send_Byte(data[count]); 
IIC_Wait_Ack(); 
}
IIC_Stop();//²úÉúÒ»¸öÍ£Ö¹Ìõ¼þ
return 1; //status == 0;
}

u8 IICreadByte(u8 dev, u8 reg, u8 *data){
 *data=I2C_ReadOneByte(dev, reg);
 return 1;
 }

 u8 IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data){
 return IICwriteBytes(dev, reg, 1, &data);
 }

 u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
 {
 u8 b;
 if (IICreadByte(dev, reg, &b) != 0) {
 u8 mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
data <<= (8 - length);
data >>= (7 - bitStart);
b &= mask;
b |= data;
return IICwriteByte(dev, reg, b);
} else {
    return 0;
}
}

u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data){
    u8 b;
    IICreadByte(dev, reg, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return IICwriteByte(dev, reg, b);
}	*/

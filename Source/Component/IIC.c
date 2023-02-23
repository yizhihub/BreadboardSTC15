#include "IIC.h"
#include "intrins.h"




void delayus()               //  k<5:  used 4 us @ 12.0000MHz
{                             // k<4:  used 3.64 @ 12.0000MHz
  uchar k;
  for(k=0;k<4;k++)
   ;
}

void i2cInit()
{
	 sda=1;
	delayus();
	 scl=1;
	 delayus();
}
void i2cStart(void)
{
	 sda=1;
	 scl=1;
	 delayus();
	 sda=0;
	 delayus();
	 scl=0;   // 已经提前将scl电平拉低
}
void i2cStop(void)
{
	sda=0;
	scl=1;
	delayus();
	sda=1;
	delayus();
}

uchar i2cRespons(void)   // 器件应答主机      FebA by yizhi 2023
{
	unsigned int i=0;
	sda=1;     // master现将sda拉高，// 
	delayus();// 等待slave产生应答信号
	scl=1;
	delayus();
	while(sda) 
    {
        if (i++ > 1000)
            return 1;
    }
	scl=0;
    return 0;
}
void i2cAnswer(uchar ans) // 主机应答期间， 1 应答 0 不应答
{
	 if(ans==1)
	 {
		 sda=0;
		 delayus();
	 }
	 else
	 {
		 sda=1;
	   delayus();
		}
		scl=1;
		delayus(); // 等待slave读取应答信号
		scl=0;
}
	 
	 
/****************************************************
function: send a byte and return reponse status
input:  dat: the byte to send
output: return reponse status
author: yizhi
******************************************************/
uchar i2cWriteByte(uchar dat)
{
	uchar i,temp;
	temp=dat;
	for(i=0;i<8;i++)
	{
		if(temp&0x80)
			sda=1;
		else
			sda=0;
		delayus();
		scl=1;
		delayus();
		scl=0;
		temp<<=1;
	  delayus();
	}
    return i2cRespons();
}
/****************************************************
function: receive a byte and if sent ack or not
input:  ucAns: 1-send ack 0-not send ack
output: received byte
author: yizhi
******************************************************/
uchar i2cReadByte(uchar ans)
{
	uchar temp,i;
	sda=1;
	delayus();
	for(i=0;i<8;i++)
	{
	   temp<<=1;
		 scl=1;
		 if(sda)
			 temp=temp|0x01;
		 scl=0;
		 delayus();
	}
    i2cAnswer(ans);
	return temp;
}

/****************************************************
function:写一个字节
input:  ad: machine address
        add: ROM   address;
        dat: the data to write;
output: none;
author: yizhi
******************************************************/
void IIC_WriteChar(uchar ad,uchar add,uchar dat)
{
	i2cStart();
	i2cWriteByte(ad);    // 0x3c HMC5883L   0xD0 MPU6050 
	i2cWriteByte(add);
	i2cWriteByte(dat);
	i2cStop();
	
}
/****************************************************
function: 读一个字节
input:  ad: machine address
        add: ROM   address;
output: the readed data;
author: yizhi
******************************************************/
uchar IIC_ReadChar(uchar ad,uchar add)
{
	  
	  i2cStart();
	  i2cWriteByte(ad);
	  i2cWriteByte(add);
	  i2cStart();
	  i2cWriteByte(ad+0x01); // 先读后写
	  return i2cReadByte(0);
	
}
#include "IIC.h"
#include "intrins.h"




void delayus()               //  k<5:  used 4 us @ 12.0000MHz
{                             // k<4:  used 3.64 @ 12.0000MHz
  uchar k;
  for(k=0;k<4;k++)
   ;
}

void iic_init()
{
	 sda=1;
	delayus();
	 scl=1;
	 delayus();
}
void iic_start(void)
{
	 sda=1;
	 scl=1;
	 delayus();
	 sda=0;
	 delayus();
	 scl=0;   // 已经提前将scl电平拉低
}
void iic_stop(void)
{
	sda=0;
	scl=1;
	delayus();
	sda=1;
	delayus();
}

void iic_respons(void)   // 器件应答主机
{
	unsigned int i=0;
	sda=1;     // master现将sda拉高，// 
	delayus();// 等待slave产生应答信号
	scl=1;
	delayus();
	while(sda&&(i<1000))  i++;
	scl=0; 
}
void iic_answer(uchar ans) // 主机应答期间， 1 应答 0 不应答
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
	 
	 

void IIC_WriteByte(uchar dat)
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
}
uchar IIC_ReadByte(void)
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
void IIC_Write1(uchar ad,uchar add,uchar dat)
{
	iic_start();
	IIC_WriteByte(ad);    // 0x3c HMC5883L   0xD0 MPU6050 
	iic_respons();
	IIC_WriteByte(add);
	iic_respons();
	IIC_WriteByte(dat);
	iic_respons();
	iic_stop();
	
}
/****************************************************
function: 读一个字节
input:  ad: machine address
        add: ROM   address;
output: the readed data;
author: yizhi
******************************************************/
uchar IIC_read1(uchar ad,uchar add)
{
	  
	  iic_start();
	  IIC_WriteByte(ad);
	  iic_respons();
	  IIC_WriteByte(add);
	  iic_respons();
	  iic_start();
	  IIC_WriteByte(ad+0x01); // 先读后写
	  iic_respons();
	  return IIC_ReadByte();
	
}
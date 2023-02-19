#ifndef _iic_h_
#define _iic_h_

#include "common.h"


//sbit sda=P0^2;
//sbit scl=P0^3;
sbit sda=P1^0;
sbit scl=P1^1;
//sbit sda=P3^2;
//sbit scl=P3^3;

  
void i2cInit();
void i2cStart();
void i2cStop();
uchar i2cRespons(void);
void i2cAnswer(uchar ans);
void i2cWriteByte(uchar dat);
uchar i2cReadByte();
void IIC_WriteChar(uchar ad,uchar add,uchar dat);
uchar IIC_ReadChar(uchar ad,uchar add);      // 读写一个字节 
//void  IIC_WriteN(uchar ad,uchar add,uchar *j);
//void  IIC_ReadN(uchar ad, uchar add,uchar k); // 读写N个字节





#endif
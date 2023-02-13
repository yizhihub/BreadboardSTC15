#ifndef _iic_h_
#define _iic_h_

#include "common.h"


//sbit sda=P0^2;
//sbit scl=P0^3;
sbit sda=P1^1;
sbit scl=P1^2;
//sbit sda=P3^2;
//sbit scl=P3^3;

  
void iic_init();
void iic_start();
void iic_stop();
void iic_respons();
void iic_answer(uchar ans);
void IIC_WriteByte(uchar dat);
uchar IIC_ReadByte();
void IIC_Write1(uchar ad,uchar add,uchar dat);
uchar IIC_Read1(uchar ad,uchar add);      // 读写一个字节 
//void  IIC_WriteN(uchar ad,uchar add,uchar *j);
//void  IIC_ReadN(uchar ad, uchar add,uchar k); // 读写N个字节





#endif
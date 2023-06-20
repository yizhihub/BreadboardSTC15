#ifndef  _common_h_
#define  _common_h_
#include "STC15F2K60S2.h"
#include "intrins.h"
#include "stdio.h"

sbit fmq1=P2^5;
sbit LED0=P5^5; 
sbit LED1=P5^5; // 新制 PCB打烊板子
sbit Steer=P0^5;
sbit Camera=P2^6;

typedef unsigned long uint32;
typedef unsigned long uint32_t;

typedef unsigned int uint;
typedef unsigned int uint16;
typedef unsigned int uint16_t;

typedef unsigned char uint8;
typedef unsigned char uint8_t;
typedef unsigned char uchar;

typedef char           int8;
typedef char           int8_t;

typedef int            int16;
typedef int            int16_t;

typedef long           int32;
typedef long           int32_t;


// 类型重定义 ,兼容振楠FAT文件系统

#define UINT8X  unsigned char xdata
#define UINT8   unsigned char
#define UINT16X unsigned int xdata
#define UINT16  unsigned int
#define UINT32X unsgined long xdata
#define UINT32  unsigned long 

#define INT8X   char xdata
#define INT8    char 
#define INT16X  int  xdata
#define INT16   int
#define INT32X  long xdata
#define INT32   long 


#define IAP15W413ASSOP20    //IAP15W413ASDIP28  IAP15W413ASDIP16 STC15W408ASDIP16  IAP15W413ASSOP20 IAP15W413ASDIP28
//#define SENSOR_SHT3x
//#define SENSOR_HMC5883L
//#define SENSOR_MPU6500
//#define SENSOR_DS18B20
//#define PORT_IIC
//#define PORT_SPI





void yanshi(void);
void msDelay(uint16_t m);
void Delay_us(uint16 m);
void Delay10us(uchar m);
uint16 ABS(int m);
int Mid(int a,int b,int c);
void Didi(void);
void Di(void);
void LED1_Turn();
void LED0_Turn();
int16 SortNum(int16 value_buf[],uchar N);
int16 Float2Int(float v);


#endif 
#ifndef _bOLED_H
#define _bOLED_H

#include "common.h"
//#include "fsl_common.h"
//#include "fsl_snvs_hp.h"


typedef struct yizhi_RTC_Time_s
{
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char week;
    unsigned char date;
    unsigned char month;
    unsigned char year; //  ture year=2000+year;  
}RTC_Time_s;	//定义的时间类型

#if 1                //  1 for 51 platform   0 for arm platform
//sbit OLED_DC =P0^0;
//sbit OLED_RST=P0^1;
//sbit OLED_SCL=P0^3;
//sbit OLED_SDA=P0^2;

//sbit OLED_SCL= P1^2;                                                  /* 15W408AS  DIP20 DIP16 Right 倒置*/
//sbit OLED_SDA= P1^3;
//sbit OLED_RST= P1^4;
//sbit OLED_DC = P1^5;
//sbit OLED_CS = P5^4;

//sbit OLED_SCL= P2^6;                                                  /* 15W408AS  DIP28   倒置       */
//sbit OLED_SDA= P2^7;
//sbit OLED_RST= P1^0;
//sbit OLED_DC = P1^1;
//sbit OLED_CS = P1^2;

sbit OLED_SCL= P5^4;                                                    /* 15W408AS  DIP20 DIP16 Left 正置 */
sbit OLED_SDA= P1^5;
sbit OLED_RST= P1^4;
sbit OLED_DC = P1^3;
sbit OLED_CS = P1^2;


#define OLED_SCL_0    OLED_SCL = 0
#define OLED_SCL_1    OLED_SCL = 1
#define OLED_SDA_0    OLED_SDA = 0
#define OLED_SDA_1    OLED_SDA = 1
#define OLED_RST_0    OLED_RST = 0
#define OLED_RST_1    OLED_RST = 1
#define OLED_DC_0     OLED_DC = 0
#define OLED_DC_1     OLED_DC = 1
#define OLED_CS_0     OLED_CS = 0
#define OLED_CS_1     OLED_CS = 1

#else 
/* OLED Pin Group 1*/
//#define OLED_SCL_0    GPIO5->DR  &= ~(1 << 2)
//#define OLED_SCL_1    GPIO5->DR  |=  (1 << 2)
//#define OLED_SDA_0    GPIO3->DR  &= ~(1 << 4)
//#define OLED_SDA_1    GPIO3->DR  |= (1 << 4)
//#define OLED_RST_0    GPIO3->DR  &= ~(1 << 2)
//#define OLED_RST_1    GPIO3->DR  |= (1 << 2)
//#define OLED_DC_0     GPIO2->DR  &= ~(1 << 30)
//#define OLED_DC_1     GPIO2->DR  |= (1 << 30)
//#define OLED_CS_0     GPIO1->DR  &= ~(1 << 20)
//#define OLED_CS_1     GPIO1->DR  |= (1 << 20)

/* OLED Pin Group 2*/
#define OLED_SCL_0    GPIO5->DR  &= ~(1 << 1)
#define OLED_SCL_1    GPIO5->DR  |=  (1 << 1)
#define OLED_SDA_0    GPIO3->DR  &= ~(1 << 5)
#define OLED_SDA_1    GPIO3->DR  |= (1 << 5)
#define OLED_RST_0    GPIO3->DR  &= ~(1 << 3)
#define OLED_RST_1    GPIO3->DR  |= (1 << 3)
#define OLED_DC_0     GPIO2->DR  &= ~(1 << 31)
#define OLED_DC_1 1```````````                                                              222222222222222    GPIO2->DR  |= (1 << 31)
#define OLED_CS_0     GPIO1->DR  &= ~(1 << 21)
#define OLED_CS_1     GPIO1->DR  |= (1 << 21)
#endif


void OLED_Init(void);
void OLED_CLS(void);
void OLED_Fill(uint8_t dat);
void OLED_PutPixel(uint8_t x,uint8_t y);
void OLED_DrawBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1);
/* 汉字显示 */
void OLED_P14x16Ch(uint8_t x,uint8_t y,uint8_t ch[]);
void OLED_Print(uint8_t x, uint8_t y, uint8_t ch[]);
void OLED_P16x16Str(uint8_t x,uint8_t y,const uint8_t ch[][32],uint8_t len);
/* 字符显示 */
void OLED_P6x8Str(uint8_t x,uint8_t y,uint8_t ch[],uint8_t yn);
void OLED_P8x16Char(uint8_t x,uint8_t y,uint8_t wan);
void OLED_P8x16Str(uint8_t x,uint8_t y,uint8_t ch[],uint8_t yn);
/* 数值显示 */
void OLED_P8x16Three(uint8_t x,uint8_t y,int16_t m);  // 显示3位整数  大  
void OLED_P8x16Four(uint8_t x,uint8_t y,int m);  //  显示4位整数  大 
void OLED_P8x16Num3(uint8_t x,uint8_t y,uint16_t m); //  显示3位正整数
void OLED_P8x16Num4(uint8_t x,uint8_t y,uint16_t m); //  显示4位正整数
void OLED_P8x16Num5(uint8_t x,uint8_t y,uint16_t m); //  显示5位正整数
/* 特殊数值显示 */
void OLED_P16x32Num(uint8_t p,int num,uint8_t unit);// 特大字体
void OLED_P8x16Dot(uint8_t x,uint8_t y,float m, uint8_t ucFracNum, uint8_t ucUnit);   // 显示小数
void OLED_P6x8Four(uint8_t x, uint8_t y, int  num);// 显示4位整数  小 
void OLED_HexDisp(uint8_t x,uint8_t y,uint8_t *dat,uint8_t N);
void OLED_P16x32Time(uint8_t p, RTC_Time_s *ptTime);//大字体时钟显示
void OLED_P8x16Time(uint8 x,uint8 y,RTC_Time_s * time);//小字体时钟显示;
void timeClockStep(RTC_Time_s *ptTime);
 
#endif


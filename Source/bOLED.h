#ifndef _bOLED_H
#define _bOLED_H

#include "common.h"
//#include "fsl_common.h"
//#include "fsl_snvs_hp.h"

/*
 * other alternative feature
 */
//#define FEATURE_F6x8
#define FEATURE_F8x16
#define FEATURE_F16x32
#define FEATURE_HANZI
//#define FEATURE_BMP

typedef struct yizhi_RTC_Time_s
{
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char week;
    unsigned char date;
    unsigned char month;
    unsigned char year; //  ture year=2000+year;  
}RTC_Time_s;	//�����ʱ������

#if 1                //  1 for 51 platform   0 for arm platform

#if defined(IAP15W413ASDIP16)
sbit OLED_SCL= P1^2;                                                  /* 15W408AS  DIP16 Right ����*/
sbit OLED_SDA= P1^3;
sbit OLED_RST= P1^4;
sbit OLED_DC = P1^5;
sbit OLED_CS = P5^4;
#elif defined(IAP15W413ASDIP28)
sbit OLED_SCL= P2^6;                                                  /* 15W408AS  DIP28   ����       */
sbit OLED_SDA= P2^7;
sbit OLED_RST= P1^0;
sbit OLED_DC = P1^1;
sbit OLED_CS = P1^2;
#elif defined(STC15W408ASDIP16)
sbit OLED_SCL= P5^4;                                                    /* 15W408AS  DIP16 Left ���� */
sbit OLED_SDA= P1^5;
sbit OLED_RST= P1^4;
sbit OLED_DC = P1^3;
sbit OLED_CS = P1^2;
#endif

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
#define OLED_DC_1     GPIO2->DR  |= (1 << 31)
#define OLED_CS_0     GPIO1->DR  &= ~(1 << 21)
#define OLED_CS_1     GPIO1->DR  |= (1 << 21)
#endif


void OLED_Init(void);
void OLED_CLS(void);
void OLED_Fill(uint8_t dat);
void OLED_PutPixel(uint8_t x,uint8_t y);
void OLED_DrawBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1);
/* ������ʾ */
void OLED_P14x16Ch(uint8_t x,uint8_t y,uint8_t ch[]);
void OLED_Print(uint8_t x, uint8_t y, uint8_t ch[]);
void OLED_P16x16Str(uint8_t x,uint8_t y,const uint8_t ch[][32],uint8_t len);
/* �ַ���ʾ */
void OLED_P6x8Str(uint8_t x,uint8_t y,uint8_t ch[],uint8_t yn);
void OLED_P8x16Char(uint8_t x,uint8_t y,uint8_t wan);
void OLED_P8x16Str(uint8_t x,uint8_t y,uint8_t ch[],uint8_t yn);
/* ��ֵ��ʾ */
void OLED_P6x8Num(uint8_t x, uint8_t y, int  num, uint8_t ucLen);// ��ʾ4λ����  С 
void OLED_P6x8Dot(uint8_t x,uint8_t y,float m, uint8_t ucFracNum, uint8_t ucUnit);
void OLED_P8x16Dot(uint8_t x,uint8_t y,float m, uint8_t ucFracNum, uint8_t ucUnit);   // ��ʾС��
void OLED_P8x16Num(uint8_t x,uint8_t y, int m, uint8_t ucLen);
/* ������ֵ��ʾ */
void OLED_P16x32Num(uint8_t p,int num,uint8_t unit);// �ش�����
void OLED_HexDisp(uint8_t x,uint8_t y,uint8_t *dat,uint8_t N);
void OLED_P16x32Time(uint8_t p, RTC_Time_s *ptTime);//������ʱ����ʾ
void OLED_P8x16Time(uint8 x,uint8 y,RTC_Time_s * time);//С����ʱ����ʾ;
void OLED_P6x8Time(uint8 x,uint8 y,RTC_Time_s * time); //С����ʱ����ʾ;
void timeClockStep(RTC_Time_s *ptTime);
 
#endif


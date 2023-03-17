#ifndef _bOLED_H
#define _bOLED_H

#include "common.h"
//#include "fsl_common.h"
//#include "fsl_snvs_hp.h"

/*
 * other alternative feature
 */
#define FEATURE_F6x8
#define FEATURE_F8x16
//#define FEATURE_F16x32
#define FEATURE_HANZI
//#define FEATURE_BMP


/*
 * slection between arm or 51 for const data 
 */
#define CONST_DATA code
extern CONST_DATA unsigned char F6x8[][6];

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
void OLED_P6x8Num(uint8_t x, uint8_t y, int  num, uint8_t ucLen);// 显示4位整数  小 
void OLED_P6x8Dot(uint8_t x,uint8_t y,float m, uint8_t ucFracNum, uint8_t ucUnit);
void OLED_P8x16Dot(uint8_t x,uint8_t y,float m, uint8_t ucFracNum, uint8_t ucUnit);   // 显示小数
void OLED_P8x16Num(uint8_t x,uint8_t y, int m, uint8_t ucLen);
/* 特殊数值显示 */
void OLED_P16x32Num(uint8_t p,int num,uint8_t unit);// 特大字体
void OLED_HexDisp(uint8_t x,uint8_t y,uint8_t *dat,uint8_t N);
void OLED_P16x32Time(uint8_t p, RTC_Time_s *ptTime);//大字体时钟显示
void OLED_P8x16Time(uint8 x,uint8 y,RTC_Time_s * time);//小字体时钟显示;
void OLED_P6x8Time(uint8 x,uint8 y,RTC_Time_s * time); //小字体时钟显示;
void timeClockStep(RTC_Time_s *ptTime);
 
#endif


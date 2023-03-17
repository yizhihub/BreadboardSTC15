#ifndef _bOLED_H
#define _bOLED_H

/*
 * slection between arm or 51 for const data 
 */
#ifdef ARM_PLATEFORM
#define CONST_DATA  const
#else
#define CONST_DATA  code
#endif

#ifdef STM32F10X_HD
#include "sys.h"
#include "delay.h"
#elif defined(RT1052)
#include "fsl_common.h"
#include "fsl_snvs_hp.h"
#else
#include "common.h"
#endif



/*
 * slection among CHIP_SSD1306,CHIP_SH1106,CHIP_SSD1309,CHIP_SSD1331,CHIP_SSD1351
 */
#define CHIP_SSD1306


#define XLevelL		0x00
#define XLevelH		0x10
#define XLevel		((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	128
#define Max_Row		64
#define Landscope   0xa0     // Set SEG/Column Mapping       0xa0左右反置 0xa1正常
#define Verticall   0xc0     // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
#define	Brightness	0x7F 
#define OLED_WIDTH 128
#define OLED_HIGH  64
#define LINE_HEIGHT 2

/*
 * 显示屏尺寸调整   为什么不起作用呢 
 */
#if defined(CHIP_SSD1331)
#undef OLED_WIDTH
#undef LINE_HEIGHT
#define OLED_WIDTH 96
#define LINE_HEIGHT 16
#define OLED_LINE_MAX  3*LINE_HEIGHT
#elif defined(CHIP_SSD1351)
#undef OLED_HIGH
#undef LINE_HEIGHT
#define OLED_HIGH   128
#define LINE_HEIGHT 16
#define OLED_LINE_MAX  7*LINE_HEIGHT
#else
#define OLED_LINE_MAX  3*LINE_HEIGHT
#endif

#if defined(STC15W408ASDIP16)
#undef Landscope
#undef Verticall
#define Landscope   0xa1     // Set SEG/Column Mapping       0xa0左右反置 0xa1正常
#define Verticall   0xc1     // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
#endif

#define OLED_LINE0 0*LINE_HEIGHT
#define OLED_LINE1 1*LINE_HEIGHT
#define OLED_LINE2 2*LINE_HEIGHT
#define OLED_LINE3 3*LINE_HEIGHT
#define OLED_LINE4 4*LINE_HEIGHT
#define OLED_LINE5 5*LINE_HEIGHT
#define OLED_LINE6 6*LINE_HEIGHT
#define OLED_LINE7 7*LINE_HEIGHT
/*
 * other alternative feature
 */
#define FEATURE_F6x8
#define FEATURE_F8x16
//#define FEATURE_F16x32
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
}RTC_Time_s;	//定义的时间类型

#ifdef C51_PLATEFORM                //  1 for 51 platform   0 for arm platform

#if defined(IAP15W413ASDIP16)
sbit OLED_SCL= P1^2;                                                  /* 15W408AS  DIP16 Right 倒置*/
sbit OLED_SDA= P1^3;
sbit OLED_RST= P1^4;
sbit OLED_DC = P1^5;
sbit OLED_CS = P5^4;
#elif defined(IAP15W413ASDIP28)
sbit OLED_SCL= P2^6;                                                  /* 15W408AS  DIP28   倒置       */
sbit OLED_SDA= P2^7;
sbit OLED_RST= P1^0;
sbit OLED_DC = P1^1;
sbit OLED_CS = P1^2;
#elif defined(STC15W408ASDIP16)
sbit OLED_SCL= P5^4;                                                    /* 15W408AS  DIP16 Left 正置 */
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

#elif defined(STM32F10X_HD)

#define OLED_SCL  PCout(6)
#define OLED_SDA  PBout(0)
#define OLED_RST  PBout(2)            /* 在MINISTM32上直接接到了STM32的复位脚！*/
#define OLED_DC   PBout(4) 
#define OLED_CS   PBout(6)

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

#elif defined(RT1052)
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
void OLED_Fill(unsigned int usData);
void OLED_PutPixel(uint8_t x,uint8_t y);
void OLED_DrawBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1);
/* 汉字显示 */
void OLED_P14x16Ch(uint8_t x,uint8_t y,uint8_t ch[], uint16_t ucYn);
void OLED_Print(uint8_t x, uint8_t y, uint8_t ch[], uint16_t ucYn);
void OLED_P16x16Str(uint8_t x,uint8_t y,const uint8_t ch[][32],uint8_t len, uint16_t ucYn);
/* 字符显示 */
void OLED_P6x8Char(uint8_t x,uint8_t y,char m, uint16_t ucYn);
void OLED_P6x8Str(uint8_t x,uint8_t y,uint8_t ch[],uint16_t ucYn);
void OLED_P8x16Char(uint8_t x,uint8_t y,uint8_t wan, uint16_t ucYn);
void OLED_P8x16Str(uint8_t x,uint8_t y,uint8_t ch[],uint16_t ucYn);
/* 数值显示 */
void OLED_P6x8Num(uint8_t x, uint8_t y, int  num, uint8_t ucLen, uint16_t ucYn);// 显示4位整数  小 
void OLED_P6x8Dot(uint8_t x,uint8_t y,float m, uint8_t ucFracNum, uint8_t ucUnit, uint16_t ucYn);
void OLED_P8x16Num(uint8_t x,uint8_t y, int m, uint8_t ucLen, uint16_t ucYn);
void OLED_P8x16Dot(uint8_t x,uint8_t y,float m, uint8_t ucFracNum, uint8_t ucUnit, uint16_t ucYn);   // 显示小数

/* 特殊数值显示 */
void OLED_P16x32Num(uint8_t p,int num,uint8_t unit);// 特大字体
void OLED_HexDisp(uint8_t x,uint8_t y,uint8_t *dat,uint8_t N, uint16_t ucYn);
void OLED_P16x32Time(uint8_t p, RTC_Time_s *ptTime);//大字体时钟显示
void OLED_P8x16Time(u8 x,u8 y,RTC_Time_s * time, uint16_t ucYn);//小字体时钟显示;
void OLED_P6x8Time(u8 x,u8 y,RTC_Time_s * time, uint16_t ucYn); //小字体时钟显示;
void timeClockStep(RTC_Time_s *ptTime);

//颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //棕色
#define BRRED 			     0XFC07 //棕红色
#define GRAY  			     0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			     0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)
#endif


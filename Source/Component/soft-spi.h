/*
 * soft-spi.h
 *
 *  Created on: 2023.03.16
 *      Author: swj
 */

#ifndef _SOFT_SPI_H_
#define	_SOFT_SPI_H_

#include "common.h"

#if 1                //  1 for 51 platform   0 for arm platform

#if defined(IAP15W413ASDIP16)
sbit OLED_SCL= P1^2;                                                  /* 15W408AS  DIP16 Right µ¹ÖÃ*/
sbit OLED_SDA= P1^3;
sbit OLED_RST= P1^4;
sbit OLED_DC = P1^5;
sbit OLED_CS = P5^4;
#elif defined(IAP15W413ASDIP28)
sbit OLED_SCL= P2^6;                                                  /* 15W408AS  DIP28   µ¹ÖÃ       */
sbit OLED_SDA= P2^7;
sbit OLED_RST= P1^0;
sbit OLED_DC = P1^1;
sbit OLED_CS = P1^2;
#elif defined(STC15W408ASDIP16)
sbit OLED_SCL= P5^4;                                                    /* 15W408AS  DIP16 Left ÕýÖÃ */
sbit OLED_SDA= P1^5;
sbit OLED_RST= P1^4;
sbit OLED_DC = P1^3;
sbit OLED_CS = P1^2;
#endif

sbit OLCD_BLK = P1^3;

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
#define OLCD_BLK_0		OLCD_BLK = 0
#define OLCD_BLK_1		OLCD_BLK = 1
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

void SPI_WrDat(unsigned char dat);
void SPI_WrCmd(unsigned char cmd);
void SPI_WrDat16(unsigned short dat);

#endif
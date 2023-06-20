/*********************************************Copyright (c)***********************************************
**                                Yuwell MCU Technology Co., Ltd.
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               IIC.h
** Created By  :            yizhi
** Created Date:            2023-03-15
** Last Version:            v1.0 
** Descriptions:            include most hard ware init
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
**--------------------------------------------------------------------------------------------------------
**
*********************************************************************************************************/
#ifndef _iic_h_
#define _iic_h_

/*
 * slection between arm or 51 for const data 
 */
#ifdef ARM_PLATEFORM
#define CONST_DATA  const
#define ___NOP()     __asm("NOP")
#else
#define CONST_DATA  code
#define ___NOP()     _nop_()
#endif

/*
 * slection platfrom header files 
 */
#if defined(STM32F10X_HD)
    #include "sys.h"
    #include "delay.h"
#elif defined(STM32F10X_MD)
    #include "ZMD_Includes.h"
#elif defined(RT1052)
    #include "fsl_common.h"
    #include "fsl_snvs_hp.h"
#elif defined(LKS32MC08x)
    #include "basic.h"
    #include "hardware_config.h"
#else
    #include "common.h"
#endif


#if defined(STM32F10X_MD)
    #define IIC_SCL          PBout(6)
    #define IIC_SDA          PBout(7)
    #define IIC_SDA_R       PBin(7)
    #define IIC_SDA_IN      {GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= 8 << 28;}
    #define IIC_SDA_OUT     {GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= 3 << 28;}

#else
    sbit IIC_SDA = P1^0;
    sbit IIC_SCL = P1^1;
    #define IIC_SDA_R   IIC_SDA
    #define IIC_SDA_IN
    #define IIC_SDA_OUT
#endif

#define IIC_SCL_0    IIC_SCL = 0
#define IIC_SCL_1    IIC_SCL = 1
#define IIC_SDA_0    IIC_SDA = 0
#define IIC_SDA_1    IIC_SDA = 1
  
void i2cInit();
void i2cStart();
void i2cStop();
uint8_t i2cRespons(void);
void i2cAnswer(uint8_t ans);
uint8_t i2cWriteByte(uint8_t dat);
uint8_t i2cReadByte(uint8_t ans);
void IIC_WriteChar(uint8_t ad,uint8_t add,uint8_t dat);
uint8_t IIC_ReadChar(uint8_t ad,uint8_t add);      // 读写一个字节 
//void  IIC_WriteN(uint8_t ad,uint8_t add,uint8_t *j);
//void  IIC_ReadN(uint8_t ad, uint8_t add,uint8_t k); // 读写N个字节





#endif
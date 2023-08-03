/*********************************************Copyright (c)***********************************************
**                                Yuwell MCU Technology Co., Ltd.
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               bHMI.h
** Created By  :            YZ
** Created Date:            2023-06-15
** Last Version:            v1.0 
** Descriptions:            C51_PLATEFORM and ARM_PLATEFORM
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
#include "bHMI.h"
#include "bOLED.h"
#include "stm32f10x_tim.h"
//#include "string.h"
//#include "oled.h"
//#include "key.h"
//#include "adc.h"
//#include "NRF24L01.h"
//#include "MPU6050.h"
//#include "HMC5883.h"
//#include "spi.h"
//#include "iic.h"
//#include "SMG.h"
//#include "DS18B20.h"
//#include "DS1302.h" 
//#include "isr.h"

static uchar MenuItem[7][17];
static uchar NowItem;
static uchar FirstItem;
PWM_CTRL_UNION PwmCtrl;

/**
********************************************************************************************************
** @nameis HMI_Draw
** @effect darw the item according need 
** @import none
** @export none
** @return
** @create yizhi 2023.8.1
** @modify 
*********************************************************************************************************/
void HMI_Draw(uint8_t ucX, uint8_t ucStart, uint8_t str[], uint16_t ucYn)
{
    switch (ucX) {
        
    case 0:
        OLED_Print(0 + ucStart, OLED_LINE1, str, ucYn); // 正常显示    
        break;
    
    case 1:
        OLED_Print(64 + ucStart, OLED_LINE1, str, ucYn); // 正常显示    
        break;
    
    case 2:
        OLED_Print(0 + ucStart, OLED_LINE2, str, ucYn); // 正常显示    
        break;
    
    case 3:
        OLED_Print(64 + ucStart, OLED_LINE2, str, ucYn); // 正常显示    
        break;
    
    case 4:
        OLED_Print(0 + ucStart, OLED_LINE3, str, ucYn); // 正常显示    
        break;
    
    default:
        break;
    }
}
/**
********************************************************************************************************
** @nameis PWM_ProtoUpdate
** @effect update related protocal paramater 
** @import none
** @export none
** @return
** @create yizhi 2023.8.1
** @modify 
*********************************************************************************************************/
void PWM_ProtoUpdate(PWM_CTRL_DATA * ptPwm, INT16U ePwmProto)
{
    switch (ePwmProto) {
        
    case 0u:                                                           /* 400Hz tradiional drone throttle protocol 1~2ms */ 
        ptPwm->sPwmWidthStart   = 1000;
        ptPwm->sPwmWidthEnd     = 2000;
        ptPwm->usPwmModulo      = 2500;
        ptPwm->sPwmWidthRange   = ptPwm->sPwmWidthEnd - ptPwm->sPwmWidthStart;
        ptPwm->sPwmWidthDflt    = 1000;
        break;
    
    case 1u:                                                          /* 300Hz Yuwell throttle protocol 100us~2250us    */
        ptPwm->sPwmWidthStart   = 100;                               /* 100us-310us-420us-2250us-2450us                */
        ptPwm->sPwmWidthEnd     = 2250;
        ptPwm->usPwmModulo      = 3333;
        ptPwm->sPwmWidthRange   = ptPwm->sPwmWidthEnd - ptPwm->sPwmWidthStart;;
        ptPwm->sPwmWidthDflt    = 100;
        break;
    
    case 2u:                                                         /* 1000Hz Yuwell throttle protocol 100us~2250us   */
        ptPwm->sPwmWidthStart    = 0;                              /* -50us-80us-100us-900us-950us                   */
        ptPwm->sPwmWidthEnd      = 1000;
        ptPwm->usPwmModulo       = 1000;
        ptPwm->sPwmWidthRange    = ptPwm->sPwmWidthEnd - ptPwm->sPwmWidthStart;;
        ptPwm->sPwmWidthDflt     = 50;
        break;
    
    default:
        break;
    }
}

/**
********************************************************************************************************
** @nameis PWM_KeyCallback
** @effect KeyValue change then do something
** @import none
** @export none
** @return
** @create yizhi 2023.8.1
** @modify 
*********************************************************************************************************/
void PWM_KeyCallback(uint8_t ucItem)
{
   if (ucItem == 4) {
        if (PwmCtrl.sPwmCtrl[4] > 2)  PwmCtrl.sPwmCtrl[4] = 2;  /* max ePwmProtocol is 2 */
        PWM_ProtoUpdate(&PwmCtrl.tPwmCtrl, PwmCtrl.tPwmCtrl.ePwmProtocol);
        TIM_SetAutoreload(TIM3, PwmCtrl.tPwmCtrl.usPwmModulo);
        PwmCtrl.tPwmCtrl.sPwmWidthSetting = PwmCtrl.tPwmCtrl.sPwmWidthDflt;
    } else if (ucItem == 0) {
        PwmCtrl.tPwmCtrl.sPwmWidthLow  = PwmCtrl.tPwmCtrl.sPwmWidthStart + 
                                         PwmCtrl.tPwmCtrl.sPwmWidthRange * PwmCtrl.tPwmCtrl.sPwmLowVal /  1000u;
        if (PwmCtrl.tPwmCtrl.sPwmWidthLow < PwmCtrl.tPwmCtrl.sPwmWidthDflt)
            PwmCtrl.tPwmCtrl.sPwmWidthLow = PwmCtrl.tPwmCtrl.sPwmWidthDflt;
        PwmCtrl.tPwmCtrl.sPwmWidthSetting =  PwmCtrl.tPwmCtrl.sPwmWidthLow;
    } else if (ucItem == 2) {
        PwmCtrl.tPwmCtrl.sPwmWidthHigh = PwmCtrl.tPwmCtrl.sPwmWidthStart + 
                                         PwmCtrl.tPwmCtrl.sPwmWidthRange * PwmCtrl.tPwmCtrl.sPwmHighVal /  1000u; 
        if (PwmCtrl.tPwmCtrl.sPwmWidthHigh < PwmCtrl.tPwmCtrl.sPwmWidthDflt)
            PwmCtrl.tPwmCtrl.sPwmWidthHigh = PwmCtrl.tPwmCtrl.sPwmWidthDflt;
        PwmCtrl.tPwmCtrl.sPwmWidthSetting =  PwmCtrl.tPwmCtrl.sPwmWidthHigh;
    } else {
        PwmCtrl.tPwmCtrl.sPwmWidthSetting = PwmCtrl.tPwmCtrl.sPwmWidthDflt;
    }
}
/***********************************************************************************************************
# 函数名称: DramMenu()
# 输入参数: char item[][17], 存储的条目文本 uint8 num 多少条目
# 输出参数: uint8
# 功    能: 绘制菜单条目
# 其    他：menuitem ： 菜单的缓存 ， num ：菜单总共的条目数  tile: 是否显示标题  1 显示  0 不显示 
# 作    者: 2015/3/8, by yizhi
************************************************************************************************************/
uchar DrawMenu(uchar MenuItem[][17],uchar num,uchar title)  
{
  uchar ii;
  uchar key,title2;
  uchar screen_flag = 1;
  if(title)    
    {        
      title2=0;
      //OLED_P6x8Str(0,0,"-TSL1401Send2PC-",1);
    }
  else 
    {        
        title2=1;
        //OLED_CLS(); 注释标记 AAA
    }
  while(1)
  {
    if(screen_flag)
    {
      screen_flag=0;
      if(num>2)
      {
        for(ii=0;ii<(3+title2);ii++)   //  
        {
          if(NowItem!=ii)
          {
            OLED_PutStr(0,(ii*2+title*2),MenuItem[FirstItem+ii], 8, 1); // 正常显示
          }
          else
          {
            OLED_PutStr(0,(ii*2+title*2),MenuItem[FirstItem+ii], 8, 0); // 高亮显示
          }
        }
      }
      else
      {
        for(ii =0;ii<2;ii++)
        {
          if(NowItem!=ii)
          {
            OLED_PutStr(0,(ii*2+2),MenuItem[FirstItem+ii], 8, 1);
          }
          else
          {
            OLED_PutStr(0,(ii*2+2),MenuItem[FirstItem+ii], 8, 1);
          }
        }
        
      }
      while(ADKey_Scan()!=0); // 
    /*这句话加上必要性是，1、首次进入该层菜单，从从任务退出或者
    //从此菜单退出，由于退出（或者返回）的触发按键都为 KEY_CANCEL，这样这里不卡一下的话，就会直接
    //从任务函数退到首页菜单。2，加上这句话，则菜单界面下就不能有按着上下键不松开高亮条连续滑动的
    //效果。 当然若是只有一层菜单。可以注释掉。*/
      }
    key = ADKey_Check2();
    if(KEY_UP==key)
    {
      // msDelay(10);
      // if(ADKey_Scan(1)==KEY_UP)
      //while(ADKey_Scan()==KEY_UP);
      if(NowItem > 0)         NowItem-=1; // 当前屏 的条目
      else if(FirstItem >0)  FirstItem-=1; // 当前屏 的第一个条目 
      else  // 已到达所有菜单的第一个条目
      {
        NowItem=3-title;
        FirstItem=num-4+title;
      }
      screen_flag=1;
      
      
    }
    if(KEY_DOWN==key)
    {
      // msDelay(10);  // 延时消除抖动
      // if(ADKey_Scan(1)==KEY_UP) 
      //while(ADKey_Scan()==KEY_DOWN);  
      if(num>2)
      {
        
        if(NowItem<2+title2)                  NowItem+=1;   
        else if((FirstItem+3+title2)<num)   FirstItem+=1;  
        else
        {
          FirstItem = 0;
          NowItem   = 0;
        }
      }
      else   // 菜单总条目<=2
      {
        if(NowItem<1)   NowItem+=1;  //  
      }
      screen_flag=1;
      
    } 
    if(KEY_ENTER==key)  //  if don't push ENTER or CANCEL  NO Jump out 
    {
      //while(ADKey_Scan()==KEY_IN); 
      usDelay(10);
      return  FirstItem+NowItem;
    }
    if(KEY_CANCEL==key)
    {
      //while(ADKey_Scan()==KEY_BACK) 
        usDelay(10);
        return 0xff;
    }
    msDelay(4); // 难道这个延时以前做飞思卡尔的时候都没有加吗？2016年1月10日加上。
  }
}

/***********************************************************************************************************
*****************************************条目函数***********************************************/
  /*******************************************************************************
# 函数原名：IMUCalibrate
# 输入参数: void
# 输出参数: void
# 功    能: 矫正陀螺仪的零点 标定加速度计 
# 其    他：
# 格    言：I just struggle myself! 
# 作    者: 2015/5/5, by yizhi
********************************************************************************/
//void IMUCalibrate(void)
//{
//  uint8 i,key;
//  uint16 cali_flag=0;
//  int16 idata temp[50]; // 一下子用掉100字节的idata空间 
//    // 此处采用时间换取空间的做法,每次只校准一个轴,分三次校准.如果单片机
//    // 内存足够大也可以一次性校准三个轴.
//  int16 temp_sum;
//  //IMURaw_s temp;
//  OLED_CLS();
//  OLED_P8x16Str(0,0,"--Gyro_Cali--",1);  
//  while(ADKey_Scan()!=KEY_CANCEL)
//  {
//         MPU6050_Read(); 
//         OLED_P8x16Three(0,2,GyroX);
//     OLED_P8x16Three(40,2,GyroY);
//     OLED_P8x16Three(80,2,GyroZ);
//         OLED_P8x16Three(0,4,GyroX_Bias);
//     OLED_P8x16Three(40,4,GyroY_Bias);
//     OLED_P8x16Three(80,4,GyroZ_Bias);
//      //OLED_Fill(0x00); // used 10.32ms @ AVR168 16.000MHz OLED_WrDat NO delay 
//         if(cali_flag==1)
//     {
//             OLED_P8x16Str(0,6,"               ",1);
//       OLED_P8x16Str(0,6,"Cali",1);
//       msDelay(1000);  // 1s后开始校准    待器件充分稳定 
//       OLED_P8x16Str(32,6,".",1);// 第一个点
//       for(i=0;i<50;i++) // 读取数据阶段必须保持静止 
//       {
//          MPU6050_Read(); 
//          temp[i]=GyroX;  
//          msDelay(5);
//       }
//       SortNum(temp,50); // 对踩到的50个点进行排序  
//             temp_sum=0;
//             for(i=10;i<40;i++)
//       { 
//         temp_sum+=temp[i];
//       }
//             GyroX_Bias+=Float2Int(temp_sum/30.0);// X轴陀螺仪校准完成 
//             OLED_P8x16Str(40,6,".",1);//第二个点 
//           for(i=0;i<50;i++) // 读取数据阶段必须保持静止 
//       {
//          MPU6050_Read(); 
//          temp[i]=GyroY;  
//          msDelay(5);
//       }
//       SortNum(temp,50); // 对踩到的50个点进行排序  
//             temp_sum=0;
//             for(i=10;i<40;i++)
//       {
//         temp_sum+=temp[i];
//       }
//             GyroY_Bias+=Float2Int(temp_sum/30.0);// Y轴陀螺仪校准完成 
//           OLED_P8x16Str(48,6,".",1);//第三个点 
//           for(i=0;i<50;i++) // 读取数据阶段必须保持静止 
//       {
//          MPU6050_Read(); 
//          temp[i]=GyroZ;  
//          msDelay(5);
//       }
//       SortNum(temp,50); // 对踩到的50个点进行排序  
//             temp_sum=0;
//             for(i=10;i<40;i++)
//       {
//         temp_sum+=temp[i];
//       }
//       GyroZ_Bias+=Float2Int(temp_sum/30.0);// Z轴陀螺仪校准完成 
//       OLED_P8x16Str(56,6,".",1);// 第四个点
//       OLED_P8x16Str(64,6,".",1);
////       OLED_P8x16Str(72,4,".",1);msDelay(100);//六
////       OLED_P8x16Str(80,4,".",1);msDelay(100);//七
////       OLED_P8x16Str(88,4,".",1);msDelay(100);//八 
//       OLED_P8x16Str(72,6,"OK!",1); 
//       cali_flag=0; // 陀螺仪已校准
//       //Update_ParamValue();
//      }
//            key=ADKey_Scan();    
//            if(key==KEY_UP)
//            {
//              while(ADKey_Scan()==KEY_UP);// 此处加入松手监测的原因是,不同人按下按键后松开的时间不一样
//                // 所以以松开手的时刻为准,目的是让板子陀螺仪充分稳定.
//              cali_flag=1;
//             //  redraw_flag=1;
//            }    
//            msDelay(50); 
//    }
//    while(ADKey_Scan()!=0);  // 仍有按键 按下 就卡在这里 
//  Delay_us(10);
//}
/********************The last study************************
#functionADKey_Cali()
#input:  void
#output: void 
#others: AD按键 AD值校准，出现误判，需校准AD按键
#date:  2016-01-10
#author:  yizhi 
***********************************************************/
void ADKey_Cali(void)
{
//     uint ad;
//     OLED_CLS();
//     OLED_P8x16Str(0,0,"AD Value is:",1);
//     while(ADKey_Scan()!=KEY_CANCEL)
//     {
//         ad=Get_ADC10bitResult(0x03); 
//         msDelay(300);
//     OLED_P8x16Num4(0,2,ad);
//   }
}
/*********prepare to lanqiao competition*************************
#function: IR_Test
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description: 
#author::  2016-03-16 by'  yizhi 
******************************************************************/
void IR_Test(void)
{
//     uchar quit_flag=0;
//   OLED_CLS();
//     EX1=1; // enable Int1         
//     TMOD&=0xf0; // T0定时器用于红外解码测量
//     AUXR&=~0x80;// 1T模式 慢
//   ET0=0;//关闭中断
//   TR0=0;//停止定时器
//     EA=1; 
//     OLED_P8x16Str(0,0,"IR_Docode:",1);
//     IR_OK_Flag=0;
//     
//     while(ADKey_Check()!=KEY_UP) // 使用向上键作为退出键
//     {
//         if(IR_OK_Flag)
//         {
//             LED0_Turn();
//             IR_OK_Flag=0;
//             OLED_HexDisp(0,2,IR_Buf,4);
//         }
//         msDelay(5);
//     }
//     EX1=0;// 关闭外部中断
     
}
/*********prepare to lanqiao competition*************************
#function: RTC_Timer
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description: 数码管 OLED同步显示 
#author::  2016-03-16 by'  yizhi 
******************************************************************/
void RTC_Timer()
{ 
    /*
     RTC_Time_s time={0,0,0,0,0,0};
   OLED_CLS();
     Initial_DS1302();
     TMOD&=0x0f;
     AUXR&=~0x40;// 1T
     TH1=63536>>8;
     TL1=63536&0x00ff;
     TF1=0;
     TR1=1;
     ET1=1;
     EA=1;
     DS1302_GetTime(&time);
     OLED_P8x16Str(0,0,"RTC_Timer:",1);
     //time={0,0,0,0,0,0};  // 不能这样赋值 
     while(ADKey_Scan()!=KEY_CANCEL)
     {
         //ad=Get_ADC10bitResult(0x03); 
       DS1302_GetTime(&time);
         GPSTime_Disply(0,2,&time);
         UpdateTime(&time); // 刷数码管显示显存
         msDelay(990);
     }
   TR0=0;
   ET0=0;
     EA=0; // 关闭定时器,释放资源
     //OLED_P8x16Num4(0,2,ad);   
    */
}
void DS18B20(void)
{
//      INT16U temp;
//      OLED_CLS();
//      while(Init_DS18B20())
//        {
//            
//             LED0_Turn();
//       OLED_P8x16Str(0,2,"DS18B20 is't connected!!",1);
//            if(KEY_CANCEL==ADKey_Scan()) // ADKey_Check2()
//            {
//                return;
//            }    
//       msDelay(20);            
//        }
//      Init_DS18B20();
//      msDelay(10);
//      OLED_Print(0,0,"温度Value:");
//      
//      while(ADKey_Scan()!=KEY_CANCEL)
//        {
//            temp=ReadTemperature();
//            OLED_P16x32Num(1,temp,1); 
//            msDelay(50); // 这里加个延时比较好   @2016-04-24 加上
//            LED0_Turn();
//        }

}

/********************The last study************************
#functionMPU6050_Test 
#input:  
#output:  
#others: 
#date:  2016-01-10
#author:  yizhi 
***********************************************************/ 
/*********prepare to lanqiao competition*************************
#function:  HMC_Test();
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description:
#author::  2016-04-28 by'  yizhi 
******************************************************************/
/***********************************************************************************************************
# Function: Menu_Display()
# Input: void
# Output: void
# Description: 
# Others:
# Date  : 2015/3/8, by yizhi
************************************************************************************************************/
void Menu_Display(void)
{
    uchar sel;
    FirstItem = 0;
    NowItem   = 0;
    // TopDisp  =1;

    while(1)  // 根菜单
    { 
        strcpy((char*)MenuItem[0] ,"1:MPU6050_Test  ");
        strcpy((char*)MenuItem[1] ,"2:NRF24L01_Test ");
        strcpy((char*)MenuItem[2] ,"3:RTC_Timer     ");
        strcpy((char*)MenuItem[3] ,"4:DS18B20_Test  ");
        strcpy((char*)MenuItem[4] ,"5:IR_Test       ");
        strcpy((char*)MenuItem[5] ,"6:ADkey_Cali    ");
        strcpy((char*)MenuItem[6] ,"7:HMC5883L_Test ");

            
            
        sel=DrawMenu(MenuItem,7,0); 
        msDelay(5);
        usDelay(5);
        switch (sel)  {
            
        case 0:
            break;

        case 1:
            break;

        case 2:
            RTC_Timer();
            break;

        case 3:
            DS18B20();
            break;

        case 4:
            IR_Test();
            break;
        case 5:
            ADKey_Cali();
            break;

        case 6:
            break;

         default :
              break;
         
        }
    }
}
/*********************2016年1月再次调试菜单函数总结*******************************/ 
/*
1、一次发现我在NRF_test菜单里面点击NOName的条目（为编写对应条目任务函数）不会感觉到屏幕
闪烁一下，而在主菜单中，我点击未命名的条目，菜单会闪烁一下，纠结了半个小时，最终找出是因为
的NRF-test二级菜单留出的标题位置，而首页菜单（主菜单）未留出标题的位置，在程序中我发现执行了
OLED_CLS() [搜索AAA注释标识]。可以注释掉，注释后。问题得到解决。以后再遇到闪屏现象，可能就是
函数哪里执行了OLED_CLS(),清屏函数执行时间较长，导致闪屏。
2、本HMI文件共设置一个二级菜单（NRF_Test）和一个在首页菜单下的任务函数（ADCalibra）其中二级菜单
主循环条件采用while(!quit_flag)方式  而二级菜单下的任务函数和首页菜单下的任务函数的主循环条件为
while(ADKey_Scan()!=KEY_CANCEL)。 
*/





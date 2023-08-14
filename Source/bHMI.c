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
        OLED_Print(0 + ucStart, OLED_LINE1, str, ucYn); // ������ʾ    
        break;
    
    case 1:
        OLED_Print(64 + ucStart, OLED_LINE1, str, ucYn); // ������ʾ    
        break;
    
    case 2:
        OLED_Print(0 + ucStart, OLED_LINE2, str, ucYn); // ������ʾ    
        break;
    
    case 3:
        OLED_Print(64 + ucStart, OLED_LINE2, str, ucYn); // ������ʾ    
        break;
    
    case 4:
        OLED_Print(0 + ucStart, OLED_LINE3, str, ucYn); // ������ʾ    
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
# ��������: DramMenu()
# �������: char item[][17], �洢����Ŀ�ı� uint8 num ������Ŀ
# �������: uint8
# ��    ��: ���Ʋ˵���Ŀ
# ��    ����menuitem �� �˵��Ļ��� �� num ���˵��ܹ�����Ŀ��  tile: �Ƿ���ʾ����  1 ��ʾ  0 ����ʾ 
# ��    ��: 2015/3/8, by yizhi
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
        //OLED_CLS(); ע�ͱ�� AAA
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
            OLED_PutStr(0,(ii*2+title*2),MenuItem[FirstItem+ii], 8, 1); // ������ʾ
          }
          else
          {
            OLED_PutStr(0,(ii*2+title*2),MenuItem[FirstItem+ii], 8, 0); // ������ʾ
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
    /*��仰���ϱ�Ҫ���ǣ�1���״ν���ò�˵����Ӵ������˳�����
    //�Ӵ˲˵��˳��������˳������߷��أ��Ĵ���������Ϊ KEY_CANCEL���������ﲻ��һ�µĻ����ͻ�ֱ��
    //���������˵���ҳ�˵���2��������仰����˵������¾Ͳ����а������¼����ɿ�����������������
    //Ч���� ��Ȼ����ֻ��һ��˵�������ע�͵���*/
      }
    key = ADKey_Check2();
    if(KEY_UP==key)
    {
      // msDelay(10);
      // if(ADKey_Scan(1)==KEY_UP)
      //while(ADKey_Scan()==KEY_UP);
      if(NowItem > 0)         NowItem-=1; // ��ǰ�� ����Ŀ
      else if(FirstItem >0)  FirstItem-=1; // ��ǰ�� �ĵ�һ����Ŀ 
      else  // �ѵ������в˵��ĵ�һ����Ŀ
      {
        NowItem=3-title;
        FirstItem=num-4+title;
      }
      screen_flag=1;
      
      
    }
    if(KEY_DOWN==key)
    {
      // msDelay(10);  // ��ʱ��������
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
      else   // �˵�����Ŀ<=2
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
    msDelay(4); // �ѵ������ʱ��ǰ����˼������ʱ��û�м���2016��1��10�ռ��ϡ�
  }
}

/***********************************************************************************************************
*****************************************��Ŀ����***********************************************/
  /*******************************************************************************
# ����ԭ����IMUCalibrate
# �������: void
# �������: void
# ��    ��: ���������ǵ���� �궨���ٶȼ� 
# ��    ����
# ��    �ԣ�I just struggle myself! 
# ��    ��: 2015/5/5, by yizhi
********************************************************************************/
//void IMUCalibrate(void)
//{
//  uint8 i,key;
//  uint16 cali_flag=0;
//  int16 idata temp[50]; // һ�����õ�100�ֽڵ�idata�ռ� 
//    // �˴�����ʱ�任ȡ�ռ������,ÿ��ֻУ׼һ����,������У׼.�����Ƭ��
//    // �ڴ��㹻��Ҳ����һ����У׼������.
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
//       msDelay(1000);  // 1s��ʼУ׼    ����������ȶ� 
//       OLED_P8x16Str(32,6,".",1);// ��һ����
//       for(i=0;i<50;i++) // ��ȡ���ݽ׶α��뱣�־�ֹ 
//       {
//          MPU6050_Read(); 
//          temp[i]=GyroX;  
//          msDelay(5);
//       }
//       SortNum(temp,50); // �Բȵ���50�����������  
//             temp_sum=0;
//             for(i=10;i<40;i++)
//       { 
//         temp_sum+=temp[i];
//       }
//             GyroX_Bias+=Float2Int(temp_sum/30.0);// X��������У׼��� 
//             OLED_P8x16Str(40,6,".",1);//�ڶ����� 
//           for(i=0;i<50;i++) // ��ȡ���ݽ׶α��뱣�־�ֹ 
//       {
//          MPU6050_Read(); 
//          temp[i]=GyroY;  
//          msDelay(5);
//       }
//       SortNum(temp,50); // �Բȵ���50�����������  
//             temp_sum=0;
//             for(i=10;i<40;i++)
//       {
//         temp_sum+=temp[i];
//       }
//             GyroY_Bias+=Float2Int(temp_sum/30.0);// Y��������У׼��� 
//           OLED_P8x16Str(48,6,".",1);//�������� 
//           for(i=0;i<50;i++) // ��ȡ���ݽ׶α��뱣�־�ֹ 
//       {
//          MPU6050_Read(); 
//          temp[i]=GyroZ;  
//          msDelay(5);
//       }
//       SortNum(temp,50); // �Բȵ���50�����������  
//             temp_sum=0;
//             for(i=10;i<40;i++)
//       {
//         temp_sum+=temp[i];
//       }
//       GyroZ_Bias+=Float2Int(temp_sum/30.0);// Z��������У׼��� 
//       OLED_P8x16Str(56,6,".",1);// ���ĸ���
//       OLED_P8x16Str(64,6,".",1);
////       OLED_P8x16Str(72,4,".",1);msDelay(100);//��
////       OLED_P8x16Str(80,4,".",1);msDelay(100);//��
////       OLED_P8x16Str(88,4,".",1);msDelay(100);//�� 
//       OLED_P8x16Str(72,6,"OK!",1); 
//       cali_flag=0; // ��������У׼
//       //Update_ParamValue();
//      }
//            key=ADKey_Scan();    
//            if(key==KEY_UP)
//            {
//              while(ADKey_Scan()==KEY_UP);// �˴��������ּ���ԭ����,��ͬ�˰��°������ɿ���ʱ�䲻һ��
//                // �������ɿ��ֵ�ʱ��Ϊ׼,Ŀ�����ð��������ǳ���ȶ�.
//              cali_flag=1;
//             //  redraw_flag=1;
//            }    
//            msDelay(50); 
//    }
//    while(ADKey_Scan()!=0);  // ���а��� ���� �Ϳ������� 
//  Delay_us(10);
//}
/********************The last study************************
#function�NADKey_Cali()
#input:  void
#output: void 
#others: AD���� ADֵУ׼���������У���У׼AD����
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
//     TMOD&=0xf0; // T0��ʱ�����ں���������
//     AUXR&=~0x80;// 1Tģʽ ��
//   ET0=0;//�ر��ж�
//   TR0=0;//ֹͣ��ʱ��
//     EA=1; 
//     OLED_P8x16Str(0,0,"IR_Docode:",1);
//     IR_OK_Flag=0;
//     
//     while(ADKey_Check()!=KEY_UP) // ʹ�����ϼ���Ϊ�˳���
//     {
//         if(IR_OK_Flag)
//         {
//             LED0_Turn();
//             IR_OK_Flag=0;
//             OLED_HexDisp(0,2,IR_Buf,4);
//         }
//         msDelay(5);
//     }
//     EX1=0;// �ر��ⲿ�ж�
     
}
/*********prepare to lanqiao competition*************************
#function: RTC_Timer
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description: ����� OLEDͬ����ʾ 
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
     //time={0,0,0,0,0,0};  // ����������ֵ 
     while(ADKey_Scan()!=KEY_CANCEL)
     {
         //ad=Get_ADC10bitResult(0x03); 
       DS1302_GetTime(&time);
         GPSTime_Disply(0,2,&time);
         UpdateTime(&time); // ˢ�������ʾ�Դ�
         msDelay(990);
     }
   TR0=0;
   ET0=0;
     EA=0; // �رն�ʱ��,�ͷ���Դ
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
//      OLED_Print(0,0,"�¶�Value:");
//      
//      while(ADKey_Scan()!=KEY_CANCEL)
//        {
//            temp=ReadTemperature();
//            OLED_P16x32Num(1,temp,1); 
//            msDelay(50); // ����Ӹ���ʱ�ȽϺ�   @2016-04-24 ����
//            LED0_Turn();
//        }

}

/********************The last study************************
#function�NMPU6050_Test 
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

    while(1)  // ���˵�
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
/*********************2016��1���ٴε��Բ˵������ܽ�*******************************/ 
/*
1��һ�η�������NRF_test�˵�������NOName����Ŀ��Ϊ��д��Ӧ��Ŀ������������о�����Ļ
��˸һ�£��������˵��У��ҵ��δ��������Ŀ���˵�����˸һ�£������˰��Сʱ�������ҳ�����Ϊ
��NRF-test�����˵������ı���λ�ã�����ҳ�˵������˵���δ���������λ�ã��ڳ������ҷ���ִ����
OLED_CLS() [����AAAע�ͱ�ʶ]������ע�͵���ע�ͺ�����õ�������Ժ��������������󣬿��ܾ���
��������ִ����OLED_CLS(),��������ִ��ʱ��ϳ�������������
2����HMI�ļ�������һ�������˵���NRF_Test����һ������ҳ�˵��µ���������ADCalibra�����ж����˵�
��ѭ����������while(!quit_flag)��ʽ  �������˵��µ�����������ҳ�˵��µ�����������ѭ������Ϊ
while(ADKey_Scan()!=KEY_CANCEL)�� 
*/





#include "hmi.h"
#include "string.h"
#include "oled.h"
#include "key.h"
#include "adc.h"
#include "NRF24L01.h"
#include "MPU6050.h"
#include "HMC5883.h"
#include "spi.h"
#include "iic.h"
#include "SMG.h"
#include "DS18B20.h"
#include "DS1302.h" 
#include "isr.h"
uchar  MenuItem[7][17];


uchar NowItem;
uchar FirstItem;
uchar TopDisp;

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
            OLED_P8x16Str(0,(ii*2+title*2),MenuItem[FirstItem+ii],1); // ������ʾ
          }
          else
          {
            OLED_P8x16Str(0,(ii*2+title*2),MenuItem[FirstItem+ii],0); // ������ʾ
          }
        }
      }
      else
      {
        for(ii =0;ii<2;ii++)
        {
          if(NowItem!=ii)
          {
            OLED_P8x16Str(0,(ii*2+2),MenuItem[FirstItem+ii],1);
          }
          else
          {
            OLED_P8x16Str(0,(ii*2+2),MenuItem[FirstItem+ii],1);
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
      Delay_us(10);
      return  FirstItem+NowItem;
    }
    if(KEY_CANCEL==key)
    {
      //while(ADKey_Scan()==KEY_BACK) 
        Delay_us(10);
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
void IMUCalibrate(void)
{
  uint8 i,key;
  uint16 cali_flag=0;
  int16 idata temp[50]; // һ�����õ�100�ֽڵ�idata�ռ� 
	// �˴�����ʱ�任ȡ�ռ������,ÿ��ֻУ׼һ����,������У׼.�����Ƭ��
	// �ڴ��㹻��Ҳ����һ����У׼������.
  int16 temp_sum;
  //IMURaw_s temp;
  OLED_CLS();
  OLED_P8x16Str(0,0,"--Gyro_Cali--",1);  
  while(ADKey_Scan()!=KEY_CANCEL)
  {
		 MPU6050_Read(); 
		 OLED_P8x16Three(0,2,GyroX);
     OLED_P8x16Three(40,2,GyroY);
     OLED_P8x16Three(80,2,GyroZ);
		 OLED_P8x16Three(0,4,GyroX_Bias);
     OLED_P8x16Three(40,4,GyroY_Bias);
     OLED_P8x16Three(80,4,GyroZ_Bias);
	  //OLED_Fill(0x00); // used 10.32ms @ AVR168 16.000MHz OLED_WrDat NO delay 
		 if(cali_flag==1)
     {
			 OLED_P8x16Str(0,6,"               ",1);
       OLED_P8x16Str(0,6,"Cali",1);
       msDelay(1000);  // 1s��ʼУ׼	����������ȶ� 
       OLED_P8x16Str(32,6,".",1);// ��һ����
       for(i=0;i<50;i++) // ��ȡ���ݽ׶α��뱣�־�ֹ 
       {
          MPU6050_Read(); 
          temp[i]=GyroX;  
          msDelay(5);
       }
       SortNum(temp,50); // �Բȵ���50�����������  
			 temp_sum=0;
			 for(i=10;i<40;i++)
       { 
         temp_sum+=temp[i];
       }
			 GyroX_Bias+=Float2Int(temp_sum/30.0);// X��������У׼��� 
			 OLED_P8x16Str(40,6,".",1);//�ڶ����� 
		   for(i=0;i<50;i++) // ��ȡ���ݽ׶α��뱣�־�ֹ 
       {
          MPU6050_Read(); 
          temp[i]=GyroY;  
          msDelay(5);
       }
       SortNum(temp,50); // �Բȵ���50�����������  
			 temp_sum=0;
			 for(i=10;i<40;i++)
       {
         temp_sum+=temp[i];
       }
			 GyroY_Bias+=Float2Int(temp_sum/30.0);// Y��������У׼��� 
		   OLED_P8x16Str(48,6,".",1);//�������� 
		   for(i=0;i<50;i++) // ��ȡ���ݽ׶α��뱣�־�ֹ 
       {
          MPU6050_Read(); 
          temp[i]=GyroZ;  
          msDelay(5);
       }
       SortNum(temp,50); // �Բȵ���50�����������  
			 temp_sum=0;
			 for(i=10;i<40;i++)
       {
         temp_sum+=temp[i];
       }
       GyroZ_Bias+=Float2Int(temp_sum/30.0);// Z��������У׼��� 
       OLED_P8x16Str(56,6,".",1);// ���ĸ���
       OLED_P8x16Str(64,6,".",1);
//       OLED_P8x16Str(72,4,".",1);msDelay(100);//��
//       OLED_P8x16Str(80,4,".",1);msDelay(100);//��
//       OLED_P8x16Str(88,4,".",1);msDelay(100);//�� 
       OLED_P8x16Str(72,6,"OK!",1); 
       cali_flag=0; // ��������У׼
       //Update_ParamValue();
      }
			key=ADKey_Scan();    
			if(key==KEY_UP)
			{
		  	while(ADKey_Scan()==KEY_UP);// �˴��������ּ���ԭ����,��ͬ�˰��°������ɿ���ʱ�䲻һ��
				// �������ɿ��ֵ�ʱ��Ϊ׼,Ŀ�����ð��������ǳ���ȶ�.
			  cali_flag=1;
			 //  redraw_flag=1;
			}    
			msDelay(50); 
    }
	while(ADKey_Scan()!=0);  // ���а��� ���� �Ϳ������� 
  Delay_us(10);
}
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
	 uint ad;
	 OLED_CLS();
	 OLED_P8x16Str(0,0,"AD Value is:",1);
	 while(ADKey_Scan()!=KEY_CANCEL)
	 {
		 ad=Get_ADC10bitResult(0x03); 
		 msDelay(300);
     OLED_P8x16Num4(0,2,ad);
   }
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
	 uchar quit_flag=0;
   OLED_CLS();
	 EX1=1; // enable Int1 		
	 TMOD&=0xf0; // T0��ʱ�����ں���������
	 AUXR&=~0x80;// 1Tģʽ ��
   ET0=0;//�ر��ж�
   TR0=0;//ֹͣ��ʱ��
	 EA=1; 
	 OLED_P8x16Str(0,0,"IR_Docode:",1);
	 IR_OK_Flag=0;
	 
	 while(ADKey_Check()!=KEY_UP) // ʹ�����ϼ���Ϊ�˳���
	 {
		 if(IR_OK_Flag)
		 {
			 LED0_Turn();
			 IR_OK_Flag=0;
			 OLED_HexDisp(0,2,IR_Buf,4);
		 }
		 msDelay(5);
	 }
	 EX1=0;// �ر��ⲿ�ж�
	 
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
}
void DS18B20(void)
{
	  uint16 temp;
	  OLED_CLS();
	  while(Init_DS18B20())
		{
			
			 LED0_Turn();
       OLED_P8x16Str(0,2,"DS18B20 is't connected!!",1);
			if(KEY_CANCEL==ADKey_Scan()) // ADKey_Check2()
			{
				return;
			}	
       msDelay(20);			
		}
	  Init_DS18B20();
	  msDelay(10);
	  OLED_Print(0,0,"�¶�Value:");
	  
	  while(ADKey_Scan()!=KEY_CANCEL)
		{
			temp=ReadTemperature();
			OLED_P16x32Num(1,temp,1); 
			msDelay(50); // ����Ӹ���ʱ�ȽϺ�   @2016-04-24 ����
            LED0_Turn();
		}

}
/********************The last study************************
#function�NNRF_Test 
#input:  
#output:  
#others: 
#date:  2016-01-10
#author:  yizhi 
***********************************************************/
void  NRF_Test(void)   // �ú���Ϊ�����˵����ṹ��menu_display����һ��
{
	static uchar sel;
	uchar quit_flag=0;
  	
   OLED_CLS();
   while(!NRF_CheckLink())
   {
			LED0_Turn();
			msDelay(20);
			OLED_P8x16Str(0,0,"NRF is not connected !!",1); 
			if(KEY_CANCEL==ADKey_Scan()) // ADKey_Check2()
			{
				quit_flag=1;
				break; 
			}		
   } 
//  // ��������һ�����棬���ú������ܷ�������,�ɺ��ԣ��ж϶���û������ô�����룡
//  OLED_P6x8Str(0,0,"--NRF24L01_Test--",1);
//	strcpy((char*)MenuItem[0] ,"1:CCD1 original");
//  strcpy((char*)MenuItem[1] ,"2:CCD1 dealed  ");
//  strcpy((char*)MenuItem[2] ,"3:CCD2 original");
//  strcpy((char*)MenuItem[3] ,"4:CCD2 dealed  "); 
  FirstItem = 0;
  NowItem   = 0;// ���ø���λ��
  while(!quit_flag)  
  {
		//OLED_CLS();
		OLED_P8x16Str(0,0,"--NRF24L01_Test--",1);//���������Ķ��� �� д����
	  strcpy((char*)MenuItem[0] ,"1:NRF_Transimite");
	  strcpy((char*)MenuItem[1] ,"2:NRF_Receiver  "); 
	  strcpy((char*)MenuItem[2] ,"3:Scan_Channel  ");
	  strcpy((char*)MenuItem[3] ,"4:NRF_ConstantCW");// �����������˵��»��������˵�����д�����˵�
  //��ˢMenuItem[][]�洢������Ҫ��֤�ص������˵�ʱ�ٴ�ˢ�ض����˵���Ŀ���Ƶ�MenuItem[][].���Խ���Щ
  //strcpy����while(!quit_flag)�ڲ���
    sel=DrawMenu(MenuItem,4,1);  // ����˵�����������λ��
    msDelay(5);
		switch(sel)
		{
		 case 0:
		 {
		   uchar temp;
		   NRF_TXMode(); // ����Ϊ����ģʽ
			 OLED_CLS();
			 OLED_P8x16Str(0,0,"NRF is Sending!!",1);
			 while(ADKey_Scan()!=KEY_CANCEL)
			 {
					temp=NRF_SendPacket(NRF_TX_Buf,32);
					NRF_TX_Buf[1]++;
					//OLED_DrawWave(NRF_TX_Buf[1]);		
					OLED_P8x16Num3(0,2,temp>>4); // PLOS_CNT 
					OLED_P8x16Num3(40,2,temp&0x0f);  // ARC_Cnt 
					OLED_P8x16Num3(80,2,NRF_TX_Buf[1]);  // ARC_Cnt
					msDelay(20);                             
	     }
     }break;
		
		 case 1:
		 {
		     NRF_RXMode(); //����Ϊ����ģʽ
		     OLED_CLS();
			 OLED_P8x16Str(0,0,"NRF Receiver~",1);
			 while(ADKey_Scan()!=KEY_CANCEL)
			 {
					if(NRF_ReceivePacket(NRF_RX_Buf,32))
					{
					  // LED0_Turn();
					  OLED_P8x16Num3(0,4,NRF_RX_Buf[1]);
					} 
	      msDelay(18);    // ��ѯ��ʽ                                    
//				if(NRF_RX_Flag)   
//				{
//					NRF_RX_Flag=0;
//					OLED_P8x16Num3(0,4,NRF_RX_Buf[1]);
//				}                  // �жϷ�ʽ
		    }
      } break;
		  case 2:
		  {
		   NRF_RXMode(); // �����Ƚ��н���ģʽ�ĳ�ʼ�������ܽ���������ŵ�����
			 while(ADKey_Scan()!=KEY_CANCEL)
			 {
	           NRF_Evaluate();
	     }		  
	    }break;
		  case 3:
		  {
		  uchar  channel=40; // Ĭ�ϵĳ�����������ŵ�
      KEYn_e key1;
		  OLED_CLS();
			OLED_P8x16Str(0,0,"-Constant CW-",1);
			OLED_P8x16Str(0,2,"channel:",1);
			OLED_P8x16Num3(64,2,channel);
		  while(ADKey_Scan()!=KEY_CANCEL)
			{
			   key1 = ADKey_Check2();
			   if(key1==KEY_RIGHT)
			   {
			    channel++; 
				  NRF_ConstantCW(channel);
				  OLED_P8x16Num3(64,2,channel);
			   }
			   else if(key1==KEY_LEFT)
			   {
			    channel--;
				  NRF_ConstantCW(channel);
				  OLED_P8x16Num3(64,2,channel);
			   }
         msDelay(4);
	    }
		  }break;
		  case 0xff:  // ע�������0xff�����ǰ����ļ��룬����DrawMenu()�����ķ���ֵ 
		  {
            quit_flag=1;			   //quit current menu ��
      }break;
		   default :
			 break;
    }
  }
  FirstItem  = 0;
  NowItem    = 1;	 //������������ֵʱΪ�˴Ӵμ��˵����ص��ϲ�˵��󣬱��ֽ���ʱ�ĸ�����Ŀ
}
	  //while(ADKey_Scan()==KEY_CANCEL); // ������仰������"�˳�����������Ӧ�������ɿ��� 
   /* [�Ժ�� while(ADKey_Scan()!=KEY_CANCEL) ��ѭ������]
    ����Ҫʵ�֡����°���������Ӧ���������ɿ��ֺ���Ӧ@2016-02-09���� ��˵����棨���⼶�˵�����ѭ���������ܺ���������ѭ������
	  һ�£����һ�»�������������Ĳ˵�������ԭ���Լ��롣
	 */
	  //OLED_Fill(0x00); // used 10.32ms @ AVR168 16.000MHz OLED_WrDat NO delay 
/********************The last study************************
#function�NMPU6050_Test 
#input:  
#output:  
#others: 
#date:  2016-01-10
#author:  yizhi 
***********************************************************/ 
void MPU6050_Test(void)
{ 
	static uint8 sel;
  uchar quit_flag=0;
  OLED_CLS();
  iic_init();
	while(!MPU6050_CheckLink())
	{
		OLED_P8x16Str(0,2,"MPU is't connect!",1); 
		LED0_Turn();
		msDelay(20);  
		if(KEY_CANCEL==ADKey_Scan()) // ADKey_Check2()
		{
			quit_flag=1;
			break; 
		}		
	}
	MPU6050_Init(); // ��������ʼ�� 
	FirstItem = 0;
	NowItem   = 0;// ���ø���λ��
	while(!quit_flag) 
	{
		//OLED_CLS();
		OLED_P8x16Str(0,0,"--MPU6050_Test---",1);//���������Ķ��� �� д����
		strcpy((char*)MenuItem[0] ,"1:View RawData  ");
		strcpy((char*)MenuItem[1] ,"2:View 3axis    "); 
		strcpy((char*)MenuItem[2] ,"3:MPU_Send2PC   ");
		strcpy((char*)MenuItem[3] ,"4:MPU_Calibrate ");// �����������˵��»��������˵�����д�����˵�
	//��ˢMenuItem[][]�洢������Ҫ��֤�ص������˵�ʱ�ٴ�ˢ�ض����˵���Ŀ���Ƶ�MenuItem[][].���Խ���Щ
	//strcpy����while(!quit_flag)�ڲ���
		sel=DrawMenu(MenuItem,4,1);  // ����˵�����������λ��
		msDelay(5);
		switch(sel)
		{
		 case 0:
		 {
		   //uchar temp; 
			 OLED_CLS();
			 OLED_P8x16Str(0,0,"MPU6050 RawData:",1);
			 while(ADKey_Scan()!=KEY_CANCEL)
			 {
					MPU6050_Read();  
					OLED_P6x8Four(0,2,GyroX);
					OLED_P6x8Four(42,2,GyroY);
					OLED_P6x8Four(84,2,GyroZ);
					OLED_P6x8Four(0,3,AcceX);
					OLED_P6x8Four(42,3,AcceY);
					OLED_P6x8Four(84,3,AcceZ); 
         	//LED_P8x16Four(0,4,GyroTemp); 	
          OLED_P8x16Dot(0,4,GyroTemp/10.0,2,2); 				 
					msDelay(150);
					//LED1_Turn();                    
	     }
     }break;
		
		 case 1:
		 {
		   OLED_CLS();
			 OLED_P8x16Str(0,0,"MPU6050 Degree:",1);
			 OLED_P8x16Str(0,2,"X:",1);
			 OLED_P8x16Str(0,4,"Y:",1);
			 OLED_P8x16Str(0,6,"Z:",1);
			 angle_x=0;
			 angle_y=0;
			 angle_z=0;
			 while(ADKey_Scan()!=KEY_CANCEL)
			 {
				 Angle_Calc();
				 OLED_P8x16Dot(16,2,angle_x,2,0);
				 OLED_P8x16Dot(16,4,angle_y,2,0);
				 OLED_P8x16Dot(16,6,angle_z,2,0);
         msDelay(10);	
       }
		 }break;
		  case 2:
		  {
			 while(ADKey_Scan()!=KEY_CANCEL)
			 {
				  OLED_CLS();
	        msDelay(10);    
	     }		  
	    }break;
		  case 3:
		  {
        IMUCalibrate();
		  }break;
		  case 0xff:  // ע�������0xff�����ǰ����ļ��룬����DrawMenu()�����ķ���ֵ 
		  {
         quit_flag=1;			   //quit current menu �� 
				 FirstItem = 0;
       	 NowItem   = 0;// ���ø���λ��
				    
      }break;
		  default :
			break;
    }
	}
}


/*********prepare to lanqiao competition*************************
#function:  HMC_Test();
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description:
#author::  2016-04-28 by'  yizhi 
******************************************************************/
void HMC_Test(void)
{
    OLED_CLS();
    iic_init();

		while(!HMC5883_CheckLink())
		{
			OLED_P8x16Str(0,2,"HMC is't connect!",1); 
			LED0_Turn();
			msDelay(20);  
			if(KEY_CANCEL==ADKey_Scan()) // ADKey_Check2()
			{
//				quit_flag=1;
//				break; 
				 return;
			}		
	 }
	HMC5883_Init();
	OLED_CLS();
	 OLED_Print(0,0,"ָ����"); 
	 OLED_P8x16Str(55,0,"Z:",1);
	 OLED_P8x16Str(0,2,"X:",1);
	 OLED_P8x16Str(56,2,"Y:",1);
	 
	 while(ADKey_Scan()!=KEY_CANCEL)
	 {
		   
	    HMC5883_Read();	 
		  OLED_P8x16Four(71,0,COMZ);
		  OLED_P8x16Four(16,2,COMX);
		  OLED_P8x16Four(72,2,COMY);
 		  OLED_P16x32Num(1,CompassHead*10,0);
		  msDelay(18);	 
		  TI=1;
		  printf("%5d %5d;",COMX,COMY);
		  while(!TI);
		  TI=0;
	 }	
}
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

		
		
    //sel=DrawMenu(MenuItem,7,0); 
    msDelay(5);
    //while(ADKey_Scan()!=0);  
    Delay_us(5);
    sel = 3;
    switch(sel)
    {
    case 0:
      {
       
			MPU6050_Test();// TopDisp = 2; 
      }break;
      
    case 1:
      {
        NRF_Test(); 
      }break;
    case 2:
      {
        RTC_Timer();
      }break;
      
    case 3:
      {
        DS18B20();
      }break;
	  case 4:
      {
        IR_Test();
      }break;
	  case 5:
      {
        ADKey_Cali();
      }break;
		case 6:
	   	{
			  HMC_Test();
		  }break;
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

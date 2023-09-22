#include "isr.h"
#include "bOLED.h"
#include "uart.h"

volatile uchar TimeControl_Flag;
volatile uchar NRF_RX_Flag=0;
//volatile uchar IR_Busy_Flag=0; // ���������ܲ���Ҫæ�ź�,���չ�����һֱ���ж�����
volatile uchar  IR_OK_Flag=0; // ���������ȷ������ɱ�־λ
uchar   IR_buf[4] = {0}; 
uchar  GucT5msFlg = 0;
uchar  GucT1sFlg  = 0;
uchar  GucEC11Flg = 0;

sbit IR_R=P3^3;
static uint16 idata LowTime,HighTime;
bit DeCode(void);
extern RTC_Time_s GtTime;
sbit EC11_A_R  = P3^5;

void Delay50us()		//@12.000MHz �������ר��
{
	unsigned char i, j;

	i = 1;
	j = 146;
	do
	{
		while (--j);
	} while (--i);
}

/*
 *  5ms����һ���ж�
 */ 
void Timer0() interrupt 1
{
    static idata uint16 sCnt=0;
    //P16=~P16;
    //DisplayScan();	//2msɨ����ʾһλ 
    GucT5msFlg = 1;
    if (sCnt++ >= 199)
    {
        sCnt=0;
        GucT1sFlg = 1;
//        DS1302_GetTime(&time);
//        UpdateTime(&time); // ˢ�������ʾ�Դ�
        timeClockStep(&GtTime);
    }
    
}
void Timer1() interrupt 3
{
	static idata uint16 cnt=0;
//	static idata RTC_Time_s time;
//	LED0_Turn();
//  TimeControl_Flag=0;	
//	  DisplayScan();	//2msɨ����ʾһλ 
	cnt++;
	if(cnt>=445)
  {
		cnt=0;
//	  DS1302_GetTime(&time);
//		UpdateTime(&time); // ˢ�������ʾ�Դ�
	}
}
void Timer2() interrupt 12
{
	LED1_Turn();
  TimeControl_Flag=0;	
}
//void ExterInt0() interrupt 1
//{ 
//	//LED0_Turn();
//}
void ExterInt1() interrupt 2
{ 
	unsigned char i,j;

//	for(i = 0;i < 10;i++) //14
//	{
//		Delay_us(40); // 0.4ms
//		if(IR_R)    //9MS���иߵ�ƽ�����ж�Ϊ���ţ��˳��������
//		{
//		  //EIMSK |= (1 << INT0);    //ʹ���ⲿ�ж�4
//			EA=1;
//		  return;
//		}
//	}
	//while(!IR_R) ; //�ȴ�9ms�͵�ƽ��ȥ
	//while(IR_R); //�ȴ�4.5ms�ߵ�ƽ��ȥ
   EA=0;
	 TR0=0;
   TH0=0;
	 TL0=0;
	 TR0=1;
	 while(IR_R==0); // �͵�ƽһ�㲻��һֱ��,��������д����,�����while(IR_R=1),һ�㲻Ҫ������
	 // ���ֹ�,�����ж���������������.
	 TR0=0; 
	 LowTime=TH0<<8|TL0;
	 TH0=0;
	 TL0=0;
	 TR0=1; // ��ʼ��ʱ�������ĸߵ�ƽʱ��
	 if(LowTime<8500||LowTime>9500)  // ���ֲ����ϵĵ͵�ƽ����ʱ��,�����˳�
	 {                                  
		 EA=1;
		 return; 
	 }
	 while(IR_R==1) // �����øߵ�ƽֻ����4.5ms 
	 {
		if(TH0>20) // ����5ms��,�ߵ�ƽ��û����,�˳�
		{
		 EA=1;
		 return;
		}
	 }
	 TR0=0; //ֹͣ��ʱ��ʱ // ˵ʵ�� ����IR�Ľ������,д���ǳ����.
   HighTime=TH0*256+TL0; 
   if(HighTime<4000) // ���ߵ�ƽ����ʱ�䲻��4000us ,�˳�
	 {
		 EA=1;
		 return;
	 }
	 for(i=0;i<4;i++)
	 {
		 for(j=0;j<8;j++)
		 {
			 IR_Buf[i]>>=1; 
			 TH0=0;
			 TL0=0;
			 while(IR_R==0);// �ȴ���Ϊ��
			 TR0=1;//��ʼ��ʱ
			 while(IR_R==1);
			 TR0=0;//ֹͣ��ʱ
			 HighTime=TH0*256+TL0;
			 if((HighTime>460)&&(HighTime<660))
				 IR_Buf[i]&=~0x80;
			 if((HighTime>1600)&&(HighTime<1800))
				 IR_Buf[i]|=0x80;
		 }
	 }
	 IR_OK_Flag=1;  
	 EA=1;
	//IR_OK_Flag=1; // �ߵ���һ��,�������������!
}
/***********************************************************************
#function: ����1�жϴ�����
#input: none 
#output: none
#others:
#date: 2014-10-25
#author: yizhi
************************************************************************/
uint8_t ucDataByte; 
void UART1() interrupt 4 using 1
{
     if(RI)
     {
        RI = 0;                 //���RIλ
        ucDataByte = SBUF;
        SBUF = ucDataByte;
     }
     if(TI)
     {
        TI=0;
        GbUartBusyFlg = 0;
     }
}

/***********************************************************************
#function: ����2�жϴ�����
#input: none 
#output: none
#others:
#date: 2014-10-25
#author: yizhi
************************************************************************/
void UART2() interrupt 8 
{
  uchar temp;
 if(S2CON&0x01)
 {
	LED0=1;
 /***************���������Ϣ*********************/
  S2CON&=0xfe; // 
	temp=S2BUF;
 } 
 /*if(S2CON&0x02)
 {
	 
   S2CON&=0xfd;
   UART2_Busy=0; 
 } */
}

/***********************************************************************
#function: ����3�жϴ�����
#input: none 
#output: none
#others:
#date: 2023-09-14
#author: song.wj
************************************************************************/
void UART3() interrupt 17
{
    if (S3CON&0X01)
    {
        S3CON &= 0xFE;
    }
}

/***********************************************************************
#function: �ⲿ�ж�3������
#input: none 
#output: none
#others:
#date: 2023-09-13
#author: song.wj
************************************************************************/
void ExterInt3() interrupt 11
{
    if (EC11_A_R)
        GucEC11Flg = 1;
    else
        GucEC11Flg = 2;
    
//    INT_CLKO &= 0xDF;   //�����Ҫ�ֶ�����жϱ�־λ�����ȹر��ж�
//                          //��ʱϵͳ���Զ�����ڲ����жϱ�־λ
//    INT_CLKO |= 0x20;   //Ȼ���ٴ��жϼ���
}

/****************************����������***********************/
//	for(i = 0;i < 4;i++)               //
//	{
//		for(j = 0;j < 8;j++)           //
//		{
//			while(IR_R); //�ȴ�4.5ms�ߵ�ƽ��ȥ
//			while(!(IR_R)); //�ȴ���ߵ�ƽ
//			while(IR_R)  //����ߵ�ƽʱ��
//			{
//				 Delay50us();// 105us
//				 k++; 
//				 if(k >= 40) //�ߵ�ƽʱ����������˳�������� K>=40��������2ms 
//				 {
//				   //EIMSK |= (1 << INT0);    //ʹ���ⲿ�ж�
//					EA=1;
//				  return;            //
//				 } 		
//			}
//			IR_Buf[i] = IR_Buf[i] >> 1;  //����һλ,��Ϊ LSB come firstly
//			if(k >= 15)// K=11 ��Ӧ 550us  K=34  ��Ӧ 1690us
//			{
//				IR_Buf[i] = IR_Buf[i] | 0x80;   //�ߵ�ƽʱ�����0.56����Ϊ����1
//			}
//			k = 0;                  //��ʱ����
//		}
//	}
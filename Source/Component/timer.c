#include "timer.h"
/*********prepare to lanqiao competition*************************
#function: Timer0_function 
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description:
#author::  2016-03-16 by'  yizhi 
******************************************************************/
void Timer0_Init(uint16 cnt)		//   @12.000MHz
{
	AUXR &= 0x7F;   //��ʱ��ʱ��Ϊ12Tģʽ(fsoc/12)// ������һ��,�ɹ���ʱ��ʵ�����  
	//AUXR |= 0x80; //��ʱ��ʱ��Ϊ1 Tģʽ(fsoc) //   
	TMOD &= 0xF0;		//16λ�Զ���װ��  
	TL0 = (65536-cnt);		
	TH0 = (65536-cnt)>>8;		 //  1ms 
	TF0 = 0;
  ET0=1;	// ���ж�
	TR0 = 1;		
}
/*********prepare to lanqiao competition*************************
#function: Timer1_function
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description:
#author::  2016-03-16 by yizhi 
******************************************************************/
void Timer1_Init(uint16 cnt)
{
  AUXR &= 0xBF;	 //��ʱ��ʱ��Ϊ12Tģʽ 
	//AUXR |= 0x40;//��ʱ��ʱ��Ϊ 1Tģʽ, ����
	TMOD &= 0x0F;	//�趨����ģʽ
	TL1 = (65536-cnt)&0x00ff;		//
	TH1 = (65536-cnt)>>8;		//8ms��ʱ����ֵ
	TF1 = 0;		 //���־λ
	ET1=1;	     // �򿪶�ʱ���ж�
	TR1 = 1;		 //��ʼ��ʱ
}
//void Timer1_Rst(void) // ��ʱ����ʱ��λ
//{
//  TL1=0xC0;
//  TH1=0xE0;    // delay for 8ms @12.000MHz
//}
/*********prepare to lanqiao competition*************************
#function: Timer2_Init
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description:
#author::  2016-03-16 by'  yizhi 
******************************************************************/
void Timer2_Init(uint16 cnt)
{
	AUXR &=0xFB;  // ��ʱ��ʱ��Ϊ12Tģʽ
  T2L=(65536-cnt)&0x00ff;
  T2H=(65536-cnt)>>8;//55536>>8;   
	AUXR|=0x10;  // start the timer2 
 	IE2|=0x04;   // enable the interrupt 	
}
//void Timer2_Rst(void)
//{
//	T2L=0xC0;
//  T2H=0xE0;    // delay for 20ms @12.000MHz
//}
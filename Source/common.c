#include "common.h"


void yanshi(void)
{ 
 ;;;;; ;;;;;
}          

/*********************************************************************************************************
** Function name:           msDleay()
** Descriptions:            �����ms��ʱ����
*********************************************************************************************************/
void msDelay(uint16_t m)
{
    unsigned char i, j;
    while (m--) {                    //24MHz
        i = 24;
        j = 85;
        do
        {
            while (--j);
        } while (--i);
    }
}


void Delay_us(uint m)
{
	 while(m--)
	 {
   _nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_(); _nop_();_nop_(); _nop_();_nop_();  
	 }		 
    	
}
// 10���ֺź�10��_nop_()��䲻һ��
  // used 930ns   used 1.8us
/*����IAP15F2K��Ƭ����ʱ��
 10��nop��� ��ʱ �ߵ�ƽ  1.84us
 100��nop��� ��ʱ10us    �ߵ͵�ƽ����ʱ�䲻һ�£� �ߵ�ƽ10us �͵�ƽ��10.32us ��
 200��nop��� �� 19.03us  ��ƽ����ʱ�䲻һ�£� �ߵ�ƽ19.03us  �͵�ƽ 19.34us
 1000��nop��� ��ʱ91.30us                              91.30us          91.76us
 ���ϲ��� �� ���� 11.0592MHz��   ������nop���ӣ� û��for while ���
 ����while��m--������m=1 ִ��100��nop ��ʱ��Ϊ12.26us
*/
//void Delay10us(uchar m)
//{
//	 while(m--)
//	{
//  _nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_(); _nop_();_nop_(); _nop_();_nop_();   
//	_nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_(); _nop_();_nop_(); _nop_();_nop_();
//	 _nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_(); _nop_();_nop_(); _nop_();_nop_();
//	 _nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_(); _nop_();_nop_(); _nop_();_nop_();
//	 _nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_(); _nop_();_nop_(); _nop_();_nop_();
//	 _nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_(); _nop_();_nop_(); _nop_();_nop_();
//	_nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_(); _nop_();_nop_(); _nop_();_nop_();
//  _nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_(); _nop_();_nop_(); _nop_();_nop_();
//	_nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_(); _nop_();_nop_(); _nop_();_nop_();
//	_nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_(); _nop_();_nop_(); _nop_();_nop_();
//	_nop_(); _nop_(); 
//  }
//	/* ������� delay10us(1) used 11.2us  delay10us(50) used 480us @ 12.000MHz �� */

//		
//}

void LED1_Turn()
{
	 LED1=~LED1;
}
void LED0_Turn()
{
	 LED0=~LED0;
}
void Di(void)
{
 
 fmq1=1;
 //fmq2=0;
 msDelay(60);
 fmq1=0;
// fmq2=1;
}

void Didi(void)
{
	 fmq1=1;

	msDelay(100);
	 fmq1=0;

	msDelay(50);
	 fmq1=1;

	msDelay(100);
	 fmq1=0;

}
/*
void Delay_us(uint m)
{
	;;;;;;;
} */
uint ABS(int m)
{
	 if(m<0)
		 m=-m;
	 return m;
}
// ȡ���������м�ֵ
int Mid(int a,int b,int c) 
{
	 int temp;
    if(a>=b)
		{			 
			if(b>=c)
			temp=b;
			else
				if(a>=c)
				 temp=c;
				else
				 temp=a;
	  }
		else    // a<b
 		{
			if(a>=c)
				temp=a;
			else //a<c
				if(b>=c)
					temp=c;
			  else
					temp=b;		
		}
	 return temp;
}
//��������
int16 Float2Int(float v) 
{
  if(v>0)  return  (int16)(v+0.5);
  else     return  (int16)(v-0.5);
}
/************To make helicopter***************************
#function�� ����һ��������ð�����򷨴�С��������
#input:   *v:  the alley  m :the toall number 
#output:   none
#others: 
#date:  2014-08-21
#author:  yizhi 
***********************************************************/
//int16 SortNum(int16 value_buf[],uchar N)
//{
////   uchar i,j; 
////	 int temp;
////   for(i=0;i<N-1;i++)// cycle for N-1 times
////		 for(j=0;j<N-1-i;j++)
////	     if(databuf[j]>databuf[j+1])
////			 {
////				 temp=databuf[j];
////				 databuf[j]=databuf[j+1];
////				 databuf[j+1]=temp; // big number sink to bottom
////			 }
////	  return databuf[N/2];
//	   uint8   i,j;
//		 int16  temp;	
//		 for (j=0;j<N-1;j++)
//		 {
//				for (i=0;i<N-1-j;i++)
//				{
//					 if ( value_buf[i]>value_buf[i+1] )
//					 {
//							temp = value_buf[i];
//							value_buf[i] = value_buf[i+1]; 
//						  value_buf[i+1] = temp;
//					 }
//				}
//		 }  return  (value_buf [N/2]);
//}
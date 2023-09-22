#include "isr.h"
#include "bOLED.h"
#include "uart.h"

volatile uchar TimeControl_Flag;
volatile uchar NRF_RX_Flag=0;
//volatile uchar IR_Busy_Flag=0; // 红外解码接受不需要忙信号,接收过程中一直在中断里面
volatile uchar  IR_OK_Flag=0; // 红外解码正确接收完成标志位
uchar   IR_buf[4] = {0}; 
uchar  GucT5msFlg = 0;
uchar  GucT1sFlg  = 0;
uchar  GucEC11Flg = 0;

sbit IR_R=P3^3;
static uint16 idata LowTime,HighTime;
bit DeCode(void);
extern RTC_Time_s GtTime;
sbit EC11_A_R  = P3^5;

void Delay50us()		//@12.000MHz 红外解码专用
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
 *  5ms进入一次中断
 */ 
void Timer0() interrupt 1
{
    static idata uint16 sCnt=0;
    //P16=~P16;
    //DisplayScan();	//2ms扫描显示一位 
    GucT5msFlg = 1;
    if (sCnt++ >= 199)
    {
        sCnt=0;
        GucT1sFlg = 1;
//        DS1302_GetTime(&time);
//        UpdateTime(&time); // 刷数码管显示显存
        timeClockStep(&GtTime);
    }
    
}
void Timer1() interrupt 3
{
	static idata uint16 cnt=0;
//	static idata RTC_Time_s time;
//	LED0_Turn();
//  TimeControl_Flag=0;	
//	  DisplayScan();	//2ms扫描显示一位 
	cnt++;
	if(cnt>=445)
  {
		cnt=0;
//	  DS1302_GetTime(&time);
//		UpdateTime(&time); // 刷数码管显示显存
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
//		if(IR_R)    //9MS内有高电平，则判断为干扰，退出处理程序
//		{
//		  //EIMSK |= (1 << INT0);    //使能外部中断4
//			EA=1;
//		  return;
//		}
//	}
	//while(!IR_R) ; //等待9ms低电平过去
	//while(IR_R); //等待4.5ms高电平过去
   EA=0;
	 TR0=0;
   TH0=0;
	 TL0=0;
	 TR0=1;
	 while(IR_R==0); // 低电平一般不会一直低,所以这样写可以,如果是while(IR_R=1),一般不要这样干
	 // 出现过,卡在中断里面出不来的情况.
	 TR0=0; 
	 LowTime=TH0<<8|TL0;
	 TH0=0;
	 TL0=0;
	 TR0=1; // 开始计时接下来的高电平时间
	 if(LowTime<8500||LowTime>9500)  // 发现不符合的低电平持续时间,趁早退出
	 {                                  
		 EA=1;
		 return; 
	 }
	 while(IR_R==1) // 期望该高电平只持续4.5ms 
	 {
		if(TH0>20) // 超过5ms了,高电平还没结束,退出
		{
		 EA=1;
		 return;
		}
	 }
	 TR0=0; //停止定时计时 // 说实在 对于IR的解码程序,写法非常灵活.
   HighTime=TH0*256+TL0; 
   if(HighTime<4000) // 若高电平持续时间不够4000us ,退出
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
			 while(IR_R==0);// 等待变为高
			 TR0=1;//开始计时
			 while(IR_R==1);
			 TR0=0;//停止计时
			 HighTime=TH0*256+TL0;
			 if((HighTime>460)&&(HighTime<660))
				 IR_Buf[i]&=~0x80;
			 if((HighTime>1600)&&(HighTime<1800))
				 IR_Buf[i]|=0x80;
		 }
	 }
	 IR_OK_Flag=1;  
	 EA=1;
	//IR_OK_Flag=1; // 走到这一步,红外解码接收完成!
}
/***********************************************************************
#function: 串口1中断处理函数
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
        RI = 0;                 //清除RI位
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
#function: 串口2中断处理函数
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
 /***************添加握手信息*********************/
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
#function: 串口3中断处理函数
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
#function: 外部中断3处理函数
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
    
//    INT_CLKO &= 0xDF;   //如果需要手动清除中断标志位，可先关闭中断
//                          //此时系统会自动清除内部的中断标志位
//    INT_CLKO |= 0x20;   //然后再打开中断即可
}

/****************************红外解码程序***********************/
//	for(i = 0;i < 4;i++)               //
//	{
//		for(j = 0;j < 8;j++)           //
//		{
//			while(IR_R); //等待4.5ms高电平过去
//			while(!(IR_R)); //等待变高电平
//			while(IR_R)  //计算高电平时间
//			{
//				 Delay50us();// 105us
//				 k++; 
//				 if(k >= 40) //高电平时间过长，则退出处理程序 K>=40即超过了2ms 
//				 {
//				   //EIMSK |= (1 << INT0);    //使能外部中断
//					EA=1;
//				  return;            //
//				 } 		
//			}
//			IR_Buf[i] = IR_Buf[i] >> 1;  //右移一位,因为 LSB come firstly
//			if(k >= 15)// K=11 对应 550us  K=34  对应 1690us
//			{
//				IR_Buf[i] = IR_Buf[i] | 0x80;   //高电平时间大于0.56，则为数据1
//			}
//			k = 0;                  //计时清零
//		}
//	}
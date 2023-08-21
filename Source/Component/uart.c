/*2016��5���޸�Ϊ   �������ͽ��պ�����Ϊһ������*/  

#include "uart.h"
#include "stdio.h"
#include "string.h"  // the use of strlen 
#include "stdarg.h"  // the use of vsprintf


volatile uchar GbUartBusyFlg = 0;
uint8_t xdata GucUartTxBuf[20] = {0x2B, 0x16};


unsigned long strtou32(char *str); 
void u32tostr(unsigned long dat,char *str);
/*********prepare to lanqiao competition*************************
#function: UART1Indep_Init 
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description:���������ʷ���������������.BRT 
#author::  2016-03-16 by'  yizhi 
****************************************************************/
void UART1Indep_Init(void)        //115200bps@11.0592MHz
{
    PCON &= 0x7F;        //�����ʲ�����
    SCON = 0x50;        //8λ���ݿɱ䲨����
    AUXR |= 0x04;    //���������ʷ�����ʱ��Ϊ FOSC (1T)
  BRT = 0xFD;    //115200bps@11.0592MHz
    //BRT = 0xFA; //115200bps@22.1184MHz 
    //BRT = 0xFB; //115200bps@18.432 MHz   
    AUXR |= 0x01;        //����һѡ����������ʷ�����Ϊ�����ʷ�����
    AUXR |= 0x10;        //�������������ʷ�����
    ES=0;    // ʹ�ܴ����ж� 
}
/*********prepare to lanqiao competition*************************
#function: UART1_Init() 
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description: ��ѡ�� T1 ����T2 ��Ϊ�����ʷ�����
#author::  2016-03-16 by'  yizhi 
******************************************************************/
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7
void UART1_Init(Timer_e timer_sel)   
{ 
    PCON &=0x7f;        // �����ʱ���ѡ�� 
    SCON = 0x50;        //8λ���ݿɱ�baud�� �����ڽ��գ�REN = 1��
    if(timer_sel==timer0)
    { 
        while(1)   // ����ʹ��timer0 ��Ϊ�����ʷ�����
        {
            LED0=~LED0; msDelay(500);
        }            
    }
    else if(timer_sel==timer1)               // 413AS ����ʹ��timer1 ��Ϊ�����ʷ����� 
    {
//        AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
//        AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
//        TMOD &= 0x0F;		//�趨��ʱ��1Ϊ16λ�Զ���װ��ʽ
//        TL1 = 0xCC;		//�趨��ʱ��ֵ
//        TH1 = 0xFF;		//�趨��ʱ��ֵ
//        ET1 = 0;		//��ֹ��ʱ��1�ж�
//        TR1 = 1;		//������ʱ��1
        AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
        AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
        TMOD &= 0x0F;		//�����ʱ��1ģʽλ
        TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
        TL1 = 0xF9;		//�趨��ʱ��ֵ
        TH1 = 0xF9;		//�趨��ʱ����װֵ
        ET1 = 0;		//��ֹ��ʱ��1�ж�
        TR1 = 1;		//������ʱ��1
        //STC12
        //TL1=0xC7;TH1=0xFE;                //9600bps @12.000MHz
        //TL1=0xFD;TH1=0xFD;(8λ�Զ���װ)   // 115200bps@11.0592MHz
        //STC15
        //TL1= 0xE6;TH1= 0xFF; //115200bps@ 12MHz
        //TL1= 0xC7;TH1= 0xFE; //9600bps@ 12MHz
    }
    else if(timer_sel==timer2)
    {
        AUXR |= 0x04; 
        AUXR |= 0x01;  //��ʱ�� Ϊ 1Tģʽ ѡ��ʱ��2Ϊbaud������ 
	T2L = 0xC7;			//���ö�ʱ��ʼֵ
	T2H = 0xFE;			//���ö�ʱ��ʼֵ
                              //T2L = 0xCC;T2H = 0xFF; 115200bps@24MHz
                              //           0xE6��0xFF;115200bps@12MHz
                              // C7 FE 19200bps@24MHz
        AUXR |=0x10; // ������ʱ��2 
        TI = 1;
    }
    ES = 0;  // �������ж� */
} 
/*********prepare to lanqiao competition*************************
#function: UART2_Init 
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description: ����2ֻ�ܲ���T2��Ϊ�����ʷ�����
#author::  2016-03-16 by'  yizhi 
******************************************************************/
void UART2_Init(void)
{
    P_SW2=0x01;  // �ܽ����ã�RXD:P4^6 TXD:P4^7
    S2CON=0x50; // ����ģʽ���� 8λ�ɱ䲨����
    AUXR |=0x04;// T2Ϊ1Tģʽ
    T2L=0xE6; 
    T2H=0xFF; // 115200bps @12M
    AUXR |=0x10;//��������ʱ��2 
    //IE2=0x01; // ʹ�� ����2�ж�
}


void  uartPutChar(Uart_e uartX,uchar dat)
{
    //while(UART_Busy);    // �ȴ�ǰ������ݷ������ ����ֺ���ĥ����,�����������һ�仰, ��ʱ��д��һ���ֺ����û���֡�
    // UARTcount++;         //  ��������һɾ�����ڷ��;ͳ���
    // UART_Busy=1; 
    /*
     * 2014��8��16������ û�����Ϊʲô���ж������Busy��־λ�Ͳ������ˣ
     * ����Ϊ��sometimes,TI��λ���޷����봮���жϣ���RI��λ����ԣ�����ֻ�ܲ��õȴ���
     * ��ʽ��ʵ�ִ��ڷ��� 
     * �������������������Դ��ʱ��ע�ͱʼǣ���ʱ��û��ѧϰ������ϵͳ���ź����ĸ�����Ѿ������뵽��UART_Busy�ķ�ʽ���������ڷ��͵�Ч���ˡ�
     */
    if(uartX==uart1)
    {
        while(TI == 0);      // �ȴ�ǰ������ݷ�����ɣ�������ɺ�TI����1  ���ڳ�ʼ����ʱ����Ҫ��TI��1. 
        SBUF=dat;
        TI = 0;
    }
    else if(uartX==uart2)
    {
        S2BUF=dat;
        while(!(S2CON&0x02));
        S2CON&=0xfd;
    }
    else
    {
        while(1)   // ����
        {
            LED0=~LED0; msDelay(200);
        }
    }
}

void uartPutBuf(Uart_e uartX,uint8 *buf,uint8 len)
{
    while(len--)
    {
        uartPutChar(uartX,*buf++); 
    }
}

void uartPutStr(Uart_e uartX,char *s)
{
     while(*s)
     uartPutChar(uartX,*s++);
}

void UART_Print(Uart_e uartX,char *s,uint m,uint n,uint o,uint32 p)
{
    uchar pdata ge,shi,bai,qian;
    char  temp[20];
    uartPutStr(uartX,s);
    ge=m%10;
    shi=m/10%10;
    bai=m/100%10;
    qian=m/1000;    
    uartPutChar(uartX,48+qian);
    uartPutChar(uartX,48+bai);
    uartPutChar(uartX,48+shi);
    uartPutChar(uartX,48+ge);
    uartPutChar(uartX,' ');
    ge=n%10;
    shi=n/10%10;
    bai=n/100%10;
    qian=n/1000;    
    uartPutChar(uartX,48+qian);
    uartPutChar(uartX,48+bai);
    uartPutChar(uartX,48+shi);
    uartPutChar(uartX,48+ge);
    uartPutChar(uartX,' ');
    ge=o%10;
    shi=o/10%10;
    bai=o/100%10;
    qian=o/1000;    
    uartPutChar(uartX,48+qian);
    uartPutChar(uartX,48+bai);
    uartPutChar(uartX,48+shi);
    uartPutChar(uartX,48+ge);
    uartPutChar(uartX,' '); 
//     ge=p%10;
//     shi=p/10%10;
//     bai=p/100%10;
//     qian=p/1000;    
//     uartPutChar(uartX,48+qian);
//     uartPutChar(uartX,48+bai);
//     uartPutChar(uartX,48+shi);
//     uartPutChar(uartX,48+ge);
//     uartPutChar(uartX,' '); 

   u32tostr(p,temp);
   uartPutStr(uartX,temp);
   uartPutStr(uartX,"\r\n");    
}



/*void UART2_printf(const char *fmt,...)  
{  
    va_list ap;  
    char xdata string[512];//�����ڲ���չRAM        
    va_start(ap,fmt);  
    vsprintf(string,fmt,ap);//ʹ��sprinf���
    UART_SendString(uart1,string);  
    va_end(ap);  
}  */



/******************************************************************
 - ������������һ��32λ�ı���datתΪ�ַ����������1234תΪ"1234"
 - ����ģ�飺��������ģ��
 - �������ԣ��ⲿ���û��ɵ���
 - ����˵����dat:��ת��long�͵ı���
             str:ָ���ַ������ָ�룬ת������ֽڴ���������           
 - ����˵������
 ******************************************************************/
void u32tostr(unsigned long dat,char *str) 
{
 char temp[20];
 unsigned char i=0,j=0;
 i=0;
 while(dat)
 {
  temp[i]=dat%10+0x30;
  i++;
  dat/=10;
 }
 j=i;
 for(i=0;i<j;i++)
 {
  str[i]=temp[j-i-1];
 }
 if(!i) {str[i++]='0';}
 str[i]=0;
}

/******************************************************************
 - ������������һ���ַ���תΪ32λ�ı���������"1234"תΪ1234
 - ����ģ�飺��������ģ��
 - �������ԣ��ⲿ���û��ɵ���
 - ����˵����str:ָ���ת�����ַ���           
 - ����˵����ת�������ֵ
 ******************************************************************/

unsigned long strtou32(char *str) 
{
 unsigned long temp=0;
 unsigned long fact=1;
 unsigned char len=strlen(str);
 unsigned char i;
 for(i=len;i>0;i--)
 {
  temp+=((str[i-1]-0x30)*fact);
  fact*=10;
 }
 return temp;
}
 
unsigned char    GucThrotSize[2];                              // max support 4 throttle cmd
short            Gq15ThrotCmd[2]; // _at_ 0x014A;
short            Gq15ReportData[4];
unsigned char    GbReportFlg = 0, GbSetupScopeSent = 0, GbBluetoothOK = 0;

void uartAppSendThrot(int16_t sValueRpm)
{
    uint8_t cSumCheck = 0;
	uint8_t counter = 1;
    int     qTemp1; 
    
    GucUartTxBuf[counter++] = 0x16;   // commander message
	cSumCheck += GucUartTxBuf[counter - 1];
	
    GucUartTxBuf[counter++] = 4u;     // length of payload
	cSumCheck += GucUartTxBuf[counter - 1];
	
    GucUartTxBuf[counter++] = 1u;     // number of var. to send
	cSumCheck += GucUartTxBuf[counter - 1];
	
    GucUartTxBuf[counter++] = 2u;     // size of var.
	cSumCheck += GucUartTxBuf[counter - 1];
    
    qTemp1 = (int)((float)sValueRpm * RPM2Q15_FACTOR);
    
    GucUartTxBuf[counter++] = *((uint8 *)(&qTemp1));
	cSumCheck += GucUartTxBuf[counter - 1];
	if (GucUartTxBuf[counter - 1] == 0x2B)
	{
		GucUartTxBuf[counter++] = 0x2B;
	}

    GucUartTxBuf[counter++] = *((uint8 *)(&qTemp1) + 1);
    cSumCheck += GucUartTxBuf[counter - 1];
	if (GucUartTxBuf[counter] == 0x2B)
	{
		GucUartTxBuf[counter++] = 0x2B;
	}

    GucUartTxBuf[counter++] = 0x100 - cSumCheck;
    
    
    uartPutBuf(uart1, GucUartTxBuf, counter);
}

void FMSTR_WriteVar16(uint16_t usAddr, int16_t sValue)
{
    uint8_t ucSumCheck = 0, counter = 1;
    
    GucUartTxBuf[counter] = 0xE4;   // commander message
    ucSumCheck += GucUartTxBuf[counter++];
    
    GucUartTxBuf[counter] = *((uint8_t *)&usAddr);
    if (GucUartTxBuf[counter] == 0x2B)
	{
		GucUartTxBuf[++counter] = 0x2B;
	}
    ucSumCheck += GucUartTxBuf[counter++];
    
    GucUartTxBuf[counter] = *((uint8_t *)&usAddr + 1);
    if (GucUartTxBuf[counter] == 0x2B)
	{
		GucUartTxBuf[++counter] = 0x2B;
	}
    ucSumCheck += GucUartTxBuf[counter++];
    
    GucUartTxBuf[counter] = *((uint8 *)(&sValue));
    if (GucUartTxBuf[counter] == 0x2B)
	{
		GucUartTxBuf[++counter] = 0x2B;
	}
    ucSumCheck += GucUartTxBuf[counter++];
    
    GucUartTxBuf[counter] = *((uint8 *)(&sValue) + 1);
    if (GucUartTxBuf[counter] == 0x2B)
	{
		GucUartTxBuf[++counter] = 0x2B;
	}
    ucSumCheck += GucUartTxBuf[counter++];

    GucUartTxBuf[counter++] = 0x100 - ucSumCheck;
    
    uartPutBuf(uart1, GucUartTxBuf, counter);
}

void FMSTR_WriteVar8(uint16_t usAddr, uint8_t ucValue)
{
    uint8_t ucSumCheck = 0, i;
    
    GucUartTxBuf[1] = 0xE3;   // commander message
    GucUartTxBuf[2] = *((uint8_t *)&usAddr);
    GucUartTxBuf[3] = *((uint8_t *)&usAddr + 1);
    GucUartTxBuf[4] = *((uint8 *)(&ucValue));
    GucUartTxBuf[5] = 0;
    
    for (i = 1; i < 6; i++) {
        ucSumCheck += GucUartTxBuf[i];
    }
    GucUartTxBuf[6] = 0x100 - ucSumCheck;
    
    uartPutBuf(uart1, GucUartTxBuf, 7);
}
/**
*********************************************************************************************************
** @nameis uartAppSetupScope
** @effect setupҪ��ȡ�ı���
** @import pAddr Ҫ��ȡ�����ĵ�ַ �� �����Ĵ�С��ucNum Ҫ��ȡ��������
** @export none
** @return none
** @create yizhi 2023.03.27
** @modify  
*********************************************************************************************************/
void uartAppSetupScope(uint16_t ppAddr[][2], uint8_t ucNum)
{
    uint8_t i, j = 1, ucSumCheck = 0;
    
    GucUartTxBuf[j++] = 0x08;   // commander message
    GucUartTxBuf[j++] = 1 + 3 * ucNum;   // length of payload
    GucUartTxBuf[j++] = ucNum;   // number of var's addrresses to send
    
    for (i = 0; i < ucNum; i++) {
        
        GucUartTxBuf[j++] = ppAddr[i][1];   // size of var1 
        GucUartTxBuf[j++] = *((uint8_t *)&ppAddr[i][0]);
        GucUartTxBuf[j++] = *((uint8_t *)&ppAddr[i][0] + 1);
    }
    
    for (i = 1; i < j; i++) {
        ucSumCheck +=GucUartTxBuf[i];
    }
    GucUartTxBuf[j] = 0x100 - ucSumCheck;   // checksum 
    
    uartPutBuf(uart1, GucUartTxBuf, j + 1);    // PS ��ʵ�⼸��App�������Խ���FreeMaster��Э��㷢�ͣ�Э���ÿ��ѭ������һ���ֽڸ�����
}

void uartAppReadScope(void)
{
    GucUartTxBuf[1] = 0xC5;   // commander message
    GucUartTxBuf[2] = 0x3B;   // checksum
    
    uartPutBuf(uart1, GucUartTxBuf, 3);
}

// ���ڵĶ���˼�� 
// �ӻ��ζ�����ȡһ�������� 
/*uchar GetOneByte(void)
{
     uchar pos;
    if(ByteNum>0) // ����������������δ����
    {
         pos=read_head;
        if(read_head+1==ByteMax)
        {
      read_head=0;            
    }
        else
          read_head=read_head+1;
        ByteNum--;
        return RoundBuffer[pos];
    }
    else
    {
        UART2_printf("The buffer is empty \n");
        return 0;
  }
}
// �뻷�ζ����з���һ��Ԫ�� 
void PutOneByte(uchar val)
{
     if(ByteNum<ByteMax) // ���еط�
     {
         RoundBuffer[write_head]=val;
    if(write_head+1==ByteMax)
        {
      write_head=0;            
    }
        else
          write_head=write_head+1;    
    ByteNum++;        
   }
     else
     {
         UART2_printf("The buffer is full \n");
   }
}
*/

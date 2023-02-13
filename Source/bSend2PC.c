#include "send.h"
#include "uart.h"
#include "common.h"

unsigned short CRC_CHECK(unsigned char *Buf ,unsigned char CRC_CNT)
{
    unsigned short pdata CRC_temp;
    unsigned char pdata i,j;
    CRC_temp=0xffff;
    for(i=0;i<CRC_CNT;i++)
    {
        CRC_temp=CRC_temp^Buf[i];
        for(j=0;j<8;j++)
        {    
            if(CRC_temp&0x01)
                
            {
                CRC_temp=(CRC_temp>>1)^0xa001;
                
            }
            else
            {
                CRC_temp=CRC_temp>> 1;
            }
            
        }      
    }
    return(CRC_temp);
}


/*void SeriPush2Bytes(short value)
{
	unsigned char t_char;
	if(value < 0) {
		value = -value;
		uart_putchar(UART0,0xf0);
	}
	else {
		uart_putchar(UART0,0xf5);     // 先发送符号位
	}
	t_char = value/256;

    uart_putchar(UART0,t_char &0x80); // 发第一个字节的最高位
	uart_putchar(UART0,t_char & 0x7f);// 发其余7位
	t_char = value%256;

    uart_putchar(UART0,t_char &0x80); // 发第二个字节的最高位
	uart_putchar(UART0,t_char & 0x7f);// 发其余7位
} 
void Xiaopi_Show(short data1, short data2, short data3)
{
	SeriPush2Bytes(data1);	     
	SeriPush2Bytes(data2);	     
	SeriPush2Bytes(data3);	       
	uart_putchar(UART0,0xff);   // 	    上位机所需的数据帧格式； 
}								   
*/
void Amo_Send(int x1,int x2,int x3,int x4)
{
   uchar pdata Dat[10],i;
   uint pdata CRC_temp=0;
   
   Dat[0]=x1&0x00ff;
   Dat[1]=(x1&0xff00)>>8;
   Dat[2]=x2&0x00ff;
   Dat[3]=(x2&0xff00)>>8;
   Dat[4]=x3&0x00ff;
   Dat[5]=(x3&0xff00)>>8;
   Dat[6]=x4&0x00ff;
   Dat[7]=(x4&0xff00)>>8;
   
   CRC_temp=CRC_CHECK(Dat,8);
   Dat[8]=CRC_temp%256;
   Dat[9]=CRC_temp/256;
   for(i=0;i<10;i++)
	{
		UART1_Putchar(Dat[i]);
	}
       
} 






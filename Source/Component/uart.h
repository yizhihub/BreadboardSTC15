#ifndef _uart_H_
#define _uart_H_

#include "common.h"
#include "timer.h"



#define FOSC 11059200L       // somewhere set the value 11059200L 
#define BAUD 9600


typedef enum UART_yizhi
{
	uart0=0,
	uart1,
	uart2,
	uartMax,
}Uart_e;


extern volatile uchar GbUartBusyFlg;
extern uint8_t xdata GucUartTxBuf[20];

extern short            Gq15ReportData[4];
extern unsigned char    GucThrotSize[2];                              // max support 4 throttle cmd
extern short            Gq15ThrotCmd[2]; // _at_ 0x014A;
extern unsigned char    GbReportFlg, GbSetupScopeSent;

void UART1_Init(Timer_e timer_sel); 
void UART2_Init(void);
//void UART1Indep_Init(void);
void uartPutBuf(Uart_e uart,uint8 *buf,uint8 len);
void  uartPutChar(Uart_e uart,uchar dat);
//void UART2_Putchar(uchar dat);
void uartPutStr(Uart_e uart,char *s);
//void UART2_SendString(char *s);
void UART_Print(Uart_e uart,char *s,uint m,uint n,uint o,uint32 p);
//void UART2_Print(char *s,uint m,uint n,uint o,uint p);
//void UART2_printf(const char *fmt,...);
void uartAppSendThrot(int16_t sValue);
void uartAppSetupScope(uint16_t usAddr1, uint16_t usAddr2);
void uartAppReadScope(void);



#endif
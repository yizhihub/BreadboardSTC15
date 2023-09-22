#ifndef _isr_h_
#define _isr_h_

#include "common.h"

extern volatile uchar TimeControl_Flag; // 控制每次大循环时间
extern volatile uchar NRF_RX_Flag;

extern volatile uchar  IR_OK_Flag; // 红外解码正确接收完成标志位
extern volatile uchar  IR_Buf[4];  

extern volatile uchar  GucT5msFlg;
extern volatile uchar  GucT1sFlg;
extern volatile uchar  GucEC11Flg;
#endif 
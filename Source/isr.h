#ifndef _isr_h_
#define _isr_h_

#include "common.h"

extern volatile uchar TimeControl_Flag; // ����ÿ�δ�ѭ��ʱ��
extern volatile uchar NRF_RX_Flag;

extern volatile uchar  IR_OK_Flag; // ���������ȷ������ɱ�־λ
extern volatile uchar  IR_Buf[4];  

extern volatile uchar  GucT5msFlg;
extern volatile uchar  GucT1sFlg;
extern volatile uchar  GucEC11Flg;
#endif 
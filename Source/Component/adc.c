#include "adc.h"
#include "intrins.h"

//========================================================================
// ����: void	ADC_Inilize(ADC_InitTypeDef *ADCx)
// ����: ADC��ʼ������.
// ����: ADCx: �ṹ����,��ο�adc.h��Ķ���.
// ����: none.
//========================================================================
void	ADC_Inilize(ADC_InitTypeDef *ADCx)
{
	P1ASF = ADCx->ADC_Px;
	ADC_CONTR = (ADC_CONTR & ~ADC_90T) | ADCx->ADC_Speed;
	if(ADCx->ADC_Power == ENABLE)	ADC_CONTR |= 0x80;
	else							ADC_CONTR &= 0x7F;
	if(ADCx->ADC_AdjResult == ADC_RES_H2L8)	PCON2 |=  (1<<5);	//10λAD����ĸ�2λ��ADC_RES�ĵ�2λ����8λ��ADC_RESL��
	else									PCON2 &= ~(1<<5);	//10λAD����ĸ�8λ��ADC_RES����2λ��ADC_RESL�ĵ�2λ��
	if(ADCx->ADC_Interrupt == ENABLE)	EADC = 1;			//�ж�����		ENABLE,DISABLE
	else								EADC = 0;
	if(ADCx->ADC_Polity == PolityHigh)	PADC = 1;		//���ȼ�����	PolityHigh,PolityLow
	else								PADC = 0;
}


//========================================================================
// ����: void	ADC_PowerControl(u8 pwr)
// ����: ADC��Դ���Ƴ���.
// ����: pwr: ��Դ����,ENABLE��DISABLE.
// ����: none.
//========================================================================
void	ADC_PowerControl(uchar pwr)
{
	if(pwr == ENABLE)	ADC_CONTR |= 0x80;
	else				ADC_CONTR &= 0x7f;
}

//========================================================================
// ����: u16	Get_ADC10bitResult(u8 channel)
// ����: ��ѯ����һ��ADC���.
// ����: channel: ѡ��Ҫת����ADC.
// ����: 10λADC���.
//========================================================================
uint	Get_ADC10bitResult(uchar channel)	//channel = 0~7
{
	uint	adc;
	uchar	i;

	if(channel > ADC_CH7)	return	1024;	//����,����1024,���õĳ����ж�	
	ADC_RES = 0;
	ADC_RESL = 0;

	ADC_CONTR = (ADC_CONTR & 0xe0) | ADC_START | channel; 
	//NOP(4);			//��ADC_CONTR������Ҫ4T֮����ܷ���
	_nop_();_nop_();_nop_();_nop_();

	for(i=0; i<250; i++)		//��ʱ
	{
		if(ADC_CONTR & ADC_FLAG)
		{
			ADC_CONTR &= ~ADC_FLAG;
			if(PCON2 &  (1<<5))		//10λAD����ĸ�2λ��ADC_RES�ĵ�2λ����8λ��ADC_RESL��
			{
				adc = (uint)(ADC_RES & 3);
				adc = (adc << 8) | ADC_RESL;
			}
			else		//10λAD����ĸ�8λ��ADC_RES����2λ��ADC_RESL�ĵ�2λ��
			{
				adc = (uint)ADC_RES;
				adc = (adc << 2) | (ADC_RESL & 3);
			}
			return	adc;
		}
	}
	return	1024;	//����,����1024,���õĳ����ж�
}


//========================================================================
// ����: void ADC_int(void) interrupt ADC_VECTOR
// ����: ADC�жϺ���.
// ����: none.
// ����: none.
// �汾: V1.0, 2012-10-22
//========================================================================
void ADC_int (void) interrupt 5
{
	ADC_CONTR &= ~ADC_FLAG;
}

void	ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;				//�ṹ����
	ADC_InitStructure.ADC_Px        = ADC_P13;	//����Ҫ��ADC��IO,	ADC_P10 ~ ADC_P17(�����),ADC_P1_All
	ADC_InitStructure.ADC_Speed     = ADC_540T;			//ADC�ٶ�			ADC_90T,ADC_180T,ADC_360T,ADC_540T
	ADC_InitStructure.ADC_Power     = ENABLE;			//ADC��������/�ر�	ENABLE,DISABLE
	ADC_InitStructure.ADC_AdjResult = ADC_RES_H8L2;		//ADC�������,	ADC_RES_H2L8,ADC_RES_H8L2
	ADC_InitStructure.ADC_Polity    = PolityLow;		//���ȼ�����	PolityHigh,PolityLow
	ADC_InitStructure.ADC_Interrupt = DISABLE;			//�ж�����		ENABLE,DISABLE
	ADC_Inilize(&ADC_InitStructure);					//��ʼ��
	ADC_PowerControl(ENABLE);							//������ADC��Դ��������, ENABLE��DISABLE
}


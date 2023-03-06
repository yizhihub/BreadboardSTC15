#include "common.h"
#include "isr.h"
#include "boled.h"
#include "bkey.h"
#include "bSHT3x.h"
#include "uart.h"
#include "adc.h"
#include "iic.h"
#include "freemaster.h"

RTC_Time_s GtTime = {0, 0, 0, 4, 5, 6, 7};

/**
 * int idata Vbg_RAM _at_ 0xef;                    // ��������ʱ��ȡFlashĩβд��idata��bandgap��ѹֵ��λmv  ���Ի�������
 * Bandgap stored address definition
 */
#if  (defined(IAP15W413ASDIP28) || defined(IAP15W413ASDIP16))
uint code  Vbg_ROM _at_ 0x33f7;                  /* STC-ISP���س���ʱд��Flashĩβ��bandgap��ѹֵ��λmv   */
#elif defined(STC15W408ASDIP16)
uint code  Vbg_ROM _at_ 0x1ff7;                  /* STC-ISP���س���ʱд��Flashĩβ��bandgap��ѹֵ��λmv   */
#else 
    #error "NO MCU SELECTED"
#endif

#define  VER_ID    "====BdbrdSTC===="
char code  pcVerStr1[] = __TIME__;
char code  pcVerStr2[] = __DATE__;
char code  pcVerStr3[] = VER_ID; 
char xdata cVerID[]    = VER_ID;
char xdata cBuidT[]    = __TIME__;

void port_init(void)
{
    P2 = 0xFC;               // ��������, �䵱�ӵء�
    LED0 = 0;                // P5.5�䵱OLED�ĵ�Դ�أ�ʱ��Ϊ�͵�ƽ��

#if defined(PORT_IIC)
    P1M1=0x03;
    P1M0=0x03; // 0000 0011 P1^0 P1^1 set to open drain for  IIC sda scl
#endif
}


void main(void)
{
    int16_t data sSpeedSet = 0, sSpdAct;
    float   data fVolAct;
    uint16_t data sVccPower, sVbgMv, sTimeCnt1 = 0;
    uint8_t  data ucTmp[2];
#if defined(PORT_IIC)
    int16_t sShtTemperature;
    uint8_t ucShtHumidity;
#endif
    KEYn_e eKeyPress;

    sVbgMv = Vbg_ROM;
    ucTmp[0] = ucTmp[1];
    EA = 0;
    port_init();
    msDelay(50);       // OLED�ϵ���ӳ�50ms
    OLED_Init(); 
    OLED_Fill(0xFF);   // FullRefresh time = 18.977ms @24MHz
#if defined(FEATURE_F8x16)
    OLED_P8x16Str(0, 0, cVerID, 0);
    OLED_P8x16Str(32, 2, pcVerStr1, 0);
    OLED_P8x16Str(16, 4, pcVerStr2, 0);
#endif
    msDelay(1000);
    OLED_Fill(0x00);
    
#if defined(FEATURE_HANZI)
    OLED_Print(0, 2, "��ǰ��ѹ:-----V");
    OLED_Print(0, 4, "��ǰת��:-----RPM");
    OLED_Print(0, 6, "�趨ת��:-----RPM");
#endif
    
//    OLED_P8x16Str(0, 2, "SpeedSet:1000RPM", 1);
//    OLED_P8x16Str(0, 4, "RX:", 1);
//    OLED_P8x16Str(0, 6, "KEY:", 1); 
    
	AUXR &= 0x7F;			//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;			//���ö�ʱ��ģʽ
	TL0 = 0xF0;				//���ö�ʱ��ʼֵ
	TH0 = 0xD8;				//���ö�ʱ��ʼֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ
    ET0 = 1;
    PT0 = 1;
    
    UART1_Init(timer2);    // ���Է�c��ֻ�ܲ���timer2��Ϊ����1�Ĳ����ʷ�����, IAP15F2K�ǿ���ѡ��timer1�ģ�Ӧ����413AS����֧�֡� 
    
    /*
     * ADC Init
     */
    ADC_config();
    
    EA = 1;
    
    while(1) {

//        if (sTimeCnt1++ == 2001) {
//            sTimeCnt1 = 0;
//            LED0= ~LED0;
//            OLED_P8x16Time(36, 0, &GtTime);
//            UART_Print(uart1, "���ԣ�", sSpeedSet, sSpeedSet, sSpeedSet, sSpeedSet);         msDelay(4); 
//        }
        if (GucT5msFlg) {
            GucT5msFlg = 0;
            eKeyPress = ADKey_Check();
            
            if (eKeyPress == KEY_UP) {
                sSpeedSet += 100;
            } else if (eKeyPress == KEY_DOWN) {
                sSpeedSet -= 100;
            } else if (eKeyPress == KEY_LEFT) {
                if (sSpeedSet == 0)
                    sSpeedSet = 500;
                else 
                    sSpeedSet = 0;
            } else if (eKeyPress == KEY_RIGHT) {
                ;
            } else {
                ;
            }
            if (sSpeedSet < 0) sSpeedSet = 0;
             
            /* 
             * �趨Ŀ��ת�� �Լ� ͨ���̶�ʱ����ȡʵ��ת�ٺ͵�ѹ��
             */
            if (eKeyPress != KEY_NONE) {
                switch (eKeyPress)
                {
                    case KEY_UP:
                    case KEY_DOWN:
                    case KEY_LEFT:
                        FMSTR_WriteVar16(ADDR_SPEEDREF, (int)((float)sSpeedSet * RPM2Q15_FACTOR));
                        OLED_P8x16Num(36 + 28, 6, sSpeedSet, 4);
                        break;
                    case KEY_RIGHT:
                        uartAppSetupScope(ADDR_BUSVOL, ADDR_SPEEDACT);
                        GbSetupScopeSent = 1;
                        break;
                    default :
                        break;
                }
            } else if (GbReportFlg){
                GbReportFlg = 0;
                fVolAct = Gq15ReportData[0] * (MAX_VOLTAG_SCALE / 32768.0f);
                sSpdAct = ((long)Gq15ReportData[1] * MAX_SPEED_SCALE) / 32768;
                OLED_P8x16Dot(36 + 28, 2, fVolAct, 1, 1);
                OLED_P8x16Num(36 + 28, 4, sSpdAct, 4);
           
            } else if (sTimeCnt1++ >= 59 && GbSetupScopeSent) {
                    sTimeCnt1 = 0;
                    uartAppReadScope();
            } else {     
                if (GucT1sFlg) {
                    GucT1sFlg = 0; 
                    /* LED0= ~LED0; */
                    sVccPower = Get_ADC10bitResult(0);
#if defined(PORT_IIC)
                    SHT3x_Read(&sShtTemperature, &ucShtHumidity);    // ���û�в�SHT3x�� ����û������ÿ��response�����Read������ǳ�����
                    OLED_P16x32Num(1, sShtTemperature, 1);
                    OLED_P8x16Dot(96, 6, ucShtHumidity, 0, 4);
#endif
                    
#if defined(FEATURE_F8x16)
                    OLED_P8x16Time(0, 0, &GtTime);
                    OLED_P8x16Dot(79, 0, (float)((long)sVbgMv * 1023 / sVccPower) / 1000.0f, 2, 1);
#elif defined(FEATURE_F6x8)
                    OLED_P6x8Time(0, 0, &GtTime);
                    OLED_P6x8Dot(92, 0, (float)((long)sVbgMv * 1023 / sVccPower) / 1000.0f, 2, 1);
#endif
                }
           }
        }
        FMSTR_Poll();
    }
}



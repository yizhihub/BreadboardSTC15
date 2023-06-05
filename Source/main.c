#include "common.h"
#include "isr.h"
#include "boled.h"
#include "bkey.h"
#include "bSHT3x.h"
#include "uart.h"
#include "adc.h"
#include "iic.h"
#include "freemaster.h"

sbit BT_STATE = P3^2;
RTC_Time_s GtTime = {0, 0, 0, 4, 5, 6, 7};

/**
 * int idata Vbg_RAM _at_ 0xef;                    // ��������ʱ��ȡFlashĩβд��idata��bandgap��ѹֵ��λmv  ���Ի�������
 * Bandgap stored address definition
 */
#if  (defined(IAP15W413ASDIP28) || defined(IAP15W413ASDIP16) || defined(IAP15W413ASSOP20))
uint code  Vbg_ROM _at_ 0x33f7;                  /* STC-ISP���س���ʱд��Flashĩβ��bandgap��ѹֵ��λmv   */
#elif defined(STC15W408ASDIP16)
uint code  Vbg_ROM _at_ 0x1ff7;                  /* STC-ISP���س���ʱд��Flashĩβ��bandgap��ѹֵ��λmv   */
#else 
    #error "NO MCU SELECTED"
#endif

#define  VER_ID    "=FU6812Remoter230509a"

char code  pcVerStr1[] = __TIME__;
char code  pcVerStr2[] = __DATE__;
char code  pcVerStr3[] = VER_ID; 
char xdata cVerID[]    = VER_ID;
char xdata cBuidT[]    = __TIME__;

/*
 *  ����һ��N�У�2�е����顣 ��һ��Ϊ�����õ�ַ���ڶ���Ϊ�����ô�С��
 */
uint16_t GVarAddrArray[4][2] = 
{
    {ADDR_BUSVOL,   2},
    {ADDR_SPEEDACT, 2},
    {0x0044,        2},                                                 /* fault.Current.Is*/
    {0x007F,        1},                                                 /* mcFaultSource   */
};


void port_init(void)
{
#if  (defined(IAP15W413ASDIP28) || defined(IAP15W413ASDIP16) || defined(STC15W408ASDIP16))	/*	*/
	
    P2 = 0xFC;               // ��������, �䵱�ӵء�
    LED0 = 0;                // P5.5�䵱OLED�ĵ�Դ�أ�ʱ��Ϊ�͵�ƽ��
#elif  defined(IAP15W413ASSOP20)
	
	P1 |= 0x03;				 // ��������, �䵱�ӵء�
	P3 |= 0xC0;
	P3 &= 0xCF;
#endif

#if defined(PORT_IIC)
    P1M1=0x03;
    P1M0=0x03; // 0000 0011 P1^0 P1^1 set to open drain for  IIC sda scl
#endif
    P3M1 = 0x00;
    P3M0 = 0x00; // P3^2 set to get Blueteeth's state.
}


void main(void)
{
    int16_t data sSpeedSet = 0, sSpdAct;
    int8_t  data mcFaultSource;
    float   data fVolAct, fIsAct;
    uint16_t data sVccPower, sVbgMv, sTimeCnt1 = 0;
    uint8_t  data ucTmp[2];
#if defined(PORT_IIC)
    int16_t sShtTemperature;
    uint8_t ucShtHumidity;
#endif
    KEYn_e eKeyPress;

    sVbgMv = Vbg_ROM + 20;
    ucTmp[0] = ucTmp[1];
    EA = 0;
    port_init();
    msDelay(50);       // OLED�ϵ���ӳ�50ms
    
//    OLED_Init(); 
//    OLED_Fill(0xFF);   // FullRefresh time = 18.977ms @24MHz
//    OLED_PutStr(0,  OLED_LINE0, cVerID,    6, 1);
//    OLED_PutStr(32, OLED_LINE2, pcVerStr1, 8, 1);
//    OLED_PutStr(16, OLED_LINE3, pcVerStr2, 8, 1);
//    
//    OLED_Fill(BLACK);
//    OLED_PutStr(0, OLED_LINE0, (uint8_t *)"Aa123456789~!{}|", 6, GREEN);
//    OLED_Print(0, OLED_LINE1, "���ֲ���:", RED);
//    msDelay(100);

//    while(1) 
//    {
//        static float t = 3.14f;
////        OLED_ShowChar(36,52,t,12,1);//��ʾASCII�ַ�	
////        OLED_ShowNum(94,52,t,3,12);	//��ʾASCII�ַ�����ֵ    
////        OLED_Refresh_Gram();//������ʾ��OLED
////        t++;
////        if(t>'~')t=' ';
//        OLED_PutNumber(92, OLED_LINE0, t, 2, 1, "��", 6, GREEN);
//        OLED_PutNumber(64, OLED_LINE1, t, 2, 1, "��", 8, YELLOW);
//        OLED_PutNumber(0,  OLED_LINE2, t, 4, 1, "��", 16,GREEN);
//        LED0=!LED0;
//        if (t < 65350)
//            t += 0.1;
//    }
    OLED_Init();
    OLED_Fill(0xFFFF);
    OLED_PutStr(0,  OLED_LINE0, cVerID,    8, RED);
    OLED_PutStr(32, OLED_LINE2, pcVerStr1, 8, YELLOW);
    OLED_PutStr(16, OLED_LINE3, pcVerStr2, 8, BLUE);
    msDelay(1500);
    OLED_Fill(0x00);

//    OLED_PutStr(0, OLED_LINE0 + (LINE_HEIGHT >> 1), cVerID, 6, 1);
    
    OLED_Print(0, OLED_LINE1, "��   ѹ:-----V", BLUE);
    OLED_Print(0, OLED_LINE2, "ת   ��:-----RPM", BLUE);
    OLED_Print(0, OLED_LINE3, "�趨ת��:-----RPM", BLUE);
    OLED_PutStr(0, OLED_LINE0 + (LINE_HEIGHT >> 1), "Fault:-- Is:---", 6, BLUE);
    
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
        if (GucT5msFlg) {
            GucT5msFlg = 0;
            eKeyPress = ADKey_Check();
            
            /* 
             * �趨Ŀ��ת�� �Լ� ͨ���̶�ʱ����ȡʵ��ת�ٺ͵�ѹ��
             */
            if (eKeyPress != KEY_NONE) {
                switch (eKeyPress)
                {
                    case KEY_UP:
                        if (sSpeedSet >= 500 && sSpeedSet < 4500) {
                            sSpeedSet += 50;
                            uartAppSendThrot(sSpeedSet);
                            OLED_PutNum(36 + 28, OLED_LINE3, sSpeedSet, 5, 8, RED);
                        }
                        break;
                        
                    case KEY_DOWN:
                        if (sSpeedSet > 500) {
                            sSpeedSet -= 50;
                            uartAppSendThrot(sSpeedSet);
                            OLED_PutNum(36 + 28, OLED_LINE3, sSpeedSet, 5, 8, RED);
                        }
                        break;
                        
                    case KEY_LEFT:
                        
                        if (sSpeedSet == 0) sSpeedSet = 800;
                        else                sSpeedSet = 0;
/*                        FMSTR_WriteVar16(ADDR_SPEEDREF, (int)((float)sSpeedSet * RPM2Q15_FACTOR));  */
                        uartAppSendThrot(sSpeedSet);
                        OLED_PutNum(36 + 28, OLED_LINE3, sSpeedSet, 5, 8, RED);
                        break;
                    
                    case KEY_RIGHT:
                        uartAppSetupScope(GVarAddrArray, 4);
                        break;
                    default :
                        break;
                }
            } else if (GbReportFlg){
                GbReportFlg = 0;
                fVolAct = Gq15ReportData[0] * (MAX_VOLTAG_SCALE / 32768.0f);
                sSpdAct = ((long)Gq15ReportData[1] * MAX_SPEED_SCALE) / 32768;
                fIsAct  = Gq15ReportData[2] * (MAX_CURRENT_SCALE / 32768.0f);
                mcFaultSource =  *((uint8 *)&Gq15ReportData[3]);
                OLED_PutNumber(36 + 28, OLED_LINE1, fVolAct, 3, 1,  0,  8, RED);
                OLED_PutNum(   36 + 28, OLED_LINE2, sSpdAct, 5, 8, RED);
                OLED_PutNum(   36,      OLED_LINE0 + (LINE_HEIGHT >> 1), mcFaultSource, 2, 6, GREEN);
                OLED_PutNumber(72,      OLED_LINE0 + (LINE_HEIGHT >> 1), fIsAct,  1, 1, "A", 6, GREEN); 
           
            } else if (sTimeCnt1++ >= 59 && GbSetupScopeSent) {
                    sTimeCnt1 = 0;
                    uartAppReadScope();
            } else {
                if (BT_STATE && (GbBluetoothOK == 0)) {
                    msDelay(20);
                    /*
                     * ��������FMSTR�������� uartAppSetupScope��uartAppSendThrot�����ڻᵼ�µڶ���ָ����λ�����ղ������߲�����ԭ��δ��̽����
                     * �ʲ�������취�ȷ���SetupScope, Ȼ��ִ����ʾ��������Ż��õ�10ms���ң�Ȼ����ʱ50ms(����)��Ȼ��ִ��SendThrot��
                     */
                    uartAppSetupScope(GVarAddrArray, 4);
                    OLED_PutStr(36, OLED_LINE0, "CONNECTED", 6, GREEN);
                    sSpeedSet = 800;
                    OLED_PutNum(36 + 28, OLED_LINE3, sSpeedSet, 5, 8, RED);
                    msDelay(50);
                    uartAppSendThrot(sSpeedSet);
                    
                    GbSetupScopeSent = 1;
                    GbBluetoothOK    = 1;
                }
                if (!BT_STATE && (GbBluetoothOK == 1)) {
                    OLED_PutStr(36, OLED_LINE0, "         ", 6, WHITE);
                    GbBluetoothOK = 0;
                    GbSetupScopeSent = 0;
                }
                if (GucT1sFlg) {
                    GucT1sFlg = 0; 
                    /* LED0= ~LED0; */
                    sVccPower = Get_ADC10bitResult(0);
#if defined(PORT_IIC)
                    SHT3x_Read(&sShtTemperature, &ucShtHumidity);    // ���û�в�SHT3x�� ����û������ÿ��response�����Read������ǳ�����
                    OLED_PutNumber(0, OLED_LINE2, sShtTemperature / 10.f, 2, 1, "��", 8, 1);
                    OLED_PutNumber(62,OLED_LINE2, ucShtHumidity,          2, 0, "%" , 8, 1);
#endif
                    OLED_PutTime(0, OLED_LINE0, &GtTime, 6, BLUE);
                    OLED_PutNumber(98, OLED_LINE0, (float)((long)sVbgMv * 1023 / sVccPower) / 1000.0f, 1, 2, "V", 6, BLUE);
                }
           }
        }
        FMSTR_Poll();
    }
}



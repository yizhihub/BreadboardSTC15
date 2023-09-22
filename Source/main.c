#include "common.h"
#include "isr.h"
#include "boled.h"
#include "bkey.h"
#include "bSHT3x.h"
#include "uart.h"
#include "adc.h"
#include "iic.h"
//#include "ringbuffer.h"
#include "freemaster.h"

#if defined(IAP15W4K61S4LQFP44)
sbit BT_STATE = P2^6;
#else
sbit BT_STATE = P3^2;
#endif

RTC_Time_s GtTime = {0, 0, 0, 4, 5, 6, 7};

/**
 * int idata Vbg_RAM _at_ 0xef;                    // ��������ʱ��ȡFlashĩβд��idata��bandgap��ѹֵ��λmv  ���Ի�������
 * Bandgap stored address definition
 */
#if  (defined(IAP15W413ASDIP28) || defined(IAP15W413ASDIP16) || defined(IAP15W413ASSOP20))
uint code  Vbg_ROM _at_ 0x33f7;                  /* STC-ISP���س���ʱд��Flashĩβ��bandgap��ѹֵ��λmv   */
#elif defined(STC15W408ASDIP16)
uint code  Vbg_ROM _at_ 0x1ff7;                  /* STC-ISP���س���ʱд��Flashĩβ��bandgap��ѹֵ��λmv   */
#elif defined(IAP15W4K61S4LQFP44)
uint code  Vbg_ROM _at_ 0xf3f7;
#else 
    #error "NO MCU SELECTED"
#endif

#define  VER_ID    "=FU6812Remoter230922a"
#define  VERIDLEN  20
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
    {ADDR_IS,       2},                                                 /* fault.Current.Is*/
    {ADDR_FAULT,    1},                                                 /* mcFaultSource   */
};

uint16_t GVarVerID[2][2] = 
{
    {ADDR_VERID1, VERIDLEN},
    {ADDR_BUILDT,  8},
};

//ring_buf_t rUart3RingBuf;
//char xdata cU3RxBuf[64];
uchar  GucVerIDRxFlg = 0;
char xdata cDataLen = VERIDLEN;
char xdata cFocVerID[VERIDLEN+1];// _at_ 0x100;
uint8_t ucBuf[] = {0x2B, 0x01, 0x03, 0x14, 0x00, 0xFE, 0xEA};

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
    P1M1 = 0x00;
    P1M0 = 0x00;
    
    P2M1 = 0x00;
    P2M0 = 0x00; // P3^2 set to get Blueteeth's state.
    
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
    cFocVerID[20] = '\0';
    sVbgMv = Vbg_ROM + 20;
    ucTmp[0] = ucTmp[1];
    EA = 0;
    port_init();
//    ring_buf_init(&rUart3RingBuf, cU3RxBuf, 64);
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
    OLED_Fill(0x0000);
    OLED_PutStr(0,  OLED_LINE0, cVerID,    8, RED);
    OLED_PutStr(32, OLED_LINE2, pcVerStr1, 8, YELLOW);
    OLED_PutStr(16, OLED_LINE3, pcVerStr2, 8, BLUE);
    msDelay(1500);
    OLED_Fill(0x00);
    
//    OLED_PutStr(0, OLED_LINE0 + (LINE_HEIGHT >> 1), cVerID, 6, 1);
    
    OLED_Print(0, OLED_LINE1, "��   ѹ:---- V", BLUE);
    OLED_Print(0, OLED_LINE2, "ת   ��:---- RPM", BLUE);
    OLED_Print(0, OLED_LINE3, "��   ��:---- RPM", BLUE);
    OLED_PutStr(0, OLED_LINE0, "Fault:- Is:---", 6, BLUE);
    
    /*
     * ��ʱ��0��ʼ�� 5ms
     */ 
    AUXR &= 0x7F;			//��ʱ��ʱ��12Tģʽ
    TMOD &= 0xF0;			//���ö�ʱ��ģʽ
    TL0 = 0xF0;				//���ö�ʱ��ʼֵ
    TH0 = 0xD8;				//���ö�ʱ��ʼֵ
    TF0 = 0;				//���TF0��־
    TR0 = 1;				//��ʱ��0��ʼ��ʱ
    ET0 = 1;
    PT0 = 1;
    
//    UART1_Init(timer2);    // ���Է���ֻ�ܲ���timer2��Ϊ����1�Ĳ����ʷ�����, IAP15F2K�ǿ���ѡ��timer1�ģ�Ӧ����413AS����֧�֡� 
    UART3_Init();
    FMSTR_Init();
    uartPutBuf(uart3, ucBuf, 7);
    
    /*
     * ADC Init
     */
    ADC_config();
    
    INT_CLKO |= 0x20;      //ʹ���ⲿ�ж�3
    EA = 1;
    
//    while(1) {
//        if (S3CON & 0x01) {
//            S3CON &= 0xFE;
//            ucTmp[0] = S3BUF;
//            S3BUF    = ucTmp[0];
//            while((S3CON & 0x02) == 0);
//            S3CON &= 0xFD;
//        } 
//    S3BUF    = ucTmp[0]; 
//     S3CON &= 0xFC;
//     S3CON |= 0x03;
    
//    OLED_PutStr(0, OLED_LINE0 + (LINE_HEIGHT >> 1), cFocVerID, 6, BLUE);
    
    while(1) {
//          uint8_t ucBuf[] = {0x2B, 0x01, 0x03, 0x17, 0x00, 0xFE, 0xE7};
//        if (S3CON&0x01)
//        {
//            BT_STATE = !BT_STATE;
//            S3CON &= 0xFE;
//        }
        if (GucT5msFlg == 1 && GucVerIDRxFlg == 1) {
            GucT5msFlg = 0;
            cDataLen = 7;
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
                            if (sSpeedSet > 4500)
                                sSpeedSet = 4500;
                            FMSTR_WriteVar16(ADDR_SPEEDREF, (int)((float)sSpeedSet * RPM2Q15_FACTOR));
//                            uartAppSendThrot(sSpeedSet);
                            OLED_PutNum(36 + 22, OLED_LINE3, sSpeedSet, 5, 8, RED);
                        }
                        break;
                        
                    case KEY_DOWN:
                        if (sSpeedSet > 500) {
                            sSpeedSet -= 50;
                            if (sSpeedSet < 500)
                                sSpeedSet = 500;
                            FMSTR_WriteVar16(ADDR_SPEEDREF, (int)((float)sSpeedSet * RPM2Q15_FACTOR));
//                            uartAppSendThrot(sSpeedSet);
                            OLED_PutNum(36 + 22, OLED_LINE3, sSpeedSet, 5, 8, RED);
                        }
                        break;
                        
                    case KEY_LEFT:
                        if (sSpeedSet == 0) sSpeedSet = 700;
                        else                sSpeedSet = 0;
                        FMSTR_WriteVar16(ADDR_SPEEDREF, (int)((float)sSpeedSet * RPM2Q15_FACTOR));
//                        uartAppSendThrot(sSpeedSet);
                        OLED_PutNum(36 + 22, OLED_LINE3, sSpeedSet, 5, 8, RED);
                        break;
                    
                    case KEY_RIGHT:
                        uartAppSetupScope(GVarAddrArray, 4);
//                        uartppGetVarVal(&GVarVerID[0][0]);
//                          S3CON &= 0xFD;
//                          uartPutBuf(uart3, ucBuf, 6);
//                          S3BUF=0xE7;
//                          S3CON &= 0xFD;
                          
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
                OLED_PutNumber(36 + 22, OLED_LINE1, fVolAct, 3, 1,  0,  8, RED);
                OLED_PutNum(   36 + 22, OLED_LINE2, sSpdAct, 5, 8, RED);
                OLED_PutNum(   30,      OLED_LINE0, mcFaultSource, 2, 6, GREEN);
                OLED_PutNumber(66,      OLED_LINE0, fIsAct,  2, 1, "A", 6, GREEN); 
           
            } else if (sTimeCnt1++ >= 59 && GbSetupScopeSent) {
                    sTimeCnt1 = 0;
                    uartAppReadScope();
                
                if (sSpeedSet == 0 && sSpdAct != 0) //�����һ�ΰ����ػ�δ��Ӧ���ٴη���
                    {
                        FMSTR_WriteVar16(ADDR_SPEEDREF, (int)((float)sSpeedSet * RPM2Q15_FACTOR));
                    }
                    
            } else if (GucEC11Flg){
                switch (GucEC11Flg)
                {
                    case 1:
                        if (sSpeedSet >= 500 && sSpeedSet < 4500) {
                            sSpeedSet += 10;
                            if (sSpeedSet > 4500)
                                sSpeedSet = 4500;
                            FMSTR_WriteVar16(ADDR_SPEEDREF, (int)((float)sSpeedSet * RPM2Q15_FACTOR));
//                            uartAppSendThrot(sSpeedSet);
                            OLED_PutNum(36 + 22, OLED_LINE3, sSpeedSet, 5, 8, RED);
                        }
                        break;
                    case 2:
                        if (sSpeedSet > 500) {
                            sSpeedSet -= 10;
                            if (sSpeedSet < 500)
                                sSpeedSet = 500;
                            FMSTR_WriteVar16(ADDR_SPEEDREF, (int)((float)sSpeedSet * RPM2Q15_FACTOR));
//                            uartAppSendThrot(sSpeedSet);
                            OLED_PutNum(36 + 22, OLED_LINE3, sSpeedSet, 5, 8, RED);
                        }
                        break;
                    default:
                        break;
                }
                GucEC11Flg = 0;
            }else {
                if (BT_STATE && (GbBluetoothOK == 0)) {
                    msDelay(20);
                    /*
                     * ��������FMSTR�������� uartAppSetupScope��uartAppSendThrot�����ڻᵼ�µڶ���ָ����λ�����ղ������߲�����ԭ��δ��̽����
                     * �ʲ�������취�ȷ���SetupScope, Ȼ��ִ����ʾ��������Ż��õ�10ms���ң�Ȼ����ʱ50ms(����)��Ȼ��ִ��SendThrot��
                     */
                    uartAppSetupScope(GVarAddrArray, 4);
//                    OLED_PutStr(36, OLED_LINE0, "CONNECTED", 6, GREEN);
                    sSpeedSet = 700;
                    OLED_PutNum(36 + 22, OLED_LINE3, sSpeedSet, 5, 8, RED);
                    msDelay(50);
                    FMSTR_WriteVar16(ADDR_SPEEDREF, (int)((float)sSpeedSet * RPM2Q15_FACTOR));
//                    uartAppSendThrot(sSpeedSet);
                    
                    GbSetupScopeSent = 1;
                    GbBluetoothOK    = 1;
                }
                if (!BT_STATE && (GbBluetoothOK == 1)) {
//                    OLED_PutStr(36, OLED_LINE0, "         ", 6, WHITE);
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
//                    OLED_PutTime(0, OLED_LINE0, &GtTime, 6, BLUE);
                    OLED_PutNumber(98, OLED_LINE0, (float)((long)sVbgMv * 1023 / sVccPower) / 1000.0f, 1, 2, "V", 6, BLUE);
                    OLED_PutStr(0, OLED_LINE0 + (LINE_HEIGHT >> 1), cFocVerID, 6, BLUE);
                }
           }
        }
        FMSTR_Poll();
    }
}



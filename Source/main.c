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
 * int idata Vbg_RAM _at_ 0xef;                    // 程序运行时读取Flash末尾写入idata的bandgap电压值单位mv  测试会有问题
 * Bandgap stored address definition
 */
#if  (defined(IAP15W413ASDIP28) || defined(IAP15W413ASDIP16) || defined(IAP15W413ASSOP20))
uint code  Vbg_ROM _at_ 0x33f7;                  /* STC-ISP下载程序时写入Flash末尾的bandgap电压值单位mv   */
#elif defined(STC15W408ASDIP16)
uint code  Vbg_ROM _at_ 0x1ff7;                  /* STC-ISP下载程序时写入Flash末尾的bandgap电压值单位mv   */
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
 *  设立一个N行，2列的数组。 第一列为变量得地址，第二列为变量得大小。
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
	
    P2 = 0xFC;               // 按键配置, 充当接地。
    LED0 = 0;                // P5.5充当OLED的电源地，时钟为低电平。
#elif  defined(IAP15W413ASSOP20)
	
	P1 |= 0x03;				 // 按键配置, 充当接地。
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
    msDelay(50);       // OLED上电后延迟50ms
    
//    OLED_Init(); 
//    OLED_Fill(0xFF);   // FullRefresh time = 18.977ms @24MHz
//    OLED_PutStr(0,  OLED_LINE0, cVerID,    6, 1);
//    OLED_PutStr(32, OLED_LINE2, pcVerStr1, 8, 1);
//    OLED_PutStr(16, OLED_LINE3, pcVerStr2, 8, 1);
//    
//    OLED_Fill(BLACK);
//    OLED_PutStr(0, OLED_LINE0, (uint8_t *)"Aa123456789~!{}|", 6, GREEN);
//    OLED_Print(0, OLED_LINE1, "汉字测试:", RED);
//    msDelay(100);

//    while(1) 
//    {
//        static float t = 3.14f;
////        OLED_ShowChar(36,52,t,12,1);//显示ASCII字符	
////        OLED_ShowNum(94,52,t,3,12);	//显示ASCII字符的码值    
////        OLED_Refresh_Gram();//更新显示到OLED
////        t++;
////        if(t>'~')t=' ';
//        OLED_PutNumber(92, OLED_LINE0, t, 2, 1, "℃", 6, GREEN);
//        OLED_PutNumber(64, OLED_LINE1, t, 2, 1, "℃", 8, YELLOW);
//        OLED_PutNumber(0,  OLED_LINE2, t, 4, 1, "℃", 16,GREEN);
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
    
    OLED_Print(0, OLED_LINE1, "电   压:-----V", BLUE);
    OLED_Print(0, OLED_LINE2, "转   速:-----RPM", BLUE);
    OLED_Print(0, OLED_LINE3, "设定转速:-----RPM", BLUE);
    OLED_PutStr(0, OLED_LINE0 + (LINE_HEIGHT >> 1), "Fault:-- Is:---", 6, BLUE);
    
    AUXR &= 0x7F;			//定时器时钟12T模式
    TMOD &= 0xF0;			//设置定时器模式
    TL0 = 0xF0;				//设置定时初始值
    TH0 = 0xD8;				//设置定时初始值
    TF0 = 0;				//清除TF0标志
    TR0 = 1;				//定时器0开始计时
    ET0 = 1;
    PT0 = 1;
    
    UART1_Init(timer2);    // 测试发c现只能采用timer2作为串口1的波特率发生器, IAP15F2K是可以选用timer1的，应该是413AS本身不支持。 
    
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
             * 设定目标转速 以及 通过固定时基获取实际转速和电压。
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
                     * 下面两条FMSTR发送命令 uartAppSetupScope、uartAppSendThrot若仅邻会导致第二条指令下位机接收不到或者不处理，原因未作探究。
                     * 故采用下面办法先发送SetupScope, 然后执行显示函数，大概会用掉10ms左右，然后延时50ms(必须)，然后执行SendThrot。
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
                    SHT3x_Read(&sShtTemperature, &ucShtHumidity);    // 如果没有插SHT3x， 由于没有器件每次response，这个Read操作会非常慢。
                    OLED_PutNumber(0, OLED_LINE2, sShtTemperature / 10.f, 2, 1, "℃", 8, 1);
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



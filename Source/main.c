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
 * int idata Vbg_RAM _at_ 0xef;                    // 程序运行时读取Flash末尾写入idata的bandgap电压值单位mv  测试会有问题
 * Bandgap stored address definition
 */
#if  (defined(IAP15W413ASDIP28) || defined(IAP15W413ASDIP16))
uint code  Vbg_ROM _at_ 0x33f7;                  /* STC-ISP下载程序时写入Flash末尾的bandgap电压值单位mv   */
#elif defined(STC15W408ASDIP16)
uint code  Vbg_ROM _at_ 0x1ff7;                  /* STC-ISP下载程序时写入Flash末尾的bandgap电压值单位mv   */
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
    P2 = 0xFC;               // 按键配置, 充当接地。
    LED0 = 0;                // P5.5充当OLED的电源地，时钟为低电平。

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
    msDelay(50);       // OLED上电后延迟50ms
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
    OLED_Print(0, 2, "当前电压:-----V");
    OLED_Print(0, 4, "当前转速:-----RPM");
    OLED_Print(0, 6, "设定转速:-----RPM");
#endif
    
//    OLED_P8x16Str(0, 2, "SpeedSet:1000RPM", 1);
//    OLED_P8x16Str(0, 4, "RX:", 1);
//    OLED_P8x16Str(0, 6, "KEY:", 1); 
    
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

//        if (sTimeCnt1++ == 2001) {
//            sTimeCnt1 = 0;
//            LED0= ~LED0;
//            OLED_P8x16Time(36, 0, &GtTime);
//            UART_Print(uart1, "测试：", sSpeedSet, sSpeedSet, sSpeedSet, sSpeedSet);         msDelay(4); 
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
             * 设定目标转速 以及 通过固定时基获取实际转速和电压。
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
                    SHT3x_Read(&sShtTemperature, &ucShtHumidity);    // 如果没有插SHT3x， 由于没有器件每次response，这个Read操作会非常慢。
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



#include "common.h"
#include "isr.h"
#include "boled.h"
#include "bLCD.h"
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
#if  (defined(IAP15W413ASDIP28) || defined(IAP15W413ASDIP16))
uint code  Vbg_ROM _at_ 0x33f7;                  /* STC-ISP下载程序时写入Flash末尾的bandgap电压值单位mv   */
#elif defined(STC15W408ASDIP16)
uint code  Vbg_ROM _at_ 0x1ff7;                  /* STC-ISP下载程序时写入Flash末尾的bandgap电压值单位mv   */
#else 
    #error "NO MCU SELECTED"
#endif

#define  VER_ID    "=FU6812Remoter230317a"

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
    P3M1 = 0x04;
    P3M0 = 0x00; // P3^2 set to get Blueteeth's state.
}


void main(void)
{
#if 0
	KEYn_e eKeyPress;
	unsigned short data COLOR = 30;
	  port_init();
	
     LCD_Init();//LCD初始化
	  UART1_Init(timer2);    
	//LCD_Fill(0,0,LCD_W,LCD_H,0X8F00);
	while(1)
	{
//		msDelay(5);
		LCD_Fill(0,0,LCD_W,LCD_H,COLOR); 
		eKeyPress = ADKey_Check();
//		if (eKeyPress == KEY_UP) {
		    COLOR++;
//	    }  else if (eKeyPress == KEY_DOWN) {
//			COLOR--;
//	    }
		UART_Print(uart1, "The value:", 1,0,0,COLOR);
		
	}
#else

    int16_t data sSpeedSet = 0, sSpdAct;
    float   data fVolAct;
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
	
#if 0
    OLED_Init(); 
    OLED_Fill(0xFF);   // FullRefresh time = 18.977ms @24MHz
#if defined(FEATURE_F8x16)
    OLED_P8x16Str(0, OLED_LINE0, cVerID, 0);
    OLED_P8x16Str(32, OLED_LINE2, pcVerStr1, 0);
    OLED_P8x16Str(16, OLED_LINE3, pcVerStr2, 0);
#endif

#else
	LCD_Init();
	LCD_Fill(0,0,LCD_W,LCD_H,BCOLOR); 
	msDelay(500);
#if defined(FEATURE_F8x16)
//    LCD_ShowString(0, 0, cVerID, FCOLOR, BCOLOR, 8, 0);
//    LCD_ShowString(32, 47, pcVerStr1, RED, BCOLOR, 8, 0);
//    LCD_ShowString(16, 63, pcVerStr2, BLUE, BCOLOR, 8, 0);
	OLED_P8x16Str(0, OLED_LINE0, cVerID, RED);
    OLED_P8x16Str(32, OLED_LINE2, pcVerStr1, YELLOW);
    OLED_P8x16Str(16, OLED_LINE3, pcVerStr2, BLUE);
#endif
#endif
	
    msDelay(1500);
    OLED_Fill(WHITE);
	
    
#if defined(FEATURE_F6x8)
    OLED_P6x8Str(0, OLED_LINE0 + (LINE_HEIGHT > 1), cVerID, 1);
#endif
    
#if defined(FEATURE_HANZI)
    OLED_P8x16Str(0, OLED_LINE1, "CurreVol:-----V", BLUE);
    OLED_P8x16Str(0, OLED_LINE2, "CurSpeed:-----RPM", BLUE);
    OLED_P8x16Str(0, OLED_LINE3, "SetSpeed:-----RPM", BLUE);
#endif
   
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
                        if (sSpeedSet >= 500 && sSpeedSet < 2200) {
                            sSpeedSet += 50;
                            uartAppSendThrot(sSpeedSet);
                            OLED_P8x16Num(36 + 28, OLED_LINE3, sSpeedSet, 5, RED);
                        }
                        break;
                        
                    case KEY_DOWN:
                        if (sSpeedSet > 500) {
                            sSpeedSet -= 50;
                            uartAppSendThrot(sSpeedSet);
                            OLED_P8x16Num(36 + 28, OLED_LINE3, sSpeedSet, 5, RED);
                        }
                        break;
                        
                    case KEY_LEFT:
                        
                        if (sSpeedSet == 0) sSpeedSet = 800;
                        else                sSpeedSet = 0;
/*                        FMSTR_WriteVar16(ADDR_SPEEDREF, (int)((float)sSpeedSet * RPM2Q15_FACTOR));  */
                        uartAppSendThrot(sSpeedSet);
                        OLED_P8x16Num(36 + 28, OLED_LINE3, sSpeedSet, 5, RED);
                        break;
                    
                    case KEY_RIGHT:
                        uartAppSetupScope(ADDR_BUSVOL, ADDR_SPEEDACT);
                        break;
                    default :
                        break;
                }
            } else if (GbReportFlg){
                GbReportFlg = 0;
                fVolAct = Gq15ReportData[0] * (MAX_VOLTAG_SCALE / 32768.0f);
                sSpdAct = ((long)Gq15ReportData[1] * MAX_SPEED_SCALE) / 32768;
                OLED_P8x16Dot(36 + 28, OLED_LINE1, fVolAct, 1, 1, RED);
                OLED_P8x16Num(36 + 28, OLED_LINE2, sSpdAct, 5, RED);
           
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
                    uartAppSetupScope(ADDR_BUSVOL, ADDR_SPEEDACT);
                    OLED_P8x16Str(36, OLED_LINE4, "CONNECTED", GREEN);
                    sSpeedSet = 800;
                    OLED_P8x16Num(36 + 28, OLED_LINE3, sSpeedSet, 5, RED);
                    msDelay(50);
                    uartAppSendThrot(sSpeedSet);
                    
                    GbSetupScopeSent = 1;
                    GbBluetoothOK    = 1;
                }
                if (!BT_STATE && (GbBluetoothOK == 1)) {
                    OLED_P8x16Str(36, OLED_LINE4, "         ", WHITE);
                    GbBluetoothOK = 0;
                    GbSetupScopeSent = 0;
                }
                if (GucT1sFlg) {
                    GucT1sFlg = 0; 
                    /* LED0= ~LED0; */
                    sVccPower = Get_ADC10bitResult(0);
#if defined(PORT_IIC)
                    SHT3x_Read(&sShtTemperature, &ucShtHumidity);
#endif
                    
#if defined(FEATURE_F6x8)
                    OLED_P6x8Time(0, OLED_LINE0, &GtTime, 1);
                    OLED_P6x8Dot(92, OLED_LINE0, (float)((long)sVbgMv * 1023 / sVccPower) / 1000.0f, 2, 1, 1);
#elif defined(FEATURE_F8x16)
                    OLED_P8x16Time(0, OLED_LINE0, &GtTime, BLUE);
                    OLED_P8x16Dot(92, OLED_LINE0, (float)((long)sVbgMv * 1023 / sVccPower) / 1000.0f, 2, 1, BLUE);
#endif
                }
           }
        }
        FMSTR_Poll();
    }
	
#endif
}



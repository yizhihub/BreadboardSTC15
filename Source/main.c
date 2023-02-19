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

//int idata Vbg_RAM _at_ 0xef;                    /* 程序运行时读取Flash末尾写入idata的bandgap电压值单位mv */
uint code  Vbg_ROM _at_ 0x1ff7;                  /* STC-ISP下载程序时写入Flash末尾的bandgap电压值单位mv   */

void port_init(void)
{
    P2 = 0xFC;               // 按键配置, 充当接地。
    LED0 = 0;                // P5.5充当OLED的电源地，时钟为低电平。

    P1M1=0x03;
    P1M0=0x03; // 0000 0011 P1^0 P1^1 set to open drain for  IIC sda scl
}


void main(void)
{
   int16_t data sSpeedSet = 0, sSpdAct;
   float   data fVolAct;
   uint16_t data sVccPower, sVbgMv;
   uint8_t  data ucTmp;
   KEYn_e eKeyPress;
  
   sVbgMv = Vbg_ROM;
//    P5M1 = 0xFF;
//    P5M0 = 0xFF;
    EA = 0;
    port_init();
    msDelay(50);       // OLED上电后延迟50ms
    OLED_Init(); 
    OLED_Fill(0xFF);   // FullRefresh time = 18.977ms @24MHz
    msDelay(500);
    OLED_Fill(0x00);
    
//    OLED_Print(87,0, "3.79V");
//    OLED_Print(0, 2, "当前电压:");
//    OLED_Print(0, 4, "当前转速:+0000RPM");
//    OLED_Print(0, 6, "设定转速:+0000RPM");

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
//            if (eKeyPress != KEY_NONE) {
//                switch (eKeyPress)
//                {
//                    case KEY_UP:
//                    case KEY_DOWN:
//                    case KEY_LEFT:
//                        uartAppSendThrot(sSpeedSet);
//                        OLED_P8x16Four(36 + 28, 6, sSpeedSet);
//                        break;
//                    case KEY_RIGHT:
//                        uartAppSetupScope(0, 0);
//                        GbSetupScopeSent = 1;
//                        break;
//                    default :
//                        break;
//                }
//            } else if (GbReportFlg){
//                GbReportFlg = 0;
//                fVolAct = Gq15ReportData[0] * (732.941f / 32768.0f);
//                sSpdAct = ((long)Gq15ReportData[1] * 6000) / 32768;
//                OLED_P8x16Dot(36 + 28, 2, fVolAct, 1, 1);
//                OLED_P8x16Four(36 + 28, 4, sSpdAct);
//           
//            } else if (sTimeCnt1++ >= 59 && GbSetupScopeSent) {
//                    sTimeCnt1 = 0;
//                    uartAppReadScope();
/*            } else {     */
                if (GucT1sFlg) {
                    GucT1sFlg = 0; 
                    /* LED0= ~LED0; */
//                    P1 ^= 0X03;   ucTmp = IIC_Read1(0x01, 0x02);
                    

//                    i2cStart();                       //准备接受数据
//                    i2cWriteByte((0x44 << 1) | 0x01);    //采用地址B，并左移一位，补1，准备读数据。
//                          SHT3X_CMD_READ_SERIAL_NUMBER
                    i2cStart();
                    i2cWriteByte((0x44 << 1) & 0xFE);
                    if (i2cRespons() == 0) {
                         OLED_P8x16Str(0, 0, "OK", 0);
                    } else {
                         OLED_P8x16Str(0, 0, "Fail", 0);
                    }
//                    OLED_P8x16Time(0, 0, &GtTime);
                    sVccPower = Get_ADC10bitResult(0);
                    OLED_P8x16Dot(79, 0, (float)((long)sVbgMv * 1023 / sVccPower) / 1000.0f, 2, 1);
                }
/*          }      */
            
        }
//        FMSTR_Poll();
    }
}



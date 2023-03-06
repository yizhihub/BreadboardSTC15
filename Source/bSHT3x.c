/*!
 * @file DFRo
 * @brief Defass
 * @details Tmidity sensors used to drive the SHT3x series SHT30, 
 * @n SHT31
 * @copyrightww
 * @License  
 * @author [f
 * @version  
 * @date  201
 * @url 
 */
#include "common.h"
#include "IIC.h"
#include "bOLED.h"
#include "bSHT3x.h"
 
static void SHT3x_Debug(uint8_t ucStat)
{
#if  0
    static uint8_t ucX = 0, ucY = 4;

    if (ucStat > 1) {
        ucX = 0;
        ucY = 4;
        return;
    }
#if defined(FEATURE_F8x16)
    OLED_P8x16Str(ucX, ucY, "A", ucStat);
    ucX += 6;
#elif defined(FEATURE_F6x8)
    OLED_P6x8Str(ucX, ucY, "A", ucStat);
    ucX += 6;
#endif
    if (ucX >= 126) {
        ucX = 0;
        ucY += 2;
        if (ucY >= 8) {
            ucY = 4;
            OLED_Fill(0x00);
        }
    }
#endif
}
/**
* @nameis SHT3x_Test
* @effect Test the sensorGet the measured temperature (in degrees Celsius)
* @import none
* @export
* @return Return the float temperature data 
*/
void SHT3x_Test(void)
{
    uint8_t  ucResponsStat;
    uint16_t usStat;

//  i2cStart();                       //准备接受数据
//  i2cWriteByte((0x44 << 1) | 0x01);    //采用地址B，并左移一位，补1，准备读数据。
//  SHT3X_CMD_READ_SERIAL_NUMBER
    i2cStart();
    ucResponsStat = i2cWriteByte((SHT3x_ADDRESS << 1) & 0xFE);                       SHT3x_Debug(ucResponsStat); 
    ucResponsStat = i2cWriteByte((SHT3X_READ_STATUS_REG >> 8) & 0xFF);      SHT3x_Debug(ucResponsStat);
    ucResponsStat = i2cWriteByte(SHT3X_READ_STATUS_REG & 0xFF);             SHT3x_Debug(ucResponsStat);
    i2cStart();
    ucResponsStat = i2cWriteByte((SHT3x_ADDRESS << 1) | 0x01);                       SHT3x_Debug(ucResponsStat);
    usStat  = i2cReadByte(1); usStat <<= 8;
    usStat |= i2cReadByte(0);
    i2cStop();
#if defined(FEATURE_F8x16)
    OLED_HexDisp(48, 0, (uint8*)&usStat, 2);
#endif
}
/**
* @nameis SHT3x_Test
* @effect Get the measured temperature (in degrees Celsius * 10) and Humidity (in %)
* @import pointer
* @export pTemp,pHumi
* @return none
*/
void SHT3x_Read(int16_t *psTemp, uint8_t *pucHumi)
{
    uint8_t  ucResponsStat;
    uint16_t usTemperature, usHumidity;
    uint8_t  ucCrc[2];

//  i2cStart();                       //准备接受数据
//  i2cWriteByte((0x44 << 1) | 0x01);    //采用地址B，并左移一位，补1，准备读数据。
//  SHT3X_CMD_READ_SERIAL_NUMBER
    i2cStart();
    ucResponsStat = i2cWriteByte((SHT3x_ADDRESS << 1) & 0xFE);               SHT3x_Debug(ucResponsStat); 
    ucResponsStat = i2cWriteByte((SHT3X_GETDATA_H_CLOCKENBLED >> 8) & 0xFF); SHT3x_Debug(ucResponsStat);
    ucResponsStat = i2cWriteByte(SHT3X_GETDATA_H_CLOCKENBLED & 0xFF);        SHT3x_Debug(ucResponsStat);
    i2cStop();
    msDelay(1);                          // SCL free mesurement ongoing  reference chapter4.3 diagram datasheet
    i2cStart();
    ucResponsStat = i2cWriteByte((SHT3x_ADDRESS << 1) | 0x01);               SHT3x_Debug(ucResponsStat);
    scl = 1;
    while(scl == 0);                     // waite the sensor , the sensor will put scl low while measuring.
    usTemperature  = i2cReadByte(1); usTemperature <<= 8;
    usTemperature |= i2cReadByte(1);
    ucCrc[0]      = i2cReadByte(1);
    usHumidity     = i2cReadByte(1); usHumidity <<= 8;
    usHumidity    |= i2cReadByte(1);
    ucCrc[1]      = i2cReadByte(0);
    i2cStop();
    
    *psTemp  = -450 + (((long)1750 * usTemperature) >> 16) ;
    *pucHumi = ((long)100 *usHumidity) >> 16;

//#if defined(FEATURE_F8x16)
//    OLED_P8x16Dot(0, 2, (*psTemp) / 10.0f, 1, 0);
//    OLED_P8x16Dot(79,2, (*pucHumi),        0, 4);
//#elif defined(FEATURE_F6x8)
//    OLED_P6x8Dot(0, 1,  (*psTemp) / 10.0f, 1, 0);
//    OLED_P6x8Dot(36, 1, (*pucHumi),        0, 4);
//#endif
}

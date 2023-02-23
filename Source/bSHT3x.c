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
 
uint16_t usStat;

static void SHT3x_Debug(uint8_t ucStat)
{
    static uint8_t ucX = 0, ucY = 0;

    if (ucStat > 1) {
        ucX = 0;
        ucY = 0;
        return;
    }
    OLED_P8x16Str(0, 0, "A", ucStat); 
    ucX += 8;
    if (ucX == 128) {
        ucX = 0;
        ucY += 2;
        if (ucY == 8) {
            ucY = 0;
        }
    }
}

void SHT3x_Test(void)
{
    uint8_t  ucResponsStat;

//  i2cStart();                       //准备接受数据
//  i2cWriteByte((0x44 << 1) | 0x01);    //采用地址B，并左移一位，补1，准备读数据。
//  SHT3X_CMD_READ_SERIAL_NUMBER
    i2cStart();
    ucResponsStat = i2cWriteByte((0x44 << 1) & 0xFE);                       SHT3x_Debug(ucResponsStat); 
    ucResponsStat = i2cWriteByte((SHT3X_READ_STATUS_REG >> 8) & 0xFF);      SHT3x_Debug(ucResponsStat);
    ucResponsStat = i2cWriteByte(SHT3X_READ_STATUS_REG & 0xFF);             SHT3x_Debug(ucResponsStat);
    i2cStart();
    ucResponsStat = i2cWriteByte((0x44 << 1) | 0x01);                       SHT3x_Debug(ucResponsStat);
    usStat = i2cReadByte(1); usStat <<= 8;
    usStat |= i2cReadByte(0);
    i2cStop();
    OLED_HexDisp(0, 2, (uint8*)&usStat, 2);
}

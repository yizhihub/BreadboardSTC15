#include "soft-spi.h"

/* ------------------------spi operation function------------------------ */
static void SPI_Write_Bus(uint8_t dat)
{
	unsigned char i=8;
	OLED_CS_0;

	while(i--)
	{
		OLED_SCL_0;
		if(dat&0x80)            // 注意此处若为51单片机，采用CY位的方法速度略有提升。f = 340KHz -> 390KHz @12MHz
		  {OLED_SDA_1;}
		else
		  {OLED_SDA_0;}
		//NOP();NOP();// yanshi  
		OLED_SCL_1;
	//  NOP();NOP();// yanshi
		dat<<=1;
	}
	OLED_CS_1;
}
void SPI_WrDat(unsigned char dat)
{
	SPI_Write_Bus(dat);
}

void SPI_WrCmd(unsigned char cmd)
{
	OLED_DC_0;
	SPI_Write_Bus(cmd);
	OLED_DC_1;
}

void SPI_WrDat16(unsigned short dat)
{
	SPI_Write_Bus(dat>>8);
	SPI_Write_Bus(dat);
}

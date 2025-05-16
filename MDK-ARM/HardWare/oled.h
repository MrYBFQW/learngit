#ifndef OLED_OLED_H_
#define OLED_OLED_H_

#include "stm32f1xx_hal.h"
#include "oledfont.h"
extern I2C_HandleTypeDef  hi2c1;

void OLED_Init(void);

void OLED_ShowNum(uint8_t x, uint8_t y, unsigned int num, uint8_t len, uint8_t size2, uint8_t Color_Turn);
void OLED_ShowSignedNum(uint8_t x, uint8_t y, int32_t Number, uint8_t Length, uint8_t size2, uint8_t Color_Turn);
void OLED_ShowHexNum(uint8_t x, uint8_t y, unsigned int num, uint8_t len, uint8_t size2, uint8_t Color_Turn);
void OLED_Showdecimal(uint8_t x, uint8_t y, float num, uint8_t z_len, uint8_t f_len, uint8_t size2, uint8_t Color_Turn);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size, uint8_t Color_Turn);
void OLED_ShowString(uint8_t x, uint8_t y, const char*chr, uint8_t Char_Size, uint8_t Color_Turn);
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t size, uint8_t no, uint8_t Color_Turn);
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t* BMP, uint8_t Color_Turn);
void OLED_HorizontalShift(uint8_t direction);
void OLED_Some_HorizontalShift(uint8_t direction, uint8_t start, uint8_t end);
void OLED_VerticalAndHorizontalShift(uint8_t direction);
void OLED_DisplayMode(uint8_t mode);
void OLED_IntensityControl(uint8_t intensity);
void OLED_Clear(void);
void OLED_Clear_Part(unsigned char start_page, unsigned char end_page);
void OLED_Write_Command(uint8_t cmd);
void OLED_On(void);
uint8_t daysInMonth(uint16_t year, uint8_t month);
uint8_t dayOfWeek(uint16_t year, uint8_t month, uint8_t day);
void OLED_Show_Calender(uint16_t year, uint8_t month, uint8_t day_now);
void OLED_Clear_One_Line(unsigned char page);

#endif /* OLED_OLED_H_ */

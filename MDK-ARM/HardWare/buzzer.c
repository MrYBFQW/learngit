#include "stm32f1xx_hal.h"
#include "delay.h"

void buzzer_function(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	delay_ms(100);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	delay_ms(100);
}

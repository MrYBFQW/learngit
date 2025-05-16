#include "delay.h"

extern TIM_HandleTypeDef       htim4;
void delay_us(uint32_t ustime)
{
	uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = __HAL_TIM_GET_AUTORELOAD(&htim4);

    ticks = ustime * reload / (1000);  /* 假设reload对应1ms */
    told = __HAL_TIM_GET_COUNTER(&htim4);
    while (1)
    {
        tnow = __HAL_TIM_GET_COUNTER(&htim4);
        if (tnow != told)
        {
            if (tnow > told)
            {
                tcnt += tnow - told;
            }
            else
            {
                tcnt += reload - told + tnow;
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
		}
}

void delay_ms(uint32_t mstime)
{  
	for(uint32_t i = 0; i < mstime; i++)
	{
		delay_us(1000);
	}
}

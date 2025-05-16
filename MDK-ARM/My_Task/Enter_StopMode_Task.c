#include "oled.h"
#include "FreeRTOS.h"
#include "FreeRTOSInit.h"
#include "task.h"
#include "semphr.h"
#include "Delay.h"

extern SemaphoreHandle_t  StopModeBinarySemaphore;
extern SemaphoreHandle_t xMutex;

void Enter_StopMode_Task(void *params)
{
	while(1)
	{
		vTaskDelay(100);
		OLED_Clear();
//	 if (xSemaphoreTake(xMutex, 0) == pdTRUE) 
//	 {
//			OLED_Clear();
//			OLED_ShowString(0, 0, "Bye Bye!", 16, 0);
//			xSemaphoreGive(xMutex);
//	 }
//		xSemaphoreGive(StopModeBinarySemaphore);
//		/* 进入停止模式 */
//		HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);  
	}
}

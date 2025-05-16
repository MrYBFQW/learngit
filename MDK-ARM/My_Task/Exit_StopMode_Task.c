#include "oled.h"
#include "FreeRTOS.h"
#include "FreeRTOSInit.h"
#include "task.h"
#include "semphr.h"
#include "main.h"

extern SemaphoreHandle_t  StopModeBinarySemaphore;
extern void Escape_Stop_Mode(void);

void Exit_StopMode_Task(void *params)
{
	while(1)
	{
		 Escape_Stop_Mode();
		 vTaskSuspend(NULL);  /* 自我阻塞 */
	}
}

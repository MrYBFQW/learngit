#include "oled.h"
#include "FreeRTOS.h"
#include "FreeRTOSInit.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

extern  TaskHandle_t    ShowMenuTask_Handler;
extern  QueueHandle_t	  KeyQueue;

void ShowFlahLightTask(void *params)
{
	key_value flashlight_task_key;
	int on_single = -1;
	while(1)
	{
		if(on_single == 1)			OLED_On();
		else OLED_Clear();

		xQueueReceive(KeyQueue, &flashlight_task_key, 0);/* 使用队列阻塞接收按键数值 */
		if(flashlight_task_key.key_down == 1)  /* 如果按键向下按下 */
		{		
			OLED_Clear();		/* 清屏 */
			vTaskResume(ShowMenuTask_Handler);  /* 恢复显示菜单任务 */ 
			vTaskSuspend(NULL);  /* 显示手电任务自我挂起 */ 		
			flashlight_task_key.key_down = 0;  /* 恢复按键数值 */			
		}
		else if(flashlight_task_key.key_into == 1)  /* 如果按键向下按下 */
		{		
			on_single = -on_single;		
			flashlight_task_key.key_into = 0;  /* 恢复按键数值 */			
		}
	}
}

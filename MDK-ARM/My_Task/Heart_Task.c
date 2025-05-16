#include "stm32f1xx_hal.h"
#include "oled.h"
#include "FreeRTOS.h"
#include "FreeRTOSInit.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

extern  TaskHandle_t	     ShowMenuTask_Handler;
extern  QueueHandle_t	     KeyQueue;


void ShowHeartTask(void *params)
{
	key_value heart_task_key;
	while(1)
	{
		OLED_ShowString(0, 0, "developing...", 12, 0);
		xQueueReceive(KeyQueue, &heart_task_key, 0);/* 使用队列阻塞接收按键数值 */
		if(heart_task_key.key_down == 1)  /* 如果按键向下按下 */
		{		
			OLED_Clear();		/* 清屏 */
			vTaskResume(ShowMenuTask_Handler);  /* 恢复显示菜单任务 */ 
			vTaskSuspend(NULL);  /* 测量心跳任务自我挂起 */ 		
			heart_task_key.key_down = 0;  /* 恢复按键数值 */			
		}
		else if(heart_task_key.key_into == 1)  /* 如果按键向下按下 */
		{		
			
			heart_task_key.key_into = 0;  /* 恢复按键数值 */			
		}
	}
}

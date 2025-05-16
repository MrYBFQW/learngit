#include "oled.h"
#include "FreeRTOS.h"
#include "FreeRTOSInit.h"
#include "task.h"
#include "semphr.h"

extern 	TaskHandle_t	ShowMenuTask_Handler;
extern  QueueHandle_t	     KeyQueue;
extern SemaphoreHandle_t  OledSlideBinarySemaphore;



void ShowInformationTask(void *params)
{
	key_value information_task_key;
	int infor_second_part = -1;
	while(1)
	{
		
		if(xSemaphoreTake(OledSlideBinarySemaphore, 0) == pdTRUE)
		{
			if(infor_second_part == -1)
			{
				OLED_ShowString(0, 0, information[0], 12, 0);
				OLED_ShowString(0, 1, information[1], 12, 0);
				OLED_ShowString(0, 2, information[2], 12, 0);
				OLED_ShowString(0, 3, information[3], 12, 0);
				OLED_ShowString(0, 4, information[4], 12, 0);
				OLED_ShowString(0, 5, information[5], 12, 0);
				OLED_ShowString(0, 6, information[6], 12, 0);
			}
			else 
			{
				OLED_ShowString(0, 0, information[7], 12, 0);
				OLED_ShowString(0, 1, information[8], 12, 0);
				OLED_ShowString(0, 2, information[9], 12, 0);
				OLED_ShowString(0, 3, information[10], 12, 0);
				OLED_ShowString(0, 4, information[11], 12, 0);
			}
			OLED_VerticalAndHorizontalShift(0x29);
		}
		xQueueReceive(KeyQueue, &information_task_key, 0);/* 使用队列阻塞接收按键数值 */
		if(information_task_key.key_down == 1)  /* 如果按键向下按下 */
		{		
			OLED_Write_Command(0x2e); /* 停止滑动 */
			OLED_Clear();		/* 清屏 */
			vTaskResume(ShowMenuTask_Handler);  /* 恢复显示菜单任务 */ 
			vTaskSuspend(NULL);  /* 显示信息任务自我挂起 */ 		
			information_task_key.key_down = 0;  /* 恢复按键数值 */			
		}
		else if(information_task_key.key_into == 1)
		{
			OLED_Write_Command(0x2e); /* 停止滑动 */
			OLED_Clear();		/* 清屏 */
			infor_second_part = -infor_second_part;
			information_task_key.key_into = 0;  /* 恢复按键数值 */
			xSemaphoreGive(OledSlideBinarySemaphore);
		}
	}
}

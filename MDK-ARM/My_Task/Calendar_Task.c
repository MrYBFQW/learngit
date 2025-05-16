#include "oled.h"
#include "FreeRTOS.h"
#include "FreeRTOSInit.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "Time_Task.h"

extern TaskHandle_t	ShowMenuTask_Handler;
extern  QueueHandle_t	     KeyQueue;
extern SemaphoreHandle_t  OledSlideBinarySemaphore;
extern  Task_Timer         Time_Now;

void ShowCalendarTask(void *params)
{
	key_value calendar_task_key;
	while(1)
	{
		if(xSemaphoreTake(OledSlideBinarySemaphore, 0) == pdTRUE)
		{
			OLED_Show_Calender(Time_Now.year+2000, Time_Now.month, Time_Now.date);
			OLED_Some_HorizontalShift(0x26, 0x00, 0x00);
		}
		xQueueReceive(KeyQueue, &calendar_task_key, 0);/* 使用队列阻塞接收按键数值 */
		if(calendar_task_key.key_down == 1)  /* 如果按键向下按下 */
		{		
			OLED_Write_Command(0x2e); /* 停止滑动 */
			OLED_Clear();		/* 清屏 */
			vTaskResume(ShowMenuTask_Handler);  /* 恢复显示菜单任务 */ 
			vTaskSuspend(NULL);  /* 显示信息任务自我挂起 */ 		
			calendar_task_key.key_down = 0;  /* 恢复按键数值 */			
		}
	}
}

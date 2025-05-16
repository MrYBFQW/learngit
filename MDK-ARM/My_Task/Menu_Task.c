#include "oled.h"
#include "FreeRTOS.h"
#include "FreeRTOSInit.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "buzzer.h"

extern  SemaphoreHandle_t  OledOpenBinarySemaphore;
extern  QueueHandle_t	     KeyQueue;
extern  TaskHandle_t	     ShowTemperatureTask_Handler;
extern  TaskHandle_t	     ShowTimeTask_Handler;
extern  TaskHandle_t	     ShowInformationTask_Handler;
extern  SemaphoreHandle_t  OledSlideBinarySemaphore;
extern	TaskHandle_t	     ShowFlahLightTask_Handler;
extern  TaskHandle_t       ShowCalendarTask_Handler;
extern  TaskHandle_t       ShowStopwatchTask_Handler;
extern  TaskHandle_t       SetClockTask_Handler;
extern  TaskHandle_t       SetSystemTimeTask_Handler;
extern  TaskHandle_t       ShowHeartTask_Handler;

void ShowMenuTask(void *params)/* 显示菜单任务 */
{
	unsigned char function_select = 0;
	key_value menu_task_key;
	while(1)
	{
		OLED_ShowCHinese(0, 0, 16, 9, 0);
		OLED_ShowCHinese(18, 0, 16, 10, 0);
		OLED_DrawBMP(48, 2, 80, 6, menu[function_select], 0);
		if(function_select == 0)		   OLED_ShowString(31, 7, "Temperature", 12, 0);
		else if(function_select == 1)  OLED_ShowString(40, 7, "calendar", 12, 0);
		else if(function_select == 2)  OLED_ShowString(34, 7, "FlashLight", 12, 0);
		else if(function_select == 3)  OLED_ShowString(37, 7, "StopWatch", 12, 0);
		else if(function_select == 4)  OLED_ShowString(10, 7, "Device_Information", 12, 0);
		else if(function_select == 5)	 OLED_ShowString(37, 7, "Set_Clock", 12, 0);
		else if(function_select == 6)	 OLED_ShowString(40, 7, "Set_Time", 12, 0);
		else if(function_select == 7)	 OLED_ShowString(10, 7, "Measure heart rate", 12, 0);
		xQueueReceive(KeyQueue, &menu_task_key, 0);/* 使用队列阻塞接收按键数值 */
		if(menu_task_key.key_into == 1)  /* 如果按键向内按下 */
		{
			OLED_Clear();		/* 清屏 */
			if(function_select == 0)			 vTaskResume(ShowTemperatureTask_Handler);  /* 恢复显示温度任务 */
			else if(function_select == 1)	
			{
				xSemaphoreGive(OledSlideBinarySemaphore);
				vTaskResume(ShowCalendarTask_Handler);     /* 恢复显示万年历任务 */
			}	
			else if(function_select == 2)	 vTaskResume(ShowFlahLightTask_Handler);    /* 恢复显示手电任务 */
			else if(function_select == 3)	 vTaskResume(ShowStopwatchTask_Handler);  /* 恢复显示秒表任务 */
			else if(function_select == 4)	
			{
				xSemaphoreGive(OledSlideBinarySemaphore);		/* 释放二值信号量 */
				vTaskResume(ShowInformationTask_Handler);  /* 恢复显示信息任务 */
			}
			else if(function_select == 5)  vTaskResume(SetClockTask_Handler);  /* 恢复显示设置闹钟任务 */
			else if(function_select == 6)  vTaskResume(SetSystemTimeTask_Handler);  /* 恢复显示设置系统时间任务 */
			else if(function_select == 7)  vTaskResume(ShowHeartTask_Handler);  /* 恢复显示心跳任务 */
			vTaskSuspend(NULL);  /* 显示菜单任务自我挂起 */ 		
			menu_task_key.key_into = 0;  /* 恢复按键数值 */			
		}
		else if(menu_task_key.key_down == 1)  /* 如果按键向下按下 */
		{		
			OLED_Clear();		/* 清屏 */
			vTaskResume(ShowTimeTask_Handler);  /* 恢复显示时间任务 */ 
			vTaskSuspend(NULL);  /* 显示菜单任务自我挂起 */ 		
			menu_task_key.key_down = 0;  /* 恢复按键数值 */			
		}
		else if(menu_task_key.key_up == 1)  /* 如果按键向上拨动,切换功能 */
		{		
			OLED_Clear_Part(2, 8);		/* 清空当前图片,防止切换时候图片重叠 */
			menu_task_key.key_up = 0;  /* 恢复按键数值 */	
			function_select++;
			if(function_select == 8) function_select = 0;  /* 防止越界 */			
		}
	}
}


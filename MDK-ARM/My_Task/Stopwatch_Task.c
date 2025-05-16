#include "oled.h"
#include "FreeRTOS.h"
#include "FreeRTOSInit.h"
#include "task.h"
#include "queue.h"
#include "Stopwatch_Task.h"

uint8_t stopwatch_millisecond = 0;
uint8_t stopwatch_second      = 0;
uint8_t stopwatch_minute      = 0;

extern  TimerHandle_t 	   Stopwatch_Task_Timer;
extern  QueueHandle_t	     KeyQueue;
extern  TaskHandle_t			 ShowMenuTask_Handler;

unsigned char count_round = 0;

void ShowStopwatchTask(void *params)
{
	key_value stopwatch_task_key;
	int stopwatch_single = -1;
	while(1)
	{
		OLED_ShowString(0, 0, "count:", 12, 0);
		OLED_ShowNum(40, 0, count_round, 2, 12, 0);
		OLED_DrawBMP(0, 2, 16, 6, Timer[stopwatch_minute / 10], 0);
    OLED_DrawBMP(15, 2, 31, 6, Timer[stopwatch_minute % 10], 0);
    OLED_DrawBMP(29, 2, 45, 6, Timer[10], 0);	/* 分钟 */
    OLED_DrawBMP(47, 2, 63, 6, Timer[stopwatch_second / 10], 0);
    OLED_DrawBMP(62, 2, 78, 6, Timer[stopwatch_second % 10], 0);
    OLED_DrawBMP(76, 2, 92, 6, Timer[10], 0);	/* 秒 */
    OLED_DrawBMP(94, 2, 110, 6, Timer[stopwatch_millisecond / 10], 0);
    OLED_DrawBMP(109, 2, 125, 6, Timer[stopwatch_millisecond % 10], 0);	/* 1/100秒 */
		
		xQueueReceive(KeyQueue, &stopwatch_task_key, 0);/* 使用队列阻塞接收按键数值 */
		if(stopwatch_task_key.key_up == 1)  /* 清零计数 */
		{
			stopwatch_millisecond = 0;
			stopwatch_second = 0;
			stopwatch_minute = 0;	 		
			stopwatch_task_key.key_up = 0;  /* 恢复按键数值 */			
		}
		else if(stopwatch_task_key.key_into == 1)  /* 开启/停止计时 */
		{
			if(stopwatch_single == -1)	
			{
				xTimerStart(Stopwatch_Task_Timer, 0);  /* 启动软件定时器 */
				stopwatch_single = 1;
			}
			else 
			{
				xTimerStop(Stopwatch_Task_Timer, 0);
				stopwatch_single = -1;
			}
			stopwatch_task_key.key_into = 0;  /* 恢复按键数值 */			
		}
		else if(stopwatch_task_key.key_down == 1)  /* 退出秒表功能 */
		{
			OLED_Clear();		/* 清屏 */
			vTaskResume(ShowMenuTask_Handler);  /* 恢复显示菜单任务 */ 
			vTaskSuspend(NULL);  /* 显示秒表任务自我挂起 */ 		
			stopwatch_task_key.key_down = 0;  /* 恢复按键数值 */			
		}
	}
}

void Stopwatch_Task_CallBack(TimerHandle_t xTimer)
{	
	stopwatch_millisecond++;
	if(stopwatch_millisecond == 100)	
	{
		stopwatch_millisecond = 0;
		stopwatch_second++;
	}
	if(stopwatch_second == 60)
	{
		stopwatch_second = 0;
		stopwatch_minute++;
	}
	if(stopwatch_minute == 100)
	{
		stopwatch_millisecond = 0;
		stopwatch_second = 0;
		stopwatch_minute = 0;
		count_round++;
	}
}

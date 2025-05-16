#include "oled.h"
#include "FreeRTOS.h"
#include "FreeRTOSInit.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "Time_Task.h"
#include "ds1302.h"
#include "Clock_Task.h"
#include "main.h"

Task_Timer Time_Now = {0, 0, 0, 0, 0, 0};/* 时间结构体 */

SemaphoreHandle_t  xMutex = NULL;

unsigned char battery_percent = 100;
unsigned char Clock_Ring = 0;
unsigned char Stop_Time = 0;
extern  ADC_HandleTypeDef  hadc1;
extern  TaskHandle_t			 ShowMenuTask_Handler;
extern  TaskHandle_t       ShowTemperatureTask_Handler; 
//extern  TaskHandle_t	     Enter_StopModeTask_Handler;

extern  QueueHandle_t	     KeyQueue;

extern void Escape_Stop_Mode(void);

void ShowTimeTask(void *params)/* 显示时间任务 */
{
	key_value time_task_key;
	SemaphoreHandle_t xMutex = xSemaphoreCreateMutex();
	while(1)
	{
		DS1302_ReadTime();
		if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) 
		{
			OLED_ShowString(37, 0, "Avenger", 12, 0);
			HAL_ADC_Start(&hadc1);
			battery_percent = HAL_ADC_GetValue(&hadc1) / 4096;
			if(battery_percent == 100)  OLED_ShowString(90, 0, "Full", 12, 0);
//			else if(battery_percent <= 10)  OLED_ShowString(90, 0, "Low!", 12, 0);
			else OLED_ShowNum(90, 0, battery_percent, 2, 12, 0);	/* 显示电量百分比数值 */
			OLED_DrawBMP(115, 0, 128, 1, Battery[battery_percent], 0);	/* 电池电量图标 */
			OLED_DrawBMP(0, 0, 16, 1, xinhao, 0);	/* 信号图标 */
			OLED_DrawBMP(20, 0, 26, 1, bluetooth, 0);/* 蓝牙图标 */
			OLED_DrawBMP(0, 2, 16, 6, Timer[Time_Now.hour / 10], 0);
			OLED_DrawBMP(15, 2, 31, 6, Timer[Time_Now.hour % 10], 0);
			OLED_DrawBMP(29, 2, 45, 6, Timer[10], 0);	/* 小时 */
			OLED_DrawBMP(47, 2, 63, 6, Timer[Time_Now.minute / 10], 0);
			OLED_DrawBMP(62, 2, 78, 6, Timer[Time_Now.minute % 10], 0);
			OLED_DrawBMP(76, 2, 92, 6, Timer[10], 0);	/* 分钟 */
			OLED_DrawBMP(94, 2, 110, 6, Timer[Time_Now.second / 10], 0);
			OLED_DrawBMP(109, 2, 125, 6, Timer[Time_Now.second % 10], 0);	/* 秒 */
			OLED_ShowNum(0, 6, Time_Now.year + 2000, 4, 16, 0);
			OLED_ShowCHinese(33, 6, 16, 0, 0);	/* 年 */
			OLED_ShowNum(50, 6, Time_Now.month / 10, 1, 16, 0);
			OLED_ShowNum(58, 6, Time_Now.month % 10, 1, 16, 0);
			OLED_ShowCHinese(67, 6, 16, 1, 0);	/* 月 */
	    OLED_ShowNum(84, 6, Time_Now.date / 10, 1, 16, 0);
	    OLED_ShowNum(92, 6, Time_Now.date % 10, 1, 16, 0);
			OLED_ShowCHinese(101, 6, 16, 2, 0);	/* 日 */ 
			xSemaphoreGive(xMutex);
		}
		xQueueReceive(KeyQueue, &time_task_key, 0);/* 使用队列非阻塞接收按键数值 */
		if(time_task_key.key_up == 1)  /* 如果按键向上按下 */
		{
			Escape_Stop_Mode();
			time_task_key.key_up = 0;  /* 恢复按键数值 */			
		}
		else if(time_task_key.key_into == 1)  /* 如果按键向内按下 */
		{
			OLED_Clear();		/* 清屏 */
			vTaskResume(ShowMenuTask_Handler);  /* 恢复显示菜单任务 */ 
			vTaskSuspend(NULL);  /* 显示时间任务自我挂起 */ 		
			time_task_key.key_into = 0;  /* 恢复按键数值 */			
		}
		else if(time_task_key.key_down == 1)
		{
			OLED_Clear();
			OLED_ShowString(0, 0, "Bye Bye!", 16, 0);
			vTaskDelay(pdMS_TO_TICKS(1000));	
			OLED_Clear();
			HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);  /* 进入停止模式 */
			time_task_key.key_down = 0;  /* 恢复按键数值 */
		}
	}
}


/* 软件定时器回调函数,每1000tick执行一次 */
void Time_Task_CallBack(TimerHandle_t xTimer)
{
	for(unsigned char i = 1; i < 2 * Clock[0] + 1; i+=2)
	{
		if( (Time_Now.hour / 10 + Time_Now.hour % 10) == Clock[i] && 
			  (Time_Now.minute / 10 + Time_Now.minute % 10) == Clock[i + 1])
		{
			Clock_Ring = 1;  /* 与闹钟设定数值逐一比较,只要符合闹钟数值就会释放信号量 */
		}
		/* 跳过未设置熟知的闹钟,因为未设置的闹钟数值默认值为0 */
		else if(Clock[i] == 0 && Clock[i + 1] == 0)	continue;  
	}
}


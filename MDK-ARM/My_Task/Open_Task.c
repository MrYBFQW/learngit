#include "oled.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

extern  TimerHandle_t 			Time_Task_Timer;             /* 软件定时器       */
extern  TaskHandle_t        ShowTimeTask_Handler;        /* 时间任务         */
extern  TaskHandle_t				ShowMenuTask_Handler;        /* 菜单任务         */
extern  TaskHandle_t				ShowFlahLightTask_Handler;   /* 手电任务         */
extern  TaskHandle_t				ShowTemperatureTask_Handler; /* 温度任务         */
extern 	TaskHandle_t				ShowInformationTask_Handler; /* 信息任务         */
extern 	TaskHandle_t				ShowCalendarTask_Handler;    /* 万年历任务       */
extern  TaskHandle_t  			ShowStopwatchTask_Handler;   /* 秒表任务         */
extern  TaskHandle_t        SetClockTask_Handler;        /* 测量心跳任务     */
extern  TaskHandle_t        SetSystemTimeTask_Handler;   /* 设置闹钟任务     */
extern  TaskHandle_t        ShowHeartTask_Handler;       /* 设置时间任务     */
//extern  TaskHandle_t        Enter_StopModeTask_Handler;  /* 进入停止模式任务 */
//extern  TaskHandle_t        Exit_StopModeTask_Handler;   /* 退出停止模式任务 */

void OpenTask(void *params)
{
	/* 开机动画开始前需要挂起除了时间任务外的所有任务,以防干扰 */
	vTaskSuspend(ShowTimeTask_Handler);         /* 挂起时间任务         */
	vTaskSuspend(ShowMenuTask_Handler);         /* 挂起菜单任务         */
	vTaskSuspend(ShowFlahLightTask_Handler);    /* 挂起手电任务         */
	vTaskSuspend(ShowTemperatureTask_Handler);  /* 挂起温度任务         */
	vTaskSuspend(ShowInformationTask_Handler);  /* 挂起信息任务         */
	vTaskSuspend(ShowCalendarTask_Handler);     /* 挂起万年历任务       */
	vTaskSuspend(ShowStopwatchTask_Handler);    /* 挂起秒表任务         */
	vTaskSuspend(ShowHeartTask_Handler); 				/* 挂起测量心跳任务     */
	vTaskSuspend(SetClockTask_Handler); 				/* 挂起设置闹钟任务     */
	vTaskSuspend(SetSystemTimeTask_Handler); 	  /* 挂起设置时间任务     */
//	vTaskSuspend(Enter_StopModeTask_Handler); 	/* 挂起进入停止模式任务 */
//	vTaskSuspend(Exit_StopModeTask_Handler);    /* 挂起退出停止模式任务 */
	while(1)
	{
		taskENTER_CRITICAL();											/* 进入临界区 */ 
		for(unsigned char i = 0; i < 16; i++)
		{
			OLED_DrawBMP(32, 0, 96, 8, Open[i], 1);
		}	 																				/* 显示开机动画 */ 
		OLED_Clear();		
		vTaskResume(ShowTimeTask_Handler);        /* 开机动画完成后恢复显示时间任务 */ 
		xTimerStart(Time_Task_Timer, 0);          /* 启动软件定时器 */
		taskEXIT_CRITICAL();											/* 退出临界区 */
		vTaskDelete(NULL);                        /* 开机任务自我删除 */
	}
}

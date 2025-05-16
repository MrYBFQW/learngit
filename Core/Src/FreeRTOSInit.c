#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "FreeRTOSInit.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "Queue.h"

#include "Time_Task.h"
#include "Open_Task.h"
#include "Menu_Task.h"
#include "Temperature_Task.h"
#include "Information_Task.h"
#include "FlashLight_Task.h"
#include "Calendar_Task.h"
#include "Stopwatch_Task.h"
#include "Clock_Task.h"
#include "SetSystemTime_Task.h"
#include "Heart_Task.h"
#include "Enter_StopMode_Task.h"
#include "Exit_StopMode_Task.h"

#include "delay.h"
#include "buzzer.h"
#include "main.h"

/* 所需要用到的任务句柄 */
TaskHandle_t	OpenTask_Handler	            = NULL;
TaskHandle_t	ShowTimeTask_Handler 					= NULL;
TaskHandle_t	ShowMenuTask_Handler 				  = NULL;
TaskHandle_t	ShowFlahLightTask_Handler 		= NULL;
TaskHandle_t	ShowTemperatureTask_Handler	  = NULL;
TaskHandle_t	ShowInformationTask_Handler	  = NULL;
TaskHandle_t  ShowCalendarTask_Handler      = NULL;
TaskHandle_t  ShowStopwatchTask_Handler     = NULL;
TaskHandle_t  SetClockTask_Handler          = NULL;
TaskHandle_t  SetSystemTimeTask_Handler     = NULL;
TaskHandle_t  ShowHeartTask_Handler         = NULL;

//TaskHandle_t  Enter_StopModeTask_Handler    = NULL;
//TaskHandle_t  Exit_StopModeTask_Handler     = NULL;

/* 用于执行一次OLED滑动命令的二值信号量   */
SemaphoreHandle_t  OledSlideBinarySemaphore = NULL;  

/* 用于执行进入停止模式的二值信号量   */
SemaphoreHandle_t  StopModeBinarySemaphore  = NULL;  


/* 所用到的两个定时器句柄 */
TimerHandle_t 	   Time_Task_Timer          = NULL;
TimerHandle_t 	   Stopwatch_Task_Timer     = NULL;

/* 按键队列句柄 */
QueueHandle_t	     KeyQueue                 = NULL;

/* 定义按键结构体 */
key_value key_state = {0, 0, 0};

void FreeRTOS_Init(void)
{
	taskENTER_CRITICAL();/* 进入临界区 */ 
	Time_Task_Timer = xTimerCreate	/* 创建显示时间任务的软件定时器1 */
				("Timer1",	/* 软件定时器名称	*/
				1000,				/* 周期为1000tick */
				pdTRUE,			/* 周期型软件定时器 */
				NULL,				/* 软件定时器的ID */
				(TimerCallbackFunction_t)  Time_Task_CallBack);		/* 定时器回调函数 */
	
	Stopwatch_Task_Timer = xTimerCreate	/* 创建显示时间任务的软件定时器1 */
				("Timer2",	/* 软件定时器名称	*/
				10,				  /* 周期为10tick */
				pdTRUE,			/* 周期型软件定时器 */
				NULL,				/* 软件定时器的ID */
				(TimerCallbackFunction_t)  Stopwatch_Task_CallBack);  /* 定时器回调函数 */
				
/* 创建二值信号量,用于执行一次OLED滑动命令 */
	OledSlideBinarySemaphore = xSemaphoreCreateBinary();
/* 创建二值信号量,用于进入停止模式任务的同步 */
	StopModeBinarySemaphore  = xSemaphoreCreateBinary();
/* 先获取二值信号量,之后信号量数值为0,以此作为初始状态 */
//	xSemaphoreTake(StopModeBinarySemaphore, 0);
	
	/* 创建所需要的任务 */
	xTaskCreate(OpenTask,              "OpenTask",            128, NULL, 2, &OpenTask_Handler);
  xTaskCreate(ShowTimeTask,          "ShowTimeTask", 				128, NULL, 0, &ShowTimeTask_Handler);
  xTaskCreate(ShowMenuTask,          "ShowMenuTask", 				128, NULL, 0, &ShowMenuTask_Handler);
	xTaskCreate(ShowFlahLightTask,     "ShowLightTask", 	    128, NULL, 0, &ShowFlahLightTask_Handler);
	xTaskCreate(ShowTemperatureTask,   "ShowTempTask",        256, NULL, 0, &ShowTemperatureTask_Handler);
	xTaskCreate(ShowInformationTask,   "ShowInforTask",       128, NULL, 0, &ShowInformationTask_Handler);
	xTaskCreate(ShowCalendarTask,      "ShowCalenTask", 		  128, NULL, 0, &ShowCalendarTask_Handler);
	xTaskCreate(ShowStopwatchTask,     "ShowwatchTask", 	    128, NULL, 0, &ShowStopwatchTask_Handler);
	xTaskCreate(SetClockTask,          "ShowClockTask", 	    256, NULL, 0, &SetClockTask_Handler);
	xTaskCreate(SetSystemTimeTask,     "SetSysTimeTask", 	    128, NULL, 0, &SetSystemTimeTask_Handler);
  xTaskCreate(ShowHeartTask,         "ShowHeartTask", 	    128, NULL, 0, &ShowHeartTask_Handler);
//	xTaskCreate(Enter_StopMode_Task,   "NStopModeTask", 	    128, NULL, 1, &Enter_StopModeTask_Handler);
//	xTaskCreate(Exit_StopMode_Task,    "XStopModeTask", 	    128, NULL, 1, &Exit_StopModeTask_Handler);
	KeyQueue = xQueueCreate(1, 4);  /* 创建按键数值队列 */
	
	taskEXIT_CRITICAL();/* 退出临界区 */
  vTaskStartScheduler();/* 启动任务调度器 */
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
//	if(xSemaphoreTake(StopModeBinarySemaphore, 0) == pdTRUE)  /* 可以成功take停止模式任务二值信号量 */
//	{
//		xTaskResumeFromISR(Exit_StopModeTask_Handler);
//	}
	if(GPIO_Pin == GPIO_PIN_4)  /* 向下拨动 */
	{
		delay_ms(20); /* 按键消抖 */
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_RESET)  /* 按键仍然按下 */
    {
			Stop_Time = 0;
			buzzer_function();/* 按键音 */
			key_state.key_down = 1;
			xQueueSendToBackFromISR(KeyQueue, &key_state, 0);  /* 读取到数据,写入队列 */
			key_state.key_down = 0;
		}
	}
	else if(GPIO_Pin == GPIO_PIN_5)  /* 向内按下 */
	{
		delay_ms(20);  /* 按键消抖 */
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_RESET)  /* 按键仍然按下 */
    {
			Stop_Time = 0;
			buzzer_function();/* 按键音 */
			key_state.key_into = 1;
			xQueueSendToBackFromISR(KeyQueue, &key_state, 0);  /* 读取到数据,写入队列 */
			key_state.key_into = 0;
		}
	}
	else if(GPIO_Pin == GPIO_PIN_6)  /* 向上拨动 */
	{
		delay_ms(20);  /* 按键消抖 */
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_RESET)  /* 按键仍然按下 */
    {
			Stop_Time = 0;
			buzzer_function();/* 按键音 */
			key_state.key_up = 1;
			xQueueSendToBackFromISR(KeyQueue, &key_state, 0);  /* 读取到数据,写入队列 */
			key_state.key_up = 0;	
		}
	}
}

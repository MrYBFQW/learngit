#include "oled.h"
#include "FreeRTOS.h"
#include "FreeRTOSInit.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "ds18b20.h"
extern TaskHandle_t	       ShowMenuTask_Handler;
extern  QueueHandle_t	     KeyQueue;

void ShowTemperatureTask(void *params)/* 显示温度任务 */
{
	key_value temperature_task_key;
	float Temperature = 0;
	while(1)
	{ 
		vTaskSuspendAll();  /* 挂起所有任务,防止DS18B20时序被任务调度干扰 */    
		DS18B20ConvertTemperature();  /* 启动温度转化 */
		Temperature = DS18B20ReadTemperature() - 3; /* 获取温度 */	
		xTaskResumeAll();  /* 时序完成后,恢复所有任务 */
		if(Temperature >= 26)
		{
			OLED_ShowString(0, 0, "Warning!", 12, 0);
			OLED_ShowString(0, 1, "The Temperature is", 12, 0);
			OLED_ShowString(0, 2, "too high", 12, 0);
		}
		else 
		{
			OLED_ShowString(0, 0, "Welcome!", 12, 0);
			OLED_ShowString(0, 1, "The Temperature is", 12, 0);
			OLED_ShowString(0, 2, "normal", 12, 0);
		}
		OLED_ShowCHinese(0, 3, 12, 11, 0);
		OLED_ShowCHinese(12, 3, 12, 12, 0);
		OLED_ShowCHinese(24, 3, 12, 13, 0);
		OLED_ShowCHinese(36, 3, 12, 14, 0);
		OLED_Showdecimal(0, 5, Temperature, 2, 2, 12, 0);
		OLED_ShowString(30, 5, "'C", 12, 0);
		xQueueReceive(KeyQueue, &temperature_task_key, 0);  /* 使用队列阻塞接收按键数值 */
		if(temperature_task_key.key_down == 1)  /* 如果按键向下按下 */
		{		
			OLED_Clear();		/* 清屏 */
			vTaskResume(ShowMenuTask_Handler);  /* 恢复显示菜单任务 */ 
			vTaskSuspend(NULL);  /* 显示温度任务自我挂起 */ 		
			temperature_task_key.key_down = 0;  /* 恢复按键数值 */			
		}
	}
}

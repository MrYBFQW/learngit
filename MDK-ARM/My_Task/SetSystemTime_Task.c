#include "oled.h"
#include "FreeRTOS.h"
#include "FreeRTOSInit.h"
#include "task.h"
#include "queue.h"
#include "Time_Task.h"
#include "ds1302.h"

extern  QueueHandle_t	     KeyQueue;
extern  TaskHandle_t			 ShowMenuTask_Handler;
extern  Task_Timer         Time_Now;

void SetSystemTimeTask(void *params)
{
	key_value set_time_task_key;
	unsigned char set_year   = Time_Now.year,     set_month  = Time_Now.month, 
								set_date   = Time_Now.date,     set_hour   = Time_Now.hour, 
								set_minute = Time_Now.minute,   set_second = Time_Now.second;
	unsigned char choose = 0;
	while(1)
	{
		OLED_ShowString(4, 0, "Set The Sysyem Time!", 12, 0);
		OLED_ShowString(0, 1, "Hour:", 12, 0);
		OLED_ShowString(0, 2, "Min:", 12, 0);
		OLED_ShowString(0, 3, "Sec:", 12, 0);
		OLED_ShowString(0, 4, "Year:", 12, 0);
		OLED_ShowString(0, 5, "Month:", 12, 0);
		OLED_ShowString(0, 6, "Date:", 12, 0);
		OLED_ShowNum(32, 1, set_hour, 2, 12, !(choose-1));
		OLED_ShowNum(26, 2, set_minute, 2, 12, !(choose-2));
		OLED_ShowNum(26, 3, set_second, 2, 12, !(choose-3));
		OLED_ShowNum(32, 4, set_year+2000, 4, 12, !(choose-4));
		OLED_ShowNum(38, 5, set_month, 2, 12, !(choose-5));
		OLED_ShowNum(32, 6, set_date, 2, 12, !(choose-6));
		xQueueReceive(KeyQueue, &set_time_task_key, 0);/* 使用队列阻塞接收按键数值 */
		if(set_time_task_key.key_up == 1)  /* 数值加 */
		{
	 		if(choose == 1) 
			{
				set_hour++;
				if(set_hour == 24) set_hour = 0;
			}						
			else if(choose == 2) 
			{
				set_minute++;
				if(set_minute == 60) set_minute = 0;
			}
			else if(choose == 3) 
			{
				set_second++;
				if(set_second == 60) set_second = 0;
			}
			else if(choose == 4)  set_year++;
			else if(choose == 5)  
			{
				set_month++;
				if(set_month == 13)	set_month = 1;
			}
			else if(choose == 6)  
			{
				set_date++;
				if(set_date == daysInMonth(set_year+2000, set_month)+1)  set_date = 1;
			}
			set_time_task_key.key_up = 0;  /* 恢复按键数值 */			
		}
		else if(set_time_task_key.key_into == 1)  /* 选择设置 */
		{
			choose++;
			if(choose == 7) choose = 0;
			set_time_task_key.key_into = 0;  /* 恢复按键数值 */			
		}
		else if(set_time_task_key.key_down == 1)  /* 退出设置时间功能 */
		{
			if(choose == 0)
			{
				DS1302_SetTime(set_year, set_month, set_date, set_hour, set_minute, set_second);
				OLED_Clear();		/* 清屏 */
				vTaskResume(ShowMenuTask_Handler);  /* 恢复显示菜单任务 */ 
				vTaskSuspend(NULL);  /* 设置时间任务自我挂起 */ 		
			}
			else 
			{
				if(choose == 1) 
				{
					set_hour--;
					if(set_hour == 255) set_hour = 23;
				}						
				else if(choose == 2) 
				{
					set_minute--;
					if(set_minute == 255) set_minute = 59;
				}
				else if(choose == 3) 
				{
					set_second++;
					if(set_second == 255) set_second = 59;
				}
				else if(choose == 4)  set_year--;
				else if(choose == 5)  
				{
					set_month--;
					if(set_month == 0)	set_month = 12;
				}
				else if(choose == 6)  
				{
					set_date--;
					if(set_date == 0)  set_date = 1;
				}
			}
			set_time_task_key.key_down = 0;  /* 恢复按键数值 */	
		}
	}
}

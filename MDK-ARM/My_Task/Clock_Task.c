#include "oled.h"
#include "FreeRTOS.h"
#include "FreeRTOSInit.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "queue.h"

#define Clock_Max_Num  6
unsigned char clock_number = 0;
extern  TaskHandle_t	     ShowMenuTask_Handler;
extern  QueueHandle_t	     KeyQueue;

short Clock[12] = {0};
void SetClockTask(void *params)
{
	key_value SetClockTask_Key;
	KeyQueue = xQueueCreate(1, 4);  /* 创建按键数值队列 */
	unsigned char clock_choose = 0, clock_num_ok = 0;
	while(1)
	{
		OLED_ShowString(0,  1, "Clock1:", 12, 0);             /*    Clock[1]  : Clock[2]      */
		OLED_ShowString(0,  2, "Clock2:", 12, 0);             /*    Clock[3]  : Clock[4]      */
		OLED_ShowString(0,  3, "Clock3:", 12, 0);             /*    Clock[5]  : Clock[6]      */
		OLED_ShowString(0,  4, "Clock4:", 12, 0);             /*    Clock[7]  : Clock[8]      */
		OLED_ShowString(0,  5, "Clock5:", 12, 0);             /*    Clock[9]  : Clock[10]     */
		OLED_ShowString(0,  6, "Clock6:", 12, 0);             /*    Clock[11] : Clock[12]     */
		OLED_ShowString(0, 0, "Clock Num:", 12, 0);           /*       常态显示闹钟数量       */
		OLED_ShowNum(64, 0, clock_number, 2, 12, 0);
		
		if(clock_choose != 0 && clock_choose < (2 * clock_number + 1))  OLED_ShowString(0, 7, "Clock value none ok!", 12, 0);
		else if(clock_choose == 0)  OLED_ShowString(0, 7, "Clock Num None ok!", 12, 0);
		else if(clock_choose == (2 * clock_number + 1))  OLED_ShowString(0, 7, "You can leave!", 12, 0);

		if(clock_number != 0)  /* 闹钟数值常态显示 */ 
		{
			for(unsigned char i = 0; i < clock_number; i++)
			{
				OLED_ShowNum(   44, i + 1, Clock[2 * i + 1], 2, 12, 0);
				OLED_ShowString(60, i + 1, ":", 12, 0);
				OLED_ShowNum(   70, i + 1, Clock[2 * i + 2], 2, 12, 0);
			}  
		}
		else if(clock_number == 0)
		{
			for(unsigned char i = 0; i < Clock_Max_Num; i++)
				OLED_ShowString(44, i + 1, "         ", 12, 0);  /* 清除该行所有数值 */
		}	
		xQueueReceive(KeyQueue, &SetClockTask_Key, 0);  /* 使用队列阻塞接收按键数值 */
		if(SetClockTask_Key.key_up == 1)  /* 向上拨动 */
		{
			/* clock_choose = 0:处于闹钟数量选择阶段, clock_number < Clock_Max_Num:闹钟数量并未达到最大值 */
			if(clock_choose == 0)
			{
				clock_num_ok = 1;   /* 标志位,表示可以切换为设置闹钟数值阶段 */
				clock_number++;     /*  闹钟数量增加 */ 
				if(clock_number == Clock_Max_Num + 1)  clock_number = 0;  /* 防止闹钟数量越界 */
			}
			/* clock_choose != 0:处于闹钟数值设置阶段 */
			if(clock_choose != 0)  
			{
				Clock[clock_choose]++; /* 对应闹钟数值自增 */
				if(clock_choose % 2 != 0 && Clock[clock_choose] == 24)  Clock[clock_choose] = 0;
				else if(clock_choose % 2 == 0 && Clock[clock_choose] == 60)	Clock[clock_choose] = 0;
			}	
			SetClockTask_Key.key_up = 0;  /* 恢复按键数值 */			
		}	
		else if(SetClockTask_Key.key_into == 1) /* 向内按下 */
		{
			if(clock_num_ok == 1)  /* 闹钟数量刚好选择完成 */
			{
				OLED_Clear_One_Line(7);
				OLED_ShowString(0, 7, "Clock Num OK", 12, 0);
				vTaskDelay(500);
				OLED_Clear_One_Line(7);  /* 显示闹钟数量设置完成 */
				clock_num_ok = 0;  			 /* 清除标志位 */
				clock_choose++;				   /* 进入闹钟数值设置阶段*/
			}
			else  
			{
				clock_choose++;  /* 选择自增 */
				if(clock_choose == (2 * clock_number + 1) )   /* 显示闹钟数值设置完成 */
				{
					OLED_Clear_One_Line(7);
					OLED_ShowString(0, 7, "Clock Value OK", 12, 0);
					vTaskDelay(500);
				  OLED_Clear_One_Line(7); 
				}
				else if(clock_choose == (2 * clock_number + 2))  clock_choose = 1;  /* 防止越界 */
			}			
			SetClockTask_Key.key_into = 0;  /* 恢复按键数值 */			
		}
		else if(SetClockTask_Key.key_down == 1)  /* 向下拨动 */
		{
			if(clock_choose > 0 && clock_choose < (2 * clock_number + 1) )  /* 处于闹钟数值设置阶段 */ 
			{
				Clock[clock_choose]--;  /* 对应闹钟数值自减 */
				if(clock_choose % 2 != 0 && Clock[clock_choose] < 0)  Clock[clock_choose] = 23;
				else if(clock_choose % 2 == 0 && Clock[clock_choose] < 0)	Clock[clock_choose] = 59;
			}
			else if(clock_choose == 0 && clock_number != 0)  /* 处于闹钟数量设置阶段 */	
			{
				clock_num_ok = 1;  /* 标志位,表示可以切换为设置闹钟数值阶段 */
				OLED_ShowString(44, clock_number, "         ", 12, 0);  /* 清除当前所在行的闹钟数值 */
				clock_number--;  /* 闹钟数量减少1 */
			} 
			else if(clock_choose == (2 * clock_number + 1))  /* 处于闹钟数值设置完成阶段 */
			{
				OLED_Clear();		/* 清屏 */
				Clock[0] = clock_number;
				clock_num_ok = 0;
				clock_choose = 0;  /* 清空标志位 */
				vTaskResume(ShowMenuTask_Handler);  /* 恢复显示菜单任务 */ 
				vTaskSuspend(NULL);  /* 设置闹钟任务自我挂起 */
			}
			SetClockTask_Key.key_down = 0;  /* 恢复按键数值 */	
		}
	}
}





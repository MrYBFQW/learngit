#ifndef TIME_TASK_H_
#define TIME_TASK_H_

#include "timers.h"
void ShowTimeTask(void *params);
void Time_Task_CallBack(TimerHandle_t xTimer);
void Stop_Task_CallBack(TimerHandle_t xTimer);


typedef struct   
{
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	uint16_t year;
	unsigned char month;
	unsigned char date;
}Task_Timer;

extern unsigned char Stop_Time;
#endif

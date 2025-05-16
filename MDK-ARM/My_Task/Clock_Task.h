#ifndef CLOCK_TASK_H_
#define CLOCK_TASK_H_

void SetClockTask(void *params);//设置闹钟任务
typedef struct clock  
{
	unsigned char key_up;
	unsigned char key_into;
	unsigned char key_down;
}set_clock;
extern short Clock[12];
#endif

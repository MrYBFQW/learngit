/*
 * FreeRTOS V202212.01
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/
 
/* 1:使用抢占式调度 0:合作式调度 */
#define configUSE_PREEMPTION															1	
/* 1:支持使用静态方式管理内存 0:默认值,动态 */
#define configSUPPORT_STATIC_ALLOCATION          					0
/* 1:支持使用动态方式管理内存,且是默认值 */
#define configSUPPORT_DYNAMIC_ALLOCATION        					1
/*0:不使用空闲任务钩子函数 1:使用空闲任务钩子函数 */
#define configUSE_IDLE_HOOK																0
/*0:不使用滴答定时器钩子函数 1:使用滴答定时器钩子函数 */
#define configUSE_TICK_HOOK																0
/* 定义CPU主频,单位为Hz(72MHz) */
#define configCPU_CLOCK_HZ																( ( unsigned long ) 72000000 )/* CPU主频(72MHz) */
/* 定义定义系统时钟节拍数,单位Hz,n就是1个tick是1/n秒 */
#define configTICK_RATE_HZ																( ( TickType_t ) 1000 )
/* 定义可供用户使用的任务的最大优先级数量,优先级值在0~(configMAX_PRIORITIES - 1)之间,数字越小优先级越低 */
#define configMAX_PRIORITIES															( 5 )
/* 定义空闲任务的栈空间大小，单位是字 */
#define configMINIMAL_STACK_SIZE													( ( unsigned short ) 128 )
/* 定义堆大小 */
#define configTOTAL_HEAP_SIZE															( ( size_t ) ( 12 * 1024 ) )
/* 定义任务名称的最大字符长度 */
#define configMAX_TASK_NAME_LEN														( 16 )
/* 1:在抢占调度下,同等优先级的任务可抢占空闲任务 */
#define configIDLE_SHOULD_YIELD														1
/* 1:使用互斥信号量 0:不使用互斥信号量 */
#define configUSE_MUTEXES                      					  1
/* 定义可以注册的队列和信号量的最大数量 */
#define configQUEUE_REGISTRY_SIZE             					  8
/* 1:使能递归互斥信号量 0:不使能递归互斥信号量*/
#define configUSE_RECURSIVE_MUTEXES             					1
/* 1:使能计数型信号量 0:不使能计数型信号量 */
#define configUSE_COUNTING_SEMAPHORES           					1
/* 1:系统节拍计数器的数据类型为16位无符号整形 0:系统节拍计数器的数据类型为32位无符号整形 */
#define configUSE_16_BIT_TICKS														0

/* 1:启用低功耗Tickless模式 0:不启用低功耗Tickless模式 */
#define configUSE_TICKLESS_IDLE														1	


/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

/* 任务管理的相关宏定义,1:使能该API函数 0:失能该API函数 */
#define INCLUDE_vTaskPrioritySet           		    		1
#define INCLUDE_uxTaskPriorityGet          		    		1
#define INCLUDE_vTaskDelete                		    		1
#define INCLUDE_vTaskCleanUpResources      		    		0
#define INCLUDE_vTaskSuspend               		    		1
#define INCLUDE_vTaskDelayUntil            		    		1
#define INCLUDE_vTaskDelay                 		    		1
#define INCLUDE_xTaskGetSchedulerState     		    		1
#define INCLUDE_xTimerPendFunctionCall     		    		1
#define INCLUDE_xQueueGetMutexHolder       		    		1
#define INCLUDE_uxTaskGetStackHighWaterMark		    		1
#define INCLUDE_eTaskGetState              		    		1

/* 软件定时器的相关宏定义 */

/* 1:使能软件定时器功能 0:不使用软件定时器功能 */
#define configUSE_TIMERS                    			    1
/* 设置软件定时器处理任务的优先级 */
#define configTIMER_TASK_PRIORITY           			    ( 34 )
/* 定义软件定时器队列的长度 */
#define configTIMER_QUEUE_LENGTH            			    10
/* 设置软件定时器处理任务的栈空间大小 */
#define configTIMER_TASK_STACK_DEPTH        			    configMINIMAL_STACK_SIZE

/* 此宏定义用于配置STM32的8位优先级设置寄存器实际使用的位数 */
#ifdef __NVIC_PRIO_BITS
     #define configPRIO_BITS       								    __NVIC_PRIO_BITS
#else								
     #define configPRIO_BITS       								    4       
#endif

/* MCU硬件(此处为STM32F103C8T6)中断最低优先级 */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			  15  


/* FreeRTOS系统可管理的最高中断优先级 
 * 指0~5不被FreeRTOS管控,中断5~15被freertos管控 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5 


/* This is the raw value as per the Cortex-M3 NVIC.  Values can be 255
(lowest) to 0 (1?) (highest). */
/* 此宏应定义为 MCU 的最低中断优先等级在中断优先级配置寄存器中的值 
 * 对configLIBRARY_LOWEST_INTERRUPT_PRIORITY进行移位即可 */
#define configKERNEL_INTERRUPT_PRIORITY 							( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )	
/*15 = 0000 1110, << 4, = 1110 0000, = 240 */


/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
/* 此宏应定义为 FreeRTOS 可管理中断的最高优先等级在中断优先级配置寄存器中的值
 * 对configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY进行左移位即可	*/
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 					( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) ) 
/* equivalent to 0x05, or priority 5. */

/* This is the value being used as per the ST library which permits 16
priority values, 0 to 15.  This must correspond to the
configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
NVIC value of 255. */
/* 此宏应定义为 MCU (此处为stm32f103c8t6)的最低中断优先等级,这里使用的是4bit优先级,所以最低为15 */  
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY	 			15	/* or 0x0f */

#endif /* FREERTOS_CONFIG_H */


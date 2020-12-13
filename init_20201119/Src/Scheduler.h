/*
 * @Descripttion: 
 * @version: 
 * @Author: zicong
 * @Date: 2020-12-09 21:37:16
 * @LastEditors: zicong
 * @LastEditTime: 2020-12-10 22:02:02
 */
#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
#include "main.h"
#include "stm32f4xx.h"

typedef struct
{
void(*task_func)(void);
uint16_t rate_hz;
uint16_t interval_ticks;
uint32_t last_run;
}sched_task_t;

void Scheduler_Setup(void);
void Scheduler_Run(void);



typedef struct
{
	unsigned char data_correct_flag;// 1 数据正确标志 
	unsigned char data_origin; // 数据来源
	unsigned char operation_object; // 操作对象
	unsigned char operation_command; // 操作命令
}_usart_data_st;
//==数据声明
extern _usart_data_st usart_data_handle;
#endif


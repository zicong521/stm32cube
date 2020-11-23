#ifndef __OPMV_H
#define __OPMV_H

#include "stm32f4xx.h"

//所以本函数需要在串口接收中断中调用，每接收一字节数据，调用本函数一次

void opmv_DataAnl_blobMV (uint8_t);
void opmv_DataAnl_circleMV (uint8_t,uint8_t*);
int opmv_GetOneByte_blobMV_en (void);
int opmv_GetOneByte_blobMV_unen (void);
int opmv_GetOneByte_circleMV_en (void);
int opmv_GetOneByte_circleMV_unen (void);
int opmv_close_turn_flag(void);
void opmv_process (unsigned char);

typedef struct
{
    int move_y_flag; // 前进 后退
    int move_x_flag; // 左移 右移 
    u8 turn_left_flag_90; // 左转90 转完发标志位
    u8 turn_right_flag_90; // 右转90 转完发标志位
    u8 turn_left_flag;// 左转标志
    u8 turn_right_flag;//右转标志
    u8 empty_move_flag;// 空闲任务
	u8 data_correct_flag;// 1 数据正确标志 
     u8   direct_down_flag;
}_openmv_data_st;
//==数据声明
extern _openmv_data_st opmv;
#endif

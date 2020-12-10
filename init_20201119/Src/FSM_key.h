/*
 * @Descripttion: 
 * @version: 
 * @Author: zicong
 * @Date: 2019-10-27 21:33:36
 * @LastEditors: zicong
 * @LastEditTime: 2020-12-10 19:59:45
 */
#ifndef __FSM_KEY_H
#define __FSM_KEY_H

#include <stdint.h>
/*
 * 基于有限状态机(FSM)的面向对象的独立按键库
 *
 * Author : Jayant Tang
 *
 * Email  : jayant97@hust.edu.cn
 *
 *
 * 简介:
 *      采用轮询方式读取GPIO电平(每10ms读取一次)；
 *
 *      支持单击、双击、长按；
 *
 *      可为单击和双击配置回调函数；
 *      单击和双击后都会各自调用自己的回调函数；
 *
 *      长按1s后，每隔0.5s就会调用一次单击回调函数，直到按键弹起
 *
 * 关于移植：
 *          本文件用于TI的Tiva C系列微控制器 TM4C123GH6PM芯片
 *      但是采用了便于移植的写法，即：
 *      (1)所有头文件中可以看到的外部函数，都是和硬件无关的纯软件算法
 *      (2)若想移植到其他单片机上，只需修改FSM_key.c文件中的与硬件相关的两个内部函数即可
 *
 */

//按键状态枚举
typedef enum
{
    NORMAL = 0, //正常
    PRE_ACTIVE,
    PRE_HIT,
    PRE_LONG_PRESS,
    SINGLE_HIT, //单击
    DOUBLE_HIT, //双击
    LONG_PRESS, //长按
} keyState_Enum;

//按键极性枚举
typedef enum
{
    LOW = 0, //按下时为低电平
    HIGH
} keyPolarity_Enum;

//按键结构体
struct fsmKey_Struct
{
    uint32_t port;                     //端口基地址
    uint32_t num;                      //引脚编号
    keyPolarity_Enum polarity;         //极性
    void (*singleClickCallback)(void); //单击回调函数
    void (*doubleClickCallback)(void); //双击回调函数

    keyState_Enum state;        //按键状态
    uint8_t sample[4];          //短采样
    uint16_t counter;           //用于处理双击和长按的计数器
    struct fsmKey_Struct *next; //链表结构，指向下一个按键的索引
};
typedef struct fsmKey_Struct key_TypeDef;

extern void keyInit(key_TypeDef *key,
                    uint32_t Port,
                    uint8_t PinNumber,
                    keyPolarity_Enum Polarity,
                    void (*sigleClick)(void),
                    void (*doubleClick)(void));

extern void keyScanAll(void);
extern void keyScan(key_TypeDef *key);

#endif /* __FSM_KEY_H */

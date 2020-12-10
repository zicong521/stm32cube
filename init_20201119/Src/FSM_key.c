#include "FSM_key.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "main.h"
/*声明硬件相关的内部函数*/
static int keyGPIOInit(uint32_t Port, uint32_t PinNumber);     //配置GPIO为输入
static uint8_t keyGPIORead(uint32_t Port, uint32_t PinNumber); //读取GPIO电平

/*声明全局变量*/
volatile key_TypeDef *g_first_key = NULL; //记录第一个按键结构体的地址

/*实现外部函数*/

/*
 * 按键初始化
 * key      :     按键结构体指针
 * Port     :     GPIO端口基地址(转换为uint32_t)
 * PinNumber:     GPIO引脚号(整数)
 * Polarity :     极性，硬件上，按键按下时GPIO将会读到的电平(HIGH或LOW)
 */
void keyInit(key_TypeDef *key,
             uint32_t Port,
             uint8_t PinNumber,
             keyPolarity_Enum Polarity,
             void (*sigleClick)(void),
             void (*doubleClick)(void))
{
    static key_TypeDef *s_previous_key = NULL; //记录上一个按键的地址

    //按键配置保存
    key->port = Port;
    key->num = PinNumber;
    key->polarity = Polarity;
    key->singleClickCallback = sigleClick;  //单击回调函数
    key->doubleClickCallback = doubleClick; //双击回调函数
    key->state = NORMAL;
    key->counter = 0;
    key->sample[0] = 0;
    key->sample[1] = 0;
    key->sample[2] = 0;
    key->sample[3] = 0;
    key->next = NULL;

    //配置GPIO
    keyGPIOInit(Port, PinNumber);

    if (s_previous_key == NULL)
    {
        //如果是第一个被初始化的按键，进行特殊配置
        g_first_key = key; //保存它的地址到全局变量
    }
    else
    {
        //不是第一个被初始化的按键，则把本按键的链表连接到上一个按键上
        s_previous_key->next = key;
    }

    //全部配置完成，把本按键的地址留作下次使用
    s_previous_key = key;
}

//扫描单个按键
void keyScan(key_TypeDef *key)
{
    uint32_t voltage;
    const uint8_t key_event_press[4] = {1, 1, 0, 0};    //press事件
    const uint8_t key_event_realease[4] = {0, 0, 1, 1}; //release事件
    bool press_flag = 0, release_flag = 0;

    //保存旧采样值
    key->sample[3] = key->sample[2];
    key->sample[2] = key->sample[1];
    key->sample[1] = key->sample[0];

    voltage = keyGPIORead(key->port, key->num);

    //新采样值
    if ((voltage && (key->polarity)) || ((!voltage) && (!(key->polarity))))
    {
        key->sample[0] = 1;
    }
    else
    {
        key->sample[0] = 0;
    }

    //事件
    if (!memcmp(key->sample, key_event_press, 4))
    {
        press_flag = 1;
    }
    else if (!memcmp(key->sample, key_event_realease, 4))
    {
        release_flag = 1;
    }

    //状态机
    switch (key->state)
    {
    case NORMAL:
        if (press_flag)
        {
            key->state = PRE_ACTIVE;
            key->counter = 0;
        }
        break;

    case PRE_ACTIVE:
        if (key->counter < 4)
        {
            if (release_flag)
            { //4个采样周期内释放按键
                key->state = PRE_HIT;
                key->counter = 0;
                break;
            }
            key->counter++;
        }
        else
        { //未释放
            key->state = PRE_LONG_PRESS;
            key->counter = 0;
        }
        break;

    case PRE_LONG_PRESS:
        if (key->counter < 100)
        {
            if (release_flag)
            {                         //100周期内释放了
                key->state = PRE_HIT; //算单击(或双击)
                key->counter = 0;
                break;
            }
            key->counter++;
        }
        else
        { //超过了1s
            key->state = LONG_PRESS;
            key->counter = 0;
        }
        break;

    case PRE_HIT:
        if (key->counter < 30)
        { //300ms内
            if (press_flag)
            { //又按下一次
                key->state = DOUBLE_HIT;
                key->counter = 0;
                break;
            }
            key->counter++;
        }
        else
        { //超过了300ms
            key->state = SINGLE_HIT;
            key->counter = 0;
        }
        break;

    case SINGLE_HIT:
        //执行单击回调函数
        if (key->singleClickCallback != NULL)
        {
            key->singleClickCallback();
        }
        key->state = NORMAL;
        break;

    case DOUBLE_HIT:
        //执行双击回调函数
        if (key->doubleClickCallback != NULL)
        {
            key->doubleClickCallback();
        }
        else if (key->singleClickCallback != NULL)
        { //若双击回调函数未注册，则执行两次单击回调函数
            key->singleClickCallback();
            key->singleClickCallback();
        }
        key->state = NORMAL;
        break;

    case LONG_PRESS:
        //长按状态下，每0.5s执行一次单击回调函数，循环执行
        if (release_flag)
        {
            key->state = NORMAL;
            key->counter = 0;
            break;
        }
        if (!(key->counter % 20))
        { //到达0.5s
            //执行单击回调函数
            if (key->singleClickCallback != NULL)
            {
                key->singleClickCallback();
            }
        }
        key->counter++;
        key->counter %= 1000;
        break;

    default:
        break;
    }
}


//扫描全体按键
void keyScanAll(void)
{
    key_TypeDef *key_pointer = (key_TypeDef *)g_first_key;

    while (1)
    {
        if (key_pointer != NULL)
        {
            keyScan(key_pointer);            //扫描单个按键
            key_pointer = key_pointer->next; //沿链表移动到下一个按键
        }
        else
        {
            break;
        }
    }
    return;
}

/*实现内部函数*/

//GPIO读取电压
static uint8_t keyGPIORead(uint32_t Port, uint32_t PinNumber) //读取GPIO电平
{
    uint8_t voltage = 0;
    voltage = GPIOPinRead(Port, 0x00000001 << PinNumber);

    return voltage;
}

//GPIO配置函数
static int keyGPIOInit(uint32_t Port, uint32_t PinNumber)
{
    uint32_t sysctl_periph_base; //系统控制外设基地址
    uint8_t unlock_flag = 0;     //是否解锁

    //参数只传入了GPIO的外设基地址，需要自己推导出时钟使能需要的值
    switch (Port)
    {
    case GPIO_PORTA_BASE:
    case GPIO_PORTA_AHB_BASE:
        sysctl_periph_base = SYSCTL_PERIPH_GPIOA;
        break;
    case GPIO_PORTB_BASE:
    case GPIO_PORTB_AHB_BASE:
        sysctl_periph_base = SYSCTL_PERIPH_GPIOB;
        break;
    case GPIO_PORTC_BASE:
    case GPIO_PORTC_AHB_BASE:
        sysctl_periph_base = SYSCTL_PERIPH_GPIOC;
        break;
    case GPIO_PORTD_BASE:
    case GPIO_PORTD_AHB_BASE:
        sysctl_periph_base = SYSCTL_PERIPH_GPIOD;
        break;
    case GPIO_PORTE_BASE:
    case GPIO_PORTE_AHB_BASE:
        sysctl_periph_base = SYSCTL_PERIPH_GPIOE;
        break;
    case GPIO_PORTF_BASE:
    case GPIO_PORTF_AHB_BASE:
        sysctl_periph_base = SYSCTL_PERIPH_GPIOF;
        break;
    default:
        return -1; //发生错误
    }

    //外设使能
    SysCtlPeripheralEnable(sysctl_periph_base);

    //判断是否解锁
    switch (Port)
    {
    default:
        unlock_flag = 0;
        break;
    case GPIO_PORTC_BASE:
        if (PinNumber < 4)
        { //PC[3:0]被锁定
            unlock_flag = 1;
        }
        break;
    case GPIO_PORTC_AHB_BASE:
        if (PinNumber < 4)
        { //PC[3:0]被锁定
            unlock_flag = 1;
        }
        break;
    case GPIO_PORTD_BASE:
        if (PinNumber == 7)
        { //PD7被锁定
            unlock_flag = 1;
        }
        break;
    case GPIO_PORTD_AHB_BASE:
        if (PinNumber == 7)
        { //PD7被锁定
            unlock_flag = 1;
        }
        break;
    case GPIO_PORTF_BASE:
        if (PinNumber == 0)
        { //PF0被锁定
            unlock_flag = 1;
        }
        break;
    case GPIO_PORTF_AHB_BASE:
        if (PinNumber == 0)
        { //PF0被锁定
            unlock_flag = 1;
        }
        break;
    }

    //解锁
    if (unlock_flag)
    {
        HWREG(Port + GPIO_O_LOCK) = GPIO_LOCK_KEY;
        HWREG(Port + GPIO_O_CR) |= 0x01 << PinNumber;
        HWREG(Port + GPIO_O_LOCK) = 0;
    }

    //GPIO配置为输入模式
    GPIOPinTypeGPIOInput(Port, 0x00000001 << PinNumber); //GPIO配置为输入

    return 0;
}

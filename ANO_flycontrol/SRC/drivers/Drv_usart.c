/******************** (C) COPYRIGHT 2017 ANO Tech ********************************
 * 作者    ：匿名科创
 * 官网    ：www.anotc.com
 * 淘宝    ：anotc.taobao.com
 * 技术Q群 ：190169595
 * 描述    ：串口驱动
**********************************************************************************/
#include "Drv_usart.h"
#include "Ano_DT.h"
#include "Ano_OF.h"
#include "include.h"
#include <Drv_openmv.h>
#include "stm32f4xx.h"
#include <stdio.h>
#include "Ano_DT.h"
void Usart2_Init ( u32 br_num )
{
    USART_InitTypeDef USART_InitStructure;
    USART_ClockInitTypeDef USART_ClockInitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd ( RCC_APB1Periph_USART2, ENABLE ); //开启USART2时钟
    RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOD, ENABLE );

    //串口中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_UART2_P;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_UART2_S;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init ( &NVIC_InitStructure );


    GPIO_PinAFConfig ( GPIOD, GPIO_PinSource5, GPIO_AF_USART2 );
    GPIO_PinAFConfig ( GPIOD, GPIO_PinSource6, GPIO_AF_USART2 );

    //配置PD5作为USART2　Tx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init ( GPIOD, &GPIO_InitStructure );
    //配置PD6作为USART2　Rx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init ( GPIOD, &GPIO_InitStructure );

    //配置USART2
    //中断被屏蔽了
    USART_InitStructure.USART_BaudRate = br_num;       //波特率可以通过地面站配置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8位数据
    USART_InitStructure.USART_StopBits = USART_StopBits_1;   //在帧结尾传输1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;    //禁用奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //发送、接收使能
    //配置USART2时钟
    USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;  //时钟低电平活动
    USART_ClockInitStruct.USART_CPOL = USART_CPOL_Low;  //SLCK引脚上时钟输出的极性->低电平
    USART_ClockInitStruct.USART_CPHA = USART_CPHA_2Edge;  //时钟第二个边沿进行数据捕获
    USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable; //最后一位数据的时钟脉冲不从SCLK输出

    USART_Init ( USART2, &USART_InitStructure );
    USART_ClockInit ( USART2, &USART_ClockInitStruct );

    //使能USART2接收中断
    USART_ITConfig ( USART2, USART_IT_RXNE, ENABLE );
    //使能USART2
    USART_Cmd ( USART2, ENABLE );
//	//使能发送（进入移位）中断
//	if(!(USART2->CR1 & USART_CR1_TXEIE))
//	{
//		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
//	}


}

u8 TxBuffer[256];
u8 TxCounter = 0;
u8 count = 0;

u8 Rx_Buf[256];	//串口接收缓存

void Usart2_IRQ ( void )
{
    u8 com_data;

    if ( USART2->SR & USART_SR_ORE ) //ORE中断
    {
        com_data = USART2->DR;
    }

    //接收中断
    if ( USART_GetITStatus ( USART2, USART_IT_RXNE ) )
    {
        USART_ClearITPendingBit ( USART2, USART_IT_RXNE ); //清除中断标志

        com_data = USART2->DR;
        ANO_DT_Data_Receive_Prepare ( com_data );
    }
    //发送（进入移位）中断
    if ( USART_GetITStatus ( USART2, USART_IT_TXE ) )
    {

        USART2->DR = TxBuffer[TxCounter++]; //写DR清除中断标志
        if ( TxCounter == count )
        {
            USART2->CR1 &= ~USART_CR1_TXEIE;		//关闭TXE（发送中断）中断
        }


        //USART_ClearITPendingBit(USART2,USART_IT_TXE);
    }



}

void Usart2_Send ( unsigned char *DataToSend , u8 data_num )
{
    u8 i;
    for ( i = 0; i < data_num; i++ )
    {
        TxBuffer[count++] = * ( DataToSend + i );
    }

    if ( ! ( USART2->CR1 & USART_CR1_TXEIE ) )
    {
        USART_ITConfig ( USART2, USART_IT_TXE, ENABLE ); //打开发送中断
    }

}



void Uart5_Init ( u32 br_num )
{
    USART_InitTypeDef USART_InitStructure;
    //USART_ClockInitTypeDef USART_ClockInitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd ( RCC_APB1Periph_UART5, ENABLE ); //开启USART2时钟
    RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOC, ENABLE );
    RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOD, ENABLE );

    //串口中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_UART5_P;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_UART5_S;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init ( &NVIC_InitStructure );


    GPIO_PinAFConfig ( GPIOC, GPIO_PinSource12, GPIO_AF_UART5 );
    GPIO_PinAFConfig ( GPIOD, GPIO_PinSource2, GPIO_AF_UART5 );

    //配置PC12作为UART5　Tx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init ( GPIOC, &GPIO_InitStructure );
    //配置PD2作为UART5　Rx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init ( GPIOD, &GPIO_InitStructure );

    //配置UART5
    //中断被屏蔽了
    USART_InitStructure.USART_BaudRate = br_num;       //波特率可以通过地面站配置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8位数据
    USART_InitStructure.USART_StopBits = USART_StopBits_1;   //在帧结尾传输1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;    //禁用奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //发送、接收使能
    USART_Init ( UART5, &USART_InitStructure );



    //使能UART5接收中断
    USART_ITConfig ( UART5, USART_IT_RXNE, ENABLE );
    //使能USART5
    USART_Cmd ( UART5, ENABLE );
//	//使能发送（进入移位）中断
//	if(!(USART2->CR1 & USART_CR1_TXEIE))
//	{
//		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
//	}

}
u8 Tx5Buffer[256];
u8 Tx5Counter = 0;
u8 count5 = 0;

void Uart5_IRQ ( void )
{
    //接收中断
    if ( USART_GetITStatus ( UART5, USART_IT_RXNE ) )
    {
        USART_ClearITPendingBit ( UART5, USART_IT_RXNE ); //清除中断标志

        u8 com_data = UART5->DR;
        
//        opmv_GetOneByte_circleMV( com_data );
        
    }

    //发送（进入移位）中断
    if ( USART_GetITStatus ( UART5, USART_IT_TXE ) )
    {

        UART5->DR = Tx5Buffer[Tx5Counter++]; //写DR清除中断标志

        if ( Tx5Counter == count5 )
        {
            UART5->CR1 &= ~USART_CR1_TXEIE;		//关闭TXE（发送中断）中断
        }


        //USART_ClearITPendingBit(USART2,USART_IT_TXE);
    }

}

void Uart5_Send ( unsigned char *DataToSend , u8 data_num )
{
    u8 i;
    for ( i = 0; i < data_num; i++ )
    {
        Tx5Buffer[count5++] = * ( DataToSend + i );
    }

    if ( ! ( UART5->CR1 & USART_CR1_TXEIE ) )
    {
        USART_ITConfig ( UART5, USART_IT_TXE, ENABLE ); //打开发送中断
    }

}



void Uart4_Init ( u32 br_num )
{
    USART_InitTypeDef USART_InitStructure;
    //USART_ClockInitTypeDef USART_ClockInitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd ( RCC_APB1Periph_UART4, ENABLE ); //开启USART2时钟
    RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOA, ENABLE );

    //串口中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_UART4_P;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_UART4_S;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init ( &NVIC_InitStructure );


    GPIO_PinAFConfig ( GPIOA, GPIO_PinSource0, GPIO_AF_UART4 );
    GPIO_PinAFConfig ( GPIOA, GPIO_PinSource1, GPIO_AF_UART4 );

    //配置PC12作为UART5　Tx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init ( GPIOA, &GPIO_InitStructure );
    //配置PD2作为UART5　Rx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init ( GPIOA, &GPIO_InitStructure );

    //配置UART5
    //中断被屏蔽了
    USART_InitStructure.USART_BaudRate = br_num;       //波特率可以通过地面站配置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8位数据
    USART_InitStructure.USART_StopBits = USART_StopBits_1;   //在帧结尾传输1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;    //禁用奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //发送、接收使能
    USART_Init ( UART4, &USART_InitStructure );

    //使能UART5接收中断
    USART_ITConfig ( UART4, USART_IT_RXNE, ENABLE );
    //使能USART5
    USART_Cmd ( UART4, ENABLE );
}
u8 Tx4Buffer[256];
u8 Tx4Counter = 0;
u8 count4 = 0;

void Uart4_IRQ ( void )
{
    u8 com_data;

    //接收中断
    if ( USART_GetITStatus ( UART4, USART_IT_RXNE ) )
    {
        USART_ClearITPendingBit ( UART4, USART_IT_RXNE ); //清除中断标志

        com_data = UART4->DR;

        AnoOF_GetOneByte ( com_data );
    }

    //发送（进入移位）中断
    if ( USART_GetITStatus ( UART4, USART_IT_TXE ) )
    {

        UART4->DR = Tx4Buffer[Tx4Counter++]; //写DR清除中断标志

        if ( Tx4Counter == count4 )
        {
            UART4->CR1 &= ~USART_CR1_TXEIE;		//关闭TXE（发送中断）中断
        }


        //USART_ClearITPendingBit(USART2,USART_IT_TXE);
    }

}
void Uart4_Send ( unsigned char *DataToSend , u8 data_num )
{
    u8 i;
    for ( i = 0; i < data_num; i++ )
    {
        Tx4Buffer[count5++] = * ( DataToSend + i );
    }

    if ( ! ( UART4->CR1 & USART_CR1_TXEIE ) )
    {
        USART_ITConfig ( UART4, USART_IT_TXE, ENABLE ); //打开发送中断
    }

}

void Usart3_Init ( u32 br_num )
{
    USART_InitTypeDef USART_InitStructure;
    //USART_ClockInitTypeDef USART_ClockInitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd ( RCC_APB1Periph_USART3, ENABLE ); //开启USART2时钟
    RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOB, ENABLE );

    //串口中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_UART3_P;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_UART3_S;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init ( &NVIC_InitStructure );


    GPIO_PinAFConfig ( GPIOB, GPIO_PinSource10, GPIO_AF_USART3 );
    GPIO_PinAFConfig ( GPIOB, GPIO_PinSource11, GPIO_AF_USART3 );

    //配置PC12作为UART5　Tx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init ( GPIOB, &GPIO_InitStructure );
    //配置PD2作为UART5　Rx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init ( GPIOB, &GPIO_InitStructure );

    //配置UART5
    //中断被屏蔽了
    USART_InitStructure.USART_BaudRate = br_num;       //波特率可以通过地面站配置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8位数据
    USART_InitStructure.USART_StopBits = USART_StopBits_1;   //在帧结尾传输1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;    //禁用奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //发送、接收使能
    USART_Init ( USART3, &USART_InitStructure );

    //使能UART5接收中断
    USART_ITConfig ( USART3, USART_IT_RXNE, ENABLE );
    //使能USART5
    USART_Cmd ( USART3, ENABLE );
}
u8 Tx3Buffer[256];
u8 Tx3Counter = 0;
u8 count3 = 0;

void Usart3_IRQ ( void )
{
  

    //接收中断
    if ( USART_GetITStatus ( USART3, USART_IT_RXNE ) )
    {
        USART_ClearITPendingBit ( USART3, USART_IT_RXNE ); //清除中断标志

     //
       // com_data = USART3->DR;

       // opmv_GetOneByte_blobMV( com_data );
    }

    //发送（进入移位）中断
    if ( USART_GetITStatus ( USART3, USART_IT_TXE ) )
    {

        USART3->DR = Tx4Buffer[Tx3Counter++]; //写DR清除中断标志
        if ( Tx3Counter == count3 )
        {
            USART3->CR1 &= ~USART_CR1_TXEIE;		//关闭TXE（发送中断）中断
        }


        //USART_ClearITPendingBit(USART2,USART_IT_TXE);
    }

}
void Usart3_Send ( unsigned char *DataToSend , u8 data_num )
{
    u8 i;
    for ( i = 0; i < data_num; i++ )
    {
        Tx4Buffer[count5++] = * ( DataToSend + i );
    }

    if ( ! ( USART3->CR1 & USART_CR1_TXEIE ) )
    {
        USART_ITConfig ( USART3, USART_IT_TXE, ENABLE ); //打开发送中断
    }

}



// 以下为正点原子USART1例程
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}

#endif

//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound){
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、


	
}


void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 com_data;

    //接收中断
    if ( USART_GetITStatus ( USART1, USART_IT_RXNE ) )
    {

        USART_ClearITPendingBit ( USART1, USART_IT_RXNE ); //清除中断标志

        com_data = USART1->DR;
        
        
        opmv_process( com_data );
    }
		 
} 




/******************* (C) COPYRIGHT 2014 ANO TECH *****END OF FILE************/


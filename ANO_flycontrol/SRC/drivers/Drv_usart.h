#ifndef _USART_H
#define _USART_H

#include "stm32f4xx.h"

extern u8 Rx_Buf[];
void Usart2_Init(u32 br_num);
void Usart2_IRQ(void);
void Usart2_Send(unsigned char *DataToSend ,u8 data_num);

void Usart3_Init(u32 br_num);
void Usart3_IRQ(void);
void Usart3_Send(unsigned char *DataToSend ,u8 data_num);

void Uart4_Init(u32 br_num);
void Uart4_IRQ(void);
void Uart4_Send(unsigned char *DataToSend ,u8 data_num);

void Uart5_Init(u32 br_num);
void Uart5_IRQ(void);
void Uart5_Send(unsigned char *DataToSend ,u8 data_num);

#define USART_REC_LEN  			200  	//֨ӥخճޓ˕ؖޚ˽ 200
#define EN_USART1_RX 			1		//ʹŜè1é/޻ֹè0éԮࠚ1ޓ˕
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //ޓ˕ۺԥ,خճUSART_REC_LENٶؖޚ.ĩؖޚΪۻѐػ 
extern u16 USART_RX_STA;         		//ޓ˕״̬Ҫ݇	
//ɧڻЫԮࠚא׏ޓ˕ìȫһҪעˍӔЂ۪֨ӥ
void uart_init(u32 bound);
#endif

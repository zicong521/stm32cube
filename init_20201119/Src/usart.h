// 一个不定长接受的库
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define RX_LEN 1024  

typedef struct  
{  
uint8_t  RX_flag:1;        //IDLE receive flag
uint16_t RX_Size;          //receive length
uint8_t  RX_pData[RX_LEN]; //DMA receive buffer
}USART_RECEIVETYPE;  


extern USART_RECEIVETYPE UsartType; 
/* USER CODE END Private defines */

extern void Error_Handler(void);
extern void UsartReceive_IDLE(UART_HandleTypeDef *huart);  

/* USER CODE BEGIN Prototypes */
void Usart_Data_Receive_Prepare(uint8_t data);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

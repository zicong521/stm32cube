#include "usart.h"


/* USER CODE BEGIN 0 */
USART_RECEIVETYPE UsartType; 
/* USER CODE END 0 */



/* USER CODE BEGIN 1 */
/**
  * @brief  Retargets the C library printf function to the USART.
  */
int fputc(int ch,FILE *f)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
	return ch;
}

/**
  * @brief  This function handles USART1 IDLE interrupt.
  */
void UsartReceive_IDLE(UART_HandleTypeDef *huart)  
{  
    uint32_t temp;  
  
    if((__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE) != RESET))  
    {   
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);  
        HAL_UART_DMAStop(&huart1);  
        temp = huart1.hdmarx->Instance->NDTR;  
        UsartType.RX_Size =  RX_LEN - temp;   
        UsartType.RX_flag=1;  
        HAL_UART_Receive_DMA(&huart1,UsartType.RX_pData,RX_LEN);  
    }  
}  
/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

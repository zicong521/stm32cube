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

void Usart_Data_Receive_Prepare(uint8_t data)
{
	static uint8_t state = 0;
	
	if(state==0&&data==0xAA)	//帧头0xAA
	{
		state=1;
	}
	else if(state==1)	//数据来源，例如0x01表示数据来自串口屏
	{
		state=2;
		usart_data_handle.data_origin = data;
	}
	else if(state==2)		//操作对象
	{
		state=3;
		usart_data_handle.operation_object = data;
	}
	else if(state==3)		//操作命令
	{
		state=4;
		usart_data_handle.operation_command = data;
	}
	else if(state==4 && data==0xFF)		//数据帧结尾
	{
		state = 0;
		usart_data_handle.data_correct_flag = 1;
	}
	else
		state = 0;
}
/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

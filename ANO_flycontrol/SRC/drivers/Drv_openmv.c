/***********************
 * 描述：1.openMV数据解析
 *       2.openMV控制信号传输
************************/
#include "include.h"
#include "Ano_ProgramCtrl_User.h"
#include "Drv_openmv.h"
#include "Drv_usart.h"
#include "stdio.h"

_openmv_data_st opmv ;
static uint8_t _datatemp_MV1;
static uint8_t _datatemp_MV2;
static uint16_t _datatemp;


// 启用openmv
int opmv_GetOneByte_blobMV_en(void)
{
          uint16_t DataToSend_circleMV[3] = {0xFA,0xFB,0xF2};
   for(int i = 0;i<3;i++)
   {
       USART_SendData(USART1, DataToSend_circleMV[i]); //测试
       Delay_ms(1);    
   }
   return 1;
}
//关闭openmv
int opmv_GetOneByte_blobMV_unen(void)
{
    uint16_t DataToSend_circleMV[3] = {0xFA,0xFB,0xF3};
    for(int i = 0;i<3;i++)
    {
        USART_SendData(USART1, DataToSend_circleMV[i]); //测试
        Delay_ms(1); 
    }
    return 1;
}
// 关闭正在定向旋转标志位
int opmv_close_turn_flag(void)
{
    uint16_t DataToSend_circleMV[3] = {0xFA,0xFB,0xF4};
    for(int i = 0;i<3;i++)
    {
        USART_SendData(USART1, DataToSend_circleMV[i]); //测试
        Delay_ms(1); 
    }
    return 1;
}
// 未用
/*
void opmv_GetOneByte_blobMV(uint8_t com_data )
{
    static u8 state = 0;
	
	if(state==0 && com_data==0xAA)    //帧首
	{
		state=1;
	}
	else if(state==1 && com_data==0xAF)	//源地址
	{
		state=2;
	}
	else if(state==2)			//功能字
	{
		state=0;
		_datatemp_blobMV=com_data;
        opmv_DataAnl_blobMV(_datatemp_blobMV);     //功能字筛选
	}
	else
		state = 0;
}
*/
/*
//描述：功能字筛别  色块mv
void opmv_DataAnl_blobMV (uint8_t data_buf)
{
    if (data_buf == 0xF0)
    {
        //上升 pass ;
    }
    else if (data_buf == 0xF1)
    {
        //下降 pass ;
        
    }
    else if (data_buf == 0xF2)
    {
        // 悬停 
        opmv.hover_flag = 1;
        opmv.data_correct_flag = 1;
    }
    else 
    {
        ANO_DT_SendString("1: DATA ERROR!");
    }
}
*/

// 接受指令 放在U1里
void opmv_process (unsigned char com_data)
{
    
	static u8 state = 0;
	
	if(state==0 && com_data==0xAA)    //帧首
	{
		state=1;
	}
	else if(state==1 && com_data==0xAF)	//源地址
	{
		state=2;
	}
	else if(state==2)			//功能字1
	{
        _datatemp_MV1 =com_data;
      state=3;
             
	}
    else if(state==3)
    {
        state=0;
		_datatemp_MV2=com_data;
       _datatemp = (_datatemp_MV1 << 8) | _datatemp_MV2;
   
        switch(_datatemp)
        {
            case 0XF0F0:
                opmv.data_correct_flag = 1;
                opmv.move_y_flag = 0;
                opmv.move_x_flag = 0;
                break;
            case 0XF0F1:
                opmv.data_correct_flag = 1;
                opmv.move_y_flag = 0;
                opmv.move_x_flag = 1;
                break;
            case 0XF0F2:
                opmv.data_correct_flag = 1;
                opmv.move_y_flag = 0;
                opmv.move_x_flag = -1;
                break;
            case 0XF1F0:
                opmv.data_correct_flag = 1;
                opmv.move_y_flag = 1;
                opmv.move_x_flag = 0;
                break;
            case 0XF1F1:
                opmv.data_correct_flag = 1;
                opmv.move_y_flag = 1;
                opmv.move_x_flag = 1;
                break;
            case 0XF1F2:
                opmv.data_correct_flag = 1;
                opmv.move_y_flag = 1;
                opmv.move_x_flag = -1;
                break;
            case 0XF2F0:
                opmv.data_correct_flag = 1;
                opmv.move_y_flag = -1;
                opmv.move_x_flag = 0;
                break;
             case 0XF2F1:
                opmv.data_correct_flag = 1;
                opmv.move_y_flag = -1;
                opmv.move_x_flag = 1;
                break;
            case 0XF2F2:
                opmv.data_correct_flag = 1;
                opmv.move_y_flag = -1;
                opmv.move_x_flag = -1;
                break;
            case 0XF3F3:
                opmv.data_correct_flag = 1;
                opmv.turn_left_flag_90 = 1;
                break;
            case 0XF3F4:
                opmv.data_correct_flag = 1;
                opmv.turn_left_flag = 1;
                break;
            case 0XF4F3:
                opmv.data_correct_flag = 1;
                opmv.turn_right_flag_90 = 1;
                break;
            case 0XF4F4:
                opmv.data_correct_flag = 1;
                opmv.turn_right_flag = 1;
                break;
            case 0XF5F5:
                opmv.data_correct_flag = 1;
                opmv.empty_move_flag = 1;
                break;

        }
    }
	else
		state = 0;
}



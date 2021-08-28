/**
  ******************************************************************************
  * @file    Remote_Control.c
  * @author  DJI 
  * @version V1.0.0
  * @date    2015/11/15
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#include "Remote_Control.h"
#include "main.h"
#include "bsp_usart.h"

RC_Type remote_control;
uint32_t  Latest_Remote_Control_Pack_Time = 0;
uint32_t  LED_Flash_Timer_remote_control = 0;
uint16_t TIM_COUNT[2];
/*******************************************************************************************
  * @Func		void Callback_RC_Handle(RC_Type* rc, uint8_t* buff)
  * @Brief  DR16接收机协议解码程序
  * @Param		RC_Type* rc　存储遥控器数据的结构体　　uint8_t* buff　用于解码的缓存
  * @Retval		None
  * @Date    
 *******************************************************************************************/
void Callback_RC_Handle(RC_Type* rc, uint8_t* buff)
{
//	rc->ch1 = (*buff | *(buff+1) << 8) & 0x07FF;	offset  = 1024
	rc->ch1 = (buff[0] | buff[1]<<8) & 0x07FF;
	rc->ch1 -= 1024;
	rc->ch2 = (buff[1]>>3 | buff[2]<<5 ) & 0x07FF;
	rc->ch2 -= 1024;
	rc->ch3 = (buff[2]>>6 | buff[3]<<2 | buff[4]<<10) & 0x07FF;
	rc->ch3 -= 1024;
	rc->ch4 = (buff[4]>>1 | buff[5]<<7) & 0x07FF;		
	rc->ch4 -= 1024;
	
	rc->switch_left = ( (buff[5] >> 4)& 0x000C ) >> 2;
	rc->switch_right =  (buff[5] >> 4)& 0x0003 ;
	
	rc->mouse.x = buff[6] | (buff[7] << 8);	//x axis
	rc->mouse.y = buff[8] | (buff[9] << 8);
	rc->mouse.z = buff[10]| (buff[11] << 8);
	
	rc->mouse.press_left 	= buff[12];	// is pressed?
	rc->mouse.press_right = buff[13];
	
	rc->keyBoard.key_code = buff[14] | buff[15] << 8; //key borad code
	
	rc->pulley = (buff[16] | buff[17] << 8) & 0x07FF;
	rc->pulley -= 1024;
	
	Latest_Remote_Control_Pack_Time = HAL_GetTick();
	
	if(Latest_Remote_Control_Pack_Time - LED_Flash_Timer_remote_control>500){
			
			HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
			
			LED_Flash_Timer_remote_control = Latest_Remote_Control_Pack_Time;
		
			
	}
	
}

extern uint16_t TIM_COUNT[];
int16_t HighTime;


/*******************************************************************************************
  * @Func		void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
  * @Brief  PWM接收机脉宽计算
  * @Param		TIM_HandleTypeDef *htim 用于测量PWM脉宽的定时器。
  * @Retval		None
  * @Date    
 *******************************************************************************************/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

	HighTime = (TIM_COUNT[1] - TIM_COUNT[0])>0?(TIM_COUNT[1] - TIM_COUNT[0]):((TIM_COUNT[1] - TIM_COUNT[0])+10000);
	
	Latest_Remote_Control_Pack_Time = HAL_GetTick();
			
  remote_control.ch4 = (HighTime - 4000)*660/4000;
	
	if(Latest_Remote_Control_Pack_Time - LED_Flash_Timer_remote_control>500){
			
			HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);		
			LED_Flash_Timer_remote_control = Latest_Remote_Control_Pack_Time;
					
	}
	
}

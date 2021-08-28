#include "key.h" 
#include "Remote_Control.h"
#include "user_lib.h"
#include "string.h"
#include "usart.h"

struct key_mouse keyboard;

int8_t sand_key[4]={0};

void key_init()     //按键初始化（归零）
{
	memset(&keyboard, 0, sizeof(struct key_mouse));
}

void key_task()
{
	/***********板间串口通信**************/
	sand_key[0]=keyboard.help_card;
	sand_key[1]=keyboard.moto_init;
	sand_key[2]=(uint8_t)remote_control.switch_left;
	sand_key[3]='F';
	HAL_UART_Transmit(&huart8, (uint8_t *)&sand_key, 4, 10);
	
	/***********初始化********************/
	if(remote_control.keyBoard.key_code & KEY_CTRL)
	{
		if(remote_control.keyBoard.key_code & KEY_SHIFT)
		{
			if(remote_control.keyBoard.key_code & KEY_R)
			{
				keyboard.moto_init = 1;
			}
		}
	}
	/***********传送矿石电机**************/
	if(remote_control.keyBoard.key_code == KEY_G && (keyboard.TP ==1 || keyboard.TP == 2))
	{
		keyboard.TP = 0;
	}
	
	if(remote_control.keyBoard.key_code & KEY_CTRL && (keyboard.TP ==1 || keyboard.TP == 0))
	{
		if(remote_control.keyBoard.key_code & KEY_G && (keyboard.TP ==1 || keyboard.TP == 0))
		{
			keyboard.TP = 2;
		}
	}
	
	if(remote_control.keyBoard.key_code & KEY_SHIFT && (keyboard.TP ==0 || keyboard.TP == 2))
	{
		if(remote_control.keyBoard.key_code & KEY_G && (keyboard.TP ==0 || keyboard.TP == 2))
		{
			keyboard.TP = 1;
		}
	}
	
	
	/***********机械爪动作**************/
	/*正常夹取*/
	if(remote_control.keyBoard.key_code == KEY_E && keyboard.get_resource==0)
	{
		if(keyboard.get_ground_resource == 0 && keyboard.get_third_resource == 0 &&  keyboard.exchange == 0 && keyboard.push_resource == 0)
		{
			keyboard.get_resource = 1;
		}
	}
	/*地面矿石夹取*/
	if(remote_control.keyBoard.key_code & KEY_CTRL && keyboard.get_ground_resource==0)
	{
		if(remote_control.keyBoard.key_code & KEY_E && keyboard.get_ground_resource==0)
		{
			if(keyboard.get_resource == 0 && keyboard.get_third_resource == 0 && keyboard.exchange == 0 && keyboard.push_resource == 0)
			{
				keyboard.get_ground_resource =1;
			}
		}
	}
	/*第三块小矿石夹取*/
	if(remote_control.keyBoard.key_code & KEY_SHIFT && keyboard.get_third_resource==0)
	{
		if(remote_control.keyBoard.key_code & KEY_V && keyboard.get_third_resource==0)
		{
			if(keyboard.get_resource == 0 && keyboard.get_ground_resource == 0 && keyboard.exchange == 0 && keyboard.push_resource == 0)
			{
				keyboard.get_third_resource = 1;
			}
		}
	}
	/*兑换矿石(及推动)*/
	if(remote_control.keyBoard.key_code & KEY_SHIFT && keyboard.exchange==0)
	{
		if(remote_control.keyBoard.key_code & KEY_C && keyboard.exchange==0)
		{
			if(keyboard.get_resource == 0 && keyboard.get_third_resource == 0 && keyboard.get_ground_resource == 0 && keyboard.push_resource == 0)
			{
				keyboard.exchange = 1;
			}
		}
	}
	
	/*兑换第二块*/
	if(remote_control.keyBoard.key_code & KEY_SHIFT && keyboard.exchange==0)
	{
		if(remote_control.keyBoard.key_code & KEY_R && keyboard.exchange==0)
		{
			if(keyboard.get_resource == 0 && keyboard.get_third_resource == 0 && keyboard.get_ground_resource == 0 && keyboard.push_resource == 0 && keyboard.moto_init == 0)
			{
				keyboard.exchange = 3;
			}
		}
	}
	
	/*推第二块*/
	if(remote_control.keyBoard.key_code & KEY_CTRL && keyboard.push_resource==0)
	{
		if(remote_control.keyBoard.key_code & KEY_R && keyboard.push_resource==0)
		{
			if(keyboard.get_resource == 0 && keyboard.get_third_resource == 0 && keyboard.get_ground_resource == 0 && keyboard.exchange == 0 && keyboard.moto_init == 0)
			{
				keyboard.push_resource = 2;
			}
		}
	}
	
	/*兑换第三块*/
	if(remote_control.keyBoard.key_code & KEY_SHIFT && keyboard.push_resource==0)
	{
		if(remote_control.keyBoard.key_code & KEY_E && keyboard.push_resource==0)
		{
			if(keyboard.get_resource == 0 && keyboard.get_third_resource == 0 && keyboard.get_ground_resource == 0 && keyboard.push_resource == 0)
			{
				keyboard.exchange = 2;
			}
		}
	}
	/*推*/
	if(remote_control.keyBoard.key_code & KEY_SHIFT && keyboard.push_resource==0)
	{
		if(remote_control.keyBoard.key_code & KEY_F && keyboard.push_resource==0)
		{
			if(keyboard.get_resource == 0 && keyboard.get_third_resource == 0 && keyboard.get_ground_resource == 0 && keyboard.exchange == 0)
			{
				keyboard.push_resource = 1;
			}
		}
	}
	
	
	/*************抬升动作***************/
	/*初始位置*/
	if(remote_control.keyBoard.key_code == KEY_Q && (keyboard.up_altitude==1 || keyboard.up_altitude==2))
	{
		keyboard.up_altitude = 0;
	}
	/*小资源岛抬升*/	
	if(remote_control.keyBoard.key_code & KEY_SHIFT && (keyboard.up_altitude==0 || keyboard.up_altitude==2))
	{
		if(remote_control.keyBoard.key_code & KEY_Q && (keyboard.up_altitude==0 || keyboard.up_altitude==2))
		{
			keyboard.up_altitude =1;
		}
	}
	/*大资源岛抬升*/
	if(remote_control.keyBoard.key_code & KEY_CTRL && (keyboard.up_altitude==0 || keyboard.up_altitude==1))
	{
		if(remote_control.keyBoard.key_code & KEY_Q && (keyboard.up_altitude==0 || keyboard.up_altitude==1))
		{
			keyboard.up_altitude =2;
		}
	}
	
	/*************救援卡****************/
//	if(remote_control.keyBoard.key_code == KEY_F && keyboard.help_card==1)
//	{
//		keyboard.help_card = 0;
//	}
//		
//	if(remote_control.keyBoard.key_code & KEY_CTRL && keyboard.help_card==0)
//	{
//		if(remote_control.keyBoard.key_code & KEY_F && keyboard.help_card==0)
//		{
//			keyboard.help_card =1;
//		}
//	}
	
	/***********救援气缸*************/
//	if(remote_control.keyBoard.key_code == KEY_C && keyboard.help_air==0)
//	{
//		keyboard.help_air = 1;
//	}
//	
//	if(remote_control.keyBoard.key_code & KEY_CTRL && keyboard.help_air==1)
//	{
//		if(remote_control.keyBoard.key_code & KEY_C && keyboard.help_air==1)
//		{
//			keyboard.help_air = 0;
//		}
//	}
	/**************矿石一键兑换*************/
	if(remote_control.keyBoard.key_code & KEY_CTRL && keyboard.TogetherExchange_three == 0)
	{
		if(remote_control.keyBoard.key_code & KEY_F && keyboard.TogetherExchange_three == 0)
		{
//			if(keyboard.get_resource == 0 && keyboard.get_third_resource == 0 && keyboard.get_ground_resource == 0 && keyboard.exchange == 0 && keyboard.push_resource==0 && keyboard.TogetherExchange_two == 0)
//			{
//				keyboard.TogetherExchange_three = 1;
//			}
			keyboard.TogetherExchange_three = 1;
		}
	}
	
	if(remote_control.keyBoard.key_code & KEY_CTRL && keyboard.TogetherExchange_two == 0)
	{
		if(remote_control.keyBoard.key_code & KEY_C && keyboard.TogetherExchange_two == 0)
		{
			if(keyboard.get_resource == 0 && keyboard.get_third_resource == 0 && keyboard.get_ground_resource == 0 && keyboard.exchange == 0 && keyboard.push_resource==0 && keyboard.TogetherExchange_three == 0)
			{
				keyboard.TogetherExchange_two = 1;
			}
		}
	}
	
	
	
	/*************弹舱******************/
	/*左*/
	if(remote_control.keyBoard.key_code == KEY_B && keyboard.right_supply==1)
	{
		keyboard.right_supply = 0;
	}
	if(remote_control.keyBoard.key_code & KEY_CTRL && keyboard.right_supply==0)
	{
		if(remote_control.keyBoard.key_code & KEY_B && keyboard.right_supply==0)
		{
			keyboard.right_supply = 1;
		}
	}
	/*右*/
	if(remote_control.keyBoard.key_code == KEY_V && keyboard.left_supply==1)
	{
		keyboard.left_supply = 0;
	}
	if(remote_control.keyBoard.key_code & KEY_CTRL && keyboard.left_supply==0)
	{
		if(remote_control.keyBoard.key_code & KEY_V && keyboard.left_supply==0)
		{
			keyboard.left_supply = 1;
		}
	}
	
	/*************舵机云台***************/
//	if(remote_control.keyBoard.key_code == KEY_R)
//	{
//		keyboard.view_change = 0;
//	}
//	if(remote_control.keyBoard.key_code & KEY_CTRL && remote_control.keyBoard.key_code != 304)
//	{
//		if(remote_control.keyBoard.key_code & KEY_R)
//		{
//			keyboard.view_change = 1;
//		}
//	}
//	if(remote_control.keyBoard.key_code & KEY_SHIFT && remote_control.keyBoard.key_code != 304)
//	{
//		if(remote_control.keyBoard.key_code & KEY_R)
//		{
//			keyboard.view_change = 2;
//		}
//	}
	
	/***************障碍块夹取*****************/
	if(remote_control.keyBoard.key_code == KEY_X)
	{
		keyboard.block = 2;
	}
	if(remote_control.keyBoard.key_code & KEY_CTRL)
	{
		if(remote_control.keyBoard.key_code & KEY_X)
		{
			keyboard.block = 1;
		}
	}
	if(remote_control.keyBoard.key_code & KEY_SHIFT && keyboard.block == 0)
	{
		if(remote_control.keyBoard.key_code & KEY_X && keyboard.block == 0)
		{
			keyboard.block = 3;
		}
	}
	if(remote_control.keyBoard.key_code == 0)
	{
		keyboard.block = 0;
	}
	
	/*****************障碍块抬升********************/
	if(remote_control.keyBoard.key_code == KEY_Z && (keyboard.block_up == 0 || keyboard.block_up == 1))
	{
		keyboard.block_up = 1;
	}
	if(remote_control.keyBoard.key_code & KEY_CTRL && (keyboard.block_up == 0 || keyboard.block_up == 2))
	{
		if(remote_control.keyBoard.key_code & KEY_Z && (keyboard.block_up == 0 || keyboard.block_up == 2))
		{
			keyboard.block_up = 2;
		}
	}
	if(remote_control.keyBoard.key_code == 0)
	{
		keyboard.block_up = 0;
	}
	
}	




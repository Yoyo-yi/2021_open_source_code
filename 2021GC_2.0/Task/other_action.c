#include "Other_action.h"
#include "Remote_Control.h"
#include "tim.h"
#include "bsp_can.h"
#include "pid.h"
#include "key.h"
#include "math.h"
#include "cmsis_os.h"

#define Block_speed 8000         //夹紧电机速度（驱动蜗轮蜗杆）


pid_typedef help_moto_pid;          //救援卡电机速度环PID结构体
pid_typedef help_moto_angle_pid;    //救援卡电机位置环PID
pid_typedef block_moto_pid;         //障碍块夹紧电机速度环PID
pid_typedef block_moto_angle_pid;   //障碍块夹紧电机位置环PID

extern moto_measure help_moto;      //救援卡电机反馈数据
extern moto_measure block_moto;     //障碍块夹紧电机反馈数据
extern struct key_mouse keyboard;

/******************气缸*****************/
#define AIR_HELP_ON      HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1 , GPIO_PIN_SET);       //救援气缸
#define AIR_HELP_OFF     HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1 , GPIO_PIN_RESET);
/****************微动开关***************/
#define MOTO_SIGNAL_DWON   HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);                    
#define MOTO_SIGNAL_UP     HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);
#define MOTO_SIGNAL1_DWON   HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2);
#define MOTO_SIGNAL1_UP     HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
/************障碍块电机宏定义***********/

#define IN1_UP         HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);          //电机状态控制（正反转），L298N IN口输入
#define IN1_DOWN       HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
#define IN2_UP         HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
#define IN2_DOWN       HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);

//电机限位函数声明
extern int limit_moto(CAN_HandleTypeDef *hcan ,moto_measure *mea ,pid_typedef *angle, pid_typedef *speed, int32_t set, int32_t angle_offset);

//===================任务初始化====================//

//==救援卡初始化==//
int32_t help_moto_start_angle;
void help_card_init()
{
	pid_init(&help_moto_pid);
	pid_init(&help_moto_angle_pid);
	help_moto_pid.f_param_init(&help_moto_pid, 0,    8000,   16384,  500000,   5,      0, 5, 0, 0);
										//目标值,积分限幅,最大输出,最大误差,死区,pid_out, p, 	  i, d
	help_moto_angle_pid.f_param_init(&help_moto_angle_pid,0, 8000, 16384, 500000, 100, 0, 5, 0.0001, 1);
	
	for(int i=0; i<20000; i++)
	{
		help_moto_pid.f_calculate(&help_moto_pid, help_moto.speed, -1000);
		SET_CAN2Back_MOTOR_CURRENT(&hcan2, help_moto_pid.pid_out, block_moto_pid.pid_out, 0, 0);	
	}
	help_moto_start_angle = help_moto.total_angle+5000;
}

//==障碍块夹紧初始化==//
void block_moto_init()
{
	pid_init(&block_moto_pid);
	block_moto_pid.f_param_init(&block_moto_pid, 0,    16000,   16384,   18000,   10,      0, 7.3, 0.5, 0.01);
										  //目标值, 积分限幅,最大输出,最大误差,死区,pid_out,   p,    i,    d
}


//==障碍块抬升初始化==//
void block_up_init()
{
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);			//初始化电机速度
	TIM2->CCR1 = 20000;
}

//==舵机初始化==//
void steer_init()
{
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);       //左弹仓舵机
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);       //右弹舱舵机
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);       //图传舵机PWM通道使能
}

//==初始化集成==//
void Other_task_init()
{
	help_card_init();
	block_moto_init();
	steer_init();
	block_up_init();
}

//===================功能函数===================//

//==夹紧电机复位函数==//
void moto_reset()
{
	for(int i=0; i<3 ;i++)
	{
		block_moto_pid.set[i] = 0;
		block_moto_pid.get[i] = 0;
		block_moto_pid.err[i] = 0;
	}
	block_moto_pid.pout = 0;
	block_moto_pid.iout = 0;
	block_moto_pid.dout = 0;
	keyboard.block = 0;
}

//==微动开关动态识别==//
int8_t Micro_single[4];  	//信号数组：[0]:下微动开关 [1]:上微动开关 （1为闭合状态）

int8_t block_up_flag;    	//电机动作标识位

void PIN_change()			//微动开关状态识别（障碍块抬升姿态读取）
{
	int k = keyboard.block_up;
	Micro_single[0] = MOTO_SIGNAL_DWON;                  //读取微动开关in口的电平信号
	Micro_single[1] = MOTO_SIGNAL_UP;
	
	Micro_single[2] = MOTO_SIGNAL1_DWON;                  //读取微动开关in口的电平信号
	Micro_single[3] = MOTO_SIGNAL1_UP;
	
	//信号判断
	if(Micro_single[0]==0 && Micro_single[1]==0)      //两个开关都未触碰时，电机正常上下
	{
		switch (k)
		{
			case 0 :
				block_up_flag = 0;
			break;
			
			case 1 :
				block_up_flag = 2;
			break;
			
			case 2 :
				block_up_flag =1;
			break;
				
		}
	}
	else if(Micro_single[0]==0 && Micro_single[1]==1)  //下开关闭合，电机不可再向下运动，下按键输出为悬空
	{
		switch (k)
		{
			case 0 :
				block_up_flag = 0;
			break;
			
			case 1 :
				block_up_flag = 2;
			break;
			
			case 2 :
				block_up_flag =0;
			break;
				
		}
	}
	else if(Micro_single[0]==1 && Micro_single[1]==0)  //上开关闭合，电机不可向上运动，上按键输出为悬空
	{
		switch (k)
		{
			case 0 :
				block_up_flag = 0;
			break;
			
			case 1 :
				block_up_flag = 0;
			break;
			
			case 2 :
				block_up_flag =1;
			break;
				
		}
	}
}

void block_moto_speed_change()          //调整障碍块夹紧机构蜗轮蜗杆速度
{
	if(remote_control.mouse.press_left == 1 && remote_control.mouse.press_right == 0)
	{
		TIM2->CCR1 = 10000;
	}
	else
	{
		TIM2->CCR1 = 20000;
	}
}

//===================任务=======================//
//==救援气缸==//
void help_air_task()
{
	if(keyboard.help_air == 1)
	{
		AIR_HELP_ON;					//救援气缸开启
	}
	else if(keyboard.help_air == 0)
	{
		AIR_HELP_OFF;					//救援气缸关闭
	}
}


//==救援卡任务==//
void help_card_task()
{
	if(remote_control.switch_left == 2)
	{
		help_moto_pid.pid_out = 0;
	}
	else
	{
		if(keyboard.help_card == 0)
		{
		limit_moto(&hcan2, &help_moto, &help_moto_angle_pid, &help_moto_pid, help_moto_start_angle+87000, 0);
		}
		else if(keyboard.help_card == 1)
		{
		limit_moto(&hcan2, &help_moto, &help_moto_angle_pid, &help_moto_pid, help_moto_start_angle, 0);
		}
	}
	
}

//==弹舱任务==//
void supply_task()
{
	if(keyboard.left_supply == 1)
	{
		TIM4->CCR2 =2500;
	}
	else if(keyboard.left_supply == 0)
	{
		TIM4->CCR2 =500;
	}
	
	if(keyboard.right_supply == 1)
	{
		TIM4->CCR1 =500;
		
	}
	else if(keyboard.right_supply == 0)
	{
		TIM4->CCR1 =2500;
		
	}
}

//==障碍块夹紧任务==//
int32_t block_v;        //电机速度

void deal_block_task()
{
	int8_t block_get_flag = keyboard.block;   //按键更新
	
/**判断电机pid->iout输出，若超过设定阈值则直接视为最大iout*************/
	if(block_moto_pid.iout > 8000)  //阈值(+8000~-8000)
	{
		block_moto_pid.iout = 16000;
	}
	else if(block_moto_pid.iout < -8000)
	{
		block_moto_pid.iout = -16000;
	}
/**********************************/
	
	if(fabs(block_moto_pid.iout) != 16000)  //判断是否超过设定阈值
	{
		if(remote_control.pulley == 0)        //在遥控器拨盘未拨动的情况下才可以键鼠操作，否则以拨杆数值为准
		{
			switch (block_get_flag)
			{
				case 0:                      //停止运动，自锁固定
					block_v = 0;
				break;
				
				case 1:                      //夹取
					block_v = Block_speed;
				break;
				
				case 2:
					block_v = -Block_speed;    //张开
				break;
				
			}
			block_moto_pid.f_calculate(&block_moto_pid, block_moto.speed, block_v);
		}
		else
		{
			block_moto_pid.f_calculate(&block_moto_pid, block_moto.speed, remote_control.pulley*Block_speed/660);
		}
	}
	else if(keyboard.block == 3)       //到达阻塞iout判断后，须iout复位操作
	{
		moto_reset();              //将iout清零 
	}
	else                               //超过阈值pid为0输出，要进行复位操作后才可以再次使用
	{
		block_moto_pid.pid_out = 0;
	}
	SET_CAN2Back_MOTOR_CURRENT(&hcan2, help_moto_pid.pid_out, block_moto_pid.pid_out, 0, 0);
}

//==障碍块抬升任务==//
void Block_up_task()
{
	block_moto_speed_change();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
	PIN_change();
	switch (block_up_flag)
	{
		case 0:               //悬空
			IN1_DOWN;
			IN2_DOWN;
		break;
		
		case 1:               //下降
			IN1_UP;
			IN2_DOWN;
		break;
		
		case 2:
			IN1_DOWN;           //上升
			IN2_UP;
		break;
		
	}
}

//==舵机云台==//	
void view_change_task()
{
//	if(keyboard.view_change == 1)					//对抗赛用到，单项赛舵机跟随抬升动作,减少操作量
//	{
//		TIM4->CCR3 = 2100;
//	}
//	else if(keyboard.view_change == 0)
//	{
//		TIM4->CCR3 = 1300;
//	}
//	else if(keyboard.view_change == 2)
//	{
//		TIM4->CCR3 = 900;
//	}
}

//========任务归总=========//
void Other_action()                 //各项任务归总
{
	help_air_task();
	help_card_task();	
	supply_task();
	deal_block_task();
	Block_up_task();
	view_change_task();
}











#include "Other_action.h"
#include "Remote_Control.h"
#include "tim.h"
#include "bsp_can.h"
#include "pid.h"
#include "key.h"
#include "math.h"
#include "cmsis_os.h"

#define Block_speed 8000         //�н�����ٶȣ����������ϸˣ�


pid_typedef help_moto_pid;          //��Ԯ������ٶȻ�PID�ṹ��
pid_typedef help_moto_angle_pid;    //��Ԯ�����λ�û�PID
pid_typedef block_moto_pid;         //�ϰ���н�����ٶȻ�PID
pid_typedef block_moto_angle_pid;   //�ϰ���н����λ�û�PID

extern moto_measure help_moto;      //��Ԯ�������������
extern moto_measure block_moto;     //�ϰ���н������������
extern struct key_mouse keyboard;

/******************����*****************/
#define AIR_HELP_ON      HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1 , GPIO_PIN_SET);       //��Ԯ����
#define AIR_HELP_OFF     HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1 , GPIO_PIN_RESET);
/****************΢������***************/
#define MOTO_SIGNAL_DWON   HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);                    
#define MOTO_SIGNAL_UP     HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);
#define MOTO_SIGNAL1_DWON   HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2);
#define MOTO_SIGNAL1_UP     HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
/************�ϰ������궨��***********/

#define IN1_UP         HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);          //���״̬���ƣ�����ת����L298N IN������
#define IN1_DOWN       HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
#define IN2_UP         HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
#define IN2_DOWN       HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);

//�����λ��������
extern int limit_moto(CAN_HandleTypeDef *hcan ,moto_measure *mea ,pid_typedef *angle, pid_typedef *speed, int32_t set, int32_t angle_offset);

//===================�����ʼ��====================//

//==��Ԯ����ʼ��==//
int32_t help_moto_start_angle;
void help_card_init()
{
	pid_init(&help_moto_pid);
	pid_init(&help_moto_angle_pid);
	help_moto_pid.f_param_init(&help_moto_pid, 0,    8000,   16384,  500000,   5,      0, 5, 0, 0);
										//Ŀ��ֵ,�����޷�,������,������,����,pid_out, p, 	  i, d
	help_moto_angle_pid.f_param_init(&help_moto_angle_pid,0, 8000, 16384, 500000, 100, 0, 5, 0.0001, 1);
	
	for(int i=0; i<20000; i++)
	{
		help_moto_pid.f_calculate(&help_moto_pid, help_moto.speed, -1000);
		SET_CAN2Back_MOTOR_CURRENT(&hcan2, help_moto_pid.pid_out, block_moto_pid.pid_out, 0, 0);	
	}
	help_moto_start_angle = help_moto.total_angle+5000;
}

//==�ϰ���н���ʼ��==//
void block_moto_init()
{
	pid_init(&block_moto_pid);
	block_moto_pid.f_param_init(&block_moto_pid, 0,    16000,   16384,   18000,   10,      0, 7.3, 0.5, 0.01);
										  //Ŀ��ֵ, �����޷�,������,������,����,pid_out,   p,    i,    d
}


//==�ϰ���̧����ʼ��==//
void block_up_init()
{
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);			//��ʼ������ٶ�
	TIM2->CCR1 = 20000;
}

//==�����ʼ��==//
void steer_init()
{
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);       //�󵯲ֶ��
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);       //�ҵ��ն��
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);       //ͼ�����PWMͨ��ʹ��
}

//==��ʼ������==//
void Other_task_init()
{
	help_card_init();
	block_moto_init();
	steer_init();
	block_up_init();
}

//===================���ܺ���===================//

//==�н������λ����==//
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

//==΢�����ض�̬ʶ��==//
int8_t Micro_single[4];  	//�ź����飺[0]:��΢������ [1]:��΢������ ��1Ϊ�պ�״̬��

int8_t block_up_flag;    	//���������ʶλ

void PIN_change()			//΢������״̬ʶ���ϰ���̧����̬��ȡ��
{
	int k = keyboard.block_up;
	Micro_single[0] = MOTO_SIGNAL_DWON;                  //��ȡ΢������in�ڵĵ�ƽ�ź�
	Micro_single[1] = MOTO_SIGNAL_UP;
	
	Micro_single[2] = MOTO_SIGNAL1_DWON;                  //��ȡ΢������in�ڵĵ�ƽ�ź�
	Micro_single[3] = MOTO_SIGNAL1_UP;
	
	//�ź��ж�
	if(Micro_single[0]==0 && Micro_single[1]==0)      //�������ض�δ����ʱ�������������
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
	else if(Micro_single[0]==0 && Micro_single[1]==1)  //�¿��رպϣ���������������˶����°������Ϊ����
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
	else if(Micro_single[0]==1 && Micro_single[1]==0)  //�Ͽ��رպϣ�������������˶����ϰ������Ϊ����
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

void block_moto_speed_change()          //�����ϰ���н����������ϸ��ٶ�
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

//===================����=======================//
//==��Ԯ����==//
void help_air_task()
{
	if(keyboard.help_air == 1)
	{
		AIR_HELP_ON;					//��Ԯ���׿���
	}
	else if(keyboard.help_air == 0)
	{
		AIR_HELP_OFF;					//��Ԯ���׹ر�
	}
}


//==��Ԯ������==//
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

//==��������==//
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

//==�ϰ���н�����==//
int32_t block_v;        //����ٶ�

void deal_block_task()
{
	int8_t block_get_flag = keyboard.block;   //��������
	
/**�жϵ��pid->iout������������趨��ֵ��ֱ����Ϊ���iout*************/
	if(block_moto_pid.iout > 8000)  //��ֵ(+8000~-8000)
	{
		block_moto_pid.iout = 16000;
	}
	else if(block_moto_pid.iout < -8000)
	{
		block_moto_pid.iout = -16000;
	}
/**********************************/
	
	if(fabs(block_moto_pid.iout) != 16000)  //�ж��Ƿ񳬹��趨��ֵ
	{
		if(remote_control.pulley == 0)        //��ң��������δ����������²ſ��Լ�������������Բ�����ֵΪ׼
		{
			switch (block_get_flag)
			{
				case 0:                      //ֹͣ�˶��������̶�
					block_v = 0;
				break;
				
				case 1:                      //��ȡ
					block_v = Block_speed;
				break;
				
				case 2:
					block_v = -Block_speed;    //�ſ�
				break;
				
			}
			block_moto_pid.f_calculate(&block_moto_pid, block_moto.speed, block_v);
		}
		else
		{
			block_moto_pid.f_calculate(&block_moto_pid, block_moto.speed, remote_control.pulley*Block_speed/660);
		}
	}
	else if(keyboard.block == 3)       //��������iout�жϺ���iout��λ����
	{
		moto_reset();              //��iout���� 
	}
	else                               //������ֵpidΪ0�����Ҫ���и�λ������ſ����ٴ�ʹ��
	{
		block_moto_pid.pid_out = 0;
	}
	SET_CAN2Back_MOTOR_CURRENT(&hcan2, help_moto_pid.pid_out, block_moto_pid.pid_out, 0, 0);
}

//==�ϰ���̧������==//
void Block_up_task()
{
	block_moto_speed_change();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
	PIN_change();
	switch (block_up_flag)
	{
		case 0:               //����
			IN1_DOWN;
			IN2_DOWN;
		break;
		
		case 1:               //�½�
			IN1_UP;
			IN2_DOWN;
		break;
		
		case 2:
			IN1_DOWN;           //����
			IN2_UP;
		break;
		
	}
}

//==�����̨==//	
void view_change_task()
{
//	if(keyboard.view_change == 1)					//�Կ����õ����������������̧������,���ٲ�����
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

//========�������=========//
void Other_action()                 //�����������
{
	help_air_task();
	help_card_task();	
	supply_task();
	deal_block_task();
	Block_up_task();
	view_change_task();
}











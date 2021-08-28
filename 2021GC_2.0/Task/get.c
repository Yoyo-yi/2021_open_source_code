#include "get.h"
#include "main.h"
#include "Remote_Control.h"
#include "can.h"
#include "bsp_can.h"
#include "pid.h"
#include "key.h"
#include "user_lib.h"
#include "math.h"
#include "cmsis_os.h"

//========�궨��==========//
#define AIR_PUSH_ON     HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0 , GPIO_PIN_SET);    //ǰ������
#define AIR_PUSH_OFF    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0 , GPIO_PIN_RESET);
#define AIR_GET_ON     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5 , GPIO_PIN_SET);     //��ȡ����
#define AIR_GET_OFF    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5 , GPIO_PIN_RESET);


pid_typedef lift_moto_pid[2];               //̧������ٶȻ�PID�ṹ��
pid_typedef lift_moto_angle_pid[2];         //̧�����λ�û�
pid_typedef tp_moto_pid[2];                 //���͵���ٶȻ�
pid_typedef spin_moto_pid[2];               //��е�з�ת����ٶȻ�
pid_typedef spin_moto_angle_pid[2];         //��е�з�ת���λ�û�

extern moto_measure lift_moto[2];           //̧������������ݽṹ��
extern moto_measure tp_moto[2];             //���͵����������
extern moto_measure spin_moto[2];           //��е�з�ת�����������
extern struct key_mouse keyboard;           //��������


//�����λ��������
extern int limit_moto(CAN_HandleTypeDef *hcan ,moto_measure *mea ,pid_typedef *angle, pid_typedef *speed, int32_t set, int32_t angle_offset);


//==���Ϳ�ʯ��ʼ��==//
void tp_init()
{
	for(int i=0; i<2; i++)
	{
		pid_init(&tp_moto_pid[i]);
		
		tp_moto_pid[i].f_param_init(&tp_moto_pid[i], 0,    8000,   16384,    15000,   0,      0, 7.3, 0.85, 0.01);
											  //Ŀ��ֵ,�����޷�,������,������,����,pid_out,   p,    i,    d
	}
}	

//==��ȡ������ʼ��==//
int32_t lift_moto_start_angle[2];            //��¼����ϵ�Ƕ�ֵ����
int32_t spin_moto_start_angle[2];
void get_init()
{
	/*̧�����*/
	//ID5��ID6
	for(int i=0; i<2; i++)
	{
		pid_init(&lift_moto_pid[i]);
		pid_init(&lift_moto_angle_pid[i]);
		
		lift_moto_pid[i].f_param_init(&lift_moto_pid[i], 0,    8000,   16384, 500000,   0,      0, 5, 0, 0);
												  //Ŀ��ֵ,�����޷�,������,������,����,pid_out, p, i, d
		lift_moto_angle_pid[i].f_param_init(&lift_moto_angle_pid[i],0, 8000, 16384, 500000, 10, 0, 7.3, 0.1, 1);
	}	
	
	lift_moto_start_angle[0] = lift_moto[0].total_angle;
	lift_moto_start_angle[1] = lift_moto[1].total_angle;
	
	/*��ȡ��ת���*/
	for(int i=0; i<2; i++)
	{
		pid_init(&spin_moto_pid[i]);
		pid_init(&spin_moto_angle_pid[i]);
		
		spin_moto_pid[i].f_param_init(&spin_moto_pid[i], 0,    8000,   16384, 5000000,   300,      0, 5, 0, 0);
												  //Ŀ��ֵ,�����޷�,������,������,	����,pid_out, p, i, d
		spin_moto_angle_pid[i].f_param_init(&spin_moto_angle_pid[i],0, 8000, 16384, 500000, 50, 0, 3.5, 0, 1);
		
	}
}

void get_task_init()		//��ȡ�����ʼ��
{
	tp_init();
	get_init();
}

//===================���ܺ���===================//

//==̧���������ˢ��==//
void lift_total_angle_update()		//ˢ��round_cnt��total_angle,�������ƫ�Ƽ����
{
	for(int i=0; i<2; i++)
	{
		lift_moto[i].round_cnt = 0;
	}
}

int32_t k=0;
int32_t p=0;
//==���������ʼ״̬==//
void spin_moto_find_set()
{
	if(keyboard.moto_init == 1)
	{
		p++;
		for(int i=0; i<20000; i++)						//��ת�����ʼ��λ
		{
			for(int j=0; j<2; j++)
			{
				spin_moto_pid[j].f_calculate(&spin_moto_pid[j], spin_moto[j].speed, pow(-1,j)*1500);
				SET_CAN2Ahead_MOTOR_CURRENT(&hcan2, tp_moto_pid[0].pid_out, tp_moto_pid[1].pid_out, spin_moto_pid[0].pid_out, spin_moto_pid[1].pid_out);
			}
		}
		spin_moto_start_angle[0] = spin_moto[0].total_angle-5000;			//���Դ�צ�ӿ��ܻ��������ѹ��̫�࣬ʹ��ת������������������ԭ�м�¼ֵ�ϼ�5000
		spin_moto_start_angle[1] = spin_moto[1].total_angle+5000;
		spin_moto_angle_pid[0].target =  spin_moto_start_angle[0];
		spin_moto_angle_pid[1].target =  spin_moto_start_angle[1];
		
		keyboard.moto_init = 0;
	}
}

//===================����=======================//

//==̧������==//
int8_t K=5;
void Lift_task()
{
	if(remote_control.switch_left == 2)			//̧������
	{
		for(int i=0; i<2; i++)
		{
			lift_moto_pid[i].pid_out = 0;
		}
	}
	else 
	{
		if(keyboard.up_altitude == 1)					//С��Դ������ʯ�һ�
		{
			
			limit_moto(&hcan1, &lift_moto[0], &lift_moto_angle_pid[0], &lift_moto_pid[0], lift_moto_start_angle[0]+253000, 0);
			limit_moto(&hcan1, &lift_moto[1], &lift_moto_angle_pid[1], &lift_moto_pid[1], lift_moto_start_angle[1]-253000, 0);	
			TIM4->CCR3 = 900;
		}
		else if(keyboard.up_altitude == 2)				//����Դ��
		{
			limit_moto(&hcan1, &lift_moto[0], &lift_moto_angle_pid[0], &lift_moto_pid[0], lift_moto_start_angle[0]+130000, 0);
			limit_moto(&hcan1, &lift_moto[1], &lift_moto_angle_pid[1], &lift_moto_pid[1], lift_moto_start_angle[1]-130000, 0);
			TIM4->CCR3 = 2100;
		}
		else if(keyboard.up_altitude == 0)				//��ʼ
		{
			for(int i=0; i<2; i++)
			{
				lift_moto_pid[i].pid_out = 0;							//�������䣨��ȷ��ÿһ�����䵽�ײ�������̧���߶Ⱥ�Ԥ��߶�ƫ�
			}
			lift_total_angle_update();									//����round_cnt��total_angle
			lift_moto_start_angle[0] = lift_moto[0].total_angle;		//ˢ�¶�̬Ŀ��ֵ
			lift_moto_start_angle[1] = lift_moto[1].total_angle;
			TIM4->CCR3 = 1300;
		}
	}
	SET_CAN1Ahead_MOTOR_CURRENT(&hcan1, lift_moto_pid[0].pid_out, lift_moto_pid[1].pid_out, chassis_moto_pid[0].pid_out, chassis_moto_pid[1].pid_out);
	
	Niming_send(lift_moto[0].angle, lift_moto[0].total_angle, lift_moto[1].angle, lift_moto[1].total_angle);		//��λ��
	
}

//==��ʯ����==//
void tp_task()
{
	if(remote_control.switch_left == 2)			//���͵������
	{
		tp_moto_pid[0].pid_out = 0;
		tp_moto_pid[1].pid_out = 0;
	}
	else
	{
		if(keyboard.TP == 1)					//����
		{
			for(int i=0; i<2; i++)
			{
				tp_moto_pid[i].f_calculate(&tp_moto_pid[i], tp_moto[i].speed, pow(-1,i)*5500);			
			}
		}
		else if(keyboard.TP == 2)				//����
		{
			for(int i=0; i<2; i++)
			{
				tp_moto_pid[i].f_calculate(&tp_moto_pid[i], tp_moto[i].speed, pow(-1,i)*-5500);
			}
		}
		else if(keyboard.TP == 0)				//ֹͣת��
		{
			tp_moto_pid[0].pid_out = 0;
			tp_moto_pid[1].pid_out = 0;
		}
	}
		SET_CAN2Ahead_MOTOR_CURRENT(&hcan2, tp_moto_pid[0].pid_out, tp_moto_pid[1].pid_out, spin_moto_pid[0].pid_out, spin_moto_pid[1].pid_out);
}

//==��ȡ����==//
int8_t time_mark = 0;							//ʱ���г��ź�
uint32_t Ti = 0;
int8_t KK;
void get_task()									//����Դ����500mm��ǰ�����׿�����С��Դ����700mm����Ҫ�����ʯ��
{
	KK = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
	spin_moto_find_set();
	/*������ȡ����*/
	if(keyboard.get_resource)				
	{
		if(time_mark == 0)						//ʱ����ʼ��¼
		{
			Ti   = HAL_GetTick();
			time_mark = 1; 
		}
		if((HAL_GetTick()-Ti)>0 && (HAL_GetTick()-Ti)<200)              
		{
			if(keyboard.up_altitude == 1)								//ʶ��̧���߶ȣ�����Դ����ǰ�ƣ�С��Դ��ֱ�Ӽ�ȡ
			{
				;														//��������
			}
			else if(keyboard.up_altitude == 2)
			{
				AIR_PUSH_ON;											//ǰ��
			}
		}
		else if((HAL_GetTick()-Ti)>200 && (HAL_GetTick()-Ti)<700)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
		}
		else if((HAL_GetTick()-Ti)>700 && (HAL_GetTick()-Ti)<900)    
		{
			AIR_GET_ON;													//��ʯ�н�
		}
		else if((HAL_GetTick()-Ti)>900 && (HAL_GetTick()-Ti)<1100)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_start_angle[0], 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_start_angle[1], 0);
			
		}
		else if((HAL_GetTick()-Ti)>1100 && (HAL_GetTick()-Ti)<1600)
		{
			AIR_PUSH_OFF;												//�ջ�
		}
		else if((HAL_GetTick()-Ti)>1600 && (HAL_GetTick()-Ti)<2700)     
		{
			
			AIR_GET_OFF;												//��ʯ�ɿ�
			keyboard.TP = 1;											//����
		}
//		else if((HAL_GetTick()-Ti)>3000 && (HAL_GetTick()-Ti)<4000)						//��ʯ��̬����
//		{
//			keyboard.TP = 2;											//����							
//		}
//		else if((HAL_GetTick()-Ti)>4000 && (HAL_GetTick()-Ti)<5500)
//		{
//			keyboard.TP = 1;											//����
//		}
//		else if((HAL_GetTick()-Ti)>5500 && (HAL_GetTick()-Ti)<5600)
//		{
//			keyboard.TP = 0;											//ֹͣ����
//			keyboard.get_resource = 0;									//��ȡ�ź�λ����
//			time_mark = 0;												//ʱ��λ����
//		}
		else if((HAL_GetTick()-Ti)>2700 && (HAL_GetTick()-Ti)<2800)
		{
			keyboard.TP = 0;											//ֹͣ����
			keyboard.get_resource = 0;									//��ȡ�ź�λ����
			time_mark = 0;												//ʱ��λ����
		}
		
	}
	/*��ȡ�����ʯ*/
	if(keyboard.get_ground_resource)
	{
		if(time_mark == 0)						//ʱ����ʼλ��¼
		{
			Ti   = HAL_GetTick();
			time_mark = 1; 
		}
		if((HAL_GetTick()-Ti)>0 && (HAL_GetTick()-Ti)<500)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת��еצ
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
		}
		else if((HAL_GetTick()-Ti)>500 && (HAL_GetTick()-Ti)<1000)
		{
			AIR_GET_ON;													//�н���ʯ
		}
		else if((HAL_GetTick()-Ti)>1000 && (HAL_GetTick()-Ti)<1100)
		{
			remote_control.keyBoard.key_code = 0x1010;					//����SHIFT+X��ˢ�¼н����iout��
		}
		else if((HAL_GetTick()-Ti)>1100 && (HAL_GetTick()-Ti)<1500)
		{
			remote_control.keyBoard.key_code = 0x1020;					//����CTRL+X���ɿ������ʯ��
		}
		else if((HAL_GetTick()-Ti)>1500 && (HAL_GetTick()-Ti)<2000)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_start_angle[0], 0);	//�ص���ʼ��¼λ��
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_start_angle[1], 0);
		}
		else if((HAL_GetTick()-Ti)>2000 && (HAL_GetTick()-Ti)<3000)
		{
			AIR_GET_OFF;												//�ɿ���ʯ
			keyboard.TP = 1;											//���Ϳ�ʯ������
		}
		else if((HAL_GetTick()-Ti)>3000 && (HAL_GetTick()-Ti)<4400)
		{
			keyboard.TP = 2;											//����							
			
			
		}
		else if((HAL_GetTick()-Ti)>4400 && (HAL_GetTick()-Ti)<6100)
		{
			keyboard.TP = 1;											//����
		}
		else if((HAL_GetTick()-Ti)>6100 && (HAL_GetTick()-Ti)<6200)
		{
			keyboard.TP = 0;											//ֹͣ����
			keyboard.get_ground_resource = 0;									//��ȡ�ź�λ����
			time_mark = 0;												//ʱ��λ����
		}
	}
	/*������С��ʯ��ȡ*/
	if(keyboard.get_third_resource)
	{
		if(time_mark == 0)
		{
			Ti   = HAL_GetTick();
			time_mark = 1; 
		}
		if((HAL_GetTick()-Ti)>0 && (HAL_GetTick()-Ti)<200)
		{
			if(keyboard.up_altitude == 1)								//ʶ��̧���߶ȣ�����Դ����ǰ�ƣ�С��Դ��ֱ�Ӽ�ȡ
			{
				;														//��������
			}
			else if(keyboard.up_altitude == 2)
			{
				AIR_PUSH_ON;											//ǰ��
			}
		}
		else if((HAL_GetTick()-Ti)>200 && (HAL_GetTick()-Ti)<700)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
		}
		else if((HAL_GetTick()-Ti)>700 && (HAL_GetTick()-Ti)<1200)
		{
			AIR_GET_ON;														//�н���ʯ
		}
		else if((HAL_GetTick()-Ti)>1200 && (HAL_GetTick()-Ti)<1700)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500/2, 0);	//��תһ���г�
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500/2, 0);
		}
		else if((HAL_GetTick()-Ti)>1700 && (HAL_GetTick()-Ti)<1800)
		{
			AIR_PUSH_OFF;
			keyboard.get_third_resource = 0;
			time_mark = 0;
		}
	}
	
	/*��ʯ�һ�*/
	if(keyboard.exchange == 1)
	{
		if(time_mark == 0)
		{
			Ti   = HAL_GetTick();
			time_mark = 1; 
		}
		if((HAL_GetTick()-Ti)>0 && (HAL_GetTick()-Ti)<1500)
		{
			keyboard.TP = 2;											//����
		}
		else if((HAL_GetTick()-Ti)>1500 && (HAL_GetTick()-Ti)<2000)
		{
			keyboard.TP = 0;											//ֹͣ����
			AIR_GET_ON;													//�н���ʯ
		}
		else if((HAL_GetTick()-Ti)>2000 && (HAL_GetTick()-Ti)<2500)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
			keyboard.exchange = 0;										//�һ���ʯ�ź�λ����
			time_mark = 0;												//ʱ��λ����
		}
//		else if((HAL_GetTick()-Ti)>1000 && (HAL_GetTick()-Ti)<1500)
//		{
//			AIR_GET_OFF;
//		}
	}
	
	/*�һ��ڶ���*/
	if(keyboard.exchange == 3)
	{
		if(time_mark == 0)
		{
			Ti   = HAL_GetTick();
			time_mark = 1; 
		}
		if((HAL_GetTick()-Ti)>0 && (HAL_GetTick()-Ti)<500)
		{
			AIR_GET_ON;													//�н���ʯ
		}
		else if((HAL_GetTick()-Ti)>500 && (HAL_GetTick()-Ti)<1000)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
			keyboard.exchange = 0;										//�һ���ʯ�ź�λ����
			time_mark = 0;												//ʱ��λ����
		}

	}
	
	/*�һ�������*/
	if(keyboard.exchange == 2)
	{
		if(time_mark == 0)
		{
			Ti   = HAL_GetTick();
			time_mark = 1; 
		}
		if((HAL_GetTick()-Ti)>0 && (HAL_GetTick()-Ti)<500)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
		}
		else if((HAL_GetTick()-Ti)>500 && (HAL_GetTick()-Ti)<600)
		{
			keyboard.exchange = 0;										//�ƿ����ź�λ����
			time_mark = 0;												//ʱ��λ����
			
		}
	}
	
	/*�ƿ�*/
	if(keyboard.push_resource == 1)
	{
		if(time_mark == 0)
		{
			Ti   = HAL_GetTick();
			time_mark = 1; 
		}
		if((HAL_GetTick()-Ti)>0 && (HAL_GetTick()-Ti)<500)
		{
			AIR_GET_OFF;												//�ɿ���ʯ
		}
		else if((HAL_GetTick()-Ti)>500 && (HAL_GetTick()-Ti)<1000)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_start_angle[0], 0);
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_start_angle[1], 0);
			
		}
		else if((HAL_GetTick()-Ti)>1000 && (HAL_GetTick()-Ti)<1500)
		{
			AIR_PUSH_ON;												//ǰ��
		}
		else if((HAL_GetTick()-Ti)>1500 && (HAL_GetTick()-Ti)<2000)
		{
			AIR_PUSH_OFF;
		}
		else if((HAL_GetTick()-Ti)>2000 && (HAL_GetTick()-Ti)<2500)
		{
			AIR_GET_ON;
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
		}
		else if((HAL_GetTick()-Ti)>2500 && (HAL_GetTick()-Ti)<3000)
		{
			AIR_PUSH_ON;
		}
		else if((HAL_GetTick()-Ti)>3000 && (HAL_GetTick()-Ti)<3200)
		{
			AIR_PUSH_OFF;
		}
		else if((HAL_GetTick()-Ti)>3200 && (HAL_GetTick()-Ti)<3500)
		{
			AIR_GET_OFF;
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_start_angle[0], 0);
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_start_angle[1], 0);
		}
		else if((HAL_GetTick()-Ti)>3500 && (HAL_GetTick()-Ti)<3600)
		{
			keyboard.push_resource = 0;									//�ƿ����ź�λ����
			time_mark = 0;												//ʱ��λ����
		}
	}
	
	/*�Ƶڶ����ʯ*/
	if(keyboard.push_resource == 2)
	{
		if(time_mark == 0)
		{
			Ti   = HAL_GetTick();
			time_mark = 1; 
		}
		if((HAL_GetTick()-Ti)>0 && (HAL_GetTick()-Ti)<500)
		{
			AIR_GET_OFF;												//�ɿ���ʯ
		}
		else if((HAL_GetTick()-Ti)>500 && (HAL_GetTick()-Ti)<1000)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_start_angle[0], 0);
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_start_angle[1], 0);
			
		}
		else if((HAL_GetTick()-Ti)>1000 && (HAL_GetTick()-Ti)<1500)
		{
			AIR_PUSH_ON;												//ǰ��
		}
		else if((HAL_GetTick()-Ti)>1500 && (HAL_GetTick()-Ti)<2000)
		{
			AIR_PUSH_OFF;
		}
		else if((HAL_GetTick()-Ti)>2000 && (HAL_GetTick()-Ti)<2500)
		{
			AIR_GET_ON;
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
		}
		else if((HAL_GetTick()-Ti)>2500 && (HAL_GetTick()-Ti)<3000)
		{
			AIR_PUSH_ON;
			keyboard.TP = 2;
		}
		else if((HAL_GetTick()-Ti)>3000 && (HAL_GetTick()-Ti)<3200)
		{
			AIR_PUSH_OFF;
		}
		else if((HAL_GetTick()-Ti)>3200 && (HAL_GetTick()-Ti)<3500)
		{
			AIR_GET_OFF;
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_start_angle[0], 0);
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_start_angle[1], 0);
		}
		else if((HAL_GetTick()-Ti)>3500 && (HAL_GetTick()-Ti)<3600)
		{
			keyboard.TP = 0;
			keyboard.push_resource = 0;									//�ƿ����ź�λ����
			time_mark = 0;												//ʱ��λ����
		}
	}
	
	/*������һ���һ�*/
	if(keyboard.TogetherExchange_three == 1)
	{
		if(time_mark == 0)
		{
			Ti   = HAL_GetTick();
			time_mark = 1; 
		}
		if((HAL_GetTick()-Ti)>0 && (HAL_GetTick()-Ti)<500)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
		}
		else if((HAL_GetTick()-Ti)>500 && (HAL_GetTick()-Ti)<700)
		{
			AIR_GET_OFF;
		}
		else if((HAL_GetTick()-Ti)>700 && (HAL_GetTick()-Ti)<1000)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_start_angle[0], 0);
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_start_angle[1], 0);
			
		}
		else if((HAL_GetTick()-Ti)>1000 && (HAL_GetTick()-Ti)<1500)
		{
			AIR_PUSH_ON;												//ǰ��
		}
		else if((HAL_GetTick()-Ti)>1500 && (HAL_GetTick()-Ti)<2000)
		{
			AIR_PUSH_OFF;
		}
		else if((HAL_GetTick()-Ti)>2000 && (HAL_GetTick()-Ti)<2500)
		{
			AIR_GET_ON;
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
		}
		else if((HAL_GetTick()-Ti)>2500 && (HAL_GetTick()-Ti)<3000)
		{
			AIR_PUSH_ON;
		}
		else if((HAL_GetTick()-Ti)>3000 && (HAL_GetTick()-Ti)<3200)
		{
			AIR_PUSH_OFF;
		}
		else if((HAL_GetTick()-Ti)>3200 && (HAL_GetTick()-Ti)<3700)
		{
			AIR_GET_OFF;
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_start_angle[0], 0);
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_start_angle[1], 0);
		}
		else if((HAL_GetTick()-Ti)>3700 && (HAL_GetTick()-Ti)<3900)
		{
			AIR_GET_ON;
		}
		else if((HAL_GetTick()-Ti)>3900 && (HAL_GetTick()-Ti)<4400)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
		}
		else if((HAL_GetTick()-Ti)>4400 && (HAL_GetTick()-Ti)<4600)
		{
			AIR_GET_OFF;
		}
		else if((HAL_GetTick()-Ti)>4600 && (HAL_GetTick()-Ti)<4900)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_start_angle[0], 0);
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_start_angle[1], 0);
			
		}
		else if((HAL_GetTick()-Ti)>4900 && (HAL_GetTick()-Ti)<5400)
		{
			AIR_PUSH_ON;												//ǰ��
		}
		else if((HAL_GetTick()-Ti)>5400 && (HAL_GetTick()-Ti)<5900)
		{
			AIR_PUSH_OFF;
		}
		else if((HAL_GetTick()-Ti)>5900 && (HAL_GetTick()-Ti)<6400)
		{
			AIR_GET_ON;
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
		}
		else if((HAL_GetTick()-Ti)>6400 && (HAL_GetTick()-Ti)<6900)
		{
			AIR_PUSH_ON;
			keyboard.TP = 2;
		}
		else if((HAL_GetTick()-Ti)>6900 && (HAL_GetTick()-Ti)<7100)
		{
			AIR_PUSH_OFF;
		}
		else if((HAL_GetTick()-Ti)>7100 && (HAL_GetTick()-Ti)<7600)
		{
			AIR_GET_OFF;
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_start_angle[0], 0);
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_start_angle[1], 0);
		}
		else if((HAL_GetTick()-Ti)>7600 && (HAL_GetTick()-Ti)<7900)
		{
			keyboard.TP = 0;
			AIR_GET_ON;
		}
		else if((HAL_GetTick()-Ti)>7900 && (HAL_GetTick()-Ti)<8300)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
		}
		else if((HAL_GetTick()-Ti)>8300 && (HAL_GetTick()-Ti)<8500)
		{
			AIR_GET_OFF;
		}
		else if((HAL_GetTick()-Ti)>8500 && (HAL_GetTick()-Ti)<9000)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_start_angle[0], 0);
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_start_angle[1], 0);
		}
		else if((HAL_GetTick()-Ti)>9000 && (HAL_GetTick()-Ti)<9500)
		{
			AIR_PUSH_ON;												//ǰ��
		}
		else if((HAL_GetTick()-Ti)>9500 && (HAL_GetTick()-Ti)<10000)
		{
			AIR_PUSH_OFF;
		}
		else if((HAL_GetTick()-Ti)>10000 && (HAL_GetTick()-Ti)<10400)
		{
			AIR_GET_ON;
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
		}
		else if((HAL_GetTick()-Ti)>10400 && (HAL_GetTick()-Ti)<10900)
		{
			AIR_PUSH_ON;
		}
		else if((HAL_GetTick()-Ti)>10900 && (HAL_GetTick()-Ti)<11100)
		{
			AIR_PUSH_OFF;
		}
		else if((HAL_GetTick()-Ti)>11100 && (HAL_GetTick()-Ti)<11600)
		{
			AIR_GET_OFF;
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_start_angle[0], 0);
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_start_angle[1], 0);
			keyboard.TogetherExchange_three = 0;					
			time_mark = 0;												//ʱ��λ����
		}
	}
		
	
	if(keyboard.TogetherExchange_two == 1)
	{
		if(time_mark == 0)
		{
			Ti   = HAL_GetTick();
			time_mark = 1; 
		}
		else if((HAL_GetTick()-Ti)>0 && (HAL_GetTick()-Ti)<200)
		{
			AIR_GET_ON;
		}
		else if((HAL_GetTick()-Ti)>200 && (HAL_GetTick()-Ti)<700)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
		}
		else if((HAL_GetTick()-Ti)>700 && (HAL_GetTick()-Ti)<900)
		{
			AIR_GET_OFF;
		}
		else if((HAL_GetTick()-Ti)>900 && (HAL_GetTick()-Ti)<1200)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_start_angle[0], 0);
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_start_angle[1], 0);
			
		}
		else if((HAL_GetTick()-Ti)>1200 && (HAL_GetTick()-Ti)<1700)
		{
			AIR_PUSH_ON;												//ǰ��
		}
		else if((HAL_GetTick()-Ti)>1700 && (HAL_GetTick()-Ti)<2200)
		{
			AIR_PUSH_OFF;
		}
		else if((HAL_GetTick()-Ti)>2200 && (HAL_GetTick()-Ti)<2700)
		{
			AIR_GET_ON;
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
		}
		else if((HAL_GetTick()-Ti)>2700 && (HAL_GetTick()-Ti)<3200)
		{
			AIR_PUSH_ON;
			keyboard.TP = 2;
		}
		else if((HAL_GetTick()-Ti)>3200 && (HAL_GetTick()-Ti)<3400)
		{
			AIR_PUSH_OFF;
		}
		else if((HAL_GetTick()-Ti)>3400 && (HAL_GetTick()-Ti)<3900)
		{
			AIR_GET_OFF;
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_start_angle[0], 0);
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_start_angle[1], 0);
		}
		else if((HAL_GetTick()-Ti)>3900 && (HAL_GetTick()-Ti)<4100)
		{
			keyboard.TP = 0;
			AIR_GET_ON;
		}
		else if((HAL_GetTick()-Ti)>4100 && (HAL_GetTick()-Ti)<4600)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
		}
		else if((HAL_GetTick()-Ti)>4600 && (HAL_GetTick()-Ti)<4800)
		{
			AIR_GET_OFF;
		}
		else if((HAL_GetTick()-Ti)>4800 && (HAL_GetTick()-Ti)<5300)
		{
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_start_angle[0], 0);
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_start_angle[1], 0);
		}
		else if((HAL_GetTick()-Ti)>5300 && (HAL_GetTick()-Ti)<5800)
		{
			AIR_PUSH_ON;												//ǰ��
		}
		else if((HAL_GetTick()-Ti)>5800 && (HAL_GetTick()-Ti)<6300)
		{
			AIR_PUSH_OFF;
		}
		else if((HAL_GetTick()-Ti)>6300 && (HAL_GetTick()-Ti)<6800)
		{
			AIR_GET_ON;
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0], spin_moto_start_angle[0]-79500, 0);	//��ת
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1], spin_moto_start_angle[1]+79500, 0);
		}
		else if((HAL_GetTick()-Ti)>6800 && (HAL_GetTick()-Ti)<7300)
		{
			AIR_PUSH_ON;
		}
		else if((HAL_GetTick()-Ti)>7300 && (HAL_GetTick()-Ti)<7800)
		{
			AIR_PUSH_OFF;
		}
		else if((HAL_GetTick()-Ti)>7800 && (HAL_GetTick()-Ti)<8300)
		{
			AIR_GET_OFF;
			limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_start_angle[0], 0);
			limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_start_angle[1], 0);
			keyboard.TogetherExchange_two = 0;					
			time_mark = 0;												//ʱ��λ����
		}
	}
	
	
	if(remote_control.switch_left == 2 || remote_control.switch_left == 1)									//�ƿ�����
	{
		for(int i=0; i<2; i++)
		{
			spin_moto_pid[i].pid_out = 0;
		}
	}
	else
	{
		limit_moto(&hcan2, &spin_moto[0] , &spin_moto_angle_pid[0], &spin_moto_pid[0],  spin_moto_angle_pid[0].target, 0);		//ȷ�������ж�����ʱ�������Ŀ��λ��
		limit_moto(&hcan2, &spin_moto[1] , &spin_moto_angle_pid[1], &spin_moto_pid[1],  spin_moto_angle_pid[1].target, 0);
	}
}












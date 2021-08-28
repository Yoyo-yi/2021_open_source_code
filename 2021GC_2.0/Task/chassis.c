#include "chassis.h"
#include "pid.h"
#include "Remote_Control.h"
#include "bsp_can.h"
#include "user_lib.h"
#include "math.h"


int16_t Max_speed=0;

/*��������***************************/
float set[4] = {0};


/*�ṹ��********************************/
pid_typedef chassis_moto_pid[4];                           //�����ٶȻ�PID�ṹ��
pid_typedef chassis_angle_pid[4];                          //����λ�û�PID�ṹ��
key_chassis chassis_move;                                  //���̼���ģʽ�ٶȷֽ�ṹ��
ramp_function_source_t  chassis_slope[4];				   //б�º���
first_order_filter_type_t first_chassis_mouse;             //һ�׵�ͨ�˲������ü�������£����ݸ���ƽ��


extern moto_measure chassis_moto[4];


void chassis_init()                         //���̳�ʼ��
{
	for(int i=0; i<4; i++)
	{
		pid_init(&chassis_moto_pid[i]);
		chassis_moto_pid[i].f_param_init(&chassis_moto_pid[i], 0,    8000,   16384,    5000,   5,      0, 7.3, 0.85, 0.01);
		                                                //Ŀ��ֵ,�����޷�,������,������,����,pid_out,   p,    i,    d
		ramp_init(&chassis_slope[i], 0.01, 660, -660);
	}
}

void chassis_max_speed()			//�����Ƶ��̼Ӽ���
{
	if(remote_control.mouse.press_left == 1 && remote_control.mouse.press_right == 0)
	{
		Max_speed = 5000;
	}
	else if(remote_control.mouse.press_left == 0 && remote_control.mouse.press_right == 1)
	{
		Max_speed = 1000;
	}
	else
	{
		Max_speed = 3000;
	}
}


void chassis_caculate(int16_t x, int16_t y, int16_t w)      //�����ٶȽ���
{
	chassis_max_speed();
	set[0] = (y-x+w)*Max_speed/660;
	set[1] = (y+x+w)*Max_speed/660;
	set[2] = (-y+x+w)*Max_speed/660;
	set[3] = (-y-x+w)*Max_speed/660;
}

void chassis_key()                          //���̼���ƽ�ƿ���
{
	if(remote_control.keyBoard.key_code & KEY_W)
	{
		chassis_move.Vw = 660;
	}
	else
	{
		chassis_move.Vw = 0;
	}
	if(remote_control.keyBoard.key_code & KEY_S)
	{
		chassis_move.Vs = -660;
	}
	else
	{
		chassis_move.Vs = 0; 
	}
	if(remote_control.keyBoard.key_code & KEY_D)
	{
		chassis_move.Vd = 660;
	}
	else
	{
		chassis_move.Vd = 0; 
	}
	if(remote_control.keyBoard.key_code & KEY_A)
	{
		chassis_move.Va = -660;
	}
	else
	{
		chassis_move.Va = 0;
	}
	chassis_move.set_x = chassis_move.Va + chassis_move.Vd;
	chassis_move.set_y = chassis_move.Vw + chassis_move.Vs;
	
	if((remote_control.keyBoard.key_code & KEY_W) == 0 &&(remote_control.keyBoard.key_code & KEY_S) == 0)   //û�а���ʱ����
	{
		chassis_move.set_y = 0;
	}
	if((remote_control.keyBoard.key_code & KEY_A) == 0&&(remote_control.keyBoard.key_code & KEY_D) == 0)
	{
		chassis_move.set_x = 0;
	}
	
	ramp_calc(&chassis_slope[0], chassis_move.set_x);      //б�����ݴ����õ��̿������ݸ���ƽ��
	ramp_calc(&chassis_slope[1], chassis_move.set_y);
	
	first_order_filter_cali(&first_chassis_mouse, remote_control.mouse.x);      //һ�׵�ͨ�˲�������귴��ֵ
}


void chassis_task()
{
	chassis_key();
	if(remote_control.switch_left == 2)         //���˿��Ƶ�������ģʽ�����ڵ���
	{
		for(int i=0; i<4; i++)
		{
			chassis_moto_pid[i].pid_out = 0;
			
		}
	}
	else
	{
		for(int i=0; i<4; i++)
		{
            //���̽��㼰PID�㷨�ջ�����
			chassis_caculate(remote_control.ch3 + chassis_slope[0].out, remote_control.ch4 + chassis_slope[1].out, remote_control.ch1 + first_chassis_mouse.out * 10);
			chassis_moto_pid[i].f_calculate(&chassis_moto_pid[i], chassis_moto[i].speed, set[i]);
		}
	}
	
    //�����������ֵ
	SET_CAN1Ahead_MOTOR_CURRENT(&hcan1, lift_moto_pid[0].pid_out, lift_moto_pid[1].pid_out, chassis_moto_pid[0].pid_out, chassis_moto_pid[1].pid_out);
	SET_CAN1Back_MOTOR_CURRENT(&hcan1, chassis_moto_pid[2].pid_out, chassis_moto_pid[3].pid_out , 0, 0);

}





















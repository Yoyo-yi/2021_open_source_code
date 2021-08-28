#include "pid.h"
#include "main.h"
#include "math.h"

void limit(float *a, float max_value)    //�޷�����
{
	if(*a > max_value)
		*a = max_value;
	if(*a < -max_value)
		*a = -max_value;
}

float pid_calculate(pid_typedef *pid, float get_speed, float set_speed)     //PID����->�������
{
	float index;
	uint8_t deadband = 5;
	pid->get[Now] = get_speed;                //ʵ��ֵ
	pid->set[Now] = set_speed;                //Ŀ��ֵ
	pid->err[Now] = set_speed - get_speed;    //���
	
	if(fabs(pid->err[Now]) > pid->Max_err && pid->Max_err != 0)
		return 0;
	
	
	if(fabs(pid->err[Now]) > deadband)             //�����ж�
		{
//			if(fabs(pid->err[Now])>50)
//			{
				pid->pout = pid->kp * pid->err[Now];     //P
//			}
			
			if(fabs(pid->err[Now])>1000)               //���ַ����׼
			{
				index = 0;
			}else{
				index = 1;
			}
			pid->iout += (pid->ki * pid->err[Now]*index);       //I
			limit(&(pid->iout), pid->I_limit);                  //I�޷�		
			
			pid->dout = pid->kd * (pid->err[Now] - pid->err[Last]);        //D			
			pid->pid_out = pid->pout + pid->iout + pid->dout;              //PID���

			limit(&(pid->pid_out), pid->Max_output);                       //����޷�
			pid->err[Last] = pid->err[Now];
			return pid->pid_out;
		}
		else
		{
			pid->err[Last] = pid->err[Now];
			return 0;
		}
}

void pid_param_init(pid_typedef *pid, float target, float I_limit, uint32_t Max_output, uint32_t Max_err, int16_t deadband, float pid_out,
										 float kp, float ki, float kd)          //Ŀ��ֵ,�����޷�,������,������,����,pid_out,p,i,d
{
	pid->target = target;
	pid->I_limit = I_limit;
	pid->Max_output = Max_output;
	pid->Max_err = Max_err;
	pid->deadband = deadband;
	pid->pid_out = pid_out;
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
					
}


void pid_init(pid_typedef *pid)                            //�ṹ���ʼ��������������ṹ�壩
{
	pid->f_param_init = pid_param_init;
	pid->f_calculate = pid_calculate;
	pid->f_calculate_position = pid_calculate;
}

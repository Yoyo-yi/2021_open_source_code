#ifndef _PID_H
#define _PID_H

#include "main.h"

typedef enum time
{
	Last,Now
}time;

typedef struct _pid_typedef        //PID�ṹ��
{
	
	float kp;               //����
	float ki;               //����
	float kd;               //΢��
	float target;           //Ŀ��ֵ
	
	float set[3];           //���õ�ֵ
	float get[3];           //�õ���ֵ
	float err[3];           //�õ������
	
	float pout;             //�������
	float iout;             //�������
	float dout;             //΢�����
	float pid_out;          //pid�����
	
	float I_limit;          //�����޷�
	uint32_t Max_output;    //����������
	uint32_t Max_err;       //������
	int16_t deadband;
	
	void (*f_param_init)(struct _pid_typedef *pid, float target, float I_limit, uint32_t Max_output, uint32_t Max_err, int16_t deadband, float pid_out,
	                        float kp, float ki, float kd);       //Ŀ��ֵ,�����޷�,������,������,����,pid_out,p,i,d
	
	float (*f_calculate)(struct _pid_typedef *pid, float get_speed, float set_speed);           //PID����
	float (*f_calculate_position)(struct _pid_typedef *pid, float get_speed, float set_speed);
	
	struct motor           //����ֵ
	{
		
		uint16_t angle;        //ת�ӽǶ�
		int16_t speed;         //�ٶ�
		int16_t real_current;  //ʵ�ʵ���
		int8_t temperature;    //�¶�
		
	}motor;
	
	
}pid_typedef;

extern pid_typedef chassis_moto_pid[4];
extern pid_typedef tp_moto_pid[2];
extern pid_typedef lift_moto_pid[2];
extern pid_typedef spin_moto_pid[2];
extern pid_typedef help_moto_pid;
extern pid_typedef block_moto_pid;
/*��������*****************************/

float pid_calculate(pid_typedef *pid, float get_speed, float set_speed);
void pid_init(pid_typedef *pid);
void pid_param_init(pid_typedef *pid, float I_limit, float target, uint32_t Max_output, uint32_t Max_err, int16_t deadband, float pid_out,
	                   float kp, float ki, float kd);

#endif

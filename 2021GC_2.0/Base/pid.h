#ifndef _PID_H
#define _PID_H

#include "main.h"

typedef enum time
{
	Last,Now
}time;

typedef struct _pid_typedef        //PID结构体
{
	
	float kp;               //比例
	float ki;               //积分
	float kd;               //微分
	float target;           //目标值
	
	float set[3];           //设置的值
	float get[3];           //得到的值
	float err[3];           //得到的误差
	
	float pout;             //比例输出
	float iout;             //积分输出
	float dout;             //微分输出
	float pid_out;          //pid总输出
	
	float I_limit;          //积分限幅
	uint32_t Max_output;    //最大输出限制
	uint32_t Max_err;       //最大误差
	int16_t deadband;
	
	void (*f_param_init)(struct _pid_typedef *pid, float target, float I_limit, uint32_t Max_output, uint32_t Max_err, int16_t deadband, float pid_out,
	                        float kp, float ki, float kd);       //目标值,积分限幅,最大输出,最大误差,死区,pid_out,p,i,d
	
	float (*f_calculate)(struct _pid_typedef *pid, float get_speed, float set_speed);           //PID计算
	float (*f_calculate_position)(struct _pid_typedef *pid, float get_speed, float set_speed);
	
	struct motor           //反馈值
	{
		
		uint16_t angle;        //转子角度
		int16_t speed;         //速度
		int16_t real_current;  //实际电流
		int8_t temperature;    //温度
		
	}motor;
	
	
}pid_typedef;

extern pid_typedef chassis_moto_pid[4];
extern pid_typedef tp_moto_pid[2];
extern pid_typedef lift_moto_pid[2];
extern pid_typedef spin_moto_pid[2];
extern pid_typedef help_moto_pid;
extern pid_typedef block_moto_pid;
/*函数声明*****************************/

float pid_calculate(pid_typedef *pid, float get_speed, float set_speed);
void pid_init(pid_typedef *pid);
void pid_param_init(pid_typedef *pid, float I_limit, float target, uint32_t Max_output, uint32_t Max_err, int16_t deadband, float pid_out,
	                   float kp, float ki, float kd);

#endif

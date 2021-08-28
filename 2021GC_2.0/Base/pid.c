#include "pid.h"
#include "main.h"
#include "math.h"

void limit(float *a, float max_value)    //限幅函数
{
	if(*a > max_value)
		*a = max_value;
	if(*a < -max_value)
		*a = -max_value;
}

float pid_calculate(pid_typedef *pid, float get_speed, float set_speed)     //PID计算->输出函数
{
	float index;
	uint8_t deadband = 5;
	pid->get[Now] = get_speed;                //实际值
	pid->set[Now] = set_speed;                //目标值
	pid->err[Now] = set_speed - get_speed;    //误差
	
	if(fabs(pid->err[Now]) > pid->Max_err && pid->Max_err != 0)
		return 0;
	
	
	if(fabs(pid->err[Now]) > deadband)             //死区判断
		{
//			if(fabs(pid->err[Now])>50)
//			{
				pid->pout = pid->kp * pid->err[Now];     //P
//			}
			
			if(fabs(pid->err[Now])>1000)               //积分分离标准
			{
				index = 0;
			}else{
				index = 1;
			}
			pid->iout += (pid->ki * pid->err[Now]*index);       //I
			limit(&(pid->iout), pid->I_limit);                  //I限幅		
			
			pid->dout = pid->kd * (pid->err[Now] - pid->err[Last]);        //D			
			pid->pid_out = pid->pout + pid->iout + pid->dout;              //PID输出

			limit(&(pid->pid_out), pid->Max_output);                       //输出限幅
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
										 float kp, float ki, float kd)          //目标值,积分限幅,最大输出,最大误差,死区,pid_out,p,i,d
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


void pid_init(pid_typedef *pid)                            //结构体初始化（将函数放入结构体）
{
	pid->f_param_init = pid_param_init;
	pid->f_calculate = pid_calculate;
	pid->f_calculate_position = pid_calculate;
}

#include "user_lib.h"
#include "usart.h"
#include "pid.h"
#include "bsp_can.h"

/**
  * @brief          一阶低通滤波初始化
  * @author         RM
  * @param[in]      一阶低通滤波结构体
  * @param[in]      间隔的时间，单位 s
  * @param[in]      滤波参数
  * @retval         返回空
  */
void first_order_filter_init(first_order_filter_type_t *first_order_filter_type, float frame_period, const float num)
{
    first_order_filter_type->frame_period = frame_period;
    first_order_filter_type->num = num;
    first_order_filter_type->input = 0.0f;
    first_order_filter_type->out = 0.0f;
}

/**
  * @brief          一阶低通滤波计算
  * @author         RM
  * @param[in]      一阶低通滤波结构体
  * @param[in]      间隔的时间，单位 s
  * @retval         返回空
  */
void first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, float input)
{
	first_order_filter_type->last_out = first_order_filter_type->out;
    first_order_filter_type->input = input;
    first_order_filter_type->out =
    first_order_filter_type->num / (first_order_filter_type->num + first_order_filter_type->frame_period) * first_order_filter_type->out + 
	first_order_filter_type->frame_period / (first_order_filter_type->num + first_order_filter_type->frame_period) * first_order_filter_type->input;
//					first_order_filter_type->input * first_order_filter_type->num[0]+(1-first_order_filter_type->num[0])*first_order_filter_type->last_out;
}



/**
  * @brief          按键重复调用
  * @param          键盘按键
  * @param          客户端按键反馈数据       
  */
static int32_t cnt = 0;
static int16_t i   = 0;
int key_count(uint16_t key, uint16_t key_target)
{
	if(key == 0)
	{
		i = 1;
	}
	if(key == key_target && i == 1)
	{
		cnt++;
		i = 0;
	}
	
	if(cnt % 2 == 1)
		return 1;
	else
		return 0;
}

//==============电机限位函数==============//

void limit_moto(CAN_HandleTypeDef *hcan ,moto_measure *mea ,pid_typedef *angle, pid_typedef *speed, int32_t set, int32_t angle_offset)
{
		angle->target = set - angle_offset;
		angle->f_calculate_position(angle, mea->total_angle, angle->target);
		speed->f_calculate(speed, mea->speed, (angle->pid_out)/5);
	
		if(hcan->Instance == CAN1)              //判断数据传输入口
		{
			SET_CAN1Ahead_MOTOR_CURRENT(&hcan1, lift_moto_pid[0].pid_out, lift_moto_pid[1].pid_out, chassis_moto_pid[0].pid_out, chassis_moto_pid[1].pid_out);
			SET_CAN1Back_MOTOR_CURRENT(&hcan1, chassis_moto_pid[2].pid_out, chassis_moto_pid[3].pid_out , 0, 0);
		}
		else if(hcan->Instance == CAN2)
		{
			SET_CAN2Ahead_MOTOR_CURRENT(&hcan2, tp_moto_pid[0].pid_out, tp_moto_pid[1].pid_out, spin_moto_pid[0].pid_out, spin_moto_pid[1].pid_out);
			SET_CAN2Back_MOTOR_CURRENT(&hcan2, help_moto_pid.pid_out, block_moto_pid.pid_out, 0, 0);
		}
}
/*******************************************************************************************	
  * @Brief  			延时函数
  * @Param				延时时间
  * @Retval
  * @Date
 *******************************************************************************************/
void my_delay(int delay_time)
{
	while(i--)
	{
		;
	}
}




/**
  * @brief    			斜坡函数初始化
  * @param    			斜坡函数结构体
  * @param    			间隔时间
  * @param    			斜坡目标值
  * @param    			斜坡源
  */
void ramp_init(ramp_function_source_t *ramp_source_type, float frame_period, float max, float min)
{
    ramp_source_type->frame_period = frame_period;
    ramp_source_type->max_value = max;
    ramp_source_type->min_value = min;
    ramp_source_type->input = 0.0f;
    ramp_source_type->out = 0.0f;
}

void ramp_calc(ramp_function_source_t *chassis_ramp, float input)
{
    chassis_ramp->input = input;
	if(input != 0)
	{
    chassis_ramp->out += chassis_ramp->input * chassis_ramp->frame_period;
    if (chassis_ramp->out > chassis_ramp->max_value)
        chassis_ramp->out = chassis_ramp->max_value;
    else if (chassis_ramp->out < chassis_ramp->min_value)
        chassis_ramp->out = chassis_ramp->min_value;
	}
	else
	{
		if(chassis_ramp->out > 5)
		  chassis_ramp->out -= 10.0f;
		else if(chassis_ramp->out <-5)
			chassis_ramp->out += 10.0f;
		else
			chassis_ramp->out = 0;
	}
}



/**
	* @brief          上位机发送函数
	* @param          自定义发送数据1
	* @param          自定义发送数据2       
	*									......
  */
uint8_t data_to_send[17];
void Niming_send(uint16_t data1, int32_t data2, uint16_t data3, int32_t data4)
{
	data_to_send[0]=0xAA;
	data_to_send[1]=0xAA;
	data_to_send[2]=0xF1;
	data_to_send[3]=12;
	data_to_send[4]=(data1>>8)|0x00;
	data_to_send[5]=data1|0x00;
	data_to_send[6]=(data2>>24)|0x00;
	data_to_send[7]=(data2>>16)|0x00;
	data_to_send[8]=(data2>>8)|0x00;
	data_to_send[9]=data2|0x00;
	data_to_send[10]=(data3>>8)|0x00;
	data_to_send[11]=data3|0x00;
	data_to_send[12]=(data4>>24)|0x00;
	data_to_send[13]=(data4>>16)|0x00;
	data_to_send[14]=(data4>>8)|0x00;
	data_to_send[15]=data4|0x00;

	
	
	uint8_t check_sum = 0;
	for(uint8_t i=0;i<18;i++)
	{
		check_sum += data_to_send[i];
	}
	data_to_send[16]=check_sum;
	HAL_UART_Transmit(&huart2,data_to_send,sizeof(data_to_send),999);
}



#ifndef USER_LIB_H
#define USER_LIB_H
#include "main.h"


typedef __packed struct
{
    float input;        //输入数据
    float out;          //滤波输出的数据
    float num;       //滤波参数
    float frame_period; //滤波的时间间隔 单位 s
		float last_out;
} first_order_filter_type_t;

typedef __packed struct
{
    float input;        //输入数据
    float out;          //输出数据
    float min_value;    //限幅最小值
    float max_value;    //限幅最大值
    float frame_period; //时间间隔
} ramp_function_source_t;

void first_order_filter_init(first_order_filter_type_t *first_order_filter_type, float frame_period, const float num);

void first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, float input);

void ramp_init(ramp_function_source_t *ramp_source_type, float frame_period, float max, float min);

void ramp_calc(ramp_function_source_t *chassis_ramp, float input);

int key_count(uint16_t key, uint16_t key_target);

void Niming_send(uint16_t data1, int32_t data2, uint16_t data3, int32_t data4);

void my_delay(int delay_time);

#endif

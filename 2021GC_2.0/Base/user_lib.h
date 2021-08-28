#ifndef USER_LIB_H
#define USER_LIB_H
#include "main.h"


typedef __packed struct
{
    float input;        //��������
    float out;          //�˲����������
    float num;       //�˲�����
    float frame_period; //�˲���ʱ���� ��λ s
		float last_out;
} first_order_filter_type_t;

typedef __packed struct
{
    float input;        //��������
    float out;          //�������
    float min_value;    //�޷���Сֵ
    float max_value;    //�޷����ֵ
    float frame_period; //ʱ����
} ramp_function_source_t;

void first_order_filter_init(first_order_filter_type_t *first_order_filter_type, float frame_period, const float num);

void first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, float input);

void ramp_init(ramp_function_source_t *ramp_source_type, float frame_period, float max, float min);

void ramp_calc(ramp_function_source_t *chassis_ramp, float input);

int key_count(uint16_t key, uint16_t key_target);

void Niming_send(uint16_t data1, int32_t data2, uint16_t data3, int32_t data4);

void my_delay(int delay_time);

#endif

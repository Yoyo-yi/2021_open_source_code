#ifndef _KEY_H
#define _KEY_H

#include "stdint.h"

struct key_mouse              //������־�ṹ��
{
	int8_t moto_init;           //�����ʼ��
	int8_t help_air;            //��Ԯ����
	int8_t help_card;           //��Ԯ��
	int8_t up_altitude;         //̧���߶�
	int8_t get_resource;        //�п�ʯ
	int8_t get_ground_resource; //�е����ʯ
	int8_t get_third_resource;  //��ȡ�������ʯ
	int8_t exchange;            //�һ�
	int8_t exchange_third_resource; //�һ��������ʯ
	int8_t push_resource;       //�ƿ�
	int8_t left_supply;         //�󵯲�
	int8_t right_supply;        //�ҵ���
	int8_t block;               //�����ϸ˼�ȡ����
	int8_t block_up;            //�����ϸ˼�ȡ����̧��
	int8_t view_change;			//ͼ���ӽǱ仯���Կ�����
	int8_t TP;                  //��ʯ����
	
	
	int8_t TogetherExchange_three;  //һ���������ʯ
	int8_t TogetherExchange_two;    //һ���������ʯ
};

void key_init(void);
void key_task(void);


#endif

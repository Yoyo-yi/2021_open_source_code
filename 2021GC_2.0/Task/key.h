#ifndef _KEY_H
#define _KEY_H

#include "stdint.h"

struct key_mouse              //按键标志结构体
{
	int8_t moto_init;           //电机初始化
	int8_t help_air;            //救援气缸
	int8_t help_card;           //救援卡
	int8_t up_altitude;         //抬升高度
	int8_t get_resource;        //夹矿石
	int8_t get_ground_resource; //夹地面矿石
	int8_t get_third_resource;  //夹取第三块矿石
	int8_t exchange;            //兑换
	int8_t exchange_third_resource; //兑换第三块矿石
	int8_t push_resource;       //推矿
	int8_t left_supply;         //左弹舱
	int8_t right_supply;        //右弹舱
	int8_t block;               //蜗轮蜗杆夹取控制
	int8_t block_up;            //蜗轮蜗杆夹取机构抬升
	int8_t view_change;			//图传视角变化（对抗赛）
	int8_t TP;                  //矿石传送
	
	
	int8_t TogetherExchange_three;  //一键兑三块矿石
	int8_t TogetherExchange_two;    //一键兑两块矿石
};

void key_init(void);
void key_task(void);


#endif

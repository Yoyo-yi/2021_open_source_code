#include "centre.h"
#include "bsp_can.h"
#include "usart.h"
#include "bsp_usart.h"
#include "user_lib.h"
#include "chassis.h"
#include "get.h"
#include "other_action.h"
#include "key.h"

/*变量定义******************************/
uint8_t UART_Buffer[100];                                           //遥控接收数组
extern uint8_t data_to_send[17];      																		//上位机数据发送


static float chassis_mouse_filter = 1.6666666667f;                  //一阶低通滤波参数——越大越稳，但响应速度降低

extern first_order_filter_type_t first_chassis_mouse;
extern ramp_function_source_t  chassis_slope[4];

/*函数声明***********************************/


/*基层初始化****************************************/
void base_init()
{
	CAN1_FILTER_CONFIG(&hcan1);                                                     //CAN过滤器配置
	CAN2_FILTER_CONFIG(&hcan2);
	
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);              //CAN中断使能
	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
	
	HAL_UART_Receive_IT_IDLE(&huart1, UART_Buffer, 100);                            //串口空闲中断使能
	
	first_order_filter_init(&first_chassis_mouse, 0.02, chassis_mouse_filter);       //一阶低通滤波初始化
	
	HAL_UART_Transmit(&huart2,data_to_send,sizeof(data_to_send),999);
	
}

void task_init()            //所有任务初始化集成
{
	chassis_init();
	get_task_init();
	key_init();
	Other_task_init();
}



void chassis_all_task()     //底盘任务
{
	chassis_task();
}

void Get_all_task()         //抓取任务
{
	Lift_task();
	tp_task();
	get_task();
}

void key_all_task()         //键鼠任务
{
	key_task();
}

void Other_all_task()       //其余辅助功能任务
{
	Other_action();
}


#include "centre.h"
#include "bsp_can.h"
#include "usart.h"
#include "bsp_usart.h"
#include "user_lib.h"
#include "chassis.h"
#include "get.h"
#include "other_action.h"
#include "key.h"

/*��������******************************/
uint8_t UART_Buffer[100];                                           //ң�ؽ�������
extern uint8_t data_to_send[17];      																		//��λ�����ݷ���


static float chassis_mouse_filter = 1.6666666667f;                  //һ�׵�ͨ�˲���������Խ��Խ�ȣ�����Ӧ�ٶȽ���

extern first_order_filter_type_t first_chassis_mouse;
extern ramp_function_source_t  chassis_slope[4];

/*��������***********************************/


/*�����ʼ��****************************************/
void base_init()
{
	CAN1_FILTER_CONFIG(&hcan1);                                                     //CAN����������
	CAN2_FILTER_CONFIG(&hcan2);
	
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);              //CAN�ж�ʹ��
	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
	
	HAL_UART_Receive_IT_IDLE(&huart1, UART_Buffer, 100);                            //���ڿ����ж�ʹ��
	
	first_order_filter_init(&first_chassis_mouse, 0.02, chassis_mouse_filter);       //һ�׵�ͨ�˲���ʼ��
	
	HAL_UART_Transmit(&huart2,data_to_send,sizeof(data_to_send),999);
	
}

void task_init()            //���������ʼ������
{
	chassis_init();
	get_task_init();
	key_init();
	Other_task_init();
}



void chassis_all_task()     //��������
{
	chassis_task();
}

void Get_all_task()         //ץȡ����
{
	Lift_task();
	tp_task();
	get_task();
}

void key_all_task()         //��������
{
	key_task();
}

void Other_all_task()       //���ศ����������
{
	Other_action();
}


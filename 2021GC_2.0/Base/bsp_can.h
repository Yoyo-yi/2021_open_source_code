#include "main.h"
#include "can.h"

/*���ID�ṹ��*/
typedef enum
{
	CAN_CHASSIS_RX_ID = 0x200,   //2006��3508������ձ�ʶ��
	CAN_3508M1_ID = 0x201,
	CAN_3508M2_ID = 0x202,
	CAN_3508M3_ID = 0x203,
	CAN_3508M4_ID = 0x204,
	CAN_3508M5_ID = 0x205,
	CAN_3508M6_ID = 0x206,
	CAN_3508M7_ID = 0x207,
	CAN_3508M8_ID = 0x208,
	
	
	CAN_6020_RX_ID = 0x1FF,     //6020������ձ�ʶ��
	CAN_6020M1_ID = 0x205,
	CAN_6020M2_ID = 0x206,
	CAN_6020M3_ID = 0x207,
	CAN_6020M4_ID = 0x208,
	
}CAN_MSG_ID;

/*CAN����ֵ�ṹ��*/
typedef struct
{
	uint16_t angle;            //ת�ӽǶ� (0~8191)
	int16_t  speed;            //ת���ٶ�
	int16_t  last_speed;
	int16_t  real_current;     //�������
	int16_t  temperature;      //����¶� 
	uint16_t last_angle;
	uint16_t offset_angle;     //�����Ƕ�
	int32_t  round_cnt;        //ת��ת��Ȧ��
	int32_t  total_angle;      //ת��ת���ܽǶ�
	int32_t  last_total_angle;
	int32_t  angle_err;
	
}moto_measure;


//��������
void CAN1_FILTER_CONFIG(CAN_HandleTypeDef* hcan);                                                    //CAN�˲�������
void CAN2_FILTER_CONFIG(CAN_HandleTypeDef* hcan);
void SET_CAN1Ahead_MOTOR_CURRENT(CAN_HandleTypeDef* hcan, int16_t K1, int16_t K2, int16_t K3, int16_t K4);    //�����ֵ
void SET_CAN1Back_MOTOR_CURRENT(CAN_HandleTypeDef* hcan, int16_t K1, int16_t K2, int16_t K3, int16_t K4);
void SET_CAN2Ahead_MOTOR_CURRENT(CAN_HandleTypeDef* hcan, int16_t K1, int16_t K2, int16_t K3, int16_t K4);
void SET_CAN2Back_MOTOR_CURRENT(CAN_HandleTypeDef* hcan, int16_t K1, int16_t K2, int16_t K3, int16_t K4);
void get_moto_measure(moto_measure *ptr, CAN_HandleTypeDef* hcan, uint8_t RXdata[8]);               //�������ֵ


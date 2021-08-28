#include "bsp_can.h"


CAN_RxHeaderTypeDef RX1message;
CAN_TxHeaderTypeDef TX1message;
uint8_t TXdata[8];
uint8_t RXdata[8];
uint32_t FlashTimer; 

moto_measure chassis_moto[4] = {0};   //���̵��
moto_measure lift_moto[2]    = {0};   //̧�����
moto_measure tp_moto[2]      = {0};   //��ʯ���͵��
moto_measure spin_moto[2]    = {0};   //��ȡ��ת���
moto_measure help_moto       = {0};   //��Ԯ����� 
moto_measure block_moto      = {0};   //�ϰ���ӽ����



/**************************************************************/
/**
  * @brief canͨ�ŵõ����������
  * @param  
	* @param can1 or can2
	* @param ���������ֵ������
  * @retval None
  */
/***************************************************************/

void get_moto_measure(moto_measure *ptr, CAN_HandleTypeDef* hcan, uint8_t RXdata[8])
{
	ptr->last_angle = ptr->angle;
	ptr->last_speed = ptr->speed;
	ptr->last_total_angle = ptr->total_angle;
	
	ptr->angle = (RXdata[0] << 8)|RXdata[1];         //�������
	ptr->speed = (RXdata[2] << 8)|RXdata[3];
	ptr->real_current = (RXdata[4] << 8)|RXdata[5];
	ptr->temperature = RXdata[6];
	
	if(ptr->angle - ptr->last_angle > 4096)
		ptr->round_cnt --;
	else if (ptr->angle - ptr->last_angle < -4096)
		ptr->round_cnt ++;
	ptr->total_angle = ptr->round_cnt * 8192 + ptr->angle - ptr->offset_angle;
	ptr->angle_err = ptr->last_total_angle - ptr->total_angle;
}	

/**************************************************************/
/**
  * @brief can�жϻص�����
  * @param can1 or can2
  * @retval None
  */
/***************************************************************/


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

	if(HAL_GetTick() - FlashTimer>500)//canͨ��������˸��ʾ��
  {	
		HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_14);
		FlashTimer = HAL_GetTick();
  }
	
	if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RX1message, RXdata)==HAL_OK)
	{
		if(hcan->Instance == CAN1)      //ID����
		{
			switch(RX1message.StdId)
			{
				case CAN_3508M1_ID: 
				case CAN_3508M2_ID:   
				{
					static int i;
					i = RX1message.StdId - CAN_3508M1_ID;
					get_moto_measure(&lift_moto[i], &hcan1, RXdata);
					break;
				}
				
				case CAN_3508M3_ID: 
				case CAN_3508M4_ID:
				case CAN_3508M5_ID:           
				case CAN_3508M6_ID:
				{
					static int j;
					j = RX1message.StdId - CAN_3508M3_ID;
					get_moto_measure(&chassis_moto[j], &hcan1, RXdata);
					break;
				}
			}
		}
		
	}
	
	if(HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &RX1message, RXdata)==HAL_OK)
	{
		if(hcan->Instance == CAN2)       
		{
			switch(RX1message.StdId)
			{         
				case CAN_3508M1_ID:
				case CAN_3508M2_ID:
				{
					static int z;
					z = RX1message.StdId - CAN_3508M1_ID;
					get_moto_measure(&tp_moto[z], &hcan2, RXdata);
					break;
				}

				case CAN_3508M3_ID:	
				case CAN_3508M4_ID:
				{
					static int x;
					x = RX1message.StdId - CAN_3508M3_ID;
					get_moto_measure(&spin_moto[x], &hcan2, RXdata);
				}
				break;

				case CAN_3508M5_ID: 
					get_moto_measure(&help_moto, &hcan2, RXdata);
				break;
				
				case CAN_3508M6_ID:
					get_moto_measure(&block_moto, &hcan2, RXdata);
				break;
			}
		}
	}
}

/**************************************************************/
/**
  * @brief ���ù�����
  * @param CAN1 or CAN2
  * @retval None
  */
/***************************************************************/

void CAN1_FILTER_CONFIG(CAN_HandleTypeDef* hcan)
{
	CAN_FilterTypeDef		CAN_FilterConfigStructure;		
	CAN_FilterConfigStructure.FilterBank = 0;
	CAN_FilterConfigStructure.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN_FilterConfigStructure.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_FilterConfigStructure.FilterIdHigh = 0x7FFE;
	CAN_FilterConfigStructure.FilterIdLow = 0x0000;
	CAN_FilterConfigStructure.FilterMaskIdHigh = 0x0000;
	CAN_FilterConfigStructure.FilterMaskIdLow = 0x0000;
	CAN_FilterConfigStructure.FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterConfigStructure.SlaveStartFilterBank = 14;
	CAN_FilterConfigStructure.FilterActivation = ENABLE;
		
		HAL_CAN_ConfigFilter(&hcan1,&CAN_FilterConfigStructure);
		HAL_CAN_Start(&hcan1);
		HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void CAN2_FILTER_CONFIG(CAN_HandleTypeDef* hcan)
{
	CAN_FilterTypeDef		CAN_FilterConfigStructure;		
	CAN_FilterConfigStructure.FilterBank = 14;
	CAN_FilterConfigStructure.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN_FilterConfigStructure.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_FilterConfigStructure.FilterIdHigh = 0xFFFF;
	CAN_FilterConfigStructure.FilterIdLow = 0x7FFF;
	CAN_FilterConfigStructure.FilterMaskIdHigh = 0x0000;
	CAN_FilterConfigStructure.FilterMaskIdLow = 0x0000;
	CAN_FilterConfigStructure.FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterConfigStructure.SlaveStartFilterBank = 14;
	CAN_FilterConfigStructure.FilterActivation = ENABLE;
		
		HAL_CAN_ConfigFilter(&hcan2,&CAN_FilterConfigStructure);
		HAL_CAN_Start(&hcan2);
		HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
}

/**************************************************************/
/**
  * @brief ��������������
  * @param CAN1 or CAN2

  * @retval None
  */
/***************************************************************/
void SET_CAN1Ahead_MOTOR_CURRENT(CAN_HandleTypeDef* hcan, int16_t K1, int16_t K2, int16_t K3, int16_t K4)    //CAN1 ID:1~4
{	
	TX1message.StdId = 0x200;                        //�����ʶ��
	TX1message.IDE = CAN_ID_STD;
	TX1message.RTR = CAN_RTR_DATA;
	TX1message.DLC = 8;                              //���ͳ��� (x�ֽ�)
	TX1message.TransmitGlobalTime = DISABLE;
	
	TXdata[0] = K1 >> 8;
	TXdata[1] = K1;
	TXdata[2] = K2 >> 8;
	TXdata[3] = K2;
	TXdata[4] = K3 >> 8;
	TXdata[5] = K3;
	TXdata[6] = K4 >> 8;
	TXdata[7] = K4;
	HAL_CAN_AddTxMessage(&hcan1, &TX1message, TXdata, CAN_FILTER_FIFO0);    //�����ݴ��������FIFOx
}


void SET_CAN1Back_MOTOR_CURRENT(CAN_HandleTypeDef* hcan, int16_t K1, int16_t K2, int16_t K3, int16_t K4)    //CAN1	ID:5~8
{	
	TX1message.StdId = 0x1FF;
	TX1message.IDE = CAN_ID_STD;
	TX1message.RTR = CAN_RTR_DATA;
	TX1message.DLC = 8;
	TX1message.TransmitGlobalTime = DISABLE;
	
	TXdata[0] = K1 >> 8;
	TXdata[1] = K1;
	TXdata[2] = K2 >> 8;
	TXdata[3] = K2;
	TXdata[4] = K3 >> 8;
	TXdata[5] = K3;
	TXdata[6] = K4 >> 8;
	TXdata[7] = K4;
	HAL_CAN_AddTxMessage(&hcan1, &TX1message, TXdata, CAN_FILTER_FIFO0);
}


void SET_CAN2Ahead_MOTOR_CURRENT(CAN_HandleTypeDef* hcan, int16_t K1, int16_t K2, int16_t K3, int16_t K4)    //CAN2 ID:1~4
{	
	TX1message.StdId = 0x200;
	TX1message.IDE = CAN_ID_STD;
	TX1message.RTR = CAN_RTR_DATA;
	TX1message.DLC = 8;
	TX1message.TransmitGlobalTime = DISABLE;
	
	TXdata[0] = K1 >> 8;
	TXdata[1] = K1;
	TXdata[2] = K2 >> 8;
	TXdata[3] = K2;
	TXdata[4] = K3 >> 8;
	TXdata[5] = K3;
	TXdata[6] = K4 >> 8;
	TXdata[7] = K4;
	HAL_CAN_AddTxMessage(&hcan2, &TX1message, TXdata, CAN_FILTER_FIFO0);
}


void SET_CAN2Back_MOTOR_CURRENT(CAN_HandleTypeDef* hcan, int16_t K1, int16_t K2, int16_t K3, int16_t K4)    //CAN2 ID:5~8
{	
	TX1message.StdId = 0x1FF;
	TX1message.IDE = CAN_ID_STD;
	TX1message.RTR = CAN_RTR_DATA;
	TX1message.DLC = 8;
	TX1message.TransmitGlobalTime = DISABLE;
	
	TXdata[0] = K1 >> 8;
	TXdata[1] = K1;
	TXdata[2] = K2 >> 8;
	TXdata[3] = K2;
	TXdata[4] = K3 >> 8;
	TXdata[5] = K3;
	TXdata[6] = K4 >> 8;
	TXdata[7] = K4;
	HAL_CAN_AddTxMessage(&hcan2, &TX1message, TXdata, CAN_FILTER_FIFO0);
}

/*******************************************************************/

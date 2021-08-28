/**
  ******************************************************************************
  * @file    Remote_Control.h
  * @author  Ginger
  * @version V1.0.0
  * @date    2015/11/14
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef __RC__
#define __RC__
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#define RC_Frame_Lentgh		18
#define KEY_W ((uint16_t)1 << 0)              //1
#define KEY_S ((uint16_t)1 << 1)              //2
#define KEY_A ((uint16_t)1 << 2)              //4
#define KEY_D ((uint16_t)1 << 3)              //8
#define KEY_SHIFT ((uint16_t)1 << 4)          //16
#define KEY_CTRL ((uint16_t)1 << 5)           //32
#define KEY_Q ((uint16_t)1 << 6)              //64
#define KEY_E ((uint16_t)1 << 7)              //128
#define KEY_R ((uint16_t)1 << 8)              //256
#define KEY_F ((uint16_t)1 << 9)              //512
#define KEY_G ((uint16_t)1 << 10)             //1024
#define KEY_Z ((uint16_t)1 << 11)             //2048
#define KEY_X ((uint16_t)1 << 12)             //4096
#define KEY_C ((uint16_t)1 << 13)             //8192
#define KEY_V ((uint16_t)1 << 14)             //16384
#define KEY_B ((uint16_t)1 << 15)             //32768

typedef struct {
	int16_t ch1;	//each ch value from -364 -- +364
	int16_t ch2;
	int16_t ch3;
	int16_t ch4;
	
	uint8_t switch_left;	//3 value
	uint8_t switch_right;
	
	struct {
		int16_t x;
		int16_t y;
		int16_t z;
	
		uint8_t press_left;
		uint8_t press_right;
	}mouse;
	
	struct {
		uint16_t key_code;
/**********************************************************************************
   * ¼üÅÌÍ¨µÀ:15   14   13   12   11   10   9   8   7   6     5     4   3   2   1
   *          V    C    X	  Z    G    F    R   E   Q  CTRL  SHIFT  D   A   S   W
************************************************************************************/

	}keyBoard;
	
	int16_t pulley;

}RC_Type;



enum{
	Switch_Up = 1,
	Switch_Middle = 3,
	Switch_Down = 2,
};

typedef struct
{
    uint8_t keyboard_Q;
    uint8_t keyboard_E;
    uint8_t keyboard_R;
    uint8_t keyboard_F;
    uint8_t keyboard_G;
    uint8_t keyboard_Z;
    uint8_t keyboard_X;
    uint8_t keyboard_C;
    uint8_t keyboard_V;
    uint8_t keyboard_B;
    uint8_t keyboard_SHIFT;
    uint8_t keyboard_CTRL;
    uint8_t keymouse_left;
    uint8_t keymouse_right;
    uint8_t last_keymouse_left;
    uint8_t last_keymouse_right;
    int16_t keymouse_x;
    int16_t keymouse_y;
    
}KEY;

extern RC_Type remote_control;
extern uint32_t  Latest_Remote_Control_Pack_Time ;
void Callback_RC_Handle(RC_Type* rc, uint8_t* buff);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
#endif



#include "usart.h"
#include "string.h"

#define BLUE  0
#define RED   1

#define U8	unsigned char
#define U16	unsigned short int
#define U32	unsigned int
	
#if 1
int is_red_or_blue(void);
void determine_ID(void);
void draw_message_1(uint32_t operate_tpye,uint16_t cmd_id, uint16_t data_id,char px1[30]);
void draw_message_2(uint32_t operate_tpye,uint16_t cmd_id, uint16_t data_id,char px2[30]);
void draw_message_3(uint32_t operate_tpye,uint16_t cmd_id, uint16_t data_id,char px3[30]);
void draw_int_number_cap(uint32_t operate_tpye,uint16_t cmd_id, uint16_t data_id);
void draw_int_number(uint32_t operate_tpye,uint16_t cmd_id, uint16_t data_id);
void draw_seven_line(uint16_t cmd_id, uint16_t data_id,uint32_t layer,uint32_t color,uint32_t start_x,uint32_t start_y,uint32_t end_x,uint32_t end_y,uint32_t ch_x,uint32_t ch_y);
typedef __packed struct   //�������ݽ�����Ϣ��0x301
{
 uint16_t data_cmd_id;
 uint16_t sender_ID;
 uint16_t receiver_ID;
} ext_student_interactive_header_data_t;

typedef __packed struct   //�����˼�ͨ��  ����ID��0x0200~0x02FF
{
   uint8_t data[112];
} robot_interactive_data_t;


typedef __packed struct   //�ͻ���ɾ��ͼ��  ����ID��0x0100
{
	uint8_t operate_tpye;
	uint8_t layer;
} ext_client_custom_graphic_delete_t;

typedef __packed struct   //ͼ��
{
	uint8_t graphic_name[3];
	uint32_t operate_tpye:3;
	uint32_t graphic_tpye:3;
	uint32_t layer:4;
	uint32_t color:4;
	uint32_t start_angle:9;
	uint32_t end_angle:9;
	uint32_t width:10;
	uint32_t start_x:11;
	uint32_t start_y:11; 
	uint32_t radius:10;
	uint32_t end_x:11;
	uint32_t end_y:11;
} graphic_data_struct_t;

typedef __packed struct        //0003���������˴������
{   
  uint8_t rising_height;//̧���߶�
  uint8_t spin_angle;//��ת�Ƕ�
  uint8_t Steer_flag;//����
  uint8_t help;//��Ԯ
}game_cmd;

typedef union
{
    uint8_t U[4];
    float F;
    int I;
    int16_t H;
}FormatTrans;

typedef __packed struct   
{
	uint8_t graphic_name[3];
	uint32_t operate_tpye:3;
	uint32_t graphic_tpye:3;
	uint32_t layer:4;
	uint32_t color:4;
	uint32_t start_angle:9;
	uint32_t end_angle:9;
	uint32_t width:10;
	uint32_t start_x:11;
	uint32_t start_y:11; 
  int32_t  i;
} graphic_data_struct_t_i;        //������

typedef __packed struct   
{
	uint8_t graphic_name[3];
	uint32_t operate_tpye:3;
	uint32_t graphic_tpye:3;
	uint32_t layer:4;
	uint32_t color:4;
	uint32_t start_angle:9;
	uint32_t end_angle:9;
	uint32_t width:10;
	uint32_t start_x:11;
	uint32_t start_y:11; 
  float  f;                      //������
} graphic_data_struct_t_f;

typedef __packed struct    //����һ��ͼ��  ����ID��0x0101
{
	 graphic_data_struct_t grapic_data_struct;
} ext_client_custom_graphic_single_t;

typedef __packed struct    //��������ͼ��  ����ID��0x0102
{
	graphic_data_struct_t grapic_data_struct[2];
} ext_client_custom_graphic_double_t;

typedef __packed struct    //�������ͼ��  ����ID��0x0103
{
	graphic_data_struct_t grapic_data_struct[5];
} ext_client_custom_graphic_five_t;

typedef __packed struct    //�����߸�ͼ��  ����ID��0x0104
{
	graphic_data_struct_t grapic_data_struct[7];
} ext_client_custom_graphic_seven_t;

typedef __packed struct     //�����ַ�  ����ID��0x0110
{
	graphic_data_struct_t grapic_data_struct;
	uint8_t data[30];

} ext_client_custom_character_t;

typedef __packed struct   
{
  U8 SOF;
	U16 DataLength;
	U8 Seq;
	U8 CRC8;
}Frameheader_t;

typedef __packed struct
{
	Frameheader_t	Frameheader;
	uint16_t cmd_id;
	ext_student_interactive_header_data_t Client_Custom_ID;  
	graphic_data_struct_t grapic_data_struct[7];
	uint16_t crc16;
}ext_draw_ui;                       //ͼ��

typedef __packed struct
{
	Frameheader_t	Frameheader;
	uint16_t cmd_id;
	ext_student_interactive_header_data_t Client_Custom_ID;  
	graphic_data_struct_t grapic_data_struct;
	char data[30];
	uint16_t crc16;
}ext_draw_p;                       //�ַ�

typedef __packed struct
{
	Frameheader_t	Frameheader;
	uint16_t cmd_id;
	ext_student_interactive_header_data_t Client_Custom_ID;  
	graphic_data_struct_t_i graphic_data_struct_t_ii;
	uint16_t crc16;
}ext_draw_i;                      //������

typedef __packed struct
{
	Frameheader_t	Frameheader;
	uint16_t cmd_id;
	ext_student_interactive_header_data_t Client_Custom_ID;  
	graphic_data_struct_t_f graphic_data_struct_t_ff;
	uint16_t crc16;
}ext_draw_f;                     //������

//usart6(receive data)
typedef __packed struct        //0x0001����״̬����
{   
  uint8_t game_type : 4;   
  uint8_t game_progress : 4;   
  uint16_t stage_remain_time; 
	uint64_t SyncTimeStamp;       //ʱ��ͬ��������
} extgame_state_t;

typedef __packed struct        //0x0002�����������
{
  uint8_t  winner;
}extGameResult_t;

typedef __packed struct        //0x0003����������Ѫ������
{   
   uint16_t red_1_robot_HP;
	 uint16_t red_2_robot_HP;
	 uint16_t red_3_robot_HP;
	 uint16_t red_4_robot_HP;
	 uint16_t red_5_robot_HP;
	 uint16_t red_7_robot_HP;
	 uint16_t red_outpost_HP;
	 uint16_t red_base_HP;
	 uint16_t blue_1_robot_HP;
	 uint16_t blue_2_robot_HP;
	 uint16_t blue_3_robot_HP;
	 uint16_t blue_4_robot_HP;
	 uint16_t blue_5_robot_HP;
	 uint16_t blue_7_robot_HP;
	 uint16_t blue_outpost_HP;
	 uint16_t blue_base_HP; 
} extgame_robot_HP_t;

typedef __packed struct        //0x0004���ڷ���״̬
{
 uint8_t dart_belong;
 uint16_t stage_remaining_time;
} extdart_status_t;

typedef __packed struct        //0x0101�����¼�����
{   
  uint32_t event_type; 
} extevent_data_t; 

typedef __packed struct       //0x0102���ز���վ������ʶ����
{   
  uint8_t supply_projectile_id;    
  uint8_t supply_robot_id;    
  uint8_t supply_projectile_step;  
  uint8_t supply_projectile_num; 
} extsupply_projectile_action_t;

typedef __packed struct      //0x0104���о�������
{
 uint8_t level;
 uint8_t foul_robot_id;
} extreferee_warning_t;

typedef __packed struct      //0x0105���ڷ���ڵ���ʱ
{
	 uint8_t dart_remaining_time;
} extdart_remaining_time_t;

typedef __packed struct        //0x0201������״̬����
{
	 uint8_t robot_id;
	 uint8_t robot_level;
	 uint16_t remain_HP;
	 uint16_t max_HP;

	 uint16_t shooter_id1_17mm_cooling_rate;      //������1��17mmǹ��ÿ����ȴֵ
   uint16_t shooter_id1_17mm_cooling_limit;     //������1��17mmǹ����������
   uint16_t shooter_id1_17mm_speed_limit;       //������1��17mmǹ�������ٶ�
	 
   uint16_t shooter_id2_17mm_cooling_rate;
   uint16_t shooter_id2_17mm_cooling_limit;
   uint16_t shooter_id2_17mm_speed_limit;
 
   uint16_t shooter_id1_42mm_cooling_rate;
   uint16_t shooter_id1_42mm_cooling_limit;
   uint16_t shooter_id1_42mm_speed_limit;
 
   uint16_t chassis_power_limit;
   uint8_t mains_power_gimbal_output : 1;
   uint8_t mains_power_chassis_output : 1;
   uint8_t mains_power_shooter_output : 1;
}extGameRobotState_t;

typedef __packed struct        //0x0202ʵʱ������������
{
	 uint16_t chassis_volt;
	 uint16_t chassis_current;
	 float chassis_power;
	 uint16_t chassis_power_buffer;

	uint16_t shooter_id1_17mm_cooling_heat;
	uint16_t shooter_id2_17mm_cooling_heat;
	uint16_t shooter_id1_42mm_cooling_heat;     
}extPowerHeatData_t;

typedef __packed struct        //0x0203������λ������
{
    float    x;
    float    y;
    float    z;
    float    yaw;
}extGameRobotPos_t;

typedef __packed struct         //0x0204��������������
{   
    uint8_t power_rune_buff; 
}extbuff_t; 

typedef __packed struct        //0x0205���л���������״̬����
{   
 uint8_t attack_time;
} aerialrobot_energy_t; 

typedef __packed struct        //0x0206�˺�״̬����
{
  uint8_t armor_id : 4;
	uint8_t hurt_type : 4; 
}extRobotHurt_t;

typedef __packed struct        //0x0207ʵʱ�����Ϣ
{
 uint8_t bullet_type;
 uint8_t shooter_id;
 uint8_t bullet_freq;
 float bullet_speed; 
}extShootData_t;

typedef __packed struct       //0x0208����ʣ�෢����
{
 	uint16_t bullet_remaining_num_17mm;
  uint16_t bullet_remaining_num_42mm;
  uint16_t coin_remaining_num;
} extbullet_remaining_t;

typedef __packed struct      //0x0209������RFID״̬
{
 uint32_t rfid_status;
} extrfid_status_t;

typedef __packed struct     //0x0200A���ڻ����˿ͻ���ָ������
{
 uint8_t dart_launch_opening_status;
 uint8_t dart_attack_target;
 uint16_t target_change_time;
 uint8_t first_dart_speed;
 uint8_t second_dart_speed;
 uint8_t third_dart_speed;
 uint8_t fourth_dart_speed;
 uint16_t last_dart_launch_time;
 uint16_t operate_launch_cmd_time;
} extdart_client_cmd_t;

typedef __packed struct       //0x0301�����˼佻������
{
    float  data1;
    float  data2;
    float  data3;
    uint8_t mask;      //21��
}extConData_t;

typedef __packed struct     //0x302�������ݽ�����Ϣ
{
	uint8_t data[30];
}Re_robot_interactive_data_t;

typedef __packed struct      //0x303С��ͼ������Ϣ��ʶ
{
	float target_position_x;
  float target_position_y;
  float target_position_z;
  uint8_t commd_keyboard;
  uint16_t target_robot_ID;
}ext_robot_command_t;

typedef __packed struct       //0x304ͼ��ң����Ϣ��ʶ
{
	int16_t mouse_x;
  int16_t mouse_y;
  int16_t mouse_z;
  int8_t left_button_down;
  int8_t right_button_down;
  uint16_t keyboard_value;
  uint16_t reserved;
}Re_ext_robot_command_t;

//���ݰ��ṹ�嶨��
//typedef __packed struct   
//{
//  U8 SOF;
//	U16 DataLength;
//	U8 Seq;
//	U8 CRC8;
//}Frameheader_t;

typedef __packed struct   
{
  U8 SOF;
	U16 DataLength;
	U8 Seq;
	U8 CRC8;
}Frameheader_r;

typedef __packed struct   
{
  U8 Low;
  U8 High;
}rxCmdId_r;

//typedef __packed struct
//{
//	Frameheader_t	Frameheader;
//	uint16_t cmd_id;
//	ext_student_interactive_header_data_t Client_Custom_ID;  
//	graphic_data_struct_t grapic_data_struct;
//	uint16_t crc16;
//}ext_draw_ui;

typedef __packed struct   
{
	Frameheader_r Frameheader_rx;
	rxCmdId_r rxCmdId_rx;
	
	extgame_state_t extgame_state;
	extGameResult_t extGameResult;
	extgame_robot_HP_t extgame_robot_HP;
	extdart_status_t  extdart_status;
	extevent_data_t extevent_data;
	extsupply_projectile_action_t extsupply_projectile_action;
	extreferee_warning_t extreferee_warning;
	extdart_remaining_time_t  extdart_remaining_time;
	extGameRobotState_t extGameRobotState;
	extPowerHeatData_t extPowerHeatData;
	extGameRobotPos_t extGameRobotPos;
	extbuff_t extbuff_musk;
	aerialrobot_energy_t aerialrobot_energy;
	extRobotHurt_t extRobotHurt;
	extShootData_t extShootData;
	extbullet_remaining_t  extbullet_remaining;
	extrfid_status_t  extrfid_status;
	extdart_client_cmd_t extdart_client_cmd;
	extConData_t extConData;
	Re_robot_interactive_data_t Re_robot_interactive_data;
	ext_robot_command_t ext_robot_command;
	Re_ext_robot_command_t Re_ext_robot_command;
}JudgementData_t;
#endif

//typedef union
//{
//    uint8_t U[4];
//    float F;
//    int I;
//}FormatTrans;

typedef enum
{
	extgame_state_ID = 0x0001,
	extGameResult_ID = 0x0002,
	extgame_robot_HP_ID = 0x0003,
	extdart_status_ID = 0x0004,
	extevent_data_ID = 0x0101,
	extsupply_projectile_action_ID = 0x0102,
	extreferee_warning_ID = 0x0104,
	extdart_remaining_time_ID = 0x0105,
	extGameRobotState_ID = 0x0201,
  extPowerHeatData_ID = 0x0202,
	extGameRobotPos_ID = 0x0203,
	extbuff_ID = 0x0204,
	aerialrobot_energy_ID = 0x0205,
	extRobotHurt_ID = 0x0206,
	extShootData_ID = 0x0207,
	extbullet_remaining_ID = 0x0208,
	extrfid_status_ID = 0x0209,
	extdart_client_cmd_ID = 0x020A,
	extConData_ID = 0x0301,
	Re_robot_interactive_data_ID = 0x0302,
	ext_robot_command_ID = 0x0303,
	Re_ext_robot_command_ID = 0x0304,
}Refree_Message_ID;


extern game_cmd super_c;


extern JudgementData_t JudgementData;

extern uint8_t uart6_rx_buff[200];
extern uint16_t u6_cmd_data;
void judgeCalculate(uint8_t *data);
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
uint32_t Verify_CRC16_Check_Sum(uint8_t    *pchMessage, uint32_t    dwLength);
void cmd_rc_callback_handler(void);
void get_data(uint16_t *LIMIT_POWER_D);

void get_ShootData(uint16_t *cooling_limit,float *cooling_heat);
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength);
void Cmd_Send_Transmit_Handler(uint16_t cmd_id, uint16_t data_id, uint16_t tx_id, uint16_t rx_id);
void refree_task(game_cmd tem_super_c);




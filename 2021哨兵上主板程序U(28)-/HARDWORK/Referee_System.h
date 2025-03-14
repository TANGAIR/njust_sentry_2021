#ifndef _REFEREE_SYSTEM_H
#define _REFEREE_SYSTEM_H
#include "sys.h"


//���̹�������
#define POWER_LIMIT 100

//�����ٶ�
extern float Up_Bullet_Speed;
extern float Down_Bullet_Speed;


/********************************************����ϵͳ������ݽṹ������*****************************************************/


/*****************************************************���ݸ�ʽ**************************************************/
//֡ͷ��ʽ
typedef struct
{                                       //�ֽ�ƫ��     ��С     ˵��
	u8 SOF;                               //   0          1      ����֡��ʼ�ֽڣ��̶�ֵΪ0XA5
	u16 DataLength;                       //   1          2      ����֡��Data���� 
	u8 Seq;                               //   3          1      ����� 
	u8 CRC8;                              //   4          1      ֡ͷCRCУ��
}FrameHeader_TypeDef;


//ͨ��Э��
typedef struct
{                                       //�ֽ�ƫ��     ��С     ˵��
	FrameHeader_TypeDef FrameHeader;      //   0          5      ֡ͷЭ��
	u16 CmdID;                            //   5          2      ������ID
	u16 Date[50];                         //   7          n      ����
	u16 FrameTail;                        //   7+n        2      CRC16У��
}RefereeInfo_TypeDef;

/*****************************************************������Ϣ**************************************************/
//����״̬��Ϣ0x0001
typedef __packed struct
{
 uint8_t game_type : 4;
 uint8_t game_progress : 4;     //4-7 bit����ǰ�����׶�? 0��δ��ʼ������ ? 1��׼���׶Σ� ? 2���Լ�׶Σ� ? 3��5s ����ʱ�� ? 4����ս�У� ? 5������������
 uint16_t stage_remain_time;    //��ǰ�׶�ʣ��ʱ�䣬��λ s
 uint64_t SyncTimeStamp;        // �����˽��յ���ָ��ľ�ȷ Unix ʱ�䣬�����ض��յ���Ч�� NTP ��������ʱ����Ч

} ext_game_state_t; 


//����ʤ������  0x0002
typedef __packed struct
{                                       //�ֽ�ƫ��     ��С     ˵��	
  uint8_t winner;                       //   0          1      0-ƽ�֣�1-�췽ʤ��2-����ʤ
}extGameResult_t;

/*****************************************************������Ϣ**************************************************/
//����������״̬��0x0201������Ƶ�ʣ�10Hz
//1���췽Ӣ�ۻ����ˣ� 2���췽���̻����ˣ�3/4/5���췽���������ˣ� 6���췽���л����ˣ� 
//7���췽�ڱ������ˣ� 8���췽���ڻ����ˣ�9���췽�״�վ��
//101������Ӣ�ۻ����ˣ�102���������̻����ˣ�103/104/105���������������ˣ�106���������л����ˣ�
//107�������ڱ������ˣ�108���������ڻ����ˣ�109�������״�վ
typedef __packed struct
{                                         //�ֽ�ƫ���� ��С ˵��
 uint8_t robot_id;                        //0 1  �������� ID��
 uint8_t robot_level;                     //1 1  �����˵ȼ��� 1��һ����2��������3������
 uint16_t remain_HP;                      //2 2  ������ʣ��Ѫ��
 uint16_t max_HP;                         //4 2  ����������Ѫ��
 uint16_t shooter_id1_17mm_cooling_rate;  //6 2  ������ 1 �� 17mm ǹ��ÿ����ȴֵ
 uint16_t shooter_id1_17mm_cooling_limit; //8 2  ������ 1 �� 17mm ǹ����������
 uint16_t shooter_id1_17mm_speed_limit;   //10 2 ������ 1 �� 17mm ǹ�������ٶ� ��λ m/s
 uint16_t shooter_id2_17mm_cooling_rate;  //12 2 ������ 2 �� 17mm ǹ��ÿ����ȴֵ
 uint16_t shooter_id2_17mm_cooling_limit; //14 2 ������ 2 �� 17mm ǹ����������
 uint16_t shooter_id2_17mm_speed_limit;   //16 2 ������ 2 �� 17mm ǹ�������ٶ� ��λ m/s
 uint16_t shooter_id1_42mm_cooling_rate;  //18 2 ������ 42mm ǹ��ÿ����ȴֵ
 uint16_t shooter_id1_42mm_cooling_limit; //20 2 ������ 42mm ǹ����������
 uint16_t shooter_id1_42mm_speed_limit;   //22 2 ������ 42mm ǹ�������ٶ� ��λ m/s
 uint16_t chassis_power_limit;            //24 2 �����˵��̹�����������
 uint8_t mains_power_gimbal_output : 1;   //26 1 ���ص�Դ��������0 bit��gimbal ������� 1 Ϊ�� 24V �����0 Ϊ�� 24v �����
 uint8_t mains_power_chassis_output : 1;  //                       1 bit��chassis �������1 Ϊ�� 24V �����0 Ϊ�� 24v �����
 uint8_t mains_power_shooter_output : 1;  //                       2 bit��shooter �������1 Ϊ�� 24V �����0 Ϊ�� 24v �����
} ext_game_robot_status_t;                

//�з�������Ѫ��
typedef struct
{
	short  Hero_1;                  //0-û���ҵ�Ŀ�꣬1-�ҵ�Ŀ�꣬2-����Ŀ��	
	short  Standar_3;
	short  Standar_4;
	short  Standar_5;
	short  Engineering_2;
	short  Sentry_7;
	char   Hero_1_Invinceble_Time;
	char   Engineering_2_Invinceble_Time;
	char   Standar_3_Invinceble_Time;
	char   Standar_4_Invinceble_Time;
	char   Standar_5_Invinceble_Time;


}Enemy_HP;








//ʵʱ�����������ݣ�0x0202������Ƶ�ʣ�50Hz
typedef __packed struct
{                                          //�ֽ�ƫ���� ��С ˵��
 uint16_t chassis_volt;                    //0 2 ���������ѹ ��λ ����
 uint16_t chassis_current;                 //2 2 ����������� ��λ ����
 float chassis_power;                      //4 4 ����������� ��λ W ��
 uint16_t chassis_power_buffer;            //8 2 ���̹��ʻ��� ��λ J ���� ��ע�����¸��ݹ��������� 250J
 uint16_t shooter_id1_17mm_cooling_heat;   //10 2 1 �� 17mm ǹ������
 uint16_t shooter_id2_17mm_cooling_heat;   //12 2 2 �� 17mm ǹ������
 uint16_t shooter_id1_42mm_cooling_heat;   //14 2 42mm ǹ������
} ext_power_heat_data_t;


//�˺�״̬��0x0206������Ƶ�ʣ��˺���������
typedef __packed struct
{                       //�ֽ�ƫ���� ��С ˵��
 uint8_t armor_id : 4;  //0 1             bit 0-3����Ѫ���仯����Ϊװ���˺�������װ�� ID��������ֵΪ 0-4 �Ŵ�������˵����װ��Ƭ������Ѫ���仯���ͣ��ñ�����ֵΪ 0��
 uint8_t hurt_type : 4; //                0x0 װ���˺���Ѫ��0x1 ģ����߿�Ѫ��0x2 �����ٿ�Ѫ��0x3 ��ǹ��������Ѫ��0x4 �����̹��ʿ�Ѫ��0x5 װ��ײ����Ѫ
} ext_robot_hurt_t;


//ʵʱ�����Ϣ��0x0207������Ƶ�ʣ�������� ,��ʼ����7
typedef __packed struct
{                        //�ֽ�ƫ���� ��С ˵��
 uint8_t bullet_type;    //0 1 �ӵ�����: 1��17mm ���� 2��42mm ����
 uint8_t shooter_id;     //1 1  ������� ID�� 1��1 �� 17mm �������   2��2 �� 17mm �������   3��42mm �������
 uint8_t bullet_freq;    //2 1 �ӵ���Ƶ ��λ Hz
 float bullet_speed;     //3 4 �ӵ����� ��λ m/s
} ext_shoot_data_t;




/*****************************************************������������Ϣ**************************************************/
//������Ѫ�����ݣ�0x0003������Ƶ�ʣ�1Hz
typedef __packed struct
{                           //�ֽ�ƫ���� ��С ˵��
 uint16_t red_1_robot_HP;   //0 2  �� 1 Ӣ�ۻ�����Ѫ����δ�ϳ��Լ�����Ѫ��Ϊ 0
 uint16_t red_2_robot_HP;   //2 2  �� 2 ���̻�����Ѫ��
 uint16_t red_3_robot_HP;   //4 2  �� 3 ����������Ѫ��
 uint16_t red_4_robot_HP;   //6 2  �� 4 ����������Ѫ��
 uint16_t red_5_robot_HP;   //8 2  �� 5 ����������Ѫ��
 uint16_t red_7_robot_HP;   //10 2 �� 7 �ڱ�������Ѫ��
 uint16_t red_outpost_HP;   //12 2 �췽ǰ��սѪ��
 uint16_t red_base_HP;      //14 2 �췽����Ѫ��
 uint16_t blue_1_robot_HP;  //16 2 �� 1 Ӣ�ۻ�����Ѫ��
 uint16_t blue_2_robot_HP;  //18 2 �� 2 ���̻�����Ѫ��
 uint16_t blue_3_robot_HP;  //20 2 �� 3 ����������Ѫ��
 uint16_t blue_4_robot_HP;  //22 2 �� 4 ����������Ѫ��
 uint16_t blue_5_robot_HP;  //24 2 �� 5 ����������Ѫ��
 uint16_t blue_7_robot_HP;  //26 2 �� 7 �ڱ�������Ѫ��
 uint16_t blue_outpost_HP;  //28 2 ����ǰ��վѪ��
 uint16_t blue_base_HP;     //30 2 ��������Ѫ��
} ext_game_robot_HP_t;


//���ڷ���״̬��0x0004������Ƶ�ʣ����ڷ�����ͣ����ͷ�Χ�����л�����
typedef __packed struct
{                                //�ֽ�ƫ���� ��С ˵��
 uint8_t dart_belong;            //0 1 ������ڵĶ��飺1���췽����    2����������
 uint16_t stage_remaining_time;  //1 2 ����ʱ��ʣ�����ʱ�䣬��λ s
} ext_dart_status_t;


//���ڷ���ڵ���ʱ��cmd_id (0x0105)������Ƶ�ʣ�1Hz ���ڷ��ͣ����ͷ�Χ������������
typedef __packed struct
{                              //�ֽ�ƫ���� ��С ˵��
 uint8_t dart_remaining_time;   //0 1             15s ����ʱ
} ext_dart_remaining_time_t;


/*****************************************************����ͳ�����Ϣ**************************************************/
//���������棺0x0204������Ƶ�ʣ�1Hz
typedef __packed struct
{                                //�ֽ�ƫ���� ��С ˵��
 uint8_t power_rune_buff;        //0 1              bit 0��������Ѫ����Ѫ״̬bit 1��ǹ��������ȴ����bit 2�������˷����ӳ�bit 3�������˹����ӳ����� bit ����
}ext_buff_t;

//������ RFID ״̬��0x0209������Ƶ�ʣ�1Hz�����ͷ�Χ����һ������
typedef __packed struct
{                       //�ֽ�ƫ���� ��С   ˵��
 uint32_t rfid_status;  //0 4
																					//bit 0����������� RFID ״̬��
																					//bit 1���ߵ������ RFID ״̬��
																					//bit 2���������ؼ���� RFID ״̬��
																					//bit 3����������� RFID ״̬��
																					//bit 4��ǰ�ڸ������ RFID ״̬��
																					//bit 5����Դ������� RFID ״̬��
																					//bit 6����Ѫ������� RFID ״̬��
																					//bit 7�����̻����˲�Ѫ�� RFID ״̬��
																					//bit 8-31������
																					//RFID ״̬����ȫ�����Ӧ������򴦷�״̬������з���ռ��ĸߵ�����㣬����
																					//��ȡ��Ӧ������Ч����
} ext_rfid_status_t; 


//�����¼����ݣ�0x0101������Ƶ�ʣ��¼��ı����
//bit 0����������վ 1 �Ų�Ѫ��ռ��״̬ 1 Ϊ��ռ�죻
//bit 1����������վ 2 �Ų�Ѫ��ռ��״̬ 1 Ϊ��ռ�죻
//bit 2����������վ 3 �Ų�Ѫ��ռ��״̬ 1 Ϊ��ռ�죻
//bit 3-5��������������״̬��? bit 3 Ϊ�����ռ��״̬��1 Ϊռ�죻 ? bit 4 ΪС�������ؼ���״̬��1 Ϊ�Ѽ�� ? bit 5 Ϊ���������ؼ���״̬��1 Ϊ�Ѽ��
//bit 6������ R2 ���θߵ�ռ��״̬ 1 Ϊ��ռ�죻
//bit 7������ R3 ���θߵ�ռ��״̬ 1 Ϊ��ռ�죻
//bit 8������ R4 ���θߵ�ռ��״̬ 1 Ϊ��ռ�죻
//bit 9���������ػ���״̬��? 1 Ϊ���������⻤��Ѫ���� ? 0 Ϊ���������⻤��Ѫ����
//bit 10������ǰ��ս״̬��? 1 Ϊǰ��ս�� ? 0 Ϊǰ��ս�����٣�
//bit 11 -31: ����
typedef __packed struct
{                        

 uint32_t event_type;
} ext_event_data_t;




/*****************************************************������Ϣ**************************************************/
//������λ�ã�0x0203������Ƶ�ʣ�10Hz
typedef __packed struct
{                                        //�ֽ�ƫ��     ��С     ˵��
	float x;                               //   0          4      λ��X����ֵ����λ����
	float y;                               //   4          4      λ��Y����ֵ����λ����
	float z;                               //   8          4      λ��Z����ֵ����λ����
	float yaw;                             //   12         4      ǹ�ڳ���Ƕȣ���λ����
}ext_game_robot_pos_t;


//�������ݽ�����Ϣ��0x0301,���� cmd_id ����Ƶ�����Ϊ 10Hz��������Ŵ���
typedef __packed struct
{                                              //�ֽ�ƫ���� ��С ˵�� ��ע
 uint16_t data_cmd_id;                         //0 2 ���ݶε����� ID  0x0200~0x02FF���������� ID ��ѡȡ������ ID �����ɲ������Զ���
 uint16_t sender_ID;                           //2 2 �����ߵ� ID      ��ҪУ�鷢���ߵ� ID ��ȷ�ԣ������ 1 ���͸��� 5��������ҪУ��� 1
 uint16_t receiver_ID;                         //4 2 �����ߵ� ID      ��ҪУ������ߵ� ID ��ȷ�ԣ����粻�ܷ��͵��жԻ����˵� ID
}ext_student_interactive_header_data_t;


//ѧ�������˼�ͨ�� cmd_id 0x0301������ ID:0x0200~0x02FF
typedef __packed struct
{                            //�ֽ�ƫ���� ��С ˵�� ��ע
  uint8_t data[110];         //6 n            ���ݶ� n��ҪС�� 113
}robot_interactive_data_t;


//С��ͼ������Ϣ,С��ͼ������Ϣ��ʶ��0x0303������Ƶ�ʣ�����ʱ���͡�
typedef __packed struct
{
	float target_position_x;
	float target_position_y;
	float target_position_z;
	uint8_t commd_keyboard;
	uint16_t target_robot_ID;
} ext_robot_command_t;





/********************************************������������չ*****************************************************/
//����ϵͳ���ݸ�ʽ
extern RefereeInfo_TypeDef 				   RefereeInfor;
 
 
 
 //������Ϣ
extern ext_game_state_t              Game_State; 
 //�������
extern extGameResult_t     				   GameResult; 
 
 
 //������״̬
extern ext_game_robot_status_t       GameRobotState;
 //ʵʱ��������
extern ext_power_heat_data_t  			 PowerHeatData; 
 //�˺�״̬
extern ext_robot_hurt_t      				 RobotHurt;
 //ʵʱ�����Ϣ
extern ext_shoot_data_t      				 ShootData;



 //����������Ѫ������
extern ext_game_robot_HP_t           GameRobotHP;
 //���ڷ���״̬
extern ext_dart_status_t             DartStatus;
 //���ڷ���ڵ���ʱ
extern ext_dart_remaining_time_t     DartRemainingTime;
 
 
 
 //����������
extern ext_buff_t       				     BuffMusk;
 //������ RFID ״̬
extern ext_rfid_status_t     				 RfidDetect;
 //�����¼�����
extern ext_event_data_t              EventData;
 
 
 
 //������λ��
extern ext_game_robot_pos_t    			 GameRobotPos;
 //ѧ�������˼�ͨ��
extern robot_interactive_data_t      ShowData;





//�����������
extern int Game_Allow_Shoot_Flag;
//��ǰ����������
extern float Power_Max; 
//����3ת����־
extern short usart3_toggle_flag; 
//���跢����Ŀ
extern int number_1;	

extern int My_Outpost_HP;

extern char Output_On;
/********************************************��������*****************************************************/
void JudgeSys_Init(void);
void USART3_InitConfig(void);
void JudgeSys_DMA_InitConfig(void); 

void Judge_Data_Process(void);
void RefereeInfo_Decode(uint8_t *UsartRx_Info);
void Student_Date_Upload( robot_interactive_data_t * Studentdata);
void Referee_System_Task(void *pvParameters);    


#endif










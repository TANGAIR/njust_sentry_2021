#ifndef USART_H
#define USART_H

#include "sys.h" 

typedef struct
{
	int8_t Get_Hit_flag;                    //2-û���ܵ�������0-װ�� 0�ܵ�������1-װ��1�ܵ�������3-����װ�׶��ܵ��˹���
	int8_t Bass_Satus;                      //2-100%������1-50%���� �� 0-�޷���
	short  Chassis_Real_Position;           //����202�������࿪ʼ����λmm
	short  Chassis_Real_Number;             //ÿ��10s��һ����
	short  Remain_HP;                       //ʣ��Ѫ������50
	int8_t Sentry_Satus;                    //1-������̰�ȫ��0-���̶ϵ�
	short  Chassis_Speed_Real;              //�����ٶ�
	char   Bullet_Type ;                    //1--����     2--С���� 
	int8_t Shoot_Allow_Flag;                //0--���ܷ���   1--���Է���
	char   Bullet_Speed;                    //�����ٶ�
	char   Locked_Flag;                     //���̶�ת��־
	char   Which_Color_I_Am;                //0---��     1---��
	char   Enemy_Armor_ID;
	u8     Enemy_State;                //�з������˵Ĵ��״̬
	u8     Enemy_HP_HL;                //�з�������Ѫ���Ӹߵ������У�01��1��Ӣ�ۣ�11��3�Ų�����00���ĺŲ�����10��5�Ų���,��λѪ����
	
	
	
	
}Up_To_Down_Typedef;

typedef struct
{
	int8_t Target_Locked;                  //0-û���ҵ�Ŀ�꣬1-�ҵ�Ŀ�꣬2-����Ŀ��
	char   Enemy_Armor_ID;
	short  Yaw_Angle_Real;
	char   NUC_Shoot_Allow_Flag;
	short   Distace;
}Down_To_Up_Typedef;

//NUC�������ݽṹ��
typedef struct
{
	int8_t Armor_Type;             //װ������  0-��  1-С  2-��
	float  Yaw_Angle;              //Y�������Ƕ�
  float  Pitch_Angle;            //P�������Ƕ�
	short  Enemy_distance;         //�з�����
	char   Armor_ID;               //�з�װ�׺���
	short  Enemy_Real_Speed;
	char   NUC_Shoot_Allow_Flag;
}NUC_Data_Typedef;




void UART7_Init(void);



void USART8_Send_Task(void *pvParameters);
void Down_To_Up_Decode_Task(void *pvParameters);
void UP_Send_NUC_Task(void *pvParameters);
void NUC_Decode_Task(void *pvParameters);






short  u8ToShort(char a[],char b);
char   floatTou8(char bit,float data);
short  int8ToShort(int8_t a[],char b);
char   shortTou8(char bit,short data);
int8_t shortToint8(char bit,short data);
u8 bit32TObit8(int index_need,int bit32);

/*********************************************���ݶ�������չ��**********************************************************************/
extern int    NUC_Lost_Time;
extern int    View_Enemy_ID;

extern NUC_Data_Typedef    Up_NUC_Data_TypeStruct;
extern Up_To_Down_Typedef  Up_To_Down_TypeStruct;
extern Down_To_Up_Typedef  Down_To_Up_TypeStruct;

extern int    Yaw_Angle;
extern	short Pitch_Angle,
							Pitch_Speed,
							Yaw_Speed;

extern	char Send_NUC[23];



//����7���տ�����Ϣ
extern uint16_t My_Usart_Recieve_Data;

#endif



#ifndef USART_H
#define USART_H

#include "sys.h"       
/*********************************************�ṹ�嶨����**********************************************************************/
//�����巢�͵����������ݽṹ��
typedef struct
{
	int8_t Get_Hit_flag;                    //2-û���ܵ�������0-װ��0�ܵ�������1-װ��1�ܵ�������3-����װ�׶��ܵ��˹���
	int8_t Bass_Status;                     //2-100%������1-50%���� �� 0-�޷���
	short  Chassis_Real_Position;           //��λmm
	short  Chassis_Real_Number;             //ÿ��10s��һ����
	int8_t Shoot_Allow_Flag;                //ʣ��Ѫ������50
	int8_t Sentry_Satus;                    //1-���ذ�ȫ��0-�����ܵ�����
	short  Chassis_Speed_Real;              //�����ٶ�
	char   Bullet_Type;                     //1--����   2--С����  
	char   Bullet_Speed;
	char   Locked_Flag;
	char   Which_Color_I_Am;                //��0  ��1
	char   Output_On;
  char   Enemy_Armor_ID;
	u8     Enemy_State;                //�з������˵Ĵ��״̬
	u8     Enemy_HP_HL;                //�з�������Ѫ���Ӹߵ������У�01��1��Ӣ�ۣ�11��3�Ų�����00���ĺŲ�����10��5�Ų���,��λѪ����
	
}Up_To_Down_Typedef;

//�����巢�͵����������ݽṹ��
typedef struct
{
	int8_t Target_Locked;                  //0-û���ҵ�Ŀ�꣬1-�ҵ�Ŀ�꣬2-����Ŀ��
	short  Target_Position;
	char   Enemy_Armor_ID;
	short  Yaw_Angle_Real;
	char   NUC_Shoot_Allow_Flag;
	short  Distance;
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

	
}NUC_Data_Typedef;


/*********************************************����������**********************************************************************/
void USART6_Init(void);
void USART8_Init(void);
void NUC_Init(void);
void USART_InitConfig(void);

void USART8_Send(void);
void USART3_Send_NUC(void);

void Up_To_Down_Decode(void);
void NUC_Decode(void);

void USART8_Send_Task(void *pvParameters);
void Up_To_Down_Decode_Task(void *pvParameters);
void NUC_Decode_Task(void *pvParameters);

short  u8ToShort(char a[],char b);
char   floatTou8(char bit,float data);
short  int8ToShort(int8_t a[],char b);
char   shortTou8(char bit,short data);
int8_t shortToint8(char bit,short data);
u8 bit32TObit8(int index_need,int bit32);
int bit8TObit32(uint8_t *change_info);
/*********************************************���ݶ�������չ��**********************************************************************/
extern Up_To_Down_Typedef  Up_To_Down_TypeStruct;
extern Down_To_Up_Typedef  Down_To_Up_TypeStruct;
extern NUC_Data_Typedef    Down_NUC_Data_TypeStruct;

extern char   Usart3Rx_Info[10];
extern int8_t Up_To_DOWN_Rx[17];
extern int8_t Down_To_Up_Tx[10];
extern char   Send_NUC[23];//��������������



#endif
    

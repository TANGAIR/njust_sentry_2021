#ifndef USART_H
#define USART_H

#include "sys.h"       
/*********************************************结构体定义区**********************************************************************/
//上主板发送到下主板数据结构体
typedef struct
{
	int8_t Get_Hit_flag;                    //2-没有受到攻击，0-装甲0受到攻击，1-装甲1受到攻击，3-两个装甲都受到了攻击
	int8_t Bass_Status;                     //2-100%防御，1-50%防御 ， 0-无防御
	short  Chassis_Real_Position;           //单位mm
	short  Chassis_Real_Number;             //每过10s减一个数
	int8_t Shoot_Allow_Flag;                //剩余血量除以50
	int8_t Sentry_Satus;                    //1-基地安全，0-基地受到攻击
	short  Chassis_Speed_Real;              //底盘速度
	char   Bullet_Type;                     //1--大弹丸   2--小弹丸  
	char   Bullet_Speed;
	char   Locked_Flag;
	char   Which_Color_I_Am;                //红0  蓝1
	char   Output_On;
  char   Enemy_Armor_ID;
	u8     Enemy_State;                //敌方机器人的存活状态
	u8     Enemy_HP_HL;                //敌方机器人血量从高到低排列，01表1号英雄，11表3号步兵，00表四号步兵，10表5号步兵,低位血量低
	
}Up_To_Down_Typedef;

//下主板发送到上主板数据结构体
typedef struct
{
	int8_t Target_Locked;                  //0-没有找到目标，1-找到目标，2-锁定目标
	short  Target_Position;
	char   Enemy_Armor_ID;
	short  Yaw_Angle_Real;
	char   NUC_Shoot_Allow_Flag;
	short  Distance;
}Down_To_Up_Typedef;

//NUC接收数据结构体
typedef struct
{
	int8_t Armor_Type;             //装甲类型  0-无  1-小  2-大
	float  Yaw_Angle;              //Y轴增量角度
  float  Pitch_Angle;            //P轴增量角度
	short  Enemy_distance;         //敌方距离
	char   Armor_ID;               //敌方装甲号码	
	short  Enemy_Real_Speed;

	
}NUC_Data_Typedef;


/*********************************************函数声明区**********************************************************************/
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
/*********************************************数据定义域拓展区**********************************************************************/
extern Up_To_Down_Typedef  Up_To_Down_TypeStruct;
extern Down_To_Up_Typedef  Down_To_Up_TypeStruct;
extern NUC_Data_Typedef    Down_NUC_Data_TypeStruct;

extern char   Usart3Rx_Info[10];
extern int8_t Up_To_DOWN_Rx[17];
extern int8_t Down_To_Up_Tx[10];
extern char   Send_NUC[23];//串口三发送数组



#endif
    

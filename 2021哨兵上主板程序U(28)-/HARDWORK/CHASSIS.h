#ifndef CHASSIS_H
#define CHASSIS_H
#include "sys.h"

/*
	3508�����ʱ��ת�ٶ�Ϊ����
	����⾶ 590 ���ھ� 410 ���м� 500 ��
	װ���������IDΪ0����һ��Ϊ1��
	
	�涨 
		  �ڱ��� �� ���ٶ�Ϊ�� ʵ���ٶȷ��� Chassis_Direction = 1������ ��Ϊ��  Chassis_Direction = -1��
		  Ŀ���ٶȷ���Ϊ�ң���ֵΪ����Chassis_Target_Direction=1��Ϊ��Ϊ����Chassis_Target_Direction=-1��
		  ����ֵΪ -1���Ҽ���ֵΪ 1
		  202���������ز�
		  �ӻ��ز࿴����  ��ֱ��--0  �����--1����ֱ��--2�������--3����ֱ��--4
		  �����ߵ��202��Ȧ������
*/

enum Chassis_mode
{
	Chassis_Plane_Fly,
	Chassis_Stop_3S,
	
	
	Chassis_Up_Find_Target,
	Chassis_Find_Target,
	Chassis_Get_Big_Bullet,
	Chassis_Get_Big_Fire,
	Chassis_Many_Enemy,
	Chassis_Get_Small_Hurt,
	Chassis_Limite,
	Chassis_Lock_Target,
	
	Chassis_Normal,
};

extern int Swag_Speed;
extern int Danger_Speed;
extern int Run_Speed;
extern int Serach_Speed;                      //230
extern int Attack_Speed;
extern int BeAttacked_Speed;


extern char Sway_Buttle;

extern char BeHurt_10s;


extern enum Chassis_mode Chassis_Mode;
extern int Enemy_Danger_Level[8];          //0,������Ӣ�ۣ����̣��������������ڱ�
extern int Formar_Place;                  //����Ŀ�����ܵ��˺�ʱ��λ��
extern int Position_Collet_Flag;          //λ�òɼ���־λ���ɼ���Ϊ1��δ�ɼ�Ϊ0
extern int8_t Chassis_Target_Direction;
extern short   Chassis_Speed;

void Chassis_Auto_Control(void);
void Chassis_Control(void);
void Chassis_Control_Task(void *pvParameters);


//ҡ�α�־λ
extern char Swing_Flag;



extern int Swing_i;
extern char Swing_Position;
extern char Danger_Coefficient;

extern float Current_Power;




#endif


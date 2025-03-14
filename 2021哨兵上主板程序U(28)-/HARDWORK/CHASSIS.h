#ifndef CHASSIS_H
#define CHASSIS_H
#include "sys.h"

/*
	3508电机逆时针转速度为正；
	轨道外径 590 ，内径 410 ，中间 500 ；
	装甲面向基地ID为0，另一侧为1；
	
	规定 
		  哨兵向 右 走速度为正 实际速度方向 Chassis_Direction = 1；，左 走为负  Chassis_Direction = -1；
		  目标速度方向为右，数值为正，Chassis_Target_Direction=1，为左为负，Chassis_Target_Direction=-1；
		  左极限值为 -1，右极限值为 1
		  202电机朝向基地侧
		  从基地侧看向轨道  左直轨--0  左弯道--1，中直轨--2，右弯道--3，右直轨--4
		  向右走电机202计圈数增加
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
extern int Enemy_Danger_Level[8];          //0,步兵，英雄，工程，步兵，步兵，哨兵
extern int Formar_Place;                  //锁定目标与受到伤害时的位置
extern int Position_Collet_Flag;          //位置采集标志位，采集完为1，未采集为0
extern int8_t Chassis_Target_Direction;
extern short   Chassis_Speed;

void Chassis_Auto_Control(void);
void Chassis_Control(void);
void Chassis_Control_Task(void *pvParameters);


//摇晃标志位
extern char Swing_Flag;



extern int Swing_i;
extern char Swing_Position;
extern char Danger_Coefficient;

extern float Current_Power;




#endif


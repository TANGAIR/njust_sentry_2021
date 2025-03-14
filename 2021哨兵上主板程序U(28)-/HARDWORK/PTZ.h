#ifndef PTZ_H
#define PTZ_H
#include "sys.h" 
//p轴电机速度向上为负，向下为正  角度  向下增大，向上减少

//P轴总的旋转角度  从最高位置到垂直位置2111 + 40297  =  42408

//Y轴旋转的空间角度自底向上看逆时针增加，顺时针减小,总的机械角也是如此
/*********************************************结构体定义区**********************************************************************/
//云台结构体
typedef struct
{
	short Critical_Pitch_Mechanical_Angle;
	short Critical_Yaw_Mechanical_Angle;
   
	int   Initial_Pitch_Angle;
	short Initial_Yaw_Angle;
   
  short Yaw_Angle_Max_Change;
  short Pitch_Angle_Max;
  short Pitch_Angle_Min;
	
	short Calibrated_Flag;
}PTZ_TypeDef;

//云台模式共用体
enum PTZ_MODE
{
	PTZ_Locked,//PTZ锁住
	PTZ_Mocalun_Locked,//摩擦轮锁住
	PTZ_Find_Enemy,//发现敌人
	PTZ_Armor_0_Be_Attacked,//装甲0被攻击
	PTZ_Armor_1_Be_Attacked,//装甲1被攻击
	PTZ_Lost_Enemy_Within_5S,//丢失敌人在5秒内
	PTZ_Lost_Enemy//丢失敌人
};
/*********************************************函数声明区**********************************************************************/
void PTZ_Init(void);                        //云台初始化
void PTZ_Angle_Value_Init(void);            //云台角度值初始化(越界处理)
void PTZ_Angle_Init(void);                  //云台角度初始化

void PTZ_PID_Init(void);              			//云台PID参数初始化


void P_Swing(int Up_Limite,int Down_Limite,int Speed);
void Y_Swing(int Left_Limite,int Right_Limite,float Speed);



void PTZ_Control_Task(void *pvParameters);
/*********************************************数据定义域拓展区**********************************************************************/

extern enum PTZ_MODE  PTZ_Mode;
extern int 					  pitch;
extern int 			      yaw;
extern PTZ_TypeDef 		PTZ_Init_Value; 
extern int            Lost_Angle;

#endif

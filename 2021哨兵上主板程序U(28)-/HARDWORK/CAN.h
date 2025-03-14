#ifndef CAN_H
#define CAN_H
#include "sys.h"

typedef struct
{
	short Real_Speed;
	short Real_MechanicalAngle;
	short Origin_MechanicalAngle;
	short Target_Speed;
	short Target_MechanicalAngle;
}CAN1_Data_TypeDef;


#define CAN1_TRANSMIT_ID_PTZ                0x1FF  //RM6623 & RM2006 公用发送ID
#define CAN1_TRANSMIT_ID_TRAVEL             0x200
#define CAN1_FEEDBACK_ID_TRAVEL_1           0x201



#define CAN1_FEEDBACK_ID_203                0x203
#define CAN1_FEEDBACK_ID_204                0x204

#define CAN1_FEEDBACK_ID_Yaw                0x205
#define CAN1_FEEDBACK_ID_Pitch              0x206
#define CAN1_FEEDBACK_ID_SHOOT_DOWN         0x207
#define CAN1_FEEDBACK_ID_SHOOT_UP           0x208


extern int64_t Yaw_Real_MechanicalAngle;                  //yaw实际角度      从上往下看顺时针为正  逆时针·为负
extern float Yaw_Real_Angle;                              //电机朝向角 
extern int Pitch_Real_MechanicalAngle;                    //Pitch电机累加得到的机械角
extern float Yaw_Send_NUC_Angle;
extern int Pitch_Motor_206_Number;
extern int Pitch_Init_MechanicalAngle;


extern short   Chassis_Motor_201_Number;

extern int8_t  Chassis_Direction;    

extern CAN1_Data_TypeDef Chassis_Motor_201;
//	                     
extern CAN1_Data_TypeDef Friction_Wheel_203,
                         Friction_Wheel_204;



extern CAN1_Data_TypeDef  Pitch_Motor_206,
													Yaw_Motor_205,
												  Shoot_Motor_207,
									        Shoot_Motor_208;

void CAN1_Init(void);
void CAN1_TX_PTZ(void);
void CAN1_TX_Chassis(void);
void Can_Receive_Data_Task(void *pvParameters);




#endif

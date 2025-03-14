#ifndef PID_H
#define PID_H
#include "sys.h" 

typedef struct
{
	float P;
	float I;
	float D;
	float CurrentError;
	float LastError;
	float ErrorIgnored;
	float Pout;
	float Iout;
	float Dout;
	float PIDout;
	float PIDOutCompensate;
	float PMax;	
	float IMax;
	float DMax;
	float PIDOutMax;
	float PIDOutLast;
	float Target_Speed_Last;
	float Speed_Ratio;
	float Acceleration;
	float LastLastError;
	float Err_Change;              //误差变化量
	float Ke;                      //误差量化因子  Ke=N/emax  在这里N=3       e=Ke*e
	float Kec;                     //误差变化量量化因子
	float Ku_P;                    //Kp的缩放因子    Kp=Ku_P*Kp_M   Kp_M为根据模糊规则算出的在【-3,3】范围内的值  Ku=umax/N
	float Ku_I;                    //KI的缩放因子
	float Ku_D;                    //Kd的缩放因子
}PID_TypeDef;

extern PID_TypeDef PID_Yaw_205_Mechanical,PID_Yaw_205_Speed,PID_Pitch_206_Mechanical,PID_Pitch_206_Speed,PID_Pitch_206_Mechanical_1,PID_Pitch_206_Speed_1,PID_Yaw_205_Mechanical_1,PID_Yaw_205_Speed_1;
extern PID_TypeDef PID_Armor_Yaw_205_Mechanical,PID_Armor_Yaw_205_Speed,PID_Armor_Pitch_206_Mechanical,PID_Armor_Pitch_206_Speed;


extern PID_TypeDef PID_Friction_Wheel_203,
                   PID_Friction_Wheel_204;


void PID_Init(void);
float Pid_Calc(PID_TypeDef *PID,float Current_Speed,float Target_Speed);
float Pid_Poisition_Calc(PID_TypeDef *PID,float Current_Speed,float Target_Speed);
float Pid_Z_Calc(PID_TypeDef *PID,float Current_Speed,float Target_Speed);
float Pid_NUC(PID_TypeDef *PID,float error);



#endif

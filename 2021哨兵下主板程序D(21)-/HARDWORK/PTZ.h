#ifndef PTZ_H
#define PTZ_H
#include "sys.h" 
//p�����ٶ�����Ϊ��������Ϊ��  �Ƕ�  �����������ϼ���

//P���ܵ���ת�Ƕ�  �����λ�õ���ֱλ��2111 + 40297  =  42408

//Y����ת�Ŀռ�Ƕ��Ե����Ͽ���ʱ�����ӣ�˳ʱ���С,�ܵĻ�е��Ҳ�����
/*********************************************�ṹ�嶨����**********************************************************************/
//��̨�ṹ��
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
//��̨ģʽ������
enum PTZ_MODE
{
	PTZ_Locked,//PTZ��ס
	PTZ_Mocalun_Locked,//Ħ������ס
	PTZ_Find_Enemy,//���ֵ���
	PTZ_Armor_0_Be_Attacked,//װ��0������
	PTZ_Armor_1_Be_Attacked,//װ��1������
	PTZ_Lost_Enemy_Within_5S,//��ʧ������5����
	PTZ_Lost_Enemy//��ʧ����
};
/*********************************************����������**********************************************************************/



void P_Swing(int Up_Limite,int Down_Limite,int Speed);
void Y_Swing(float Speed);



void PTZ_Control_Task(void *pvParameters);
/*********************************************���ݶ�������չ��**********************************************************************/








extern enum PTZ_MODE  PTZ_Mode;
extern int 					  pitch;
extern float 			    yaw;
extern PTZ_TypeDef 		PTZ_Init_Value; 
extern int            Lost_Angle;

#endif

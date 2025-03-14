#include "main.h"


#define BULLET_SPEED 26


#define FRICTION_NORMAL_SPEED 7200

float Fricion_Speed_203=FRICTION_NORMAL_SPEED;
float Fricion_Speed_204=FRICTION_NORMAL_SPEED;



												
uint16_t  UP_LongTime_Locked_Times=0; 
uint16_t  UP_Locked_Time=0;
uint16_t  UP_Locked_Flag=1;	

uint16_t  LongTime_Locked_Times=0; 
uint16_t  Locked_Time=0;
uint16_t  Locked_Flag=1;
												
/***************************************����������***************************************/
/**
  * @brief  ���亯����ʼ��
  * @param  void
  * @retval void
  * @notes  void       
  */
void Shoot_Init(void)
{
	Friction_Wheel_203.Real_Speed=0;
	Friction_Wheel_204.Real_Speed=0;
	Friction_Wheel_203.Target_Speed=0;
	Friction_Wheel_204.Target_Speed=0;
	
}


/**
  * @brief  ����Ħ���ֿ�ʼ��ת
  * @param  void
  * @retval void 
  * @notes  ����Ҫ���ݲ���ϵͳ������Ϣ����ת�٣�
  */
void MoCaLun_Open_3508(void)     //1230   1241
{
	Friction_Wheel_203.Target_Speed=Pid_Calc(&PID_Friction_Wheel_203,Friction_Wheel_203.Real_Speed,Fricion_Speed_203);
	Friction_Wheel_204.Target_Speed=Pid_Calc(&PID_Friction_Wheel_204,Friction_Wheel_204.Real_Speed,-Fricion_Speed_204);
}

/**
  * @brief  �ر�Ħ����
  * @param  void
  * @retval void 
  * @notes  
  */
void MoCaLun_Close_3508(void)
{
  Friction_Wheel_203.Target_Speed=Pid_Calc(&PID_Friction_Wheel_203,Friction_Wheel_203.Real_Speed,0);
	Friction_Wheel_204.Target_Speed=Pid_Calc(&PID_Friction_Wheel_204,Friction_Wheel_204.Real_Speed,0);
}




/**
  * @brief  �������������
  * @param  void
  * @retval void 
* @notes  ���ȼ�4������2ms��CAN���ͺ�������̹���
  */
   //����̨���
	 uint16_t UP_Roll_Back_Time=0;             //��ת��ʱ
	 uint16_t UP_Dead_UP_Locked_Flag=0;        //��סʱ���ʱ
	 char UP_Direction_Flag=1;                 //1��ת   0��ת	
	 char UP_PokeMotor_Speed=80;               //�������ת��

   short UP_PokeMotor_Speed_2=0;




void SHOOT_Control_Task(void *pvParameters)
{ 
	static char Dead_Locked_Limit=250;               //��סʱ�伫��
	
	//����̨���
	static uint16_t Roll_Back_Time=0;          //��ת��ʱ
	static uint16_t Dead_Locked_Flag=0;        //��סʱ���ʱ
	static char Direction_Flag=1;              //1��ת   0��ת
	static char PokeMotor_Speed=80;            //�������ת
	
	
	Shoot_Init();     //Ħ���ֳ�ʼ��
  Severo_Init();    //�����ʼ��
	PID_Init_SHOOT(); //����PID��ʼ��
	vTaskDelay(1000);
	
	
 while(1)
 {
	/*******************************����̨***********************************/
	{
		/*Ħ���ֿ���*/
		if(DBUS.RC.Switch_Right==RC_SW_DOWN)//���Ҳದ�˲����·��������ֶ�����ģʽʱ
		{
			if((DBUS.RC.Switch_Left==RC_SW_MID)||(DBUS.RC.Switch_Left==RC_SW_DOWN))//��ದ�����м�ʱ����������
			{
				MoCaLun_Open_3508();//���ٶ���������������Ŀ���ٶ�
			}
			else //��ದ�����Ϸ�ʱ
			{
				MoCaLun_Close_3508();//�ر�Ħ����
		  }
		}
		else if(DBUS.RC.Switch_Right==RC_SW_UP)//���Ҳದ�˲����Ϸ�����������̨�ֶ�����ģʽʱ
		{
			MoCaLun_Close_3508();//�ر�Ħ����
		}
		
		else//û����ң��������ң�������˵��м�ʱ��Ħ����Ĭ�ϴ�
		{
			  //MoCaLun_Close_3508();//�ر�Ħ����
			MoCaLun_Open_3508();
		}
		
	 /*�����ٶ�����*/
		//����ֶ�����ģʽ
		if(DBUS.RC.Switch_Right==RC_SW_DOWN)
		{
			//�������ٶ�Ϊ150����ÿ��20��
			UP_PokeMotor_Speed=120;
		}
		//����Զ�ģʽ
		else if(DBUS.RC.Switch_Right==RC_SW_MID)
		{
			
			//�з�����С��2000MM
			if(Up_NUC_Data_TypeStruct.Enemy_distance<5000)
			{
				//�������ٶ�Ϊ150����ÿ��20��
				UP_PokeMotor_Speed=120;
			}
			//�з�����С��55
			else if(Up_NUC_Data_TypeStruct.Enemy_distance<7000)
			{
				//�������ٶ�Ϊ90����ÿ��15��
				UP_PokeMotor_Speed=90;
			}
			//�з�����С��15000mm
			else if(Up_NUC_Data_TypeStruct.Enemy_distance<15000)
			{
				//�������ٶ�Ϊ90����ÿ��10��
				UP_PokeMotor_Speed=60;
			}
			//�з��������100��������
			else UP_PokeMotor_Speed=0;
			
		
			
		}
		//����Ҳದ������
		else if(DBUS.RC.Switch_Right==RC_SW_UP)
		{
			//�������ٶ�Ϊ90����ÿ��10��
			UP_PokeMotor_Speed=0;
		}
		
		UP_PokeMotor_Speed_2=36*UP_PokeMotor_Speed;
		
   /*�ϲ������϶������*/
		//��ң�����Ҳದ�����·�ʱ
		if(DBUS.RC.Switch_Right==RC_SW_DOWN)
		{
			//�������������ң������ದ�����·���û�п���
			if((UP_Heat_Allow_Flag)&&(DBUS.RC.Switch_Left==RC_SW_DOWN)&&UP_Locked_Flag)
			//if((DBUS.RC.Switch_Left==RC_SW_DOWN)&&UP_Locked_Flag)
			{	
				//�������ת			
				if(UP_Direction_Flag)
				{
          //�򿪶��
					 Shoot_Allow();
					//��������ת
					Shoot_Motor_208.Target_Speed=Pid_Calc(&PID_Shoot_208,Shoot_Motor_208.Real_Speed,UP_PokeMotor_Speed);  
				}
				//����Ƿ�ת
				else
				{
					//��תʱ������
					UP_Roll_Back_Time++;
					//�����ת����80ms
					if(UP_Roll_Back_Time>=80)
					{
						//����������ʱ������
						UP_Locked_Time++;
//					  //�򿪶��
//					  Shoot_Allow();
						//��תʱ�����
						UP_Roll_Back_Time=0;
						//����ת��Ϊ��ת
						UP_Direction_Flag=!UP_Direction_Flag;
					}
					//�����̷�ת
					Shoot_Motor_208.Target_Speed=Pid_Calc(&PID_Shoot_208,Shoot_Motor_208.Real_Speed,-UP_PokeMotor_Speed); 
				}
				
				//�����̵���ٶȾ���ֵС��20	,PID�����õ��ٶ���200/36		
				if(abs(Shoot_Motor_208.Real_Speed<200))
				{
					//������־����
					UP_Dead_UP_Locked_Flag++;
					//�������ʱ������������Ƽ�250ms
					if(UP_Dead_UP_Locked_Flag>Dead_Locked_Limit)
					{
						//ת���־λȡ��
						UP_Direction_Flag=!UP_Direction_Flag;
						//������־λΪ0
						UP_Dead_UP_Locked_Flag=0;
					}  
				}
				//�����̵���ٶȾ���ֵ��С��200	
				else UP_Dead_UP_Locked_Flag=0;
			 }
		 //��������������������ң������ದ�˲����·�����û�п�����
		 else 
		 {
       //�رն��
			 Shoot_Forbid();
			 //�����̵��ͣת
			 PID_Shoot_208.Iout=0;
			 Shoot_Motor_208.Target_Speed=Pid_Calc(&PID_Shoot_208,Shoot_Motor_208.Real_Speed,0);
		 }
		 
		}
		
		//����Ҳದ�����м�
		else if(DBUS.RC.Switch_Right==RC_SW_MID)
		{
			//���Զ�ģʽ����Ҫ�����Ҫ
			//����ϵͳ��������
			//����ϵͳ������ʼ��־λ��
			//ң��������
			//NUC����
			//�Լ�Ӳ��û�жµ�
			//������������NUC��������ң������ದ�����·���û�ж�ת�ң��������������
			
			if(UP_Heat_Allow_Flag && Up_NUC_Data_TypeStruct.NUC_Shoot_Allow_Flag && (DBUS.RC.Switch_Left==RC_SW_DOWN)&&UP_Locked_Flag)//&&Game_Allow_Shoot_Flag)
			{ 
				//�򿪶��
				 Shoot_Allow();
				//�������ת			
				if(UP_Direction_Flag)
				{

					//��������ת
					Shoot_Motor_208.Target_Speed=Pid_Calc(&PID_Shoot_208,Shoot_Motor_208.Real_Speed,UP_PokeMotor_Speed);  
				}
				//����Ƿ�ת
				else
				{
					//��תʱ������
					UP_Roll_Back_Time++;
					//�����ת����80ms
					if(UP_Roll_Back_Time>=80)
					{
						//����������ʱ������
						UP_Locked_Time++;
						//�򿪶��
//					  Shoot_Allow();
						//��תʱ�����
						UP_Roll_Back_Time=0;
						//����ת��Ϊ��ת
						UP_Direction_Flag=!UP_Direction_Flag;
					}
					//�����̷�ת
					Shoot_Motor_208.Target_Speed=Pid_Calc(&PID_Shoot_208,Shoot_Motor_208.Real_Speed,-UP_PokeMotor_Speed); 
				}
				
				//�����̵���ٶȾ���ֵС��30			
				if(abs(Shoot_Motor_208.Real_Speed<200))
				{
					//������־����
					UP_Dead_UP_Locked_Flag++;
					//�������ʱ������������Ƽ�250ms
					if(UP_Dead_UP_Locked_Flag>Dead_Locked_Limit)
					{
						//ת���־λȡ��
						UP_Direction_Flag=!UP_Direction_Flag;
						//������־λΪ0
						UP_Dead_UP_Locked_Flag=0;
					}  
				}
				//�����̵���ٶȾ���ֵ��С��200	
				else UP_Dead_UP_Locked_Flag=0;
			 }
		 //��������������������ң������ದ�˲����·�����û�п�����
		 else 
		 {
       //�رն��
			 Shoot_Forbid();
			 //�����̵��ͣת
			 PID_Shoot_208.Iout=0;
			 Shoot_Motor_208.Target_Speed=Pid_Calc(&PID_Shoot_208,Shoot_Motor_208.Real_Speed,0);
		 } 
		}
		
		//������û��ң�������Ƶ����,�����Ҳದ�����Ϸ�
		else 
		{
			//�ܷ��������㣬�ڲ���ϵͳ��Ϣ��ȡ�����е���
		
			//�رն��
			Shoot_Forbid();
			//����ͣת
			//PID_Shoot_208.Iout=0;
			Shoot_Motor_208.Target_Speed=Pid_Calc(&PID_Shoot_208,Shoot_Motor_208.Real_Speed,0);
			
		}
	}	
	/*******************************����̨***********************************/
	{
		/*�����ٶ�����*/
		//����ֶ�����ģʽ
		if(DBUS.RC.Switch_Right==RC_SW_UP)
		{
			//�������ٶ�Ϊ150����ÿ��25��
			PokeMotor_Speed=120;
		}
		//����Զ�ģʽ
		else if(DBUS.RC.Switch_Right==RC_SW_MID)
		{
			//�з�����С��40
			if(Down_To_Up_TypeStruct.Distace<4000)
			{
				//�������ٶ�Ϊ150����ÿ��25��
				PokeMotor_Speed=120;
			}
			//�з�����С��55
			else if(Down_To_Up_TypeStruct.Distace<5500)
			{
				//�������ٶ�Ϊ90����ÿ��15��
				PokeMotor_Speed=90;
			}
			//�з�����С��65
			else if(Down_To_Up_TypeStruct.Distace<13000)
			{
				//�������ٶ�Ϊ90����ÿ��10��
				PokeMotor_Speed=60;
			}
			//�з��������65��������
			else PokeMotor_Speed=0;
		}
		//����Ҳದ������
		else if(DBUS.RC.Switch_Right==RC_SW_DOWN)
		{
			//�������ٶ�Ϊ90����ÿ��10��
			PokeMotor_Speed=60;
		}
		
		/*���̿������������뿨���ж�*/
		//��ң�����Ҳದ�����Ϸ�ʱ
		if(DBUS.RC.Switch_Right==RC_SW_UP)
		{
			//�ڷ�����Ŀû�дﵽ����ʱ����Heat_Allow_Flag��ʹ�ڱ����Լ�������
			Heat_Control();
			//�������������ң������ದ�����·���û�п���
			if((Heat_Allow_Flag)&&(DBUS.RC.Switch_Left==RC_SW_DOWN)&&Locked_Flag)
			{	
				//�������ת			
				if(Direction_Flag)
				{
					//����������������
					Up_To_Down_TypeStruct.Shoot_Allow_Flag=1;
					//��������ת
					Shoot_Motor_207.Target_Speed=Pid_Calc(&PID_Shoot_207,Shoot_Motor_207.Real_Speed,PokeMotor_Speed);  
				}
				//����Ƿ�ת
				else
				{
					//��תʱ������
					Roll_Back_Time++;
					//�����ת����80ms
					if(Roll_Back_Time>=80)
					{
						//����������ʱ������
						Locked_Time++;
						////����������������
						Up_To_Down_TypeStruct.Shoot_Allow_Flag=1;
						//��תʱ�����
						Roll_Back_Time=0;
						//����ת��Ϊ��ת
						Direction_Flag=!Direction_Flag;
					}
					//�����̷�ת
					Shoot_Motor_207.Target_Speed=Pid_Calc(&PID_Shoot_207,Shoot_Motor_207.Real_Speed,-PokeMotor_Speed); 
				}
				
				//�����̵���ٶȾ���ֵС��200			
				if(abs(Shoot_Motor_207.Real_Speed<200))
				{
					//������־����
					Dead_Locked_Flag++;
					//�������ʱ������������Ƽ�250ms
					if(Dead_Locked_Flag>Dead_Locked_Limit)
					{
						//ת���־λȡ��
						Direction_Flag=!Direction_Flag;
						//������־λΪ0
						Dead_Locked_Flag=0;
					}  
				}
				//�����̵���ٶȾ���ֵ��С��20	
				else Dead_Locked_Flag=0;
			 }
		 //��������������������ң������ದ�˲����·�����û�п�����
		 else 
		 {
			 //���������岻������
			 Up_To_Down_TypeStruct.Shoot_Allow_Flag=0;
			 //�����̵��ͣת
			 Shoot_Motor_207.Target_Speed=Pid_Calc(&PID_Shoot_207,Shoot_Motor_207.Real_Speed,0);
		 }
		}
		
		//����Ҳದ�����м�������·�
		else if( (DBUS.RC.Switch_Right==RC_SW_MID) || (DBUS.RC.Switch_Right==RC_SW_DOWN) )
		{
			//��������
			Heat_Control();
			//���Զ�ģʽ����Ҫ�����Ҫ
			//����ϵͳ��������
			//����ϵͳ������ʼ��־λ��
			//ң��������
			//NUC����
			//�Լ�Ӳ��û�жµ�
			//������������NUC��������ң������ದ�����·���û�ж�ת�ң�����������������Ҳದ�����·���
		
			if(Heat_Allow_Flag && Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag && (DBUS.RC.Switch_Left==RC_SW_DOWN)&&Locked_Flag)//&&(Game_Allow_Shoot_Flag||(DBUS.RC.Switch_Right==RC_SW_DOWN)))
			{ 
					//�������ת			
				if(Direction_Flag)
				{
					//����������������
					Up_To_Down_TypeStruct.Shoot_Allow_Flag=1;
					//��������ת
					Shoot_Motor_207.Target_Speed=Pid_Calc(&PID_Shoot_207,Shoot_Motor_207.Real_Speed,PokeMotor_Speed);  
				}
				//����Ƿ�ת
				else
				{
					//��תʱ������
					Roll_Back_Time++;
					//�����ת����80ms
					if(Roll_Back_Time>=80)
					{
						//����������ʱ������
						Locked_Time++;
						////����������������
						Up_To_Down_TypeStruct.Shoot_Allow_Flag=1;
						//��תʱ�����
						Roll_Back_Time=0;
						//����ת��Ϊ��ת
						Direction_Flag=!Direction_Flag;
					}
					//�����̷�ת
					Shoot_Motor_207.Target_Speed=Pid_Calc(&PID_Shoot_207,Shoot_Motor_207.Real_Speed,-PokeMotor_Speed); 
				}
				
				//�����̵���ٶȾ���ֵС��200/36		
				if(abs(Shoot_Motor_207.Real_Speed<100))
				{
					//������־����
					Dead_Locked_Flag++;
					//�������ʱ������������Ƽ�250ms
					if(Dead_Locked_Flag>Dead_Locked_Limit)
					{
						//ת���־λȡ��
						Direction_Flag=!Direction_Flag;
						//������־λΪ0
						Dead_Locked_Flag=0;
					}  
				}
				//�����̵���ٶȾ���ֵ��С��200	
				else Dead_Locked_Flag=0;
			 }
		 //��������������������ң������ದ�˲����·�����û�п�����
		  else 
		  {
			 //���������岻������
			 Up_To_Down_TypeStruct.Shoot_Allow_Flag=0;
				
				
			 //�����̵��ͣת
			 Shoot_Motor_207.Target_Speed=Pid_Calc(&PID_Shoot_207,Shoot_Motor_207.Real_Speed,0);
		  }
		}
		//������û��ң�������Ƶ������
		else 
		{
			//�ܷ��������㣬�ڲ���ϵͳ��Ϣ��ȡ�����е���
			number_1=0;
			//�������°巢��
			Up_To_Down_TypeStruct.Shoot_Allow_Flag=0;
			//����ͣת
			Shoot_Motor_207.Target_Speed=Pid_Calc(&PID_Shoot_207,Shoot_Motor_207.Real_Speed,0);
		}
	}
	 CAN1_TX_PTZ();	
	 vTaskDelay(2);
 }	
}







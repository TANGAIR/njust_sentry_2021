#include "main.h"

#define BULLET_SPEED 28

#define FRICTION_NORMAL_SPEED  7400

float Fricion_Speed_203= FRICTION_NORMAL_SPEED;
float Fricion_Speed_204= FRICTION_NORMAL_SPEED;

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
	CAN1_TX_Friction();
	
}

/**
  * @brief  ����Ħ���ֿ�ʼ��ת
  * @param  void
  * @retval void 
  * @notes  ����Ҫ���ݲ���ϵͳ������Ϣ����ת�٣�
  */
  
void MoCaLun_Open_3508(void)     //1230   1241
{
	
	Friction_Wheel_203.Target_Speed=Pid_Calc(&PID_Friction_Wheel_203,Friction_Wheel_203.Real_Speed,-Fricion_Speed_203);
	Friction_Wheel_204.Target_Speed=Pid_Calc(&PID_Friction_Wheel_204,Friction_Wheel_204.Real_Speed,Fricion_Speed_204);
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
  * @brief  Ħ���ֿ���������
  * @param  void
  * @retval void 
  * @notes  Ħ���ֳ�ʼ��PD13,TIM4_CH2 PD14,TIM4_CH3
            �����ʼ��PD15,TIM4_CH4
            �������ȼ�Ϊ2�����������������1ms,
  */
void MoCaLun_Control_Task(void *pvParameters)
{
	Shoot_Init();//Ħ���ֳ�ʼ����PWM=400,OUT=400��min=1000,max=1400,
	Severo_Init();//�����ʼ��
	vTaskDelay(100);	
	
	int Forbid_Time=0;
	
	while(1)
	{
		/*�������*/
		if(Up_To_Down_TypeStruct.Shoot_Allow_Flag==1)//�ϰ���������־λ
		{
			Forbid_Time=0;//�ùر�ʱ��Ϊ0
			Shoot_Allow();
		}
		else if( Up_To_Down_TypeStruct.Shoot_Allow_Flag==0)
		{
			if(Forbid_Time<3000)//���ر�ʱ��С��3��ʱ���صý�һ��
			{
				Forbid_Time++;
				Shoot_Forbid1();
			}
			else 
			Shoot_Forbid();//����ص���һ��
		}
		
		
		/*Ħ���ֿ���*/
		if(DBUS.RC.Switch_Right==RC_SW_UP)//���Ҳದ�˲����Ϸ��������ֶ�����ģʽʱ
		{
			if((DBUS.RC.Switch_Left==RC_SW_MID)||(DBUS.RC.Switch_Left==RC_SW_DOWN))//��ದ�����м�ʱ������ದ�˲����·�ʱ
			{			
				MoCaLun_Open_3508();//���ٶ���������������Ŀ���ٶ�
			}
			else //��ದ�����Ϸ�ʱ
			{			
				MoCaLun_Close_3508();//�ر�Ħ����
		  }
		}
		
		else if(DBUS.RC.Switch_Right==RC_SW_DOWN)//���Ҳದ�˲����·�����������̨�ֶ�����ģʽʱ
		{			
			MoCaLun_Close_3508();//�ر�Ħ����
		}
		
		else //û����ң��������ң�����������м�ʱ��Ħ����Ĭ�ϴ�
		{	
			//	MoCaLun_Close_3508();//�ر�Ħ����
			   MoCaLun_Open_3508();//
		}	
		//Ħ���ֵ��can����
		CAN1_TX_Friction();
		
	  	vTaskDelay(2);
	}
}
































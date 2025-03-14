#include "main.h"
/*************************���ݶ�����**********************/
PTZ_TypeDef 		PTZ_Init_Value;            //��̨��ʼ��Ϣ�ṹ�����     
enum PTZ_MODE   PTZ_Mode=PTZ_Lost_Enemy;

int Lost_Angle=0;



//��̨���Ʊ���
int 			pitch=0;
float 		yaw=0;
//PITCH���λ���޶�
int PITCH_UP    = 4600;  //1439
int PITCH_DOWN  = 3600;
/*************************����������**********************/
/**
  * @brief  ��̨YAW��ʼ������
  * @param  void
  * @retval void
  * @notes  ������̨��ʼλ�ã�ʹ��˫�����ƣ���2000�ڽ���̨��������ʼλ�ã�Ȼ�����ڿ��Ƶ���������
            pitch��yaw����Ϊ��ʼλ�ö�Ӧ������
  */
void PTZ_Init_Yaw(void)
{
	int 				   Current_Yaw_Angle=0;
		              
	short					 Angle_Init_Num=0;
	

	
	 PTZ_Init_Value.Initial_Yaw_Angle   =6672;//182
	
	 vTaskDelay(200);
	
	if(Yaw_Motor_205.Real_MechanicalAngle<=2200)
	{
	 PTZ_Init_Value.Initial_Yaw_Angle  -=8192;//182
	}
  
	
	
	Current_Yaw_Angle   =   Yaw_Real_MechanicalAngle;
	
 
	
	//��̨������������������̨��������ʼ�趨λ��
  for (Angle_Init_Num=0;Angle_Init_Num<1000;Angle_Init_Num++)
  {		
		
  //YAW�������ÿ2ms�仯һ�㣬�𽥻ص���ʼλ�� 
	 if ( Current_Yaw_Angle < (PTZ_Init_Value.Initial_Yaw_Angle -6))       Current_Yaw_Angle +=7;
	 if ( Current_Yaw_Angle > (PTZ_Init_Value.Initial_Yaw_Angle +6))       Current_Yaw_Angle -=7;
	
	 //�Ƕ�λ�û���Current_Yaw_Angle���������ĽǶȱȽ�
	 PID_Yaw_205_Mechanical.PIDout = (Pid_Calc(& PID_Yaw_205_Mechanical_1,Yaw_Real_MechanicalAngle,Current_Yaw_Angle));
	 //�ٶȻ���PID_Yaw_205_Mechanical.PIDout������������ʵ�ٶȱȽ�
	 Yaw_Motor_205.Target_Speed = (Pid_Calc(&PID_Yaw_205_Speed_1,Yaw_Motor_205.Real_Speed,PID_Yaw_205_Mechanical.PIDout ));


	 CAN1_TX_PTZ();
   vTaskDelay(2);
	 
  }
	
  //yaw���Ƴ�ʼֵ���ã�ʹ���������������������
	yaw=Yaw_Send_NUC_Angle_IMU;

}
/**
  * @brief  ��̨PITCH��ʼ������
  * @param  void
  * @retval void
  * @notes  ������̨��ʼλ�ã�ʹ��˫�����ƣ���2000�ڽ���̨��������ʼλ�ã�Ȼ�����ڿ��Ƶ���������
            pitch��yaw����Ϊ��ʼλ�ö�Ӧ������
  */
void PTZ_Init_PITCH(void)
{
	int 				   Current_Pitch_Angle;
	
	short					 Angle_Init_Num;
	
	
	//���ó�ʼ����λ��
	PTZ_Init_Value.Initial_Pitch_Angle =4000;//��Flash_Array[0];
	
	

  //������̨����
	Current_Pitch_Angle =Pitch_Real_MechanicalAngle;

	

	//��̨������������������̨��������ʼ�趨λ��
  for (Angle_Init_Num=0;Angle_Init_Num<1000;Angle_Init_Num++)
  {	
		
		
		
	//PITCH�������ÿ2ms�仯һ�㣬�𽥻ص���ʼλ��
	 if ( Current_Pitch_Angle < (PTZ_Init_Value.Initial_Pitch_Angle -6))       Current_Pitch_Angle +=7;
	 if ( Current_Pitch_Angle > (PTZ_Init_Value.Initial_Pitch_Angle +6))       Current_Pitch_Angle -=7;
	 //�Ƕ�λ�û���Current_Pitch_Angle���������ĽǶȱȽ�
	 PID_Pitch_206_Mechanical.PIDout = (Pid_Calc(& PID_Pitch_206_Mechanical_1,Pitch_Real_MechanicalAngle,Current_Pitch_Angle));
	 //�ٶȻ���PID_Pitch_206_Mechanical.PIDout������������ʵ�ٶȱȽ�
	 Pitch_Motor_206.Target_Speed = (Pid_Calc(&PID_Pitch_206_Speed_1,Pitch_Motor_206.Real_Speed,PID_Pitch_206_Mechanical.PIDout));
  


	 CAN1_TX_PTZ();
   vTaskDelay(2);
	 
  }
	
	//pitch���Ƴ�ʼֵ���ã�ʹ�����õĳ�ʼֵ������Խ�紦��֮�������
	pitch=PTZ_Init_Value.Initial_Pitch_Angle;
}


/**
  * @brief  ��̨ң�����ֶ�����
  * @param  void
  * @retval void
  * @notes  ֱ��ʹ��ң�����Ŀ������ݣ�ң����ԭʼ���ݷ�ΧΪ-600~600��
            ch2��ch3���������ֱ���yaw��pitch�Ļ����ϼӻ��߼�
            ch2����yaw�ᣬ���ݷ�ΧΪ-0.2~0.2���������¿�����ֵ˳ʱ��ת������ֵ��ʱ��ת����
            ch3����pitch�ᣬ���ݷ�Χ-7~7������pitch��̧ͷʱ����ķ����Ƕ�ֵ�ȵ�ͷ�Ƕ�ֵС
                            ����ch3��ֵʱ��̨��ͷ����ֵʱ��̨̧ͷ
  */
void PTZ_Control_Hand(void)                                       
{
	//ģ��ң����
	//��Ϊÿ�ν��뺯��ʱch���ᱻ���¸�ֵ�������Ƿ�̬����Ӱ��
	static float   ch2=0;
	static float   ch3=0;
	
	//����Ҳದ�ˣ����������·������м䣬��ȡң������Ϣ
	if(DBUS.RC.Switch_Right == RC_SW_UP||DBUS.RC.Switch_Right == RC_SW_MID)
	{
		ch2 = (float)DBUS.RC.ch2/3000;     //-0.2~0.2
		ch3 = DBUS.RC.ch3/80;            //-7~7
	}
	//����Ҳದ�ˣ��������·�����û���յ�ң������Ϣ�����ֲ���
	else
	{
	   ch2=0;
		 ch3=0;
	}
	
	yaw   += ch2; 
	pitch += ch3;
	
	
	if(pitch>PITCH_UP)pitch=PITCH_UP; 
	if(pitch<PITCH_DOWN)pitch=PITCH_DOWN;
	
	
	
	//YAW��ʹ�õĽǶȺͽ��ٶȶ�������������
  //yaw�Ƕ�λ�û���Yaw_Send_NUC_Angle_IMU��yaw�Ƚ�	
	PID_Yaw_205_Mechanical.PIDout = -(Pid_Calc(& PID_Yaw_205_Mechanical,Yaw_Send_NUC_Angle_IMU,yaw));
	//yaw�ٶȻ���IMU_Real_Data.Gyro_Y��PID_Yaw_205_Mechanical.PIDout�Ƚ�
	//PID��������Ĳ����ͷ���ֵ���Ǹ������ͣ����canͨѶ�ṹ������Ա��shout����
	Yaw_Motor_205.Target_Speed = (Pid_Calc(&PID_Yaw_205_Speed,-IMU_Real_Data.Gyro_Y, PID_Yaw_205_Mechanical.PIDout));


	//PITCH��ʹ�õĽǶȺͽ��ٶ��ǵ�������ĽǶȺ��ٶ�
  PID_Pitch_206_Mechanical.PIDout = (Pid_Calc(& PID_Pitch_206_Mechanical , Pitch_Real_MechanicalAngle , pitch));
	
	Pitch_Motor_206.Target_Speed =    (Pid_Calc(&PID_Pitch_206_Speed , Pitch_Motor_206.Real_Speed ,PID_Pitch_206_Mechanical.PIDout ));

}


/**
  * @brief  ��̨NUC����
  * @param  void
  * @retval void
  * @notes  ʹ���Ӿ������ݽ��п���
  */
void PTZ_Auto_No_Limit(void)                                                        //�Զ�����  �ҵ�Ŀ��
{ 	
	float   ch2=0;
	float   ch3=0;

  //ֱ������NUC���ݣ������뵱ǰ�ǶȵĲ�ֵ,�Ӿ�����short64555,-11~11,*400,NUC-4400~4400,
	ch2=((float)Down_NUC_Data_TypeStruct.Yaw_Angle)/100;//ch2�ķ�ΧӦ��-0.2~0.2
  //ֱ������NUC���ݣ������뵱ǰ�ǶȵĲ�ֵ,�Ӿ�����short64555,-7~7,*600,NUC-4200~4200,
	ch3=Down_NUC_Data_TypeStruct.Pitch_Angle/4;           //ch3�ķ�ΧӦ��-600~600����ֵ������
	
//	if(fabs(ch2)>=0.05f)//����
//	{
		yaw=Yaw_Send_NUC_Angle_IMU-ch2;	
		//��NUC����������������
		//Down_NUC_Data_TypeStruct.Yaw_Angle=0;
	//}
//	if(fabs(ch3)>=0.5)//����
//	{
		pitch	=Pitch_Real_MechanicalAngle-ch3;
		//��NUC����������������
		//Down_NUC_Data_TypeStruct.Pitch_Angle=0;
//	}
	
  if(pitch>PITCH_UP)pitch=PITCH_UP; 
	if(pitch<PITCH_DOWN)pitch=PITCH_DOWN;
	

	
  //YAW��ʹ�õĽǶȺͽ��ٶȶ�������������
  //yaw�Ƕ�λ�û���Yaw_Send_NUC_Angle_IMU��yaw�Ƚ�	
	PID_Yaw_205_Mechanical.PIDout = -(Pid_Calc(& PID_Armor_Yaw_205_Mechanical,Yaw_Send_NUC_Angle_IMU,yaw));
	
	Yaw_Motor_205.Target_Speed = (Pid_Calc(&PID_Armor_Yaw_205_Speed,-IMU_Real_Data.Gyro_Y, PID_Yaw_205_Mechanical.PIDout));
	
	
	PID_Pitch_206_Mechanical.PIDout = (Pid_Calc(& PID_Armor_Pitch_206_Mechanical , Pitch_Real_MechanicalAngle , pitch));
	
	Pitch_Motor_206.Target_Speed = (Pid_Calc(&PID_Armor_Pitch_206_Speed , Pitch_Motor_206.Real_Speed ,PID_Pitch_206_Mechanical.PIDout  ));

}
/**
  * @brief  ��̨����������
  * @param  void
  * @retval void
  * @notes  ��CAN1,PID,�����ǣ���̨���г�ʼ������ʼ��֮���������
            ���ȼ�Ϊ3��ÿ1ms����һ�Σ�����ʱ�õ�������ģʽ��ÿ1ms��ʱ��仯��С��ֵ
            ������̨����������Ҫ�õ������ǵ������Լ��Ե�����в������������Ǻ�CAN�Լ�
            PIDӦ������̨֮ǰ����ʼ��
  */
void PTZ_Control_Task(void *pvParameters)
{

	CAN1_Init();      //CAN1ͨѶ��ʼ��
	vTaskDelay(300);  //����
	PID_Init();       //PID��ʼ��
	PTZ_Init_Yaw();
	IMU_Init();       //�����ǳ�ʼ�����������������ݻ�ȡ�жϣ���ȡ�����ǵ�����
	vTaskDelay(100);	//����
	PTZ_Init_PITCH();
	
//	Beep_ON();
//	delay_ms(300);
//	Beep_OFF();

	
	while(1)
	{
		if(DBUS.RC.Switch_Right == RC_SW_UP)//����Ҳದ�ˣ��������Ϸ�������̨�ֶ����ƣ��ɿأ�
		{
			PTZ_Control_Hand();	
		}
		else if(DBUS.RC.Switch_Right == RC_SW_MID)//����Ҳದ�ˣ��������м�
		{
			if(PTZ_Mode==PTZ_Find_Enemy)//���ֵ����Զ�����
			{
				PTZ_Auto_No_Limit();	
			}
			else//����Ӿ�ûʶ�𵽵���������̨�ڶ�
			{	
				//PTZ_Control_Hand();
				Y_Swing(0.08f);
				P_Swing(PITCH_UP-100,PITCH_DOWN+100,4);
			}

	  }
		else//����Ҳದ�ˣ��������·�����û��ң������Ϣ������̨�ֶ����ƣ����ɿأ�
		{
			PTZ_Control_Hand();		
		}
		//����̨��������
		CAN1_TX_PTZ();
		vTaskDelay(1);
	}
}
/**
  * @brief  PITCH���ͷ����
  * @param  Up_Limite����ͷ�����ޣ�һ���PITCH_UPҪ��һЩ
            Down_Limite����ͷ���½Ƕ����ƣ�һ���PITCH_DOWNСһЩ
            Speed����ͷ���ٶȣ�ÿ�����ƶ��ı�����ֵ��һ��Ϊ22��
                   ��̨���ƶ��ķ�Χ��С��5600������Լ2.5S���Դ�
                   �����ƶ������Ϸ���
  * @retval void
  * @notes  Ҫ�ı���̨pitch��ĵ�ͷ�ٶ�ֻ��Ҫ�ı�Speed��ֵ�Ϳ�����
  */
void P_Swing(int Up_Limite,int Down_Limite,int Speed)
{
	//�������൱���ֶ����Ƶ�ch3����speed��ֵΪ��ʱ��̨�����ƶ���ֵΪ��ʱ��̨�����ƶ�
	static int speed=6;
	
	//��������ת���㣬����������Ϊ����pitch��Ϊ��С����̨�����˶�
	if(Pitch_Real_MechanicalAngle>=Up_Limite) 
	{
		speed+=1;
	}
	//��������ת���㣬���������ݼ�Ϊ����pitch��Ϊ������̨�����˶�
	else if(Pitch_Real_MechanicalAngle<=Down_Limite) 
	{
		speed-=1;
	}
	//���Ƶ���ת�����Ǳ��ٵķ�Χ����ֹ�͵�ͷ
	if(speed<-Speed)     speed=-Speed;
	else if(speed>Speed) speed=Speed;
	
	//pitch��������
	pitch -= speed;
	
	if(pitch>PITCH_UP)pitch=PITCH_UP; 
	if(pitch<PITCH_DOWN)pitch=PITCH_DOWN;
	
	PID_Pitch_206_Mechanical.PIDout = (Pid_Calc(& PID_Pitch_206_Mechanical ,Pitch_Real_MechanicalAngle , pitch));
	Pitch_Motor_206.Target_Speed =    (Pid_Calc(&PID_Pitch_206_Speed , Pitch_Motor_206.Real_Speed ,PID_Pitch_206_Mechanical.PIDout));

}

/**
  * @brief  YAW����ת����
  * @param  Speed����̨��ת���ٶȣ���ΧΪ-0.2~0.2
  * @retval void
  * @notes  Ҫ�ı���̨yaw��ĵ�ͷ�ٶ�ֻ��Ҫ�ı�Speed��ֵ�Ϳ�����
  */

void Y_Swing(float Speed)
{
	yaw+=Speed;
	PID_Yaw_205_Mechanical.PIDout = -(Pid_Calc(& PID_Yaw_205_Mechanical,Yaw_Send_NUC_Angle_IMU,yaw));
	Yaw_Motor_205.Target_Speed = (Pid_Calc(&PID_Yaw_205_Speed,-IMU_Real_Data.Gyro_Y, PID_Yaw_205_Mechanical.PIDout));
}



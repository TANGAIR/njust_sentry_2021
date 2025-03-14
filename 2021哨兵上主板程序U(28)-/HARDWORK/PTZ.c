#include "main.h"
/*************************���ݶ�����**********************/
PTZ_TypeDef 		PTZ_Init_Value;            //��̨��ʼ��Ϣ�ṹ�����     
enum PTZ_MODE   PTZ_Mode=PTZ_Lost_Enemy;

int Lost_Angle=0;

//��̨���Ʊ���
int 			pitch=0;
int 		  yaw=0;






//PITCH���λ���޶�������̨������ֵ��С
short PITCH_UP=2200;//615
short PITCH_DOWN = 2660;  //
short PITCH_HIT = 2400 ;


#define YAW_MOVE_LIMIT  2000
//��ǰ��վ�����ң����ϼ���
#define YAW_HIT    6472


/*************************����������**********************/
/**
  * @brief  ��̨��ʼ������
  * @param  void
  * @retval void
  * @notes  ���ȶ���̨���з�Խ�紦��Ȼ�������̨����
  */
void PTZ_Init(void)
{
	
	 PTZ_Angle_Value_Init();
   vTaskDelay(3);
	 PTZ_Angle_Init();	
   vTaskDelay(3);
	
}

/**
  * @brief  ��̨��Խ�纯��
  * @param  void
  * @retval void
  * @notes  ������̨��ʼλ�ã�����̨���з�Խ�紦����������Ƶ������㱳��
  */
void PTZ_Angle_Value_Init(void)
{

	PTZ_Init_Value.Initial_Pitch_Angle =PITCH_HIT;//��Flash_Array[0];
	PTZ_Init_Value.Initial_Yaw_Angle   =6120;//182

  
//	//Pitch �Ƕȳ�ʼ��
//	if ( (PTZ_Init_Value.Initial_Pitch_Angle+4000)>8191 )    
//	{   
//		PTZ_Init_Value.Critical_Pitch_Mechanical_Angle = PTZ_Init_Value.Initial_Pitch_Angle +4000-8192;
//	}
//	else  
//	{   
//		PTZ_Init_Value.Critical_Pitch_Mechanical_Angle = PTZ_Init_Value.Initial_Pitch_Angle +4000;
//	}
//	
//	if (PTZ_Init_Value.Initial_Pitch_Angle < PTZ_Init_Value.Critical_Pitch_Mechanical_Angle)  
//	{
//		PTZ_Init_Value.Initial_Pitch_Angle +=8192;
//	}
	
}

/**
  * @brief  ��̨����
  * @param  void
  * @retval void
  * @notes  ʹ��˫�����ƣ���2000�ڽ���̨��������ʼλ�ã�Ȼ�����ڿ��Ƶ���������
            pitch��yaw����Ϊ��ʼλ�ö�Ӧ������
  */
void PTZ_Angle_Init(void)
{
	int 				   Current_Pitch_Angle,
		             Current_Yaw_Angle; 
	short					 Angle_Init_Num;

  //������̨����
	Current_Pitch_Angle =Pitch_Motor_206.Real_MechanicalAngle ;
	Current_Yaw_Angle   =Yaw_Real_MechanicalAngle;

	
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
		
		
		
	//PITCH�������ÿ2ms�仯һ�㣬�𽥻ص���ʼλ��
	 if ( Current_Pitch_Angle < (PTZ_Init_Value.Initial_Pitch_Angle -6))       Current_Pitch_Angle +=7;
	 if ( Current_Pitch_Angle > (PTZ_Init_Value.Initial_Pitch_Angle +6))       Current_Pitch_Angle -=7;
	



		//�Ƕ�λ�û���Current_Pitch_Angle���������ĽǶȱȽ�
	 PID_Pitch_206_Mechanical.PIDout = (Pid_Calc(& PID_Pitch_206_Mechanical_1,Pitch_Motor_206.Real_MechanicalAngle,Current_Pitch_Angle));
	
	//�ٶȻ���PID_Pitch_206_Mechanical.PIDout������������ʵ�ٶȱȽ�
	 Pitch_Motor_206.Target_Speed = (Pid_Calc(&PID_Pitch_206_Speed_1,Pitch_Motor_206.Real_Speed,PID_Pitch_206_Mechanical.PIDout));
  
 

	 CAN1_TX_PTZ();
   vTaskDelay(2);
  }
	
  //yaw���Ƴ�ʼֵ����
	yaw=Yaw_Real_MechanicalAngle;
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
	
	//����Ҳದ�ˣ��������·������м䣬��ȡң������Ϣ
	if(DBUS.RC.Switch_Right == RC_SW_DOWN||DBUS.RC.Switch_Right == RC_SW_MID)
	{
		ch2 = (float)DBUS.RC.ch2/80;     //-7~7
		ch3 = (float)DBUS.RC.ch3/100;            //-7~7
	}
	//����Ҳದ�ˣ��������Ϸ�����û���յ�ң������Ϣ�����ֲ���
	else
	{
	   ch2=0;
		 ch3=0;
	}
	
	yaw   += ch2; 
	pitch -= ch3;	
	
	//yaw��λ��yaw��ʹ�õ���Ȧ���ۼӱ���ֵ���㣬����Ҫת������㣬���Կ���ʹ��ԭʼ�ı�����ֵ
	if(yaw<PTZ_Init_Value.Initial_Yaw_Angle-YAW_MOVE_LIMIT)yaw=PTZ_Init_Value.Initial_Yaw_Angle-YAW_MOVE_LIMIT; 
	if(yaw>PTZ_Init_Value.Initial_Yaw_Angle+YAW_MOVE_LIMIT)yaw=PTZ_Init_Value.Initial_Yaw_Angle+YAW_MOVE_LIMIT;

	
	//pitch��λ������PITCH_UP��ֵ��PITCH_DOWN��ֵС��
	//������PITCH_UP�Ƚ�ʱҪ��С�ڣ���PITCH_DOWN�Ƚ�ʱҪ�ô���
	if(pitch<PITCH_UP)pitch=PITCH_UP; 
	if(pitch>PITCH_DOWN)pitch=PITCH_DOWN;
	
	
	//YAW��ʹ�õĽǶȺͽ��ٶȶ�������������
  //yaw�Ƕ�λ�û���Yaw_Real_MechanicalAngle��yaw�Ƚ�	
	PID_Yaw_205_Mechanical.PIDout = (Pid_Calc(& PID_Yaw_205_Mechanical,Yaw_Real_MechanicalAngle,yaw));
	//yaw�ٶȻ���Yaw_Motor_205.Real_Speed��PID_Yaw_205_Mechanical.PIDout�Ƚ�
	//PID��������Ĳ����ͷ���ֵ���Ǹ������ͣ����canͨѶ�ṹ������Ա��shout����
	Yaw_Motor_205.Target_Speed = (Pid_Calc(&PID_Yaw_205_Speed,Yaw_Motor_205.Real_Speed,PID_Yaw_205_Mechanical.PIDout));

	//PITCH��ʹ�õĽǶȺͽ��ٶ��ǵ�������ĽǶȺ��ٶ�
  PID_Pitch_206_Mechanical.PIDout = (Pid_Calc(& PID_Pitch_206_Mechanical , Pitch_Motor_206.Real_MechanicalAngle , pitch));
	Pitch_Motor_206.Target_Speed = (Pid_Calc(&PID_Pitch_206_Speed , Pitch_Motor_206.Real_Speed ,PID_Pitch_206_Mechanical.PIDout ));

}
//

/**
  * @brief  ��̨NUC����
  * @param  void
  * @retval void
  * @notes  ʹ���Ӿ������ݽ��п���
  */
float Yaw_Omiga_Last=0;
void PTZ_Auto_No_Limit(void)                                                        //�Զ�����  �ҵ�Ŀ��
{ 	
	float   ch2=0;
	float   ch3=0;

	//ֱ������NUC���ݣ������뵱ǰ�ǶȵĲ�ֵ,�Ӿ�����short64555,-11~11,*400,NUC-4400~4400,
	ch2=(float)Up_NUC_Data_TypeStruct.Yaw_Angle;//ch2�ķ�ΧӦ��-0.2~0.2

	
  //ֱ������NUC���ݣ������뵱ǰ�ǶȵĲ�ֵ,�Ӿ�����short64555,-7~7,*600,NUC-4200~4200,
	ch3=(float)Up_NUC_Data_TypeStruct.Pitch_Angle;           //ch3�ķ�ΧӦ��-600~600����ֵ������

	
//	if(fabs(ch2)>=1)//����
//	{
		yaw=Yaw_Real_MechanicalAngle-ch2;
		//��NUC����������������
		//����
		//Up_NUC_Data_TypeStruct.Yaw_Angle=0;
//	}
//	if(fabs(ch3)>=1)//����
//	{
		pitch	=Pitch_Motor_206.Real_MechanicalAngle+ch3;
		//��NUC����������������
		
		//Up_NUC_Data_TypeStruct.Pitch_Angle=0;
		
//	}
	
	//yaw��λ��yaw��ʹ�õ���Ȧ���ۼӱ���ֵ���㣬����Ҫת������㣬���Կ���ʹ��ԭʼ�ı�����ֵ
	if(yaw<PTZ_Init_Value.Initial_Yaw_Angle-YAW_MOVE_LIMIT)yaw=PTZ_Init_Value.Initial_Yaw_Angle-YAW_MOVE_LIMIT; 
	if(yaw>PTZ_Init_Value.Initial_Yaw_Angle+YAW_MOVE_LIMIT)yaw=PTZ_Init_Value.Initial_Yaw_Angle+YAW_MOVE_LIMIT;

	//pitch��λ������PITCH_UP��ֵ��PITCH_DOWN��ֵС��
	//������PITCH_UP�Ƚ�ʱҪ��С�ڣ���PITCH_DOWN�Ƚ�ʱҪ�ô���
	if(pitch<PITCH_UP)pitch=PITCH_UP; 
	if(pitch>PITCH_DOWN)pitch=PITCH_DOWN;

	
	
	//�ڽ���
//	if(Swing_Flag==1)
//	{
//		PID_Armor_Yaw_205_Mechanical.P=0.15;
//	
//	}
//	else
//	{
//		PID_Armor_Yaw_205_Mechanical.P=0.1;
//	}
	
	
	
	
  //YAW��ʹ�õĽǶȺͽ��ٶȶ�������������
  //yaw�Ƕ�λ�û���Yaw_Real_MechanicalAngle��yaw�Ƚ�	
	PID_Yaw_205_Mechanical.PIDout = (Pid_Calc(& PID_Armor_Yaw_205_Mechanical,Yaw_Real_MechanicalAngle,yaw));
	
//	if(fabs(Yaw_Omiga_Last-PID_Yaw_205_Mechanical.PIDout)>10)PID_Yaw_205_Mechanical.PIDout=(PID_Yaw_205_Mechanical.PIDout>Yaw_Omiga_Last)?(Yaw_Omiga_Last+10):(Yaw_Omiga_Last-10);
//	else Yaw_Omiga_Last=PID_Yaw_205_Mechanical.PIDout;
		
	
	Yaw_Motor_205.Target_Speed = (Pid_Calc(&PID_Armor_Yaw_205_Speed,Yaw_Motor_205.Real_Speed, PID_Yaw_205_Mechanical.PIDout));
	
	
	PID_Pitch_206_Mechanical.PIDout = (Pid_Calc(& PID_Armor_Pitch_206_Mechanical , Pitch_Motor_206.Real_MechanicalAngle , pitch));
	Pitch_Motor_206.Target_Speed = (Pid_Calc(&PID_Armor_Pitch_206_Speed , Pitch_Motor_206.Real_Speed ,PID_Pitch_206_Mechanical.PIDout  ));

}
/**
  * @brief  ��̨����������
  * @param  void
  * @retval void
  * @notes  ��CAN1,PID,�����ǣ���̨���г�ʼ������ʼ��֮���������
            ���ȼ�Ϊ3��ÿ1ms����һ�Σ�����ʱ�õ�������ģʽ��ÿ1ms��ʱ��仯��С��ֵ
            ������̨����������Ҫ�õ�CAN1�������Լ��Ե�����в�������CAN�Լ�
            PIDӦ������̨֮ǰ����ʼ��
  */

void PTZ_Control_Task(void *pvParameters)
{

	//const TickType_t TimeIncrement=pdMS_TO_TICKS(1);
	//TickType_t	PreviousWakeTime;
	//PreviousWakeTime=xTaskGetTickCount();	
	
	CAN1_Init();      //CAN1ͨѶ��ʼ��
	vTaskDelay(500); //����
	PID_Init_PTZ();   //PID��ʼ��
	PTZ_Init();       //��̨��������ʼ��yaw=Yaw_Real_MechanicalAngle��pitch  
	Beep_ON();
	delay_ms(500);
	Beep_OFF();
 
	
	while(1)
	{
		
		
		
		if(DBUS.RC.Switch_Right == RC_SW_UP)//����Ҳದ�ˣ��������Ϸ�������̨�ֶ����ƣ����ɿأ�
		{
			PTZ_Control_Hand();	
		}
		else if(DBUS.RC.Switch_Right == RC_SW_MID)//����Ҳದ�ˣ��������м�
		{
			

		
				if(PTZ_Mode==PTZ_Find_Enemy)//���ֵ����Զ�����
				{
					if(Chassis_Mode>Chassis_Up_Find_Target)
					{
					  Chassis_Mode=Chassis_Up_Find_Target;
					  Mode_Time=0;
					}
					
					PTZ_Auto_No_Limit();	
				}
				else//���ԣ�����Ӿ�ûʶ�𵽵�����ҡ��
				{	
					//����
					//PTZ_Control_Hand();	
					Y_Swing(Yaw_Real_MechanicalAngle+1000,Yaw_Real_MechanicalAngle-1000,2);
					P_Swing(PITCH_UP+50,PITCH_DOWN-50,3);
				}	



	
			
	  }
		else//����Ҳದ�ˣ��������·�����û���յ�ң������Ϣ������̨�ֶ����ƣ��ɿأ�
		{
			PTZ_Control_Hand();	
		}
		//����̨��������
		CAN1_TX_PTZ();  
		vTaskDelay(1);
		//vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}
}
/**
  * @brief  PITCH���ͷ����
  * @param  Up_Limite����ͷ�����ޣ�һ���PITCH_UPҪСһЩ
            Down_Limite����ͷ���½Ƕ����ƣ�һ���PITCH_DOWN��һЩ
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
	
	//��������ת���㣬�������ݼ�Ϊ����pitch��Ϊ���ӣ���̨�����˶�
	if(Pitch_Motor_206.Real_MechanicalAngle<=Up_Limite) 
	{
		speed-=1;
	}
	//��������ת���㣬����������Ϊ����pitch��Ϊ��С����̨�����˶�
	else if(Pitch_Motor_206.Real_MechanicalAngle>=Down_Limite) 
	{
		speed+=1;
	}
	//���Ƶ���ת�����Ǳ��ٵķ�Χ����ֹ�͵�ͷ
	if(speed<-Speed)     speed=-Speed;
	else if(speed>Speed) speed=Speed;
	
	//pitch��ȥ����
	pitch -= speed;
	//��λ
//	if(pitch<PITCH_UP)pitch=PITCH_UP; 
//	if(pitch>PITCH_DOWN)pitch=PITCH_DOWN;
	
	PID_Pitch_206_Mechanical.PIDout = (Pid_Calc(& PID_Pitch_206_Mechanical , Pitch_Motor_206.Real_MechanicalAngle , pitch));
	Pitch_Motor_206.Target_Speed = (Pid_Calc(&PID_Pitch_206_Speed , Pitch_Motor_206.Real_Speed ,PID_Pitch_206_Mechanical.PIDout ));

}

/**
  * @brief  ����̨YAW��ҡͷ����
  * @param  Speed����̨��ת���ٶȣ���ΧΪ-0.2~0.2
  * @retval void
  * @notes  ���ֵ���Ҳ�ֵС
  */
void Y_Swing(int Left_Limite,int Right_Limite,float Speed)
{
	//�������൱���ֶ����Ƶ�ch3����speed��ֵΪ��ʱ��̨�����ƶ���ֵΪ��ʱ��̨�����ƶ�
	static int speed=6;
	
	//��������ת���㣬�������ݼ�Ϊ����yaw��Ϊ��С����̨�����ƶ�
	if(Yaw_Real_MechanicalAngle>=PTZ_Init_Value.Initial_Yaw_Angle+YAW_MOVE_LIMIT) 
	{
		speed-=1;
	}
	//��������ת���㣬����������Ϊ����yaw��Ϊ���ӣ���̨�����˶�
	else if(Yaw_Real_MechanicalAngle<=PTZ_Init_Value.Initial_Yaw_Angle-YAW_MOVE_LIMIT) 
	{
		speed+=1;
	}
	
	//���Ƶ���ת�����Ǳ��ٵķ�Χ����ֹҡͷ
	if(speed<-Speed)     speed=-Speed;
	else if(speed>Speed) speed=Speed;
	
	//yaw��������
   yaw += speed;
	
	PID_Yaw_205_Mechanical.PIDout = (Pid_Calc(& PID_Yaw_205_Mechanical,Yaw_Real_MechanicalAngle,yaw));
	Yaw_Motor_205.Target_Speed = (Pid_Calc(&PID_Yaw_205_Speed,Yaw_Motor_205.Real_Speed, PID_Yaw_205_Mechanical.PIDout));
	
}



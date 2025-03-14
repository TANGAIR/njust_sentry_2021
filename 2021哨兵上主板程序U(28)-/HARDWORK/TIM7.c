#include "main.h"


int Danger_Time=0;                //�ܵ��˺���ʱ����㣬10s��û�ܵ��˺�����0��Up_To_Down_TypeStruct.Get_Hit_flag=2,���2S������װ��ͬʱ�ܵ�������Up_To_Down_TypeStruct.Get_Hit_flag=3
int Mode_Time=0;	                //ģʽ����ʱ�� 									
int Position_Collet_Time=0;       //λ�òɼ�����ʱ��
int Data_Return_Times=1;
u32 TimeTick1ms = 0;

//��һ�ε�������
float Last_Bullet_Speed=0;
//ÿ100ms����ĵ�����Ŀ
char Shoot_Num=0;
//�������������ÿ100ms�����跢������
short Shoot_Limit_Num=0;
//��ǰʣ������
short Current_Heat=0;
//�������
uint16_t Max_Heat=300;
//���������־λ 1--������     0����������������
char Heat_Allow_Flag=1;   
//���������־λ 1--������     0����������������
char UP_Heat_Allow_Flag=1;  
//�������±�־λ0��1  һ�θı����һ�θ���
char Update_Flag=0; 
//��һ���������±�־λ
char Last_Update_Flag=0;
//�����ٶ�����
float Bullet_Speed_Limit=30.0f;
//����ƽ���ٶ�
float Bullet_Average_Speed=20.0f;


/**
  * @brief  ��ʱ��7��ʼ������
  * @param  void
  * @retval void
  * @notes  �жϼ��Ϊ0.01ms * arr �����ȼ�Ϊ 6
  */
void Tick_TIM7_Init (int arr)  
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);  ///ʹ��TIM7ʱ��
	
   TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=840 -1 ;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);//��ʼ��TIM7
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE); //����ʱ��7�����ж�
	TIM_Cmd(TIM7,ENABLE); //ʹ�ܶ�ʱ��7

	NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; //��ʱ��7�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6; //��ռ���ȼ�6
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;        //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}






/*�����ж�*/
/**
  * @brief  ����̨�������ƺ���
  * @param  void
  * @retval void
  * @notes  ����̨Ϊ 1 �� 17mm ǹ��
            �ڷ�����������е��ã�ÿ2msִ��һ��
            �õ����ٶȵĸ��������㷢����Ŀ
            �ڷ�����Ŀû�дﵽ����ʱ����Heat_Allow_Flag��ʹ�ڱ����Լ�������
            ʵʱͳ�Ʊ�֤ÿ100ms���º󷢵������ᳬ���������޼���֤�ڱ�������Ϊ
            ����������Ѫ����������������ǹ���������¶����µ�
  */
void Heat_Control(void)
{
	//������ʱ��
	static int Allow_Time=0;
	
	//������±�־��������һ�θ��±�־λ��˵�������Ѿ�����
	if(Update_Flag!=Last_Update_Flag)
	{
		Shoot_Num=0;
	}
	//������η����ǹ��Ϊ0,ÿ�η���֮������
	if(ShootData.shooter_id==0)
	{
		//��NUC��������ͨ�Ŷ��õ�
		ShootData.shooter_id=2;
		Shoot_Num++;
	}
	//���������������
	if(Shoot_Num>=Shoot_Limit_Num)
	{
		//���������־Ϊ0
		Heat_Allow_Flag=0;
		//����ʱ����0
		Allow_Time=0;
	}
	//�������û�г�����󷢵�����
	else 
	{
		//������������־λΪ0
		if(!Heat_Allow_Flag)
		{
			//����ʱ������
			Allow_Time++;
		}
		//�������ʱ�����80
		if(Allow_Time>=80)
		{
			//�����������־λ��1
			Heat_Allow_Flag=1;
		}
	}
	//���¸��±�־λ
  Last_Update_Flag=Update_Flag;	
}
/**
  * @brief  �����жϺ���
  * @param  void
  * @retval void
  * @notes  �ڶ�ʱ��5�ж���ÿ100ms����һ��
            ���ݵ����ٶ����ƺ�ʣ���������÷�����������Shoot_Limit_Num
            ͬʱ���¸��±�־λ
            ��û�а�װ����ϵͳʱ��PowerHeatData.shootHeat0ʼ�յ���0����û����������
  */
void Heat_Judge(void)
{
	//��ǰʣ�������������������ȥǹ������
	Current_Heat=Max_Heat-PowerHeatData.shooter_id1_17mm_cooling_heat;
	//�����ǰ����С�ڵ����ٶ�����
	if(Current_Heat<=Bullet_Speed_Limit) 
	{
		//������������Ϊ0
		Shoot_Limit_Num=0;
	}
	//���������ǰ�������ڵ����ٶ�����
	//���������������ڵ�ǰ������ȥ�ٶ�����֮���ٳ���ƽ���ٶ�
	else Shoot_Limit_Num=(Current_Heat-Bullet_Speed_Limit)/Bullet_Average_Speed;
	//���¸��±�־λ
	Update_Flag=!Update_Flag;	
}




/**
  * @brief  ��ʱ��7�жϷ�����
  * @param  void
  * @retval void
  * @notes  �жϼ��Ϊ0.01ms * arr �����ȼ�Ϊ 6��1ms
            ÿ1ms����һ����������NUC�������ݺ�����ÿ10ms�ж�һ����̨״̬
            ÿ10ms����һ�δ���8���°巢����Ϣ�ĺ���
  */
void TIM7_IRQHandler(void)
{
//  static int Up_Shoot_Last_Time=0;
  static int Mode_Last_Time=0;
	BaseType_t pxHigherPriorityTaskWoken;
	if(TIM_GetITStatus(TIM7,TIM_IT_Update == SET))
   {
		//ϵͳ����ʱ������
		TimeTick1ms++;
		 
		 
	
		 //���������巢�͵�NUC������
	  vTaskNotifyGiveFromISR(UP_Send_NUC_Task_Handler,&pxHigherPriorityTaskWoken);
		
		 
		 
		 
		 
		//ÿ10ms
		if(TimeTick1ms%10==0)
		{			
			if( PTZ_Mode==PTZ_Locked)//����̨ģʽΪ����ģʽʱ
			{
				Mode_Last_Time++;
			}
			if(Mode_Last_Time>1500)//������ʱ�����15sʱ���ı���̨ģʽ
			{
				Mode_Last_Time=0;
				PTZ_Mode=PTZ_Lost_Enemy;
			}
			
			NUC_Lost_Time++;//NUCʧ��ʱ������
			
			if(NUC_Lost_Time>50)//��NUCʧ��ʱ�����500msʱ����������
			{
				Beep_ON();//NUC��ʧʱ����������
				//Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag=0;//�������巢�Ͳ������������Ϣ
				 //�رն��
			  // Shoot_Forbid();
				NUC_Lost_Flag_Music=1;
				if(PTZ_Mode==PTZ_Find_Enemy)//NUC��ʧģʽ��ǿ�Ʊ�Ϊ��ʧ����״̬
				{
					PTZ_Mode=PTZ_Lost_Enemy;
				}
			}
			else//NUC����ʱ�������ر�
			{
				NUC_Lost_Flag_Music=0;
				Beep_OFF();
			}
			//��������8��������
			vTaskNotifyGiveFromISR(USART8_Send_Task_Handler,&pxHigherPriorityTaskWoken);
			
		}
				
		
		
		
		
		//�����жϣ�ÿ100ms
		if(TimeTick1ms%100==0)
		{
			//����̨��������
			
		if(PowerHeatData.shooter_id2_17mm_cooling_heat<=300)UP_Heat_Allow_Flag=1;
	  else UP_Heat_Allow_Flag=0;
		
			
			
//			if(PowerHeatData.shooter_id1_17mm_cooling_heat<=300)Heat_Allow_Flag=1;
//	   else Heat_Allow_Flag=0;
//		
			
			
			//�����ж�
    Heat_Judge();
		
		
		
		}

		
		
		
		
		
    		
		if(TimeTick1ms%500==0)
		{
				//����λ������
			if((Chassis_Motor_201_Number<=FIRST_NOTE_NUM))
			{
				Chassis_Position=0;
			}
			else if((Chassis_Motor_201_Number>FIRST_NOTE_NUM)&&(Chassis_Motor_201_Number<SECOND_NOTE_NUM))
			{
				Chassis_Position=1;
			}
			else if(Chassis_Motor_201_Number<THIRD_NOTE_NUM)
			{
				Chassis_Position=2;
			}
			else if(Chassis_Motor_201_Number<FOURTH_NOTE_NUM)
			{
				Chassis_Position=3;
			}
			else
					Chassis_Position=4;

		}

		
		
		
    portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);		
 	}
   TIM_ClearITPendingBit (TIM7,TIM_IT_Update);
}






















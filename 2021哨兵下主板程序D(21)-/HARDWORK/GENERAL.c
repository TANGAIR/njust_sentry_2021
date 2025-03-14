#include "main.h"

int NUC_Lost_Time=0;

/**
  * @brief  ��ʱ��7��ʼ������
  * @param  void
  * @retval void
  * @notes  ��ʼ����ʱ�����������������жϲ���ʱ��Ϊ1ms���ж����ȼ�Ϊ6
  */
void Tick_TIM7_Init (int arr)   //���Ϊ0.01ms * arr��
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);  //ʹ��TIM7ʱ��
	
   TIM_TimeBaseInitStructure.TIM_Period = arr; 	       //�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=900 -1 ;     //��ʱ����Ƶ0.1mhz
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);//��ʼ��TIM7
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);          //����ʱ��7�����ж�
	TIM_Cmd(TIM7,ENABLE);                             //ʹ�ܶ�ʱ��7

	NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; //��ʱ��7�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6; //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0; //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}

/**
  * @brief  ��ʱ��7�жϷ�����
  * @param  void
  * @retval void
  * @notes  
  */
void TIM7_IRQHandler(void)
{
	static u32 TimeTick1ms = 0;
	static int Mode_Last_Time=0;
	
	BaseType_t pxHigherPriorityTaskWoken;
	
	if(TIM_GetITStatus(TIM7,TIM_IT_Update == SET))
   {
		TimeTick1ms++;//��������ʱ�����
		
		if(TimeTick1ms%10==0)//ÿ10ms����һ��
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
				NUC_Lost_Flag_Music=1;
				Beep_ON();//NUC��ʧʱ����������
				Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag=0;//�������巢�Ͳ������������Ϣ
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
		}
		//
		if(TimeTick1ms%10==0)
		{
			//���Ѵ���8��������
			vTaskNotifyGiveFromISR(USART8_Send_Task_Handler,&pxHigherPriorityTaskWoken);
		}		
 	}
   TIM_ClearITPendingBit (TIM7,TIM_IT_Update);
	 portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);//���������л�
}


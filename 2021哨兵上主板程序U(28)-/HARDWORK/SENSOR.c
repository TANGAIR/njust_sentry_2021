#include "main.h"


char Chassis_Position=0;          //��������202������Ϊ����Ϊ��
                                  //��������ݶ��ж�λ�� ACR_BYTE0_ADDRESS 
short Position_Limit_Flag=0;     //����-1���Ҽ���1

float Left_Ultrasonic_Distance=0;
float Right_Ultrasonic_Distance=0;
/**
  * @brief  ��������ʼ��
  * @param  void
  * @retval void
  * @notes 
  */
void Sensor_Init(void)
{
	Infrared_Sensor_Init();
	Ultrasonic_Awake_Init();
	TIM5_CH4_Cap_Init();//��������������ʼ��
	//TIM2_CH4_Cap_Init();
}


/**
  * @brief  ���⴫������ʼ��
  * @param  void
  * @retval void
  * @notes  ��PH11 C  ��PH12 B  L-�ڵ�Ϊ0��û��Ϊ1
  */
void Infrared_Sensor_Init(void)
{
   GPIO_InitTypeDef GPIO_InitTypeStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH,ENABLE);
	
	GPIO_InitTypeStruct.GPIO_Pin=GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitTypeStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitTypeStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitTypeStruct.GPIO_Speed=GPIO_High_Speed;
	GPIO_InitTypeStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_Init(GPIOH,&GPIO_InitTypeStruct);

}

/**
  * @brief  �������������ߵ�ƽ������ų�ʼ������
  * @param  void
  * @retval void
   * @notes  ��೬����������PF1---I1  ÿ10ms����һ�Σ��ڴ���8��������
                         ����PI0  TIM5_CH4  ---A
             �Ҳ೬����������PF10---Q1  ÿ10ms����һ�Σ��ڴ���8��������
                         ����PA3  TIM2_CH4  ---V           
  */
void Ultrasonic_Awake_Init(void)
{
  GPIO_InitTypeDef GPIO_InitTypeStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	GPIO_InitTypeStruct.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_10;
	GPIO_InitTypeStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitTypeStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitTypeStruct.GPIO_Speed=GPIO_High_Speed;
	GPIO_InitTypeStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_Init(GPIOF,&GPIO_InitTypeStruct);

	GPIO_ResetBits(GPIOI,GPIO_Pin_2|GPIO_Pin_10 );
}


/**
  * @brief  ��೬���������������ʼ������
  * @param  void
  * @retval void
  * @notes  
  */
void TIM5_CH4_Cap_Init (void)   
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_ICInitTypeDef  TIM5_ICInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  //ʹ��TIM5ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE); 	//ʹ��PORTIʱ��	
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //GPIOI0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //����
	GPIO_Init(GPIOI,&GPIO_InitStructure); //��ʼ��PI0

	GPIO_PinAFConfig(GPIOI,GPIO_PinSource0,GPIO_AF_TIM5); //PA0����λ��ʱ��5
  
	
  TIM_TimeBaseInitStructure.TIM_Period = 0xffffffff; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=90-1;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);//��ʼ��TIM5
	
	//��ʼ��TIM5���벶�����
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	ѡ������� IC4ӳ�䵽TI4��
  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
  TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; //��ʱ��5�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC4,ENABLE); //����ʱ��5��������жϣ�ͨ���Ĳ�������ж�
 	TIM_Cmd(TIM5,ENABLE); //ʹ�ܶ�ʱ��5

}

/**
  * @brief  ��ʱ��5�жϷ����� 
  * @param  void
  * @retval void
  * @notes  �������
  */

void TIM5_IRQHandler(void)
{
	//�����½����л���־λ
  static char Down_Flag=0;
	//δ�����˲��ľ���
	static float Raw_Distance=0;
  
	//�����������ж�
	if(TIM_GetITStatus(TIM5,TIM_IT_Update == SET))
	{
	 
  }
	//������������ж�
	if(TIM_GetITStatus(TIM5,TIM_IT_CC4) == SET)
	{
		//�����ǰ�������ش���
		if(Down_Flag==0)
		{
			//����ʱ������ֵ��Ϊ0
	  	TIM5->CNT=0;
			//�����½����л���־λ
			Down_Flag=1;
			//�л�Ϊ�½���
			TIM_OC4PolarityConfig(TIM5,TIM_ICPolarity_Falling);
		}
		//�����ǰ���½��ش���
		else
		{
			//�������
			Raw_Distance=((float)((TIM_GetCapture4(TIM5)/2000.0f)*340.0f));
			//�˲�
			if((Raw_Distance>50)&&(Raw_Distance<2000))
		  //��ֵ
			Left_Ultrasonic_Distance=Raw_Distance;
			//�����½����л���־λ
		  Down_Flag=0;	
			//�л�Ϊ������
			TIM_OC4PolarityConfig(TIM5,TIM_ICPolarity_Rising);
	  }
		
  }
	//����жϱ�־λ
	 TIM_ClearITPendingBit (TIM5,TIM_IT_Update|TIM_IT_CC4);
}



/**
  * @brief  �Ҳ೬���������������ʼ������
  * @param  void
  * @retval void
  * @notes  
  */
void TIM2_CH4_Cap_Init (void)   
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_ICInitTypeDef  TIM2_ICInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  //ʹ��TIM2ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//ʹ��PORTIʱ��	
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //GPIOI0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PI0

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM2); //PA0����λ��ʱ��5
  
	
  TIM_TimeBaseInitStructure.TIM_Period = 0xffffffff; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=90-1;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//��ʼ��TIM2
	
	//��ʼ��TIM2���벶�����
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	ѡ������� IC4ӳ�䵽TI4��
  TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
  TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //��ʱ��5�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	
	TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC4,ENABLE); //����ʱ��5��������жϣ�ͨ���Ĳ�������ж�
 	TIM_Cmd(TIM2,ENABLE); //ʹ�ܶ�ʱ��5

}

/**
  * @brief  ��ʱ��2�жϷ����� 
  * @param  void
  * @retval void
  * @notes  �������
  */

void TIM2_IRQHandler(void)
{
	//�����½����л���־λ
    static char Down_Flag=0;
	//δ�����˲��ľ���
	static float Raw_Distance=0;
  
	//�����������ж�
	if(TIM_GetITStatus(TIM2,TIM_IT_Update == SET))
	{
	  
  }
	//������������ж�
	if(TIM_GetITStatus(TIM2,TIM_IT_CC4) == SET)
	{
		//�����ǰ�������ش���
		if(Down_Flag==0)
		{
			//����ʱ������ֵ��Ϊ0
	  	TIM2->CNT=0;
			//�����½����л���־λ
			Down_Flag=1;
			//�л�Ϊ�½���
			TIM_OC4PolarityConfig(TIM2,TIM_ICPolarity_Falling);
		}
		//�����ǰ���½��ش���
		else
		{
			//�������
			Raw_Distance=((float)((TIM_GetCapture4(TIM2)/2000.0f)*340.0f));
			//�˲�
			if((Raw_Distance>50)&&(Raw_Distance<2000))
		  //��ֵ
			Right_Ultrasonic_Distance=Raw_Distance;
			//�����½����л���־λ
		    Down_Flag=0;	
			//�л�Ϊ������
			TIM_OC4PolarityConfig(TIM2,TIM_ICPolarity_Rising);
	  }
		
  }
	//����жϱ�־λ
	 TIM_ClearITPendingBit (TIM2,TIM_IT_Update|TIM_IT_CC4);
}






































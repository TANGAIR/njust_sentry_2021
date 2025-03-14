#include "main.h"


char Chassis_Position=0;          //方向，正对202看，左为左，右为右
                                  //依靠左侧螺钉判断位置 ACR_BYTE0_ADDRESS 
short Position_Limit_Flag=0;     //左极限-1，右极限1

float Left_Ultrasonic_Distance=0;
float Right_Ultrasonic_Distance=0;
/**
  * @brief  传感器初始化
  * @param  void
  * @retval void
  * @notes 
  */
void Sensor_Init(void)
{
	Infrared_Sensor_Init();
	Ultrasonic_Awake_Init();
	TIM5_CH4_Cap_Init();//超声波传感器初始化
	//TIM2_CH4_Cap_Init();
}


/**
  * @brief  红外传感器初始化
  * @param  void
  * @retval void
  * @notes  左PH11 C  右PH12 B  L-遮挡为0，没挡为1
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
  * @brief  超声波传感器高电平输出引脚初始化函数
  * @param  void
  * @retval void
   * @notes  左侧超声波：激活PF1---I1  每10ms发送一次，在串口8发送任务
                         捕获PI0  TIM5_CH4  ---A
             右侧超声波：激活PF10---Q1  每10ms发送一次，在串口8发送任务
                         捕获PA3  TIM2_CH4  ---V           
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
  * @brief  左侧超声波传感器捕获初始化函数
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
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  //使能TIM5时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE); 	//使能PORTI时钟	
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //GPIOI0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //下拉
	GPIO_Init(GPIOI,&GPIO_InitStructure); //初始化PI0

	GPIO_PinAFConfig(GPIOI,GPIO_PinSource0,GPIO_AF_TIM5); //PA0复用位定时器5
  
	
  TIM_TimeBaseInitStructure.TIM_Period = 0xffffffff; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=90-1;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);//初始化TIM5
	
	//初始化TIM5输入捕获参数
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	选择输入端 IC4映射到TI4上
  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
  TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; //定时器5中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC4,ENABLE); //允许定时器5溢出更新中断，通道四捕获更新中断
 	TIM_Cmd(TIM5,ENABLE); //使能定时器5

}

/**
  * @brief  定时器5中断服务函数 
  * @param  void
  * @retval void
  * @notes  计算距离
  */

void TIM5_IRQHandler(void)
{
	//上升下降沿切换标志位
  static char Down_Flag=0;
	//未经过滤波的距离
	static float Raw_Distance=0;
  
	//如果触发溢出中断
	if(TIM_GetITStatus(TIM5,TIM_IT_Update == SET))
	{
	 
  }
	//如果触发捕获中断
	if(TIM_GetITStatus(TIM5,TIM_IT_CC4) == SET)
	{
		//如果当前是上升沿触发
		if(Down_Flag==0)
		{
			//将定时器计数值设为0
	  	TIM5->CNT=0;
			//上升下降沿切换标志位
			Down_Flag=1;
			//切换为下降沿
			TIM_OC4PolarityConfig(TIM5,TIM_ICPolarity_Falling);
		}
		//如果当前是下降沿触发
		else
		{
			//计算距离
			Raw_Distance=((float)((TIM_GetCapture4(TIM5)/2000.0f)*340.0f));
			//滤波
			if((Raw_Distance>50)&&(Raw_Distance<2000))
		  //赋值
			Left_Ultrasonic_Distance=Raw_Distance;
			//上升下降沿切换标志位
		  Down_Flag=0;	
			//切换为上升沿
			TIM_OC4PolarityConfig(TIM5,TIM_ICPolarity_Rising);
	  }
		
  }
	//清除中断标志位
	 TIM_ClearITPendingBit (TIM5,TIM_IT_Update|TIM_IT_CC4);
}



/**
  * @brief  右侧超声波传感器捕获初始化函数
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
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  //使能TIM2时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//使能PORTI时钟	
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //GPIOI0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //下拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PI0

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM2); //PA0复用位定时器5
  
	
  TIM_TimeBaseInitStructure.TIM_Period = 0xffffffff; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=90-1;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//初始化TIM2
	
	//初始化TIM2输入捕获参数
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	选择输入端 IC4映射到TI4上
  TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
  TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //定时器5中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	
	TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC4,ENABLE); //允许定时器5溢出更新中断，通道四捕获更新中断
 	TIM_Cmd(TIM2,ENABLE); //使能定时器5

}

/**
  * @brief  定时器2中断服务函数 
  * @param  void
  * @retval void
  * @notes  计算距离
  */

void TIM2_IRQHandler(void)
{
	//上升下降沿切换标志位
    static char Down_Flag=0;
	//未经过滤波的距离
	static float Raw_Distance=0;
  
	//如果触发溢出中断
	if(TIM_GetITStatus(TIM2,TIM_IT_Update == SET))
	{
	  
  }
	//如果触发捕获中断
	if(TIM_GetITStatus(TIM2,TIM_IT_CC4) == SET)
	{
		//如果当前是上升沿触发
		if(Down_Flag==0)
		{
			//将定时器计数值设为0
	  	TIM2->CNT=0;
			//上升下降沿切换标志位
			Down_Flag=1;
			//切换为下降沿
			TIM_OC4PolarityConfig(TIM2,TIM_ICPolarity_Falling);
		}
		//如果当前是下降沿触发
		else
		{
			//计算距离
			Raw_Distance=((float)((TIM_GetCapture4(TIM2)/2000.0f)*340.0f));
			//滤波
			if((Raw_Distance>50)&&(Raw_Distance<2000))
		  //赋值
			Right_Ultrasonic_Distance=Raw_Distance;
			//上升下降沿切换标志位
		    Down_Flag=0;	
			//切换为上升沿
			TIM_OC4PolarityConfig(TIM2,TIM_ICPolarity_Rising);
	  }
		
  }
	//清除中断标志位
	 TIM_ClearITPendingBit (TIM2,TIM_IT_Update|TIM_IT_CC4);
}






































#include "main.h"

int NUC_Lost_Time=0;

/**
  * @brief  定时器7初始化函数
  * @param  void
  * @retval void
  * @notes  初始化定时器，主函数中设置中断产生时间为1ms，中断优先级为6
  */
void Tick_TIM7_Init (int arr)   //间隔为0.01ms * arr，
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);  //使能TIM7时钟
	
   TIM_TimeBaseInitStructure.TIM_Period = arr; 	       //自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=900 -1 ;     //定时器分频0.1mhz
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);//初始化TIM7
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);          //允许定时器7更新中断
	TIM_Cmd(TIM7,ENABLE);                             //使能定时器7

	NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; //定时器7中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6; //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0; //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}

/**
  * @brief  定时器7中断服务函数
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
		TimeTick1ms++;//程序运行时间计数
		
		if(TimeTick1ms%10==0)//每10ms运行一次
		{
			if( PTZ_Mode==PTZ_Locked)//当云台模式为锁定模式时
			{
				Mode_Last_Time++;
			}
			if(Mode_Last_Time>1500)//当锁定时间大于15s时，改变云台模式
			{
				Mode_Last_Time=0;
				PTZ_Mode=PTZ_Lost_Enemy;
			}
			
			NUC_Lost_Time++;//NUC失联时间增加
			
			if(NUC_Lost_Time>50)//当NUC失联时间大于500ms时，蜂鸣器响
			{
				NUC_Lost_Flag_Music=1;
				Beep_ON();//NUC丢失时蜂鸣器开启
				Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag=0;//向上主板发送不允许射击的信息
				if(PTZ_Mode==PTZ_Find_Enemy)//NUC丢失模式下强制变为丢失敌人状态
				{
					PTZ_Mode=PTZ_Lost_Enemy;
				}
			}
			else//NUC正常时蜂鸣器关闭
			{
				NUC_Lost_Flag_Music=0;
				Beep_OFF();
			}
		}
		//
		if(TimeTick1ms%10==0)
		{
			//唤醒串口8发送任务
			vTaskNotifyGiveFromISR(USART8_Send_Task_Handler,&pxHigherPriorityTaskWoken);
		}		
 	}
   TIM_ClearITPendingBit (TIM7,TIM_IT_Update);
	 portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);//进行任务切换
}


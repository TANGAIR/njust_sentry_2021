#include "main.h"

int Shoot_Allow_PWM=1400;
int Shoot_Forbid_PWM=850;//950  后
int Shoot_Forbid_PWM1=650;//800 先

/**
  * @brief  舵机定时器输出PWM
  * @param  void
  * @retval void 
  * @notes  TIM4_CH4,频率为50Hz，PD15
  */
void Severo_Init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeInitstruct;
	TIM_OCInitTypeDef        TIM_OCInitstruct;
	GPIO_InitTypeDef         GPIO_Initstruct;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD,ENABLE);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_TIM4);
	
	GPIO_Initstruct.GPIO_Mode =GPIO_Mode_AF;
	GPIO_Initstruct.GPIO_OType =GPIO_OType_PP;
	GPIO_Initstruct.GPIO_Pin  =GPIO_Pin_15;
	GPIO_Initstruct.GPIO_PuPd =GPIO_PuPd_UP;
	GPIO_Initstruct.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOD,&GPIO_Initstruct);
	
	TIM_TimeInitstruct.TIM_ClockDivision        =TIM_CKD_DIV1;
	TIM_TimeInitstruct.TIM_CounterMode          =TIM_CounterMode_Up;
	TIM_TimeInitstruct.TIM_Period               =20000-1;    
	TIM_TimeInitstruct.TIM_Prescaler            =90-1;                 
	TIM_TimeBaseInit(TIM4,&TIM_TimeInitstruct);
	
	TIM_OCInitstruct.TIM_OCMode         =TIM_OCMode_PWM1;
	TIM_OCInitstruct.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitstruct.TIM_OutputState  =TIM_OutputState_Enable;
	TIM_OC4Init(TIM4,&TIM_OCInitstruct);
	
	TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM4,ENABLE); 
	TIM_Cmd(TIM4,ENABLE);
}

void Shoot_Allow(void)
{
	TIM_SetCompare4(TIM4,Shoot_Allow_PWM);
}
void Shoot_Forbid(void)
{
	TIM_SetCompare4(TIM4,Shoot_Forbid_PWM);
}



void Shoot_Forbid1(void)
{
	TIM_SetCompare4(TIM4,Shoot_Forbid_PWM1);
}








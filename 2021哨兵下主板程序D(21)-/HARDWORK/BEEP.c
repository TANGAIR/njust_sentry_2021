#include "BEEP.h"



/**************************·äÃùÆ÷¸èÇú****************************************/
//Ñ¡Ôñ¸èÇú
#define MUSIC  HUAN_LE_DOU_DI_ZHU


//¸èµ¥
#define XING_WEN_LIAN_BO  1
#define HUAN_LE_DOU_DI_ZHU  2
#define HUAN_LE_DOU_DI_ZHU_2  3







/*************************ÀÖÆ×****************************************/
int tune[] =  
{
#if MUSIC	== XING_WEN_LIAN_BO
//
  L5,M1,L5,M3,M1,M5,ZERO,
	M1,M5,M7,M6,M3,
	M5,M7,M6,
	L1,M1,M3,M2,L6,
	M1,M3,M2,L6,ZERO,
	M2,M2,M3,M2,M6,M3,M5,ZERO,
	L5,L6,M1
	
#endif 	
//»¶ÀÖ¶·µØÖ÷
#if  MUSIC	== HUAN_LE_DOU_DI_ZHU
	
 M3,M3,M2,M1,M1,L6,M2,M3,M2,M3,L5,
 L6,L6,L5,L6,M1,M5,M6,M3,M5,M2,
 M3,M3,M2,M3,M5,M6,H1,M6,H1,M6,M5,M3,
 M2,M2,M3,M5,L5,M2,M3,M2,M3,M1
	
#endif 	
	
	//»¶ÀÖ¶·µØÖ÷2
#if  MUSIC	== HUAN_LE_DOU_DI_ZHU_2
	
 M1,M1,M3, M1,M1,M3,ZERO,
 M6,M6,M6,M5,M6,M5,M1,M3,ZERO,
 H1,M6,M6,M5,M6, M5,M1,M2,ZERO,
 M7,M5,M3,M5,ZERO,
 H1, M6,M6,M5,M5,M6,M6,M5,M1,M3,ZERO,
 H1, M6,M6,M5,M5,M6,M6,M5,M1,M3,ZERO,
 M3,M1,L6,M1,ZERO
#endif 
};
int delay[] =  
{ 
#if MUSIC	== XING_WEN_LIAN_BO
//
	250,250,250,250,250,900,600,
	250,200,250,250,600,
	250,200,200,
	250,200,250,250,600,
	250,200,200,650,500,
	250,250,250,250,500,500,1000,250,
  300,300,1000
#endif 	
//»¶ÀÖ¶·µØÖ÷
#if  MUSIC	== HUAN_LE_DOU_DI_ZHU
	
 200,200,100,200,100,100,100,100,100,100,400,
 200,200,100,200,100,100,100,100,100,400,
 200,200,100,200,200,100,100,100,100,300,100,100,
 200,200,150,200,200,150,150,150,150,400
#endif
	

//»¶ÀÖ¶·µØÖ÷2
#if  MUSIC	== HUAN_LE_DOU_DI_ZHU_2
	
200,200,300,200,200,300,200,


#endif	
	
};


















/*************************º¯Êý¹¦ÄÜÇø****************************************/

int Music_length = (sizeof(tune)/sizeof(tune[0]));


void Beep_Init()
{
   TIM_TimeBaseInitTypeDef  TIM_TimeInitstruct;
	TIM_OCInitTypeDef      TIM_OCInitstruct;
	GPIO_InitTypeDef  GPIO_Initstruct;
	
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12,ENABLE);
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOH,ENABLE);
	GPIO_PinAFConfig(GPIOH,GPIO_PinSource6,GPIO_AF_TIM12);
	
	GPIO_Initstruct.GPIO_Mode =GPIO_Mode_AF;
	GPIO_Initstruct.GPIO_OType =GPIO_OType_PP;
	GPIO_Initstruct.GPIO_Pin  =GPIO_Pin_6;
	GPIO_Initstruct.GPIO_PuPd =GPIO_PuPd_UP;
	GPIO_Initstruct.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOH,&GPIO_Initstruct);
	
	TIM_TimeInitstruct.TIM_ClockDivision        =TIM_CKD_DIV1;
	TIM_TimeInitstruct.TIM_CounterMode          =TIM_CounterMode_Up;
	TIM_TimeInitstruct.TIM_Period               =262;    
	TIM_TimeInitstruct.TIM_Prescaler            =83;                 
	TIM_TimeBaseInit(TIM12,&TIM_TimeInitstruct);
	
	TIM_OCInitstruct.TIM_OCMode         =TIM_OCMode_PWM1;
	TIM_OCInitstruct.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitstruct.TIM_OutputState  =TIM_OutputState_Enable;
	TIM_OC1Init(TIM12,&TIM_OCInitstruct);
	
	TIM_OC1PreloadConfig(TIM12,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM12,ENABLE); 
	TIM_Cmd(TIM12,ENABLE);
	Beep_OFF();
}








void Beep_Change_Music(int pra)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeInitstruct;
	TIM_OCInitTypeDef      TIM_OCInitstruct;
	GPIO_InitTypeDef  GPIO_Initstruct;
	
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12,ENABLE);
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOH,ENABLE);
	GPIO_PinAFConfig(GPIOH,GPIO_PinSource6,GPIO_AF_TIM12);
	
	GPIO_Initstruct.GPIO_Mode =GPIO_Mode_AF;
	GPIO_Initstruct.GPIO_OType =GPIO_OType_PP;
	GPIO_Initstruct.GPIO_Pin  =GPIO_Pin_6;
	GPIO_Initstruct.GPIO_PuPd =GPIO_PuPd_UP;
	GPIO_Initstruct.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOH,&GPIO_Initstruct);
	
	TIM_TimeInitstruct.TIM_ClockDivision        =TIM_CKD_DIV1;
	TIM_TimeInitstruct.TIM_CounterMode          =TIM_CounterMode_Up;
	TIM_TimeInitstruct.TIM_Period               =pra;    
	TIM_TimeInitstruct.TIM_Prescaler            =89;                 
	TIM_TimeBaseInit(TIM12,&TIM_TimeInitstruct);
	
	TIM_OCInitstruct.TIM_OCMode         =TIM_OCMode_PWM1;
	TIM_OCInitstruct.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitstruct.TIM_OutputState  =TIM_OutputState_Enable;
	TIM_OC1Init(TIM12,&TIM_OCInitstruct);
	
	TIM_OC1PreloadConfig(TIM12,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM12,ENABLE); 
	TIM_Cmd(TIM12,ENABLE);
	
	Beep_OFF();
 
}






void Beep_ON(void)
{	
   TIM_SetCompare1(TIM12,230);	
}


void Beep_OFF(void)
{
   TIM_SetCompare1(TIM12,0);	
}








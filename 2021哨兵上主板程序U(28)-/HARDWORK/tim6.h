#ifndef __TIM2_H
#define __TIM2_H 	
#include "sys.h"



#define GENERAL_TIM           		TIM6
#define GENERAL_TIM_CLK       		RCC_APB1Periph_TIM6

#define GENERAL_TIM_IRQn		      TIM6_DAC_IRQn
#define GENERAL_TIM_IRQHandler    TIM6_DAC_IRQHandler

void TIM2_Init(void);






#endif



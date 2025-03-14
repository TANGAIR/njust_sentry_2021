#ifndef _LASER_H
#define _LASER_H
#include "stm32f4xx.h" 

#define  LASER_ON      GPIO_SetBits(GPIOG, GPIO_Pin_13)
#define  LASER_OFF     GPIO_ResetBits(GPIOG, GPIO_Pin_13)

void Laser_InitConfig(void);
	
#endif

#ifndef LED_H
#define LED_H
#include "stm32f4xx.h" 

#define  LED_RED_ON      GPIO_ResetBits(GPIOE, GPIO_Pin_11)
#define  LED_RED_OFF     GPIO_SetBits(GPIOE, GPIO_Pin_11)
#define  LED_RED_TOGGLE  GPIO_ToggleBits(GPIOE, GPIO_Pin_11)

#define  LED_GREEN_ON        GPIO_ResetBits(GPIOF, GPIO_Pin_14)
#define  LED_GREEN_OFF       GPIO_SetBits(GPIOF, GPIO_Pin_14)
#define  LED_GREEN_TOGGLE    GPIO_ToggleBits(GPIOF, GPIO_Pin_14)






extern  char NUC_Lost_Flag_Music;







void LED_Init(void);
void Heart_Task(void *pvParameters);

#endif

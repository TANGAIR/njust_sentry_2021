#ifndef _RANDOM_H
#define _RANDOM_H
#include "stm32f4xx.h"

extern void   Random_Num_Generator_Init(void);
extern void   Random_Num_Generator_Produce(void);

extern int Random_Number;
extern int Random_Number_Last;
extern int Random_Number_Now;
#endif

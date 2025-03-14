#ifndef TIM7_H
#define TIM7_H
#include "sys.h"


extern int Danger_Time;
extern int Mode_Time;	              //模式持续时间
extern int Position_Collet_Time;
extern int Data_Return_Times;
extern u32 TimeTick1ms;


extern char Heat_Allow_Flag;
extern char UP_Heat_Allow_Flag;

void Tick_TIM5_Init(int arr);
void Heat_Control(void);
void Heat_Judge(void);



void Tick_TIM7_Init (int arr);

#endif



#ifndef _SHOOT_H
#define _SHOOT_H
#include "sys.h" 

     
extern float Fricion_Speed_203;
extern float Fricion_Speed_204;


extern uint16_t  UP_LongTime_Locked_Times; 
extern uint16_t  UP_Locked_Time;
extern uint16_t  UP_Locked_Flag;

extern uint16_t  LongTime_Locked_Times; 
extern uint16_t  Locked_Time;
extern uint16_t  Locked_Flag;



void Shoot_Init(void);

void MoCaLun_Open_3508(void);
void MoCaLun_Close_3508(void);


void SHOOT_Control_Task(void *pvParameters);

#endif

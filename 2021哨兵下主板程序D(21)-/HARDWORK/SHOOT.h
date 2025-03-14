#ifndef _SHOOT_H
#define _SHOOT_H
#include "sys.h" 


extern float Fricion_Speed_203;
extern float Fricion_Speed_204;


void Shoot_Init(void);
void MoCaLun_Open_3508(void);
void MoCaLun_Close_3508(void);

void MoCaLun_Control_Task(void *pvParameters);

#endif

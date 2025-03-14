#ifndef SENSOR_H
#define SENSOR_H
#include "sys.h"

extern char Chassis_Position; 
extern short Position_Limit_Flag;
extern float Left_Ultrasonic_Distance;
extern float Right_Ultrasonic_Distance;



void Ultrasonic_Awake_Init(void);
void TIM5_CH4_Cap_Init (void) ;
void TIM2_CH4_Cap_Init (void) ;

void Infrared_Sensor_Init(void);
void Sensor_Init(void);

#endif

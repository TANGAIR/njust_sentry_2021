#ifndef APP_TASK_H
#define APP_TASK_H

#include "task.h"

extern TaskHandle_t USART8_Send_Task_Handler;

extern TaskHandle_t Up_To_Down_Decode_Task_Handler;

extern TaskHandle_t NUC_Decode_Task_Handler;

extern TaskHandle_t PTZ_Control_Task_Handler;

//extern TaskHandle_t DBUS_Receive_Data_Task_Handler;

extern TaskHandle_t IMU_Get_Data_Task_Handler;

extern TaskHandle_t Can_Receive_Data_Task_Handler;

void startTask(void);



#endif




#ifndef APP_TASK_H
#define APP_TASK_H

#include "task.h"

void startTask(void);

extern TaskHandle_t NUC_Decode_Task_Handler;

//extern TaskHandle_t DBUS_Receive_Data_Task_Handler;

extern TaskHandle_t Referee_System_Task_Handler;

extern TaskHandle_t UP_Send_NUC_Task_Handler;

extern TaskHandle_t USART8_Send_Task_Handler;

extern TaskHandle_t Down_To_Up_Decode_Task_Handler;

extern TaskHandle_t IMU_Get_Data_Task_Handler;


#endif

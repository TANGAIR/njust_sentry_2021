#ifndef MAIN_H
#define MAIN_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "list.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"


#include "LED.h"
#include "BEEP.h"
#include "Laser.h"
#include "tim6.h"


#include "APP_task.h"
#include "USART.h"

#include "DR16.h"
#include "CAN.h"
#include "PID.h"
#include "PTZ.h"

#include "crc.h"
#include "Referee_System.h"

#include "SHOOT.h"
#include "Severo.h"

#include "SENSOR.h"
#include "CHASSIS.h"

#include "TIM7.h"
#include "random.h"

#include "SPI.h"
#include "IMU.h"


/*****************轨道位置分段*******************/
//节点LEFT  1          2        3             4  RIGHT
//圈数   0  2          92       184           270 274
      /**|**|**********|********|**************|**|**/
//位置   0  |    1     |    2   |    3         | 4

#define PART_ZERO  0
#define PART_ONE   1
#define PART_TWO   2
#define PART_TGREE 3
#define PART_FOUR  3

//左极限基础位置电机圈数 0
//从左极限位置到有极限位置的圈数,,,后期需要测试
#define LEFT_LIMIT_NUM   0
#define FIRST_NOTE_NUM   4
#define SECOND_NOTE_NUM  92
#define THIRD_NOTE_NUM   184
#define FOURTH_NOTE_NUM  270
#define RIGHT_LIMIT_NUM  276






#endif













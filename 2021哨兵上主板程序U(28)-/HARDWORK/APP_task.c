/****************头文件包含区******************************/
#include "main.h"



/**********************************任务相关定义***************************************************/
/*
分别是
任务优先级：数字越大优先级越高
任务栈大小：单位是字，即4字节，决定任务中所有局部变量的定义的总大小上限，所有任务的栈总大小不能超过系统堆大小
任务句柄：  用于对任务进行操作
*/
//启动任务
#define START_TASK_PRIO 1
#define START_STK_SIZE 512
static TaskHandle_t StartTask_Handler;

//NUC数据解码任务
#define NUC_Decode_Task_PRIO 8
#define NUC_Decode_Task_SIZE 512
TaskHandle_t NUC_Decode_Task_Handler;

////遥控器数据解码任务
//#define DBUS_Receive_Data_Task_PRIO 7
//#define DBUS_Receive_Data_Task_SIZE 512
//TaskHandle_t DBUS_Receive_Data_Task_Handler;

//云台控制任务
#define PTZ_Control_Task_PRIO 7
#define PTZ_Control_Task_SIZE 512
TaskHandle_t PTZ_Control_Task_Handler;

//裁判系统数据解码任务
#define Referee_System_Task_PRIO 6
#define Referee_System_Task_SIZE 512
TaskHandle_t Referee_System_Task_Handler;

//上主板到NUC数据发送任务
#define UP_Send_NUC_Task_PRIO 6
#define UP_Send_NUC_Task_SIZE 512
TaskHandle_t UP_Send_NUC_Task_Handler;

//USART8发送任务
#define USART8_Send_Task_PRIO 5
#define USART8_Send_Task_SIZE 512
TaskHandle_t USART8_Send_Task_Handler;

//下主板到上主板数据解码任务
#define Down_To_Up_Decode_Task_PRIO 5
#define Down_To_Up_Decode_Task_SIZE 512
TaskHandle_t Down_To_Up_Decode_Task_Handler;

//陀螺仪数据获取任务
#define IMU_Get_Data_Task_PRIO 5
#define IMU_Get_Data_Task_SIZE 512
TaskHandle_t IMU_Get_Data_Task_Handler;

//发射控制任务
#define SHOOT_Control_Task_PRIO 4
#define SHOOT_Control_Task_SIZE 512
TaskHandle_t SHOOT_Control_Task_Handler;

//底盘控制任务
#define Chassis_Control_Task_PRIO 2
#define Chassis_Control_Task_SIZE 512
TaskHandle_t Chassis_Control_Task_Handler;

//LED心跳显示任务
#define Heart_Task_PRIO 1
#define Heart_Task_SIZE 512
TaskHandle_t Heart_Task_Handler;

/**
  * @brief  启动任务函数
  * @param  void
  * @retval void
  * @notes  启动任务的函数主体，用于创建各种任务
	          前三个优先级的任务不会阻塞等待中断给出通知
						分别是：LED任务每秒一次，摩擦轮控制任务每1ms执行一次，云台控制任务每1ms执行一次				
  */
void start_task(void *pvParameters)
{
	 //进入临界区，防止中断打断
    taskENTER_CRITICAL();
	
  	/*动态创建任务*/
	
	 /**************************有中断通知的任务（没有被中断唤醒时不会被执行）********************************/			
	  //创建NUC数据解码任务
	  //任务优先级为 8
	  //USART6_IRQHandler中断优先级为 6 ，周期由NUC决定      ，发送收尾中断DMA2_Stream6_IRQHandler优先级为 9
	  //DMA:发送：DMA2_Stream6、接收：DMA2_Stream1
	  //初始化NUC串口6，开启接收和发送中断
	  //被中断唤醒后获得最高执行权限，控制云台
    xTaskCreate((TaskFunction_t)NUC_Decode_Task,
                (const char *)"NUC_Decode_Task",
                (uint16_t)NUC_Decode_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)NUC_Decode_Task_PRIO,
                (TaskHandle_t *)&NUC_Decode_Task_Handler);
	 
		//创建遥控器数据解码任务
		//任务优先级为 7
		//DMA2_Stream5_IRQHandler中断优先级为 7,周期14ms		
		//DMA:DMA2_Stream5								
		//初始化遥控器接收串口和DMA以及DMA中断，每14ms被中断通知调用一次，进行遥控器数据解码，
		//将解码后的数据储存到DBUS结构体对象中							
//    xTaskCreate((TaskFunction_t)DBUS_Receive_Data_Task,
//                (const char *)"DBUS_Receive_Data_Task",
//                (uint16_t)DBUS_Receive_Data_Task_SIZE,
//                (void *)NULL,
//                (UBaseType_t)DBUS_Receive_Data_Task_PRIO,
//                (TaskHandle_t *)&DBUS_Receive_Data_Task_Handler);	
								
		//创建裁判系统数据解码任务
		//任务优先级为 6
		//USART1_IRQHandler中断优先级为 6, 周期由裁判系统决定		
		//DMA:DMA1_Stream1								
		//初始化裁判系统串口1，关闭DMA数据流，切换裁判系统数据接收数组，重新设置数据接收长度之后再打开dma数据流
    //进行数据校验与解码							
    xTaskCreate((TaskFunction_t)Referee_System_Task,
                (const char *)"Referee_System_Task",
                (uint16_t)Referee_System_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)Referee_System_Task_PRIO,
                (TaskHandle_t *)&Referee_System_Task_Handler);						
								
		//上主板到NUC数据发送任务
		//任务优先级为 6
		//TIM7中断优先级为 6, 周期1MS		
		//DMA:DMA1_Stream0	，*********************************************************2019暂时不发，给NUC发送数据导致底层任务无法运行
    //TIM7初始化，开启定时器中断	，中断服务函数中还设置了云台模式切换							
		//将云台yaw轴和pitch轴的数据经过处理之后储存Send_NUC数组中，然后启动dma数据流将数据发送出去
		//每1ms发送一次Send_NUC到串口6，再发出
		//每500毫秒，用0.5秒损失的血量判断是否被大弹丸打击，同时进行位置判断
		//每1000毫秒，根据1s内减少的血量与受攻击状态，判断危险等级
		//每7秒，判断一次是否卡弹，拨弹盘反转时间超过560ms说明卡弹了，这时将发射锁定标志位设为1
		//每22秒，判断是否长时间卡弹，如果卡弹则Locked_Flag=0						
    xTaskCreate((TaskFunction_t)UP_Send_NUC_Task,
                (const char *)"UP_Send_NUC_Task",
                (uint16_t)UP_Send_NUC_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)UP_Send_NUC_Task_PRIO,
                (TaskHandle_t *)&UP_Send_NUC_Task_Handler);							
								
		//串口8数据发送任务
		//任务优先级为 5
		//TIM7中断优先级为 6,周期10MS		
		//DMA:DMA1_Stream3								
		//将一些数据经过处理之后发送给下板，共10个数据，帧头为0x7A，帧尾为0X7B         
    //每10毫秒，底盘模式恢复、位置采集标志位恢复、攻击标志位恢复、向下主板发送消息、红外限位  					
    xTaskCreate((TaskFunction_t)USART8_Send_Task,
                (const char *)"USART8_Send_Task",
                (uint16_t)USART8_Send_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)USART8_Send_Task_PRIO,
                (TaskHandle_t *)&USART8_Send_Task_Handler);	

		//下主板到上主板数据解码任务
		//任务优先级为 5
		//串口8中断优先级为 9,周期10ms	
		//DMA:DMA1_Stream6								
		//接受解码下主板发过来的数据					
    xTaskCreate((TaskFunction_t)Down_To_Up_Decode_Task,
                (const char *)"Down_To_Up_Decode_Task",
                (uint16_t)Down_To_Up_Decode_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)Down_To_Up_Decode_Task_PRIO,
                (TaskHandle_t *)&Down_To_Up_Decode_Task_Handler);	

    							
		//创建陀螺仪数据解码任务					
		//任务优先级为 4
	  //EXTI9_5_IRQHandler中断优先级为 7，周期1ms
	  //每1毫秒被PB8中断唤醒一次，获取陀螺仪数据， 
	  //IMU初始化函数在云台控制任务中
	  xTaskCreate((TaskFunction_t)IMU_Get_Data_Task,      			//函数入口
                (const char *)"IMU_Get_Data_Task",    				//任务名称
                (uint16_t)IMU_Get_Data_Task_SIZE,     				//任务栈大小
                (void *)NULL,                         				//任务传入参数
                (UBaseType_t)IMU_Get_Data_Task_PRIO,  				//任务优先级
                (TaskHandle_t *)&IMU_Get_Data_Task_Handler);  //任务句柄												
								
   /**************************无中断通知的任务********************************/		
			
    //创建发射控制任务
		//优先级为 4
		//完成上舵机初始化、发射PID初始化、上摩擦轮初始化，定时器5初始化
		//每2ms运行一次，使用微增量方式对云台进行控制
    xTaskCreate((TaskFunction_t)SHOOT_Control_Task,
                (const char *)"SHOOT_Control_Task",
                (uint16_t)SHOOT_Control_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)SHOOT_Control_Task_PRIO,
                (TaskHandle_t *)&SHOOT_Control_Task_Handler);

    //创建云台控制任务
		//优先级为 7
		//完成CAN初始化、云台PID初始化、云台回正
		//每1ms运行一次，使用微增量方式对云台进行控制，通过CAN向云台和拨弹盘发送数据
    xTaskCreate((TaskFunction_t)PTZ_Control_Task,
                (const char *)"PTZ_Control_Task",
                (uint16_t)PTZ_Control_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)PTZ_Control_Task_PRIO,
                (TaskHandle_t *)&PTZ_Control_Task_Handler);

    //创建底盘控制任务
		//优先级为 2
		//完成红外传感器初始化、底盘PID初始化、设置底盘速度为0
		//每2ms运行一次，使用微增量方式对云台进行控制
    xTaskCreate((TaskFunction_t)Chassis_Control_Task,
                (const char *)"Chassis_Control_Task",
                (uint16_t)Chassis_Control_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)Chassis_Control_Task_PRIO,
                (TaskHandle_t *)&Chassis_Control_Task_Handler);
    
		//创建LED心跳任务
		//优先级为 1
		//红灯每隔1s闪烁一次，表明系统在正常运行
	  xTaskCreate((TaskFunction_t) Heart_Task,
                (const char *)"Heart_Task",
                (uint16_t)Heart_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)Heart_Task_PRIO,
                (TaskHandle_t *)&Heart_Task_Handler);	
								
		//删除开始任务
    vTaskDelete(StartTask_Handler);
		 //退出临界区						
    taskEXIT_CRITICAL();           
}


/**
  * @brief  启动任务创建函数
  * @param  void
  * @retval void
  * @notes  创建一个启动任务，在开启任务调度以后系统只有一个启动任务，
            在启动任务中创建其他任务。最后将启动任务删除。
  */
void startTask(void)
{
	//动态任务创建
    xTaskCreate((TaskFunction_t)start_task,          //任务函数
                (const char *)"start_task",          //任务名称
                (uint16_t)START_STK_SIZE,            //任务堆栈大小
                (void *)NULL,                        //传递给任务函数的参数
                (UBaseType_t)START_TASK_PRIO,        //任务优先级
                (TaskHandle_t *)&StartTask_Handler); //任务句柄
}






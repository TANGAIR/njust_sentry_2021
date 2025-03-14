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
#define NUC_Decode_Task_PRIO 7
#define NUC_Decode_Task_SIZE 512
TaskHandle_t NUC_Decode_Task_Handler;

////遥控器数据解码任务
//#define DBUS_Receive_Data_Task_PRIO 5
//#define DBUS_Receive_Data_Task_SIZE 512
//TaskHandle_t DBUS_Receive_Data_Task_Handler;

//串口8发送任务
#define USART8_Send_Task_PRIO 4
#define USART8_Send_Task_SIZE 512
TaskHandle_t USART8_Send_Task_Handler;

//串口8接收数据解码任务
#define Up_To_Down_Decode_Task_PRIO 4
#define Up_To_Down_Decode_Task_SIZE 512
TaskHandle_t Up_To_Down_Decode_Task_Handler;

//陀螺仪数据获取任务
#define IMU_Get_Data_Task_PRIO 4
#define IMU_Get_Data_Task_SIZE 512
TaskHandle_t IMU_Get_Data_Task_Handler;

//云台控制任务
#define PTZ_Control_Task_PRIO 3
#define PTZ_Control_Task_SIZE 512
TaskHandle_t PTZ_Control_Task_Handler;

//摩擦轮控制任务
#define MoCaLun_Control_Task_PRIO 2
#define MoCaLun_Control_Task_SIZE 512
TaskHandle_t MoCaLun_Control_Task_Handler;

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
	
	 /**************************有中断通知的任务（没有被中断唤醒时一直不会被执行）********************************/			
	 
	  //创建NUC数据解码任务
	  //任务优先级为 7
	  //USART3_IRQHandler中断优先级为 6 ，周期由NUC决定      ，发送收尾中断DMA1_Stream3_IRQHandler优先级为 6
	  //DMA:发送：DMA1_Stream3、接收：DMA1_Stream1
	  //被中断唤醒后获得最高执行权限，控制云台，同时向上板发送数据
    xTaskCreate((TaskFunction_t)NUC_Decode_Task,
                (const char *)"NUC_Decode_Task",
                (uint16_t)NUC_Decode_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)NUC_Decode_Task_PRIO,
                (TaskHandle_t *)&NUC_Decode_Task_Handler);
								
		//创建遥控器数据解码任务
		//任务优先级为 5
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
								
		//创建陀螺仪数据解码任务					
		//任务优先级为 4
	  //EXTI9_5_IRQHandler中断优先级为 7，周期1ms
	  //每1毫秒被PB8中断唤醒一次，获取陀螺仪数据，   同时给NUC发送数据，
	  //IMU初始化函数在云台控制任务中
	  xTaskCreate((TaskFunction_t)IMU_Get_Data_Task,      			//函数入口
                (const char *)"IMU_Get_Data_Task",    				//任务名称
                (uint16_t)IMU_Get_Data_Task_SIZE,     				//任务栈大小
                (void *)NULL,                         				//任务传入参数
                (UBaseType_t)IMU_Get_Data_Task_PRIO,  				//任务优先级
                (TaskHandle_t *)&IMU_Get_Data_Task_Handler);  //任务句柄									
						
    //创建串口8接收解码任务
		//任务优先级为 4
		//UART8_IRQHandler中断优先级为 8，周期10ms
		//DMA接收:DMA1_Stream6
    //获取上主板发来的数据进行解码，并且根据是否受到攻击而改变云台状态								
    xTaskCreate((TaskFunction_t) Up_To_Down_Decode_Task,
                (const char *)" Up_To_Down_Decode_Task",
                (uint16_t)Up_To_Down_Decode_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)Up_To_Down_Decode_Task_PRIO,
                (TaskHandle_t *)&Up_To_Down_Decode_Task_Handler);
								
   	//创建串口8发送任务
		//任务优先级 4
		//TIM7_IRQHandler中断优先级为 6 周期为1ms  ，发送收尾中断DMA1_Stream0_IRQHandler优先级为 8
		//DMA发送:DMA1_Stream0         
		//每10ms向上主板发送一次信息，初始化串口8						
    xTaskCreate((TaskFunction_t)USART8_Send_Task,
                (const char *)"USART8_Send_Task",
                (uint16_t)USART8_Send_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)USART8_Send_Task_PRIO,
    						(TaskHandle_t *)&USART8_Send_Task_Handler);				
								
   /**************************无中断通知的任务********************************/	
								
		//创建云台控制任务
		//优先级为 3
		//完成CAN初始化、PID初始化、陀螺仪初始化、云台回正
		//每1ms运行一次，使用微增量方式对云台进行控制
    xTaskCreate((TaskFunction_t)PTZ_Control_Task,
                (const char *)"PTZ_Control_Task",
                (uint16_t)PTZ_Control_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)PTZ_Control_Task_PRIO,
                (TaskHandle_t *)&PTZ_Control_Task_Handler);
								
		//创建摩擦轮控制任务
		//优先级为 2
		//每1ms执行一次，根据遥控器的值，设置摩擦轮，摩擦轮的PWM输出使用斜坡函数，
		//逐渐增速，要更改摩擦轮转速，需要到MoCaLun_Open函数中更改摩擦轮最大PWM
		//控制下发射舵机						
    xTaskCreate((TaskFunction_t) MoCaLun_Control_Task,
                (const char *)"MoCaLun_Control_Task",
                (uint16_t)MoCaLun_Control_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)MoCaLun_Control_Task_PRIO,
                (TaskHandle_t *)&MoCaLun_Control_Task_Handler);
								
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

























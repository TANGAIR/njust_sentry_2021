/****************ͷ�ļ�������******************************/
#include "main.h"



/**********************************������ض���***************************************************/
/*
�ֱ���
�������ȼ�������Խ�����ȼ�Խ��
����ջ��С����λ���֣���4�ֽڣ��������������оֲ������Ķ�����ܴ�С���ޣ����������ջ�ܴ�С���ܳ���ϵͳ�Ѵ�С
��������  ���ڶ�������в���
*/
//��������
#define START_TASK_PRIO 1
#define START_STK_SIZE 512
static TaskHandle_t StartTask_Handler;

//NUC���ݽ�������
#define NUC_Decode_Task_PRIO 8
#define NUC_Decode_Task_SIZE 512
TaskHandle_t NUC_Decode_Task_Handler;

////ң�������ݽ�������
//#define DBUS_Receive_Data_Task_PRIO 7
//#define DBUS_Receive_Data_Task_SIZE 512
//TaskHandle_t DBUS_Receive_Data_Task_Handler;

//��̨��������
#define PTZ_Control_Task_PRIO 7
#define PTZ_Control_Task_SIZE 512
TaskHandle_t PTZ_Control_Task_Handler;

//����ϵͳ���ݽ�������
#define Referee_System_Task_PRIO 6
#define Referee_System_Task_SIZE 512
TaskHandle_t Referee_System_Task_Handler;

//�����嵽NUC���ݷ�������
#define UP_Send_NUC_Task_PRIO 6
#define UP_Send_NUC_Task_SIZE 512
TaskHandle_t UP_Send_NUC_Task_Handler;

//USART8��������
#define USART8_Send_Task_PRIO 5
#define USART8_Send_Task_SIZE 512
TaskHandle_t USART8_Send_Task_Handler;

//�����嵽���������ݽ�������
#define Down_To_Up_Decode_Task_PRIO 5
#define Down_To_Up_Decode_Task_SIZE 512
TaskHandle_t Down_To_Up_Decode_Task_Handler;

//���������ݻ�ȡ����
#define IMU_Get_Data_Task_PRIO 5
#define IMU_Get_Data_Task_SIZE 512
TaskHandle_t IMU_Get_Data_Task_Handler;

//�����������
#define SHOOT_Control_Task_PRIO 4
#define SHOOT_Control_Task_SIZE 512
TaskHandle_t SHOOT_Control_Task_Handler;

//���̿�������
#define Chassis_Control_Task_PRIO 2
#define Chassis_Control_Task_SIZE 512
TaskHandle_t Chassis_Control_Task_Handler;

//LED������ʾ����
#define Heart_Task_PRIO 1
#define Heart_Task_SIZE 512
TaskHandle_t Heart_Task_Handler;

/**
  * @brief  ����������
  * @param  void
  * @retval void
  * @notes  ��������ĺ������壬���ڴ�����������
	          ǰ�������ȼ������񲻻������ȴ��жϸ���֪ͨ
						�ֱ��ǣ�LED����ÿ��һ�Σ�Ħ���ֿ�������ÿ1msִ��һ�Σ���̨��������ÿ1msִ��һ��				
  */
void start_task(void *pvParameters)
{
	 //�����ٽ�������ֹ�жϴ��
    taskENTER_CRITICAL();
	
  	/*��̬��������*/
	
	 /**************************���ж�֪ͨ������û�б��жϻ���ʱ���ᱻִ�У�********************************/			
	  //����NUC���ݽ�������
	  //�������ȼ�Ϊ 8
	  //USART6_IRQHandler�ж����ȼ�Ϊ 6 ��������NUC����      ��������β�ж�DMA2_Stream6_IRQHandler���ȼ�Ϊ 9
	  //DMA:���ͣ�DMA2_Stream6�����գ�DMA2_Stream1
	  //��ʼ��NUC����6���������պͷ����ж�
	  //���жϻ��Ѻ������ִ��Ȩ�ޣ�������̨
    xTaskCreate((TaskFunction_t)NUC_Decode_Task,
                (const char *)"NUC_Decode_Task",
                (uint16_t)NUC_Decode_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)NUC_Decode_Task_PRIO,
                (TaskHandle_t *)&NUC_Decode_Task_Handler);
	 
		//����ң�������ݽ�������
		//�������ȼ�Ϊ 7
		//DMA2_Stream5_IRQHandler�ж����ȼ�Ϊ 7,����14ms		
		//DMA:DMA2_Stream5								
		//��ʼ��ң�������մ��ں�DMA�Լ�DMA�жϣ�ÿ14ms���ж�֪ͨ����һ�Σ�����ң�������ݽ��룬
		//�����������ݴ��浽DBUS�ṹ�������							
//    xTaskCreate((TaskFunction_t)DBUS_Receive_Data_Task,
//                (const char *)"DBUS_Receive_Data_Task",
//                (uint16_t)DBUS_Receive_Data_Task_SIZE,
//                (void *)NULL,
//                (UBaseType_t)DBUS_Receive_Data_Task_PRIO,
//                (TaskHandle_t *)&DBUS_Receive_Data_Task_Handler);	
								
		//��������ϵͳ���ݽ�������
		//�������ȼ�Ϊ 6
		//USART1_IRQHandler�ж����ȼ�Ϊ 6, �����ɲ���ϵͳ����		
		//DMA:DMA1_Stream1								
		//��ʼ������ϵͳ����1���ر�DMA���������л�����ϵͳ���ݽ������飬�����������ݽ��ճ���֮���ٴ�dma������
    //��������У�������							
    xTaskCreate((TaskFunction_t)Referee_System_Task,
                (const char *)"Referee_System_Task",
                (uint16_t)Referee_System_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)Referee_System_Task_PRIO,
                (TaskHandle_t *)&Referee_System_Task_Handler);						
								
		//�����嵽NUC���ݷ�������
		//�������ȼ�Ϊ 6
		//TIM7�ж����ȼ�Ϊ 6, ����1MS		
		//DMA:DMA1_Stream0	��*********************************************************2019��ʱ��������NUC�������ݵ��µײ������޷�����
    //TIM7��ʼ����������ʱ���ж�	���жϷ������л���������̨ģʽ�л�							
		//����̨yaw���pitch������ݾ�������֮�󴢴�Send_NUC�����У�Ȼ������dma�����������ݷ��ͳ�ȥ
		//ÿ1ms����һ��Send_NUC������6���ٷ���
		//ÿ500���룬��0.5����ʧ��Ѫ���ж��Ƿ񱻴�������ͬʱ����λ���ж�
		//ÿ1000���룬����1s�ڼ��ٵ�Ѫ�����ܹ���״̬���ж�Σ�յȼ�
		//ÿ7�룬�ж�һ���Ƿ񿨵��������̷�תʱ�䳬��560ms˵�������ˣ���ʱ������������־λ��Ϊ1
		//ÿ22�룬�ж��Ƿ�ʱ�俨�������������Locked_Flag=0						
    xTaskCreate((TaskFunction_t)UP_Send_NUC_Task,
                (const char *)"UP_Send_NUC_Task",
                (uint16_t)UP_Send_NUC_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)UP_Send_NUC_Task_PRIO,
                (TaskHandle_t *)&UP_Send_NUC_Task_Handler);							
								
		//����8���ݷ�������
		//�������ȼ�Ϊ 5
		//TIM7�ж����ȼ�Ϊ 6,����10MS		
		//DMA:DMA1_Stream3								
		//��һЩ���ݾ�������֮���͸��°壬��10�����ݣ�֡ͷΪ0x7A��֡βΪ0X7B         
    //ÿ10���룬����ģʽ�ָ���λ�òɼ���־λ�ָ���������־λ�ָ����������巢����Ϣ��������λ  					
    xTaskCreate((TaskFunction_t)USART8_Send_Task,
                (const char *)"USART8_Send_Task",
                (uint16_t)USART8_Send_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)USART8_Send_Task_PRIO,
                (TaskHandle_t *)&USART8_Send_Task_Handler);	

		//�����嵽���������ݽ�������
		//�������ȼ�Ϊ 5
		//����8�ж����ȼ�Ϊ 9,����10ms	
		//DMA:DMA1_Stream6								
		//���ܽ��������巢����������					
    xTaskCreate((TaskFunction_t)Down_To_Up_Decode_Task,
                (const char *)"Down_To_Up_Decode_Task",
                (uint16_t)Down_To_Up_Decode_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)Down_To_Up_Decode_Task_PRIO,
                (TaskHandle_t *)&Down_To_Up_Decode_Task_Handler);	

    							
		//�������������ݽ�������					
		//�������ȼ�Ϊ 4
	  //EXTI9_5_IRQHandler�ж����ȼ�Ϊ 7������1ms
	  //ÿ1���뱻PB8�жϻ���һ�Σ���ȡ���������ݣ� 
	  //IMU��ʼ����������̨����������
	  xTaskCreate((TaskFunction_t)IMU_Get_Data_Task,      			//�������
                (const char *)"IMU_Get_Data_Task",    				//��������
                (uint16_t)IMU_Get_Data_Task_SIZE,     				//����ջ��С
                (void *)NULL,                         				//���������
                (UBaseType_t)IMU_Get_Data_Task_PRIO,  				//�������ȼ�
                (TaskHandle_t *)&IMU_Get_Data_Task_Handler);  //������												
								
   /**************************���ж�֪ͨ������********************************/		
			
    //���������������
		//���ȼ�Ϊ 4
		//����϶����ʼ��������PID��ʼ������Ħ���ֳ�ʼ������ʱ��5��ʼ��
		//ÿ2ms����һ�Σ�ʹ��΢������ʽ����̨���п���
    xTaskCreate((TaskFunction_t)SHOOT_Control_Task,
                (const char *)"SHOOT_Control_Task",
                (uint16_t)SHOOT_Control_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)SHOOT_Control_Task_PRIO,
                (TaskHandle_t *)&SHOOT_Control_Task_Handler);

    //������̨��������
		//���ȼ�Ϊ 7
		//���CAN��ʼ������̨PID��ʼ������̨����
		//ÿ1ms����һ�Σ�ʹ��΢������ʽ����̨���п��ƣ�ͨ��CAN����̨�Ͳ����̷�������
    xTaskCreate((TaskFunction_t)PTZ_Control_Task,
                (const char *)"PTZ_Control_Task",
                (uint16_t)PTZ_Control_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)PTZ_Control_Task_PRIO,
                (TaskHandle_t *)&PTZ_Control_Task_Handler);

    //�������̿�������
		//���ȼ�Ϊ 2
		//��ɺ��⴫������ʼ��������PID��ʼ�������õ����ٶ�Ϊ0
		//ÿ2ms����һ�Σ�ʹ��΢������ʽ����̨���п���
    xTaskCreate((TaskFunction_t)Chassis_Control_Task,
                (const char *)"Chassis_Control_Task",
                (uint16_t)Chassis_Control_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)Chassis_Control_Task_PRIO,
                (TaskHandle_t *)&Chassis_Control_Task_Handler);
    
		//����LED��������
		//���ȼ�Ϊ 1
		//���ÿ��1s��˸һ�Σ�����ϵͳ����������
	  xTaskCreate((TaskFunction_t) Heart_Task,
                (const char *)"Heart_Task",
                (uint16_t)Heart_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)Heart_Task_PRIO,
                (TaskHandle_t *)&Heart_Task_Handler);	
								
		//ɾ����ʼ����
    vTaskDelete(StartTask_Handler);
		 //�˳��ٽ���						
    taskEXIT_CRITICAL();           
}


/**
  * @brief  �������񴴽�����
  * @param  void
  * @retval void
  * @notes  ����һ�����������ڿ�����������Ժ�ϵͳֻ��һ����������
            �����������д����������������������ɾ����
  */
void startTask(void)
{
	//��̬���񴴽�
    xTaskCreate((TaskFunction_t)start_task,          //������
                (const char *)"start_task",          //��������
                (uint16_t)START_STK_SIZE,            //�����ջ��С
                (void *)NULL,                        //���ݸ��������Ĳ���
                (UBaseType_t)START_TASK_PRIO,        //�������ȼ�
                (TaskHandle_t *)&StartTask_Handler); //������
}






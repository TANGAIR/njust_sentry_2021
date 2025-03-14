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
#define NUC_Decode_Task_PRIO 7
#define NUC_Decode_Task_SIZE 512
TaskHandle_t NUC_Decode_Task_Handler;

////ң�������ݽ�������
//#define DBUS_Receive_Data_Task_PRIO 5
//#define DBUS_Receive_Data_Task_SIZE 512
//TaskHandle_t DBUS_Receive_Data_Task_Handler;

//����8��������
#define USART8_Send_Task_PRIO 4
#define USART8_Send_Task_SIZE 512
TaskHandle_t USART8_Send_Task_Handler;

//����8�������ݽ�������
#define Up_To_Down_Decode_Task_PRIO 4
#define Up_To_Down_Decode_Task_SIZE 512
TaskHandle_t Up_To_Down_Decode_Task_Handler;

//���������ݻ�ȡ����
#define IMU_Get_Data_Task_PRIO 4
#define IMU_Get_Data_Task_SIZE 512
TaskHandle_t IMU_Get_Data_Task_Handler;

//��̨��������
#define PTZ_Control_Task_PRIO 3
#define PTZ_Control_Task_SIZE 512
TaskHandle_t PTZ_Control_Task_Handler;

//Ħ���ֿ�������
#define MoCaLun_Control_Task_PRIO 2
#define MoCaLun_Control_Task_SIZE 512
TaskHandle_t MoCaLun_Control_Task_Handler;

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
	
	 /**************************���ж�֪ͨ������û�б��жϻ���ʱһֱ���ᱻִ�У�********************************/			
	 
	  //����NUC���ݽ�������
	  //�������ȼ�Ϊ 7
	  //USART3_IRQHandler�ж����ȼ�Ϊ 6 ��������NUC����      ��������β�ж�DMA1_Stream3_IRQHandler���ȼ�Ϊ 6
	  //DMA:���ͣ�DMA1_Stream3�����գ�DMA1_Stream1
	  //���жϻ��Ѻ������ִ��Ȩ�ޣ�������̨��ͬʱ���ϰ巢������
    xTaskCreate((TaskFunction_t)NUC_Decode_Task,
                (const char *)"NUC_Decode_Task",
                (uint16_t)NUC_Decode_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)NUC_Decode_Task_PRIO,
                (TaskHandle_t *)&NUC_Decode_Task_Handler);
								
		//����ң�������ݽ�������
		//�������ȼ�Ϊ 5
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
								
		//�������������ݽ�������					
		//�������ȼ�Ϊ 4
	  //EXTI9_5_IRQHandler�ж����ȼ�Ϊ 7������1ms
	  //ÿ1���뱻PB8�жϻ���һ�Σ���ȡ���������ݣ�   ͬʱ��NUC�������ݣ�
	  //IMU��ʼ����������̨����������
	  xTaskCreate((TaskFunction_t)IMU_Get_Data_Task,      			//�������
                (const char *)"IMU_Get_Data_Task",    				//��������
                (uint16_t)IMU_Get_Data_Task_SIZE,     				//����ջ��С
                (void *)NULL,                         				//���������
                (UBaseType_t)IMU_Get_Data_Task_PRIO,  				//�������ȼ�
                (TaskHandle_t *)&IMU_Get_Data_Task_Handler);  //������									
						
    //��������8���ս�������
		//�������ȼ�Ϊ 4
		//UART8_IRQHandler�ж����ȼ�Ϊ 8������10ms
		//DMA����:DMA1_Stream6
    //��ȡ�����巢�������ݽ��н��룬���Ҹ����Ƿ��ܵ��������ı���̨״̬								
    xTaskCreate((TaskFunction_t) Up_To_Down_Decode_Task,
                (const char *)" Up_To_Down_Decode_Task",
                (uint16_t)Up_To_Down_Decode_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)Up_To_Down_Decode_Task_PRIO,
                (TaskHandle_t *)&Up_To_Down_Decode_Task_Handler);
								
   	//��������8��������
		//�������ȼ� 4
		//TIM7_IRQHandler�ж����ȼ�Ϊ 6 ����Ϊ1ms  ��������β�ж�DMA1_Stream0_IRQHandler���ȼ�Ϊ 8
		//DMA����:DMA1_Stream0         
		//ÿ10ms�������巢��һ����Ϣ����ʼ������8						
    xTaskCreate((TaskFunction_t)USART8_Send_Task,
                (const char *)"USART8_Send_Task",
                (uint16_t)USART8_Send_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)USART8_Send_Task_PRIO,
    						(TaskHandle_t *)&USART8_Send_Task_Handler);				
								
   /**************************���ж�֪ͨ������********************************/	
								
		//������̨��������
		//���ȼ�Ϊ 3
		//���CAN��ʼ����PID��ʼ���������ǳ�ʼ������̨����
		//ÿ1ms����һ�Σ�ʹ��΢������ʽ����̨���п���
    xTaskCreate((TaskFunction_t)PTZ_Control_Task,
                (const char *)"PTZ_Control_Task",
                (uint16_t)PTZ_Control_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)PTZ_Control_Task_PRIO,
                (TaskHandle_t *)&PTZ_Control_Task_Handler);
								
		//����Ħ���ֿ�������
		//���ȼ�Ϊ 2
		//ÿ1msִ��һ�Σ�����ң������ֵ������Ħ���֣�Ħ���ֵ�PWM���ʹ��б�º�����
		//�����٣�Ҫ����Ħ����ת�٣���Ҫ��MoCaLun_Open�����и���Ħ�������PWM
		//�����·�����						
    xTaskCreate((TaskFunction_t) MoCaLun_Control_Task,
                (const char *)"MoCaLun_Control_Task",
                (uint16_t)MoCaLun_Control_Task_SIZE,
                (void *)NULL,
                (UBaseType_t)MoCaLun_Control_Task_PRIO,
                (TaskHandle_t *)&MoCaLun_Control_Task_Handler);
								
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

























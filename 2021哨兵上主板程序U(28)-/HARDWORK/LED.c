#include "main.h"

/**
  * @brief  LED��ʼ��
  * @param  void
  * @retval void
  * @notes  ���-PE11      �̵�-PE14
  */
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitTypeStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	GPIO_InitTypeStruct.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitTypeStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitTypeStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitTypeStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitTypeStruct.GPIO_Speed=GPIO_High_Speed;
   GPIO_Init(GPIOE,&GPIO_InitTypeStruct);
	
	GPIO_InitTypeStruct.GPIO_Pin=GPIO_Pin_14;
	GPIO_InitTypeStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitTypeStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitTypeStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitTypeStruct.GPIO_Speed=GPIO_High_Speed;
   GPIO_Init(GPIOF,&GPIO_InitTypeStruct);

   LED_RED_OFF;
	LED_GREEN_OFF;
}



char NUC_Lost_Flag_Music=0;
void Heart_Task(void *pvParameters)
{
  delay_ms(500);
	while(1)
  {
		
		
		  //��ѹ
	  //	printf("PowerHeatData.chassis_volt :%d\r\n",PowerHeatData.chassis_volt );
		  //����
		//	printf("PowerHeatData.chassis_power:%f\r\n",PowerHeatData.chassis_power);
		  //��������
		    printf("PowerHeatData.chassis_power_buffer:%d\r\n",PowerHeatData.chassis_power_buffer);
		  //�����
		    printf("Power_Max:%f\r\n",Power_Max);
		  //��ǰ����
		//		printf("Current_Power:%f\r\n",Current_Power);
 
		   //����ҡ�α�־λ
//			 printf("Swing_Flag:%d\r\n",Swing_Flag);
		
		
//		 �����ٶ�
		   printf("Chassis_Motor_201.Real_Speed:%d\r\n",Chassis_Motor_201.Real_Speed/19);
		 //����Ȧ��
//		   printf("Chassis_Motor_201_Number:%d\r\n",Chassis_Motor_201_Number);
		   
		
	  	//���̷���
		// printf("Chassis_Direction:%d\r\n",Chassis_Direction);
	  	//Ŀ�귽��
	//	 printf("Chassis_Target_Direction:%d\r\n",Chassis_Target_Direction);
	  	//����Ŀ���ٶ�
	  //	printf("Chassis_Speed:%d\r\n",Chassis_Speed);
		
		//�������ڵ�ģʽ
		  printf("Danger_Coefficient:%d\r\n",Danger_Coefficient);
		  //������Ϣ
	//	  printf("My_Usart_Recieve_Data:%d\r\n",My_Usart_Recieve_Data);
		
		
		
//		
//		  //�ֶμ���
		//  printf("Swing_i:%d\r\n",Swing_i);
//		  //�ֶ����
		  printf("Swing_Position:%d\r\n",Swing_Position);
//		
//		
		
		
//		printf("Send_NUC:\r\n");
//		printf("Up_Bullet_Speed:%f\r\n",Up_Bullet_Speed);
//	  printf("Yaw_Angle:%d\r\n",Yaw_Angle);
//		printf("Pitch_Angle:%d\r\n",Pitch_Angle);
//	  printf("Pitch_Speed:%d\r\n",Pitch_Speed);
//		printf("Yaw_Speed:%d\r\n",Yaw_Speed);
	    //printf("Up_To_Down_TypeStruct.Chassis_Real_Position:%d\r\n",Up_To_Down_TypeStruct.Chassis_Real_Position);
//		printf("Send_NUC[12]:%d\r\n",Send_NUC[12]);
//	  printf("Down_To_Up_TypeStruct.Enemy_Armor_ID:%d\r\n",Down_To_Up_TypeStruct.Enemy_Armor_ID);
	   	//printf("Up_To_Down_TypeStruct.Chassis_Speed_Real:%d\r\n\r\n",Up_To_Down_TypeStruct.Chassis_Speed_Real);
//	  printf("Up_To_Down_TypeStruct.Enemy_State:%d\r\n",Up_To_Down_TypeStruct.Enemy_State);	
//		printf("Up_To_Down_TypeStruct.Enemy_HP_HL:%d\r\n\r\n",Up_To_Down_TypeStruct.Enemy_HP_HL);
//	
//		printf("Recieve_From_NUC:\r\n");
//		printf("Up_NUC_Data_TypeStruct.NUC_Shoot_Allow_Flag:%d\r\n",Up_NUC_Data_TypeStruct.NUC_Shoot_Allow_Flag);
//	  printf("Up_NUC_Data_TypeStruct.Yaw_Angle:%f\r\n",Up_NUC_Data_TypeStruct.Yaw_Angle);
//		printf("Up_NUC_Data_TypeStruct.Pitch_Angle:%f\r\n",Up_NUC_Data_TypeStruct.Pitch_Angle);
//	  printf("Up_NUC_Data_TypeStruct.Armor_ID:%d\r\n",Up_NUC_Data_TypeStruct.Armor_ID);
//		printf("Up_NUC_Data_TypeStruct.Armor_Type:%d\r\n",Up_NUC_Data_TypeStruct.Armor_Type);
//	  printf("Up_NUC_Data_TypeStruct.Enemy_distance:%d\r\n\r\n\r\n",Up_NUC_Data_TypeStruct.Enemy_distance);
	
      //����
	    printf("\r\n");
	

		
		
		 LED_RED_TOGGLE;
		 vTaskDelay(500);

	}	
}








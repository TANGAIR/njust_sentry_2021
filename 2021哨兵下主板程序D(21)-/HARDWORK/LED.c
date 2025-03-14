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


/**
  * @brief  LED����������
  * @param  void
  * @retval void
  * @notes  ÿ1msִ��һ�Σ���ʾ������������
  */
char NUC_Lost_Flag_Music=0;
void Heart_Task(void *pvParameters)
{
   
	vTaskDelay(100);
	while(1)
	{
		//���Ӿ����յ�����
		{
//			printf("Usart3Rx_Info[0]=%d\r\n",Usart3Rx_Info[0] );
//			
//			printf("Usart3Rx_Info[5]=%d\r\n",Usart3Rx_Info[5] );
//			printf("Usart3Rx_Info[6]=%d\r\n",Usart3Rx_Info[6] );
//			
//			//��NUCʧ��ʱ�����500msʱ,Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag=0;
//			printf("Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag=%d\r\n\r\n",Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag );
//			
//			
			 //printf("Down_NUC_Data_TypeStruct.Yaw_Angle =%f\r\n",Down_NUC_Data_TypeStruct.Yaw_Angle   );
			 //printf("Down_NUC_Data_TypeStruct.Pitch_Angle=%f\r\n",Down_NUC_Data_TypeStruct.Pitch_Angle  );
//			printf("Down_NUC_Data_TypeStruct.Armor_ID=%d\r\n",Down_NUC_Data_TypeStruct.Armor_ID );
//			printf("Down_NUC_Data_TypeStruct.Armor_Type=%d\r\n",Down_NUC_Data_TypeStruct.Armor_Type  );	
//			printf("Down_NUC_Data_TypeStruct.Enemy_distance=%d\r\n\r\n",Down_NUC_Data_TypeStruct.Enemy_distance  );
		}
		//���͸��Ӿ�������
		{
//			printf("Send_NUC[0]=Bullet_Speed=%d\r\n",Send_NUC[1] );
//			printf("Send_NUC[1+2]=Yaw_Angle=%f\r\n",(-Yaw_Send_NUC_Angle_IMU));
//	  	printf("Send_NUC[3+4]=Pitch_Angle=%f\r\n",((Pitch_Motor_206.Real_MechanicalAngle-5900)*(1.46502259f)+500));
//		  printf("Send_NUC[9]=%d\r\n\r\n",Send_NUC[10] );
			
		}
		
		
		
		{
		  //printf("SHOOT_STOP_PWM+abs(DBUS.RC.ch1)=%d\r\n",(1000+abs(DBUS.RC.ch1)));
		
		
		}
		 
		
//		//����
//		if(NUC_Lost_Flag_Music==1)
//		{ 
//			for(int x=0;x<Music_length;x++)//ѭ�������Ĵ���
//			{ 
//			 Beep_Change_Music(tune[x]);
//			if(tune[x]==ZERO)Beep_OFF();
//			else  Beep_ON();
//			 delay_ms(delay[x]);
//			 Beep_OFF();
//			 delay_ms(50);
//			}
//		}
		 LED_RED_TOGGLE;
		 vTaskDelay(1000);

	}	
}


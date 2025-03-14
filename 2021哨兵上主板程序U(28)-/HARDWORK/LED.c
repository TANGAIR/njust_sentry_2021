#include "main.h"

/**
  * @brief  LED初始化
  * @param  void
  * @retval void
  * @notes  红灯-PE11      绿灯-PE14
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
		
		
		  //电压
	  //	printf("PowerHeatData.chassis_volt :%d\r\n",PowerHeatData.chassis_volt );
		  //电流
		//	printf("PowerHeatData.chassis_power:%f\r\n",PowerHeatData.chassis_power);
		  //缓冲能量
		    printf("PowerHeatData.chassis_power_buffer:%d\r\n",PowerHeatData.chassis_power_buffer);
		  //最大功率
		    printf("Power_Max:%f\r\n",Power_Max);
		  //当前功率
		//		printf("Current_Power:%f\r\n",Current_Power);
 
		   //底盘摇晃标志位
//			 printf("Swing_Flag:%d\r\n",Swing_Flag);
		
		
//		 底盘速度
		   printf("Chassis_Motor_201.Real_Speed:%d\r\n",Chassis_Motor_201.Real_Speed/19);
		 //底盘圈数
//		   printf("Chassis_Motor_201_Number:%d\r\n",Chassis_Motor_201_Number);
		   
		
	  	//底盘方向
		// printf("Chassis_Direction:%d\r\n",Chassis_Direction);
	  	//目标方向
	//	 printf("Chassis_Target_Direction:%d\r\n",Chassis_Target_Direction);
	  	//底盘目标速度
	  //	printf("Chassis_Speed:%d\r\n",Chassis_Speed);
		
		//底盘现在的模式
		  printf("Danger_Coefficient:%d\r\n",Danger_Coefficient);
		  //接收信息
	//	  printf("My_Usart_Recieve_Data:%d\r\n",My_Usart_Recieve_Data);
		
		
		
//		
//		  //分段计数
		//  printf("Swing_i:%d\r\n",Swing_i);
//		  //分段情况
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
	
      //换行
	    printf("\r\n");
	

		
		
		 LED_RED_TOGGLE;
		 vTaskDelay(500);

	}	
}








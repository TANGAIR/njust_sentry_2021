#include "main.h"

#define BULLET_SPEED 28

#define FRICTION_NORMAL_SPEED  7400

float Fricion_Speed_203= FRICTION_NORMAL_SPEED;
float Fricion_Speed_204= FRICTION_NORMAL_SPEED;

/***************************************函数处理区***************************************/
/**
  * @brief  发射函数初始化
  * @param  void
  * @retval void
  * @notes  void       
  */
void Shoot_Init(void)
{
	Friction_Wheel_203.Real_Speed=0;
	Friction_Wheel_204.Real_Speed=0;
	Friction_Wheel_203.Target_Speed=0;
	Friction_Wheel_204.Target_Speed=0;
	CAN1_TX_Friction();
	
}

/**
  * @brief  左右摩擦轮开始旋转
  * @param  void
  * @retval void 
  * @notes  后期要根据裁判系统返回信息调整转速，
  */
  
void MoCaLun_Open_3508(void)     //1230   1241
{
	
	Friction_Wheel_203.Target_Speed=Pid_Calc(&PID_Friction_Wheel_203,Friction_Wheel_203.Real_Speed,-Fricion_Speed_203);
	Friction_Wheel_204.Target_Speed=Pid_Calc(&PID_Friction_Wheel_204,Friction_Wheel_204.Real_Speed,Fricion_Speed_204);
}

/**
  * @brief  关闭摩擦轮
  * @param  void
  * @retval void 
  * @notes  
  */
void MoCaLun_Close_3508(void)
{
  Friction_Wheel_203.Target_Speed=Pid_Calc(&PID_Friction_Wheel_203,Friction_Wheel_203.Real_Speed,0);
	Friction_Wheel_204.Target_Speed=Pid_Calc(&PID_Friction_Wheel_204,Friction_Wheel_204.Real_Speed,0);
}

/**
  * @brief  摩擦轮控制任务函数
  * @param  void
  * @retval void 
  * @notes  摩擦轮初始化PD13,TIM4_CH2 PD14,TIM4_CH3
            舵机初始化PD15,TIM4_CH4
            任务优先级为2，任务的运行周期是1ms,
  */
void MoCaLun_Control_Task(void *pvParameters)
{
	Shoot_Init();//摩擦轮初始化，PWM=400,OUT=400，min=1000,max=1400,
	Severo_Init();//舵机初始化
	vTaskDelay(100);	
	
	int Forbid_Time=0;
	
	while(1)
	{
		/*舵机控制*/
		if(Up_To_Down_TypeStruct.Shoot_Allow_Flag==1)//上板的允许发射标志位
		{
			Forbid_Time=0;//让关闭时间为0
			Shoot_Allow();
		}
		else if( Up_To_Down_TypeStruct.Shoot_Allow_Flag==0)
		{
			if(Forbid_Time<3000)//当关闭时间小于3秒时，关得紧一点
			{
				Forbid_Time++;
				Shoot_Forbid1();
			}
			else 
			Shoot_Forbid();//否则关得松一点
		}
		
		
		/*摩擦轮控制*/
		if(DBUS.RC.Switch_Right==RC_SW_UP)//当右侧拨杆拨到上方即启动手动控制模式时
		{
			if((DBUS.RC.Switch_Left==RC_SW_MID)||(DBUS.RC.Switch_Left==RC_SW_DOWN))//左侧拨杆在中间时或者左侧拨杆拨到下方时
			{			
				MoCaLun_Open_3508();//将速度在两秒内提升到目标速度
			}
			else //左侧拨杆在上方时
			{			
				MoCaLun_Close_3508();//关闭摩擦轮
		  }
		}
		
		else if(DBUS.RC.Switch_Right==RC_SW_DOWN)//当右侧拨杆拨到下方即启动上云台手动控制模式时
		{			
			MoCaLun_Close_3508();//关闭摩擦轮
		}
		
		else //没有用遥控器或者遥控器拨杆在中间时，摩擦轮默认打开
		{	
			//	MoCaLun_Close_3508();//关闭摩擦轮
			   MoCaLun_Open_3508();//
		}	
		//摩擦轮电机can发送
		CAN1_TX_Friction();
		
	  	vTaskDelay(2);
	}
}
































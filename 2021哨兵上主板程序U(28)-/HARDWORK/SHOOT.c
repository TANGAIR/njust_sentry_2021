#include "main.h"


#define BULLET_SPEED 26


#define FRICTION_NORMAL_SPEED 7200

float Fricion_Speed_203=FRICTION_NORMAL_SPEED;
float Fricion_Speed_204=FRICTION_NORMAL_SPEED;



												
uint16_t  UP_LongTime_Locked_Times=0; 
uint16_t  UP_Locked_Time=0;
uint16_t  UP_Locked_Flag=1;	

uint16_t  LongTime_Locked_Times=0; 
uint16_t  Locked_Time=0;
uint16_t  Locked_Flag=1;
												
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
	
}


/**
  * @brief  左右摩擦轮开始旋转
  * @param  void
  * @retval void 
  * @notes  后期要根据裁判系统返回信息调整转速，
  */
void MoCaLun_Open_3508(void)     //1230   1241
{
	Friction_Wheel_203.Target_Speed=Pid_Calc(&PID_Friction_Wheel_203,Friction_Wheel_203.Real_Speed,Fricion_Speed_203);
	Friction_Wheel_204.Target_Speed=Pid_Calc(&PID_Friction_Wheel_204,Friction_Wheel_204.Real_Speed,-Fricion_Speed_204);
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
  * @brief  发射控制任务函数
  * @param  void
  * @retval void 
* @notes  优先级4，周期2ms，CAN发送函数与底盘共用
  */
   //上云台相关
	 uint16_t UP_Roll_Back_Time=0;             //反转计时
	 uint16_t UP_Dead_UP_Locked_Flag=0;        //卡住时间计时
	 char UP_Direction_Flag=1;                 //1正转   0反转	
	 char UP_PokeMotor_Speed=80;               //拨弹电机转速

   short UP_PokeMotor_Speed_2=0;




void SHOOT_Control_Task(void *pvParameters)
{ 
	static char Dead_Locked_Limit=250;               //卡住时间极限
	
	//下云台相关
	static uint16_t Roll_Back_Time=0;          //反转计时
	static uint16_t Dead_Locked_Flag=0;        //卡住时间计时
	static char Direction_Flag=1;              //1正转   0反转
	static char PokeMotor_Speed=80;            //拨弹电机转
	
	
	Shoot_Init();     //摩擦轮初始化
  Severo_Init();    //舵机初始化
	PID_Init_SHOOT(); //发射PID初始化
	vTaskDelay(1000);
	
	
 while(1)
 {
	/*******************************上云台***********************************/
	{
		/*摩擦轮控制*/
		if(DBUS.RC.Switch_Right==RC_SW_DOWN)//当右侧拨杆拨到下方即启动手动控制模式时
		{
			if((DBUS.RC.Switch_Left==RC_SW_MID)||(DBUS.RC.Switch_Left==RC_SW_DOWN))//左侧拨杆在中间时，或者下面
			{
				MoCaLun_Open_3508();//将速度在两秒内提升到目标速度
			}
			else //左侧拨杆在上方时
			{
				MoCaLun_Close_3508();//关闭摩擦轮
		  }
		}
		else if(DBUS.RC.Switch_Right==RC_SW_UP)//当右侧拨杆拨到上方即启动下云台手动控制模式时
		{
			MoCaLun_Close_3508();//关闭摩擦轮
		}
		
		else//没有用遥控器或者遥控器拨杆到中间时，摩擦轮默认打开
		{
			  //MoCaLun_Close_3508();//关闭摩擦轮
			MoCaLun_Open_3508();
		}
		
	 /*拨盘速度限制*/
		//如果手动控制模式
		if(DBUS.RC.Switch_Right==RC_SW_DOWN)
		{
			//拨弹盘速度为150，即每秒20发
			UP_PokeMotor_Speed=120;
		}
		//如果自动模式
		else if(DBUS.RC.Switch_Right==RC_SW_MID)
		{
			
			//敌方距离小于2000MM
			if(Up_NUC_Data_TypeStruct.Enemy_distance<5000)
			{
				//拨弹盘速度为150，即每秒20发
				UP_PokeMotor_Speed=120;
			}
			//敌方距离小于55
			else if(Up_NUC_Data_TypeStruct.Enemy_distance<7000)
			{
				//拨弹盘速度为90，即每秒15发
				UP_PokeMotor_Speed=90;
			}
			//敌方距离小于15000mm
			else if(Up_NUC_Data_TypeStruct.Enemy_distance<15000)
			{
				//拨弹盘速度为90，即每秒10发
				UP_PokeMotor_Speed=60;
			}
			//敌方距离大于100，不发弹
			else UP_PokeMotor_Speed=0;
			
		
			
		}
		//如果右侧拨杆向上
		else if(DBUS.RC.Switch_Right==RC_SW_UP)
		{
			//拨弹盘速度为90，即每秒10发
			UP_PokeMotor_Speed=0;
		}
		
		UP_PokeMotor_Speed_2=36*UP_PokeMotor_Speed;
		
   /*上拨盘与上舵机控制*/
		//当遥控器右侧拨杆在下方时
		if(DBUS.RC.Switch_Right==RC_SW_DOWN)
		{
			//如果热量允许且遥控器左侧拨杆在下方且没有卡弹
			if((UP_Heat_Allow_Flag)&&(DBUS.RC.Switch_Left==RC_SW_DOWN)&&UP_Locked_Flag)
			//if((DBUS.RC.Switch_Left==RC_SW_DOWN)&&UP_Locked_Flag)
			{	
				//如果是正转			
				if(UP_Direction_Flag)
				{
          //打开舵机
					 Shoot_Allow();
					//拨弹盘正转
					Shoot_Motor_208.Target_Speed=Pid_Calc(&PID_Shoot_208,Shoot_Motor_208.Real_Speed,UP_PokeMotor_Speed);  
				}
				//如果是反转
				else
				{
					//反转时间增加
					UP_Roll_Back_Time++;
					//如果反转超过80ms
					if(UP_Roll_Back_Time>=80)
					{
						//拨弹盘锁定时间增加
						UP_Locked_Time++;
//					  //打开舵机
//					  Shoot_Allow();
						//反转时间归零
						UP_Roll_Back_Time=0;
						//将反转变为正转
						UP_Direction_Flag=!UP_Direction_Flag;
					}
					//拨弹盘反转
					Shoot_Motor_208.Target_Speed=Pid_Calc(&PID_Shoot_208,Shoot_Motor_208.Real_Speed,-UP_PokeMotor_Speed); 
				}
				
				//拨弹盘电机速度绝对值小于20	,PID计算用的速度是200/36		
				if(abs(Shoot_Motor_208.Real_Speed<200))
				{
					//锁死标志增加
					UP_Dead_UP_Locked_Flag++;
					//如果锁死时间大于锁死限制即250ms
					if(UP_Dead_UP_Locked_Flag>Dead_Locked_Limit)
					{
						//转向标志位取反
						UP_Direction_Flag=!UP_Direction_Flag;
						//锁死标志位为0
						UP_Dead_UP_Locked_Flag=0;
					}  
				}
				//拨弹盘电机速度绝对值不小于200	
				else UP_Dead_UP_Locked_Flag=0;
			 }
		 //否则如果热量不允许或者遥控器左侧拨杆不在下方或者没有卡弹了
		 else 
		 {
       //关闭舵机
			 Shoot_Forbid();
			 //拨弹盘电机停转
			 PID_Shoot_208.Iout=0;
			 Shoot_Motor_208.Target_Speed=Pid_Calc(&PID_Shoot_208,Shoot_Motor_208.Real_Speed,0);
		 }
		 
		}
		
		//如果右侧拨杆在中间
		else if(DBUS.RC.Switch_Right==RC_SW_MID)
		{
			//在自动模式下想要射击需要
			//裁判系统的热量、
			//裁判系统比赛开始标志位、
			//遥控器允许、
			//NUC允许
			//以及硬件没有堵弹
			//若热量允许且NUC允许发射且遥控器左侧拨杆在下方且没有堵转且（比赛允许射击）
			
			if(UP_Heat_Allow_Flag && Up_NUC_Data_TypeStruct.NUC_Shoot_Allow_Flag && (DBUS.RC.Switch_Left==RC_SW_DOWN)&&UP_Locked_Flag)//&&Game_Allow_Shoot_Flag)
			{ 
				//打开舵机
				 Shoot_Allow();
				//如果是正转			
				if(UP_Direction_Flag)
				{

					//拨弹盘正转
					Shoot_Motor_208.Target_Speed=Pid_Calc(&PID_Shoot_208,Shoot_Motor_208.Real_Speed,UP_PokeMotor_Speed);  
				}
				//如果是反转
				else
				{
					//反转时间增加
					UP_Roll_Back_Time++;
					//如果反转超过80ms
					if(UP_Roll_Back_Time>=80)
					{
						//拨弹盘锁定时间增加
						UP_Locked_Time++;
						//打开舵机
//					  Shoot_Allow();
						//反转时间归零
						UP_Roll_Back_Time=0;
						//将反转变为正转
						UP_Direction_Flag=!UP_Direction_Flag;
					}
					//拨弹盘反转
					Shoot_Motor_208.Target_Speed=Pid_Calc(&PID_Shoot_208,Shoot_Motor_208.Real_Speed,-UP_PokeMotor_Speed); 
				}
				
				//拨弹盘电机速度绝对值小于30			
				if(abs(Shoot_Motor_208.Real_Speed<200))
				{
					//锁死标志增加
					UP_Dead_UP_Locked_Flag++;
					//如果锁死时间大于锁死限制即250ms
					if(UP_Dead_UP_Locked_Flag>Dead_Locked_Limit)
					{
						//转向标志位取反
						UP_Direction_Flag=!UP_Direction_Flag;
						//锁死标志位为0
						UP_Dead_UP_Locked_Flag=0;
					}  
				}
				//拨弹盘电机速度绝对值不小于200	
				else UP_Dead_UP_Locked_Flag=0;
			 }
		 //否则如果热量不允许或者遥控器左侧拨杆不在下方或者没有卡弹了
		 else 
		 {
       //关闭舵机
			 Shoot_Forbid();
			 //拨弹盘电机停转
			 PID_Shoot_208.Iout=0;
			 Shoot_Motor_208.Target_Speed=Pid_Calc(&PID_Shoot_208,Shoot_Motor_208.Real_Speed,0);
		 } 
		}
		
		//否则在没有遥控器控制的情况,或者右侧拨杆在上方
		else 
		{
			//总发弹数归零，在裁判系统信息获取函数中调用
		
			//关闭舵机
			Shoot_Forbid();
			//拨盘停转
			//PID_Shoot_208.Iout=0;
			Shoot_Motor_208.Target_Speed=Pid_Calc(&PID_Shoot_208,Shoot_Motor_208.Real_Speed,0);
			
		}
	}	
	/*******************************下云台***********************************/
	{
		/*发弹速度限制*/
		//如果手动控制模式
		if(DBUS.RC.Switch_Right==RC_SW_UP)
		{
			//拨弹盘速度为150，即每秒25发
			PokeMotor_Speed=120;
		}
		//如果自动模式
		else if(DBUS.RC.Switch_Right==RC_SW_MID)
		{
			//敌方距离小于40
			if(Down_To_Up_TypeStruct.Distace<4000)
			{
				//拨弹盘速度为150，即每秒25发
				PokeMotor_Speed=120;
			}
			//敌方距离小于55
			else if(Down_To_Up_TypeStruct.Distace<5500)
			{
				//拨弹盘速度为90，即每秒15发
				PokeMotor_Speed=90;
			}
			//敌方距离小于65
			else if(Down_To_Up_TypeStruct.Distace<13000)
			{
				//拨弹盘速度为90，即每秒10发
				PokeMotor_Speed=60;
			}
			//敌方距离大于65，不发弹
			else PokeMotor_Speed=0;
		}
		//如果右侧拨杆向下
		else if(DBUS.RC.Switch_Right==RC_SW_DOWN)
		{
			//拨弹盘速度为90，即每秒10发
			PokeMotor_Speed=60;
		}
		
		/*拨盘控制热量限制与卡弹判断*/
		//当遥控器右侧拨杆在上方时
		if(DBUS.RC.Switch_Right==RC_SW_UP)
		{
			//在发弹数目没有达到限制时更新Heat_Allow_Flag，使哨兵可以继续发弹
			Heat_Control();
			//如果热量允许且遥控器左侧拨杆在下方且没有卡弹
			if((Heat_Allow_Flag)&&(DBUS.RC.Switch_Left==RC_SW_DOWN)&&Locked_Flag)
			{	
				//如果是正转			
				if(Direction_Flag)
				{
					//告诉下主板允许发射
					Up_To_Down_TypeStruct.Shoot_Allow_Flag=1;
					//拨弹盘正转
					Shoot_Motor_207.Target_Speed=Pid_Calc(&PID_Shoot_207,Shoot_Motor_207.Real_Speed,PokeMotor_Speed);  
				}
				//如果是反转
				else
				{
					//反转时间增加
					Roll_Back_Time++;
					//如果反转超过80ms
					if(Roll_Back_Time>=80)
					{
						//拨弹盘锁定时间增加
						Locked_Time++;
						////告诉下主板允许发射
						Up_To_Down_TypeStruct.Shoot_Allow_Flag=1;
						//反转时间归零
						Roll_Back_Time=0;
						//将反转变为正转
						Direction_Flag=!Direction_Flag;
					}
					//拨弹盘反转
					Shoot_Motor_207.Target_Speed=Pid_Calc(&PID_Shoot_207,Shoot_Motor_207.Real_Speed,-PokeMotor_Speed); 
				}
				
				//拨弹盘电机速度绝对值小于200			
				if(abs(Shoot_Motor_207.Real_Speed<200))
				{
					//锁死标志增加
					Dead_Locked_Flag++;
					//如果锁死时间大于锁死限制即250ms
					if(Dead_Locked_Flag>Dead_Locked_Limit)
					{
						//转向标志位取反
						Direction_Flag=!Direction_Flag;
						//锁死标志位为0
						Dead_Locked_Flag=0;
					}  
				}
				//拨弹盘电机速度绝对值不小于20	
				else Dead_Locked_Flag=0;
			 }
		 //否则如果热量不允许或者遥控器左侧拨杆不在下方或者没有卡弹了
		 else 
		 {
			 //告诉下主板不允许发射
			 Up_To_Down_TypeStruct.Shoot_Allow_Flag=0;
			 //拨弹盘电机停转
			 Shoot_Motor_207.Target_Speed=Pid_Calc(&PID_Shoot_207,Shoot_Motor_207.Real_Speed,0);
		 }
		}
		
		//如果右侧拨杆在中间或者在下方
		else if( (DBUS.RC.Switch_Right==RC_SW_MID) || (DBUS.RC.Switch_Right==RC_SW_DOWN) )
		{
			//热量控制
			Heat_Control();
			//在自动模式下想要射击需要
			//裁判系统的热量、
			//裁判系统比赛开始标志位、
			//遥控器允许、
			//NUC允许
			//以及硬件没有堵弹
			//若热量允许且NUC允许发射且遥控器左侧拨杆在下方且没有堵转且（比赛允许射击或者右侧拨杆在下方）
		
			if(Heat_Allow_Flag && Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag && (DBUS.RC.Switch_Left==RC_SW_DOWN)&&Locked_Flag)//&&(Game_Allow_Shoot_Flag||(DBUS.RC.Switch_Right==RC_SW_DOWN)))
			{ 
					//如果是正转			
				if(Direction_Flag)
				{
					//告诉下主板允许发射
					Up_To_Down_TypeStruct.Shoot_Allow_Flag=1;
					//拨弹盘正转
					Shoot_Motor_207.Target_Speed=Pid_Calc(&PID_Shoot_207,Shoot_Motor_207.Real_Speed,PokeMotor_Speed);  
				}
				//如果是反转
				else
				{
					//反转时间增加
					Roll_Back_Time++;
					//如果反转超过80ms
					if(Roll_Back_Time>=80)
					{
						//拨弹盘锁定时间增加
						Locked_Time++;
						////告诉下主板允许发射
						Up_To_Down_TypeStruct.Shoot_Allow_Flag=1;
						//反转时间归零
						Roll_Back_Time=0;
						//将反转变为正转
						Direction_Flag=!Direction_Flag;
					}
					//拨弹盘反转
					Shoot_Motor_207.Target_Speed=Pid_Calc(&PID_Shoot_207,Shoot_Motor_207.Real_Speed,-PokeMotor_Speed); 
				}
				
				//拨弹盘电机速度绝对值小于200/36		
				if(abs(Shoot_Motor_207.Real_Speed<100))
				{
					//锁死标志增加
					Dead_Locked_Flag++;
					//如果锁死时间大于锁死限制即250ms
					if(Dead_Locked_Flag>Dead_Locked_Limit)
					{
						//转向标志位取反
						Direction_Flag=!Direction_Flag;
						//锁死标志位为0
						Dead_Locked_Flag=0;
					}  
				}
				//拨弹盘电机速度绝对值不小于200	
				else Dead_Locked_Flag=0;
			 }
		 //否则如果热量不允许或者遥控器左侧拨杆不在下方或者没有卡弹了
		  else 
		  {
			 //告诉下主板不允许发射
			 Up_To_Down_TypeStruct.Shoot_Allow_Flag=0;
				
				
			 //拨弹盘电机停转
			 Shoot_Motor_207.Target_Speed=Pid_Calc(&PID_Shoot_207,Shoot_Motor_207.Real_Speed,0);
		  }
		}
		//否则在没有遥控器控制的情况下
		else 
		{
			//总发弹数归零，在裁判系统信息获取函数中调用
			number_1=0;
			//不允许下板发弹
			Up_To_Down_TypeStruct.Shoot_Allow_Flag=0;
			//拨盘停转
			Shoot_Motor_207.Target_Speed=Pid_Calc(&PID_Shoot_207,Shoot_Motor_207.Real_Speed,0);
		}
	}
	 CAN1_TX_PTZ();	
	 vTaskDelay(2);
 }	
}







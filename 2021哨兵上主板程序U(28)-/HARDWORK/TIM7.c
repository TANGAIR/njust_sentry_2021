#include "main.h"


int Danger_Time=0;                //受到伤害后时间计算，10s内没受到伤害就清0，Up_To_Down_TypeStruct.Get_Hit_flag=2,如果2S内两块装甲同时受到攻击，Up_To_Down_TypeStruct.Get_Hit_flag=3
int Mode_Time=0;	                //模式持续时间 									
int Position_Collet_Time=0;       //位置采集周期时间
int Data_Return_Times=1;
u32 TimeTick1ms = 0;

//上一次弹丸射速
float Last_Bullet_Speed=0;
//每100ms发射的弹丸数目
char Shoot_Num=0;
//根据热量计算的每100ms，弹丸发射限制
short Shoot_Limit_Num=0;
//当前剩余热量
short Current_Heat=0;
//最大热量
uint16_t Max_Heat=300;
//允许射击标志位 1--允许发射     0————不允许发射
char Heat_Allow_Flag=1;   
//允许射击标志位 1--允许发射     0————不允许发射
char UP_Heat_Allow_Flag=1;  
//热量更新标志位0，1  一次改变代表一次更新
char Update_Flag=0; 
//上一次热量更新标志位
char Last_Update_Flag=0;
//弹丸速度限制
float Bullet_Speed_Limit=30.0f;
//弹丸平均速度
float Bullet_Average_Speed=20.0f;


/**
  * @brief  定时器7初始化函数
  * @param  void
  * @retval void
  * @notes  中断间隔为0.01ms * arr ，优先级为 6
  */
void Tick_TIM7_Init (int arr)  
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);  ///使能TIM7时钟
	
   TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=840 -1 ;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);//初始化TIM7
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE); //允许定时器7更新中断
	TIM_Cmd(TIM7,ENABLE); //使能定时器7

	NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; //定时器7中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6; //抢占优先级6
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;        //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}






/*热量判断*/
/**
  * @brief  下云台热量控制函数
  * @param  void
  * @retval void
  * @notes  下云台为 1 号 17mm 枪口
            在发射控制任务中调用，每2ms执行一次
            用弹丸速度的更新来计算发弹数目
            在发弹数目没有达到限制时更新Heat_Allow_Flag，使哨兵可以继续发弹
            实时统计保证每100ms更新后发弹都不会超过热量上限即保证哨兵不会因为
            超热量而扣血，允许发弹数是随着枪口热量更新而更新的
  */
void Heat_Control(void)
{
	//允许发射时间
	static int Allow_Time=0;
	
	//如果更新标志不等于上一次更新标志位，说明热量已经更新
	if(Update_Flag!=Last_Update_Flag)
	{
		Shoot_Num=0;
	}
	//如果本次发射的枪口为0,每次发射之后会更新
	if(ShootData.shooter_id==0)
	{
		//在NUC和下主板通信都用到
		ShootData.shooter_id=2;
		Shoot_Num++;
	}
	//如果超过发弹限制
	if(Shoot_Num>=Shoot_Limit_Num)
	{
		//热量允许标志为0
		Heat_Allow_Flag=0;
		//允许时间置0
		Allow_Time=0;
	}
	//否则如果没有超过最大发弹限制
	else 
	{
		//如果热量允许标志位为0
		if(!Heat_Allow_Flag)
		{
			//允许时间增加
			Allow_Time++;
		}
		//如果允许时间大于80
		if(Allow_Time>=80)
		{
			//将热量允许标志位置1
			Heat_Allow_Flag=1;
		}
	}
	//更新更新标志位
  Last_Update_Flag=Update_Flag;	
}
/**
  * @brief  热量判断函数
  * @param  void
  * @retval void
  * @notes  在定时器5中断中每100ms调用一次
            根据弹丸速度限制和剩余热量设置发射限制数量Shoot_Limit_Num
            同时更新更新标志位
            在没有安装裁判系统时，PowerHeatData.shootHeat0始终等于0，故没有热量限制
  */
void Heat_Judge(void)
{
	//当前剩余热量等于最大热量减去枪口热量
	Current_Heat=Max_Heat-PowerHeatData.shooter_id1_17mm_cooling_heat;
	//如果当前热量小于弹丸速度限制
	if(Current_Heat<=Bullet_Speed_Limit) 
	{
		//发射限制数量为0
		Shoot_Limit_Num=0;
	}
	//否则如果当前热量大于弹丸速度限制
	//发射限制数量等于当前热量减去速度限制之后再除以平均速度
	else Shoot_Limit_Num=(Current_Heat-Bullet_Speed_Limit)/Bullet_Average_Speed;
	//更新更新标志位
	Update_Flag=!Update_Flag;	
}




/**
  * @brief  定时器7中断服务函数
  * @param  void
  * @retval void
  * @notes  中断间隔为0.01ms * arr ，优先级为 6，1ms
            每1ms唤醒一次上主板向NUC发送数据函数，每10ms判断一次云台状态
            每10ms唤醒一次串口8向下板发送信息的函数
  */
void TIM7_IRQHandler(void)
{
//  static int Up_Shoot_Last_Time=0;
  static int Mode_Last_Time=0;
	BaseType_t pxHigherPriorityTaskWoken;
	if(TIM_GetITStatus(TIM7,TIM_IT_Update == SET))
   {
		//系统计数时间增加
		TimeTick1ms++;
		 
		 
	
		 //启动上主板发送到NUC的任务
	  vTaskNotifyGiveFromISR(UP_Send_NUC_Task_Handler,&pxHigherPriorityTaskWoken);
		
		 
		 
		 
		 
		//每10ms
		if(TimeTick1ms%10==0)
		{			
			if( PTZ_Mode==PTZ_Locked)//当云台模式为锁定模式时
			{
				Mode_Last_Time++;
			}
			if(Mode_Last_Time>1500)//当锁定时间大于15s时，改变云台模式
			{
				Mode_Last_Time=0;
				PTZ_Mode=PTZ_Lost_Enemy;
			}
			
			NUC_Lost_Time++;//NUC失联时间增加
			
			if(NUC_Lost_Time>50)//当NUC失联时间大于500ms时，蜂鸣器响
			{
				Beep_ON();//NUC丢失时蜂鸣器开启
				//Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag=0;//向上主板发送不允许射击的信息
				 //关闭舵机
			  // Shoot_Forbid();
				NUC_Lost_Flag_Music=1;
				if(PTZ_Mode==PTZ_Find_Enemy)//NUC丢失模式下强制变为丢失敌人状态
				{
					PTZ_Mode=PTZ_Lost_Enemy;
				}
			}
			else//NUC正常时蜂鸣器关闭
			{
				NUC_Lost_Flag_Music=0;
				Beep_OFF();
			}
			//启动串口8发送任务
			vTaskNotifyGiveFromISR(USART8_Send_Task_Handler,&pxHigherPriorityTaskWoken);
			
		}
				
		
		
		
		
		//热量判断，每100ms
		if(TimeTick1ms%100==0)
		{
			//上云台热量限制
			
		if(PowerHeatData.shooter_id2_17mm_cooling_heat<=300)UP_Heat_Allow_Flag=1;
	  else UP_Heat_Allow_Flag=0;
		
			
			
//			if(PowerHeatData.shooter_id1_17mm_cooling_heat<=300)Heat_Allow_Flag=1;
//	   else Heat_Allow_Flag=0;
//		
			
			
			//热量判断
    Heat_Judge();
		
		
		
		}

		
		
		
		
		
    		
		if(TimeTick1ms%500==0)
		{
				//底盘位置设置
			if((Chassis_Motor_201_Number<=FIRST_NOTE_NUM))
			{
				Chassis_Position=0;
			}
			else if((Chassis_Motor_201_Number>FIRST_NOTE_NUM)&&(Chassis_Motor_201_Number<SECOND_NOTE_NUM))
			{
				Chassis_Position=1;
			}
			else if(Chassis_Motor_201_Number<THIRD_NOTE_NUM)
			{
				Chassis_Position=2;
			}
			else if(Chassis_Motor_201_Number<FOURTH_NOTE_NUM)
			{
				Chassis_Position=3;
			}
			else
					Chassis_Position=4;

		}

		
		
		
    portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);		
 	}
   TIM_ClearITPendingBit (TIM7,TIM_IT_Update);
}






















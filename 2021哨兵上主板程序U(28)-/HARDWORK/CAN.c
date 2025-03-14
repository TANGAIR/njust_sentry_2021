#include "main.h"

/****************************************数据定义区*********************************/
CAN1_Data_TypeDef Chassis_Motor_201;
//               
CAN1_Data_TypeDef Friction_Wheel_203,
                  Friction_Wheel_204;									
									
									

CAN1_Data_TypeDef Pitch_Motor_206,
                  Yaw_Motor_205,
									Shoot_Motor_207,
									Shoot_Motor_208;
									
short   Chassis_Motor_201_Number=100;                    //电机202计圈数
int8_t  Chassis_Direction=0;                           //底盘方向     -1————左    1----右     0----停止
																

static int  CAN1_Tx_Flag;                         //CAN发送完成标志位


int64_t Yaw_Real_MechanicalAngle=0;               //yaw实际角度      从上往下看顺时针为正  逆时针·为负
float Yaw_Real_Angle=0;                           //电机朝向角 
float Yaw_Send_NUC_Angle=0;                       //发送给NUC的角度

	
/*****************************************函数定义区********************************/
/**
  * @brief  CAN通信初始化函数
  * @param  void
  * @retval void
  * @notes  发送中断优先级为8，接收中断优先级为9
  */
void CAN1_Init(void)
{
	GPIO_InitTypeDef      GPIO_InitStruct;
	NVIC_InitTypeDef      NVIC_InitStruct;
	CAN_InitTypeDef       CAN_InitStruct;
	CAN_FilterInitTypeDef CAN_FilterInitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_CAN1);
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel                    = CAN1_RX0_IRQn;             //注意与中断向量表区别  refer to stm32f4xx.h file
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  = 9;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority         = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd                 = ENABLE ;
	NVIC_Init(&NVIC_InitStruct);
	
  NVIC_InitStruct.NVIC_IRQChannel                    = CAN1_TX_IRQn;              //注意与中断向量表区别  refer to stm32f4xx.h file
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  = 8;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority         = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd                 = ENABLE ;
	NVIC_Init(&NVIC_InitStruct);
  
   CAN_DeInit(CAN1);
   CAN_StructInit(&CAN_InitStruct);
    
	CAN_InitStruct.CAN_Prescaler = 3;                                               //CAN BaudRate 42/(1+9+4)/3=1Mbps  //分频系数(Fdiv)为3+1
	CAN_InitStruct.CAN_Mode      = CAN_Mode_Normal;                                 //模式设置 
	CAN_InitStruct.CAN_SJW       = CAN_SJW_1tq;                                     //重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStruct.CAN_BS1       = CAN_BS1_9tq;                                     //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStruct.CAN_BS2       = CAN_BS2_4tq;                                     //Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStruct.CAN_TTCM      = DISABLE;                                         //非时间触发通信模式
	CAN_InitStruct.CAN_ABOM      = DISABLE;                                         //软件自动离线管理	  
	CAN_InitStruct.CAN_AWUM      = DISABLE;                                         //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN_InitStruct.CAN_NART      = DISABLE;	                                        //禁止报文自动传送
	CAN_InitStruct.CAN_RFLM      = DISABLE;                                         //报文不锁定,新的覆盖旧的
	CAN_InitStruct.CAN_TXFP      = ENABLE;	                                        //优先级由报文标识符决定
	CAN_Init(CAN1,&CAN_InitStruct);
	
	CAN_FilterInitStruct.CAN_FilterIdHigh         = 0x0000;                         //32位ID
	CAN_FilterInitStruct.CAN_FilterIdLow          = 0x0000;                         //32位ID
	CAN_FilterInitStruct.CAN_FilterMaskIdHigh     = 0x0000;
	CAN_FilterInitStruct.CAN_FilterMaskIdLow      = 0x0000;
	CAN_FilterInitStruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;               //过滤器0关联到FIFO0;
	CAN_FilterInitStruct.CAN_FilterNumber         = 0;                              //过滤器0,范围0---13
	CAN_FilterInitStruct.CAN_FilterMode           = CAN_FilterMode_IdMask;
	CAN_FilterInitStruct.CAN_FilterScale          = CAN_FilterScale_32bit;
	CAN_FilterInitStruct.CAN_FilterActivation     = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStruct);
	
   CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);                                          //FIFO0消息挂号中断允许，打开接收中断
	 CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE);                                           //打开发送中断	
}

/**
  * @brief  CAN1发送函数
  * @param  void
  * @retval void
  * @notes  底盘、上发射摩擦轮数据发送（广播ID 0x200）
            201、202，给定电流，电流值范围：-16384~+16384
  */
void CAN1_TX_Chassis(void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //标准帧
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //数据帧
	CAN1_Tx_Message.DLC = 0x08;                                                     //帧长度为8
	CAN1_Tx_Message.StdId = CAN1_TRANSMIT_ID_TRAVEL;                              	//帧ID为传入参数的CAN_ID
	
	//底盘电机失去力量
	//Chassis_Motor_201.Target_Speed=0;

	CAN1_Tx_Message.Data[0] = (Chassis_Motor_201.Target_Speed>>8)&0xff;                //201接收电流高8位
	CAN1_Tx_Message.Data[1] =  Chassis_Motor_201.Target_Speed&0xff;                    //201接收电流低8位
	CAN1_Tx_Message.Data[2] = 0;                                                       //202接收电流高8位
	CAN1_Tx_Message.Data[3] = 0;                                                       //202接收电流低8位
	CAN1_Tx_Message.Data[4] = (Friction_Wheel_203.Target_Speed>>8)&0xff;               //203接收电流高8位
	CAN1_Tx_Message.Data[5] = (Friction_Wheel_203.Target_Speed)&0xff;                  //203接收电流低8位
	CAN1_Tx_Message.Data[6] = (Friction_Wheel_204.Target_Speed>>8)&0xff;               //204接收电流高8位
	CAN1_Tx_Message.Data[7] = (Friction_Wheel_204.Target_Speed)&0xff;                  //204接收电流低8位
	            
	
	CAN1_Tx_Flag = 0;
	CAN_Transmit(CAN1,&CAN1_Tx_Message);

	while(CAN1_Tx_Flag == 0); 
}

/**
  * @brief  CAN1发送函数
  * @param  void
  * @retval void
  * @notes  云台数据发送（广播ID 0x1ff）给定电流，电流值范围：-16384~+16384
  */
void CAN1_TX_PTZ (void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //标准帧
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //数据帧
	CAN1_Tx_Message.DLC = 0x08;                                                     //帧长度为8
	CAN1_Tx_Message.StdId = CAN1_TRANSMIT_ID_PTZ;                                   //帧ID为传入参数的CAN_ID

	//上云台失去力量
	//Yaw_Motor_205.Target_Speed=0;
	//Pitch_Motor_206.Target_Speed=0;
	
	CAN1_Tx_Message.Data[0] = (Yaw_Motor_205.Target_Speed>>8)&0xff;               //205接收电流高8位
	CAN1_Tx_Message.Data[1] = (Yaw_Motor_205.Target_Speed)&0xff;                  //205接收电流低8位
	CAN1_Tx_Message.Data[2] = (Pitch_Motor_206.Target_Speed>>8)&0xff;             //206接收电流高8位
	CAN1_Tx_Message.Data[3] = (Pitch_Motor_206.Target_Speed)&0xff;                //206接收电流低8位
	CAN1_Tx_Message.Data[4] = (Shoot_Motor_207.Target_Speed>>8)&0xff;             
	CAN1_Tx_Message.Data[5] = (Shoot_Motor_207.Target_Speed)&0xff;                
	CAN1_Tx_Message.Data[6] = (Shoot_Motor_208.Target_Speed>>8)&0xff;           
	CAN1_Tx_Message.Data[7] = (Shoot_Motor_208.Target_Speed)&0xff;                

	CAN1_Tx_Flag = 0; 
	CAN_Transmit(CAN1,&CAN1_Tx_Message);

	while(CAN1_Tx_Flag == 0); 
}


/**
  * @brief  CAN1发送中断函数
  * @param  void
  * @retval void
  * @notes  void
  */
void CAN1_TX_IRQHandler(void)
{
  if (CAN_GetITStatus (CAN1,CAN_IT_TME)!=RESET)                                 //if transmit mailbox is empty     
	{
		CAN1_Tx_Flag=1;
		CAN_ClearITPendingBit(CAN1,CAN_IT_TME);    
	}
}

/**
  * @brief  201位置判断函数
  * @param  void
  * @retval void
  * @notes  获得累计转动的角度，判断底盘移动方向，若停了3S且比赛开始则关闭遥控器数据接收同时切换为自动模式
  */
void Position_2_Direction_Judge(void)
{
	static short   Chassis_201_Last_Angle=0;                   //电机202上次角度计数
	static int8_t  Chassis_201_Last_Angle_Init=0;              //电机202上次角度初始化
	static int     Chassis_201_Stop_Time=0;
/***********************************位置判断*****************************/
	//若上一次角度为0，即第一次反馈角度
	if(Chassis_201_Last_Angle_Init==0)
	{
		//上一次角度等于真实角度
		Chassis_201_Last_Angle=Chassis_Motor_201.Real_MechanicalAngle;
		Chassis_201_Last_Angle_Init=1;
	}
	//若不是第一次进入
	else
	{
		//圈数计算
		//圈数计算，规定顺时针为增加，即从0顺时针转动到8192，差值为8192，圈数加1，从前往基地看 哨兵向右走时圈数增加
		if(Chassis_Motor_201.Real_MechanicalAngle-Chassis_201_Last_Angle>4000)
		{
			Chassis_Motor_201_Number--;
		}
		//从8192逆时针转到0，差值为-8192，圈数减1
		if(Chassis_Motor_201.Real_MechanicalAngle-Chassis_201_Last_Angle<-4000)
		{
			Chassis_Motor_201_Number++;
		}
		//更新上一次角度
		Chassis_201_Last_Angle=Chassis_Motor_201.Real_MechanicalAngle;
		
	}
/***********************************速度判断****************************/
	//判断速度方向，从基地看向轨道，向左为-1，向右为1，不动为0，202电机安装在基地方向，速度为正时哨兵向左移动
	if(Chassis_Motor_201.Real_Speed>20)
	{
		Chassis_Direction=1;
	}
	else if(Chassis_Motor_201.Real_Speed<-20)
	{
		Chassis_Direction=-1;
	}
	else 
	{
		Chassis_Direction=0;
	}
	
	
//	//如果底盘不动超过3秒，判断是否比赛开始
//	if(abs(Chassis_Motor_202.Real_Speed)<20)
//	{
//		//如果比赛阶段为4，且底盘没有被断电
////		if( (Game_State.game_progress==4)&&(GameRobotState.mains_power_chassis_output) )
////		{
////			//获取裁判系统数据，得到比赛开始的信息，同时停了3s
////			//则关闭遥控器数据流不再从遥控器接收数据，同时开启自动模式
////			if(Chassis_202_Stop_Time<3500)
////			{
////				Chassis_202_Stop_Time++;
////			}
////			if(Chassis_202_Stop_Time>3000)
////			{
////				DMA_Cmd(DMA2_Stream5,DISABLE);
////				DBUS.RC.Switch_Right=RC_SW_MID;
////				Chassis_202_Stop_Time=0;
////				Chassis_Mode=Chassis_Stop_3S;
////				Mode_Time=0;
////			}
////		}
//	}
//	else Chassis_202_Stop_Time=0;
}

/**
  * @brief  YAW轴位置判断函数
  * @param  void
  * @retval void
  * @notes  判断云台转过的圈数，将云台实际转过的角路程存入Yaw_Real_MechanicalAngle。
            判断云台时否处于锁定状态，如果是则换云台模式
  */
void Position_Direction_Judge(void)
{
/***********************************位置判断*****************************/
	static int Yaw_Last_Angle_Init=0;
	static int Yaw_Last_Angle=0;
	static int Yaw_Motor_205_Number=0;      //电机205计圈数
	static int Yaw_Locked_Time=0;
	static int64_t  Yaw_Last_MechanicalAngle=0;
	
	if(Yaw_Last_Angle_Init==0)//若是第一次进入函数，电机刚初始化完成，则只给两个变量赋值
	{
		Yaw_Last_Angle=Yaw_Motor_205.Real_MechanicalAngle;
		Yaw_Last_Angle_Init=1;
	}
	else//若不是第一进入函数
	{
		//电机转过跳变点时圈数计算，现在减去之前，规定逆时针为正，即从8191跳变到0时，电机转动的圈数增加
		//从0转到8192，差值为8192，顺时针转动，圈数减一
		if(Yaw_Motor_205.Real_MechanicalAngle-Yaw_Last_Angle>4000)
		{
			Yaw_Motor_205_Number--;		
		}
		//从8192转到0，差值为-8192，逆时针转动，圈数加一
		if(Yaw_Motor_205.Real_MechanicalAngle-Yaw_Last_Angle<-4000)
		{
			Yaw_Motor_205_Number++;
		}
		//加入圈数统计之后的电机编码值Yaw_Real_MechanicalAngle
		Yaw_Real_MechanicalAngle=Yaw_Motor_205.Real_MechanicalAngle+Yaw_Motor_205_Number*8192;
		
		
		
		//若遥控器的右侧拨杆再中间，即开启了自动模式
		if(DBUS.RC.Switch_Right == RC_SW_MID)
		{
			if(PTZ_Mode!=PTZ_Locked)//若云台模式不等于锁定模式
			{
				if(Yaw_Last_MechanicalAngle==Yaw_Real_MechanicalAngle)//若上一次角度等于当前角度，则云台锁定时间增加
				{
					Yaw_Locked_Time++;
					
				}
				else Yaw_Locked_Time=0;//否则云台锁定时间为0
			}
			if(Yaw_Locked_Time>5000)//若云台锁定时间增加大于5000，大约为5s，则切换云台模式为堵转或锁定
			{
				PTZ_Mode=PTZ_Locked;
				Yaw_Locked_Time=0;
			}	
		}
		//更新Yaw_Last_Angle，用于判断圈数
		Yaw_Last_Angle=Yaw_Motor_205.Real_MechanicalAngle;
		//更新Yaw_Last_MechanicalAngle，用于判断云台是否锁定
		Yaw_Last_MechanicalAngle=Yaw_Real_MechanicalAngle;
	}
}

/**
  * @brief  CAN1接收中断函数
  * @param  void
  * @retval void
  * @notes  每1ms接收一次，接收完之后分别对云台进行PITCH角度处理（为了防越界跳变），
            对YAW轴进行处理，主要是判断云台是否处于锁定状态
  */
	int Count=0;
void CAN1_RX0_IRQHandler(void)                                          
{   
	CanRxMsg CAN1_Rx_Message;
	if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
	{
		  CAN_Receive(CAN1,CAN_FIFO0,&CAN1_Rx_Message);
		
			if ( (CAN1_Rx_Message.IDE == CAN_Id_Standard) && (CAN1_Rx_Message.RTR == CAN_RTR_Data) && (CAN1_Rx_Message.DLC == 8) )//标准帧、数据帧、数据长度为8字节
			{
				switch (CAN1_Rx_Message.StdId)
				{
					//底盘
					case CAN1_FEEDBACK_ID_TRAVEL_1:                                                                               //底盘电机--外侧-->0x201
					{
						Chassis_Motor_201.Real_MechanicalAngle=(CAN1_Rx_Message.Data[0]<<8)|(CAN1_Rx_Message.Data[1]);              //机械角
						Chassis_Motor_201.Real_Speed          =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]); 					    //转速
						//201位置判断
						Position_2_Direction_Judge();
						
					}break ;

					
					//上云台
					case CAN1_FEEDBACK_ID_Pitch:                                                                                
					{
					  Count++;
						Pitch_Motor_206.Real_MechanicalAngle  =(CAN1_Rx_Message.Data[0]<<8)|(CAN1_Rx_Message.Data[1]);              //机械角
						Pitch_Motor_206.Real_Speed            =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]); 					      //转速

					}break ;
					
					
					case CAN1_FEEDBACK_ID_Yaw:                                                                               
					{
						Yaw_Motor_205.Real_MechanicalAngle=(CAN1_Rx_Message.Data[0]<<8)|(CAN1_Rx_Message.Data[1]);              //机械角
						Yaw_Motor_205.Real_Speed          =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]);              //转速

						Position_Direction_Judge();
					}break ;		
					
					//拨盘电机
					case CAN1_FEEDBACK_ID_SHOOT_DOWN:                                                                               //Shoot-->0x207
					{
						Shoot_Motor_207.Real_MechanicalAngle   =(CAN1_Rx_Message.Data[0]<<8)|(CAN1_Rx_Message.Data[1]);             //机械角
						Shoot_Motor_207.Real_Speed             =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]);             //转速
					}break;
						
					
					case CAN1_FEEDBACK_ID_SHOOT_UP:                                                                               //Shoot-->0x208
					{
						Shoot_Motor_208.Real_MechanicalAngle   =(CAN1_Rx_Message.Data[0]<<8)|(CAN1_Rx_Message.Data[1]);             //机械角
						Shoot_Motor_208.Real_Speed             =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]);             //转速
					}break;	
						
					
					//摩擦轮电机
          case CAN1_FEEDBACK_ID_203:                                                                                
					{
						Friction_Wheel_203.Real_Speed            =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]); 					      //转速
					}break ;
					
					case CAN1_FEEDBACK_ID_204:                                                                               
					{
						Friction_Wheel_204.Real_Speed          =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]);              //转速	
					}break ;
					
					
					
					
					default:
					{}
					break ;	   
				}
			}
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);		
	}
}

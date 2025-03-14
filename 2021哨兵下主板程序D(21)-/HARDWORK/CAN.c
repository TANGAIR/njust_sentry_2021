#include "CAN.h"

/****************************************数据定义区*********************************/
CAN1_Data_TypeDef Friction_Wheel_203,
                  Friction_Wheel_204;

CAN1_Data_TypeDef Pitch_Motor_206,
                  Yaw_Motor_205;
									
static int  CAN1_Tx_Flag;                         //CAN发送完成标志位

int Yaw_Motor_205_Number=0;      //电机205计圈数
int64_t Yaw_Real_MechanicalAngle=0;               //yaw实际角度      从上往下看顺时针为正  逆时针·为负
int64_t Pitch_Real_MechanicalAngle=0;               //Pitch实际角度   向上减小，向下增加


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
  * @brief  摩擦轮CAN1发送函数
  * @param  void
  * @retval void
  * @notes  底盘数据发送（广播ID 0x200）给定电流，电流值范围：-16384~+16384
  */
void CAN1_TX_Friction (void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //标准帧
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //数据帧
	CAN1_Tx_Message.DLC = 0x08;                                                     //帧长度为8
	CAN1_Tx_Message.StdId = CAN1_TRANSMIT_ID_200;                                   //帧ID为传入参数的CAN_ID


	CAN1_Tx_Message.Data[0] = 0x00;             
	CAN1_Tx_Message.Data[1] = 0x00;                
	CAN1_Tx_Message.Data[2] = 0x00;           
	CAN1_Tx_Message.Data[3] = 0x00;                
	CAN1_Tx_Message.Data[4] = (Friction_Wheel_203.Target_Speed>>8)&0xff;               //203接收电流高8位
	CAN1_Tx_Message.Data[5] = (Friction_Wheel_203.Target_Speed)&0xff;                  //203接收电流低8位
	CAN1_Tx_Message.Data[6] = (Friction_Wheel_204.Target_Speed>>8)&0xff;               //204接收电流高8位
	CAN1_Tx_Message.Data[7] = (Friction_Wheel_204.Target_Speed)&0xff;                  //204接收电流低8位
	
	CAN1_Tx_Flag = 0; 
	CAN_Transmit(CAN1,&CAN1_Tx_Message);

	while(CAN1_Tx_Flag == 0); 
}

/**
  * @brief  云台CAN1发送函数
  * @param  void
  * @retval void
  * @notes  云台数据发送（广播ID 0x1ff）给定电流，电流值范围：-30000~+30000
  */
void CAN1_TX_PTZ (void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //标准帧
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //数据帧
	CAN1_Tx_Message.DLC = 0x08;                                                     //帧长度为8
	CAN1_Tx_Message.StdId = CAN1_TRANSMIT_ID_PTZ;                                   //帧ID为传入参数的CAN_ID

	//云台脱力
  //Yaw_Motor_205.Target_Speed=0;
 // Pitch_Motor_206.Target_Speed=0;
	
	CAN1_Tx_Message.Data[0] = (Yaw_Motor_205.Target_Speed>>8)&0xff;               //205接收电流高8位
	CAN1_Tx_Message.Data[1] = (Yaw_Motor_205.Target_Speed)&0xff;                  //205接收电流低8位
	CAN1_Tx_Message.Data[2] = (Pitch_Motor_206.Target_Speed>>8)&0xff;             //206接收电流高8位
	CAN1_Tx_Message.Data[3] = (Pitch_Motor_206.Target_Speed)&0xff;                //206接收电流低8位
	CAN1_Tx_Message.Data[4] = 0x00;             
	CAN1_Tx_Message.Data[5] = 0x00;                
	CAN1_Tx_Message.Data[6] = 0x00;           
	CAN1_Tx_Message.Data[7] = 0x00;                

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
		
		
		
		
		
		//yaw电机朝向角
    //对两圈的编码值取余，再乘0.02197f即180/8192，转化为度
		Yaw_Real_Angle=(float)((Yaw_Real_MechanicalAngle-PTZ_Init_Value.Initial_Yaw_Angle)%(8192*2))*0.02197f;
		//发送给NUC的值，即当前的角度减去初始的角度*180/8192
		Yaw_Send_NUC_Angle=(float)(Yaw_Real_MechanicalAngle-PTZ_Init_Value.Initial_Yaw_Angle)*0.02197f;
		
		
		if(Yaw_Real_Angle<0) Yaw_Real_Angle+=360;
		
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
  * @brief  Pitch轴位置判断函数
  * @param  void
  * @retval void
  * @notes  判断云台转过的圈数，将云台实际转过的角路程存入Pitch_Real_MechanicalAngle。
           
  */
void Position_Direction_Judge_Pitch(void)
{
/***********************************位置判断*****************************/
	static int Pitch_Last_Angle_Init=0;
	static int Pitch_Last_Angle=0;
	static int Pitch_Motor_206_Number=0;      //电机205计圈数
	
	
	if(Pitch_Last_Angle_Init==0)//若是第一次进入函数，电机刚初始化完成，则只给两个变量赋值
	{
		Pitch_Last_Angle=Pitch_Motor_206.Real_MechanicalAngle;
		Pitch_Last_Angle_Init=1;
	}
	else//若不是第一进入函数
	{
		//电机转过跳变点时圈数计算，现在减去之前，规定向下为正，即从8191跳变到0时，电机转动的圈数增加
		//从0转到8192，差值为8192，上转动，圈数减一
		if(Pitch_Motor_206.Real_MechanicalAngle-Pitch_Last_Angle>4000)
		{
			Pitch_Motor_206_Number--;		
		}
		//从8192转到0，差值为-8192，向下转动，圈数加一
		if(Pitch_Motor_206.Real_MechanicalAngle-Pitch_Last_Angle<-4000)
		{
			Pitch_Motor_206_Number++;
		}
		//加入圈数统计之后的电机编码值Yaw_Real_MechanicalAngle
		Pitch_Real_MechanicalAngle=Pitch_Motor_206.Real_MechanicalAngle+Pitch_Motor_206_Number*8192;
		
		//更新Yaw_Last_Angle，用于判断圈数
		Pitch_Last_Angle=Pitch_Motor_206.Real_MechanicalAngle;
	
	}
}
/**
  * @brief  CAN1接收中断函数
  * @param  void
  * @retval void
  * @notes  每1ms接收一次，接收完之后分别对云台进行PITCH角度处理（为了防越界跳变），
            对YAW轴进行处理，主要是判断云台是否处于锁定状态
  */
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
	
					//摩擦轮
          case CAN1_FEEDBACK_ID_203:                                                                                
					{
						Friction_Wheel_203.Real_Speed            =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]); 					      //转速
					}break ;
					
					case CAN1_FEEDBACK_ID_204:                                                                               
					{
						Friction_Wheel_204.Real_Speed          =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]);              //转速	
					}break ;
					
					
					//云台
					case CAN1_FEEDBACK_ID_Pitch:                                                                                
					{
						Pitch_Motor_206.Real_MechanicalAngle =(CAN1_Rx_Message.Data[0]<<8)|(CAN1_Rx_Message.Data[1]);              //机械角
						Pitch_Motor_206.Real_Speed            =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]); 					      //转速
            
						Position_Direction_Judge_Pitch();

					}break ;
					
					case CAN1_FEEDBACK_ID_Yaw:                                                                               
					{
						Yaw_Motor_205.Real_MechanicalAngle=(CAN1_Rx_Message.Data[0]<<8)|(CAN1_Rx_Message.Data[1]);              //机械角
						Yaw_Motor_205.Real_Speed          =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]);              //转速

						Position_Direction_Judge();
					}break ;	

					default:
					{}
					break ;	   
				}
			}
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);		
	}
}

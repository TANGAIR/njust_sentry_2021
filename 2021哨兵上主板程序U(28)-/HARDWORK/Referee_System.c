 #include "main.h"
/***********************裁判系统相关数据结构体定义***********************/
 //裁判系统数据格式
 RefereeInfo_TypeDef 				   RefereeInfor;
 
 
 
 //比赛信息
 ext_game_state_t              Game_State; 
 //比赛结果
 extGameResult_t     				   GameResult; 
 
 
 
 //机器人状态
 ext_game_robot_status_t       GameRobotState;
 //实时功率热量
 ext_power_heat_data_t  			 PowerHeatData; 
 //伤害状态
 ext_robot_hurt_t      				 RobotHurt;
 //实时射击信息
 ext_shoot_data_t      				 ShootData;



 //其他机器人血量数据
 ext_game_robot_HP_t           GameRobotHP;
 //飞镖发射状态
 ext_dart_status_t             DartStatus;
 //飞镖发射口倒计时
 ext_dart_remaining_time_t     DartRemainingTime;
 
 
 
 //机器人增益
 ext_buff_t       				     BuffMusk;
 //机器人 RFID 状态
 ext_rfid_status_t     				 RfidDetect;
 //场地事件数据
 ext_event_data_t              EventData;
 
 
 
 //机器人位置
 ext_game_robot_pos_t    			 GameRobotPos;
 //学生机器人间通信
 robot_interactive_data_t      ShowData;


 //小地图
 ext_robot_command_t Small_Map;


//
Enemy_HP My_Enemy_HP;



//每次进入中断，标志位取反，若一个标志位持续时间很长，则认为没进入中断，此时限制射频
short usart3_toggle_flag=0; 
//危险缓冲能量
float Power_Danger=30;
//最大底盘功率
float Power_Max=200;


//弹丸发射数量
int number_1=0; 

//比赛射击限制标志
int Game_Allow_Shoot_Flag=0;

//无人机起飞标志//1起飞，0未飞
int Plane_Fly_Flag=0;                            

//裁判系统数据接收数组长度
#define JUDGE_MAX_LENGTH  200

//裁判系统接收数组
uint8_t JUDGE_RX_BUFF1[JUDGE_MAX_LENGTH];
uint8_t JUDGE_RX_BUFF2[JUDGE_MAX_LENGTH];

//两个数组接收的数据个数
uint16_t JUDGE_BUFF1_RX_NUMBER=0;
uint16_t JUDGE_BUFF2_RX_NUMBER=0;

//用户数据发送数组长度
#define referee_tx_len  200
//用户数据上传帧数组
unsigned char referee_upload[referee_tx_len];

//枪口信息
#define UP_SHOOT_ID 2
#define DOWN_SHOOT_ID 0


//无敌时间,单位秒
#define INVINCEBLE_TIME 11

float Up_Bullet_Speed=26;
float Down_Bullet_Speed=26;

int My_Outpost_HP=1000;
char Output_On=1;



int YAW_RIGHT=0;
char KEY_ON_FLAG=0;



/*空闲BUFF枚举*/
typedef enum{
      BUFF_NO1=0,
      BUFF_NO2=1
}BUFF_NO;
//空闲BUFF
BUFF_NO  Free_BUFF_NO;   

//DMA结构体，因为在两个地方被使用所以采用宏定义
DMA_InitTypeDef   dma;


/*************************************数据格式转换函数*************************************/ 
/**
 * @Function : 位数转换
 * @Input    : void
 * @Output   : void
 * @Notes    : 8位-32位 采用共用体结构将各数组数据填入连续的一段内存中再一并读出
 * @Copyright: Aword
 **/
 float bit8TObit32(uint8_t *change_info)
{
	union
	{
    float f;
		char  byte[4];
	}u32val;
    u32val.byte[0]=change_info[0];
    u32val.byte[1]=change_info[1];
    u32val.byte[2]=change_info[2];
    u32val.byte[3]=change_info[3];
	return u32val.f;
}

u8 bit32TObit8(int index_need,int bit32)
{
	union
	{
    int  f;
		u8  byte[4];
	}u32val;
   u32val.f = bit32;
	return u32val.byte[index_need];
}

int16_t bit8TObit16(uint8_t *change_info)
{
	union
	{
    uint16_t f;
		char  byte[2];
	}u16val;
    u16val.byte[0]=change_info[0];
    u16val.byte[1]=change_info[1];
	return u16val.f;
}


/*************************************辅助计算函数函数*************************************/ 
/**
 * @Function : 敌方存活判断函数
 * @Input    : void
 * @Output   : void
 * @Notes    : 每秒执行一次，0代表死亡或无敌，1代表存活，从低位到高位依次是1~5号，7号
 * @Copyright: Aword
 **/
u8 Enemy_Alive_Judge(void)
{
	//1号
	if(My_Enemy_HP.Hero_1==0)My_Enemy_HP.Hero_1_Invinceble_Time=INVINCEBLE_TIME;
	if(My_Enemy_HP.Hero_1_Invinceble_Time>0)My_Enemy_HP.Hero_1_Invinceble_Time--;
	//2号
	if(My_Enemy_HP.Engineering_2==0)My_Enemy_HP.Engineering_2_Invinceble_Time=INVINCEBLE_TIME;
	if(My_Enemy_HP.Engineering_2_Invinceble_Time>0)My_Enemy_HP.Engineering_2_Invinceble_Time--;
	//3号
	if(My_Enemy_HP.Standar_3==0)My_Enemy_HP.Standar_3_Invinceble_Time=INVINCEBLE_TIME;
	if(My_Enemy_HP.Standar_3_Invinceble_Time>0)My_Enemy_HP.Standar_3_Invinceble_Time--;
	//4号
	if(My_Enemy_HP.Standar_4==0)My_Enemy_HP.Standar_4_Invinceble_Time=INVINCEBLE_TIME;
	if(My_Enemy_HP.Standar_4_Invinceble_Time>0)My_Enemy_HP.Standar_4_Invinceble_Time--;
	//5号
	if(My_Enemy_HP.Standar_5==0)My_Enemy_HP.Standar_5_Invinceble_Time=INVINCEBLE_TIME;
	if(My_Enemy_HP.Standar_5_Invinceble_Time>0)My_Enemy_HP.Standar_5_Invinceble_Time--;
	
	//只有当血量大于0且不在复活后10秒的时候才能被打
	return (((My_Enemy_HP.Hero_1>0)&&(My_Enemy_HP.Hero_1_Invinceble_Time==0)?1:0)||\
		      (((My_Enemy_HP.Engineering_2)&&(!My_Enemy_HP.Engineering_2_Invinceble_Time)?1:0)<<1)||\
		      (((My_Enemy_HP.Standar_3)&&(!My_Enemy_HP.Standar_3_Invinceble_Time)?1:0)<<2)||\
		      (((My_Enemy_HP.Standar_4)&&(!My_Enemy_HP.Standar_4_Invinceble_Time)?1:0)<<3)||\
		      (((My_Enemy_HP.Standar_5)&&(!My_Enemy_HP.Standar_5_Invinceble_Time)?1:0)<<4)||\
			    (((My_Enemy_HP.Sentry_7)?1:0)<<5)\
	       );

}

/**
 * @Function : 敌方作战力量血量排序函数
 * @Input    : void
 * @Output   : void
 * @Notes    : 英雄代号01，3号步兵10，四号步兵11，五号步兵00，被打优先级1>3>4>5
 * @Copyright: Aword
 **/
u8 Enemy_Sort(void)
{
   char Hero_1_Remove=0;//0，2，4，6；0，2，2，6；0，2，4，4
	 char Standar_3_Remove=0;
   char Standar_4_Remove=0;
   char Standar_5_Remove=-2;

   //将四个机器人的血量对比，如果大于一个就移位
	 for(char j=0;j<=3;j++)
	 {
		 if(My_Enemy_HP.Hero_1>*(&My_Enemy_HP.Hero_1+2*j))Hero_1_Remove+=2;
		 if(My_Enemy_HP.Standar_3>*(&My_Enemy_HP.Hero_1+2*j))Standar_3_Remove+=2;
		 if(My_Enemy_HP.Standar_4>*(&My_Enemy_HP.Hero_1+2*j))Standar_4_Remove+=2;
		 if(My_Enemy_HP.Standar_5>=*(&My_Enemy_HP.Hero_1+2*j))Standar_5_Remove+=2;//五号后被打
	 }
	 
	 if((Hero_1_Remove==Standar_3_Remove)&&(Standar_3_Remove==Standar_4_Remove))
	 {
	  Standar_3_Remove+=2;
		Standar_4_Remove+=4; 
	 }
	 else if(Hero_1_Remove==Standar_3_Remove)Standar_3_Remove+=2;
	 else if(Standar_3_Remove==Standar_4_Remove)Standar_4_Remove+=2;
	 else if(Hero_1_Remove==Standar_4_Remove)Standar_4_Remove+=2;
	 
	 

		 
	 
	 return ((0<<(Hero_1_Remove+1)||1<<(Hero_1_Remove))||\
					(1<<(Standar_3_Remove+1)||0<<(Standar_3_Remove))||\
					(1<<(Standar_4_Remove+1)||1<<(Standar_4_Remove))||\
					(0<<(Standar_5_Remove+1)||0<<(Standar_5_Remove))\
				  );


}
	





/***********************************************裁判系统数据函数***********************************************/ 
/**
  * @brief  串口3初始化函数
  * @param  void
  * @retval void
  * @notes  开启串口3空闲中断，中断优先级为 6
  */
void USART3_InitConfig(void)
{	
  GPIO_InitTypeDef   GPIO_InitStruct;
	USART_InitTypeDef  USART_InitStruct;
	NVIC_InitTypeDef   NVIC_InitStruct;
	
	/*enabe clocks*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	/*open the alternative function*/
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3);
	
	/*Configure PD8,PD9 as GPIO_InitStruct1 input*/
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	USART_InitStruct.USART_BaudRate            = 115200;
	USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits            = USART_StopBits_1;
	USART_InitStruct.USART_Parity              = USART_Parity_No;
	USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART3,&USART_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel                    = USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  = 6;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority         = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd                 = ENABLE ;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART3,ENABLE);		
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);  //使能串口空闲中断


}

/**
  * @brief  串口3接收DMA初始化
  * @param  void
  * @retval void
  * @notes  结构体定义使用宏定义因为中断函数中还要使用，使用DMA1_Stream1_DMA_Channel_4,
            最初时，将裁判系统返回的信息存在JUDGE_RX_BUFF1
  */
void JudgeSys_DMA_InitConfig(void)
{
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	
	DMA_DeInit(DMA1_Stream1);
	DMA_StructInit(&dma);
	
	dma.DMA_Channel             =DMA_Channel_4;             //通道4
	dma.DMA_PeripheralBaseAddr  =(uint32_t)&(USART3->DR);
	dma.DMA_Memory0BaseAddr     =(uint32_t)JUDGE_RX_BUFF1;
	dma.DMA_DIR                 =DMA_DIR_PeripheralToMemory;
	dma.DMA_BufferSize          =JUDGE_MAX_LENGTH;          //一次传输的数据量的大小
	dma.DMA_PeripheralInc       =DMA_PeripheralInc_Disable; 
	dma.DMA_MemoryInc           =DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize  =DMA_PeripheralDataSize_Byte;
	dma.DMA_MemoryDataSize      =DMA_MemoryDataSize_Byte;
	dma.DMA_Mode                =DMA_Mode_Circular;         //Mode应该选择Circular模式，Normal模式只能接收一次数据
	dma.DMA_Priority            =DMA_Priority_VeryHigh;     //DMA通道优先级
  dma.DMA_FIFOMode            =DMA_FIFOMode_Disable;      //不开FIFO
	dma.DMA_FIFOThreshold       =DMA_FIFOThreshold_Full;
	dma.DMA_MemoryBurst         =DMA_MemoryBurst_Single;    //外设突发传输配置
	dma.DMA_PeripheralBurst     =DMA_PeripheralBurst_Single;//外设突发传输配置
	DMA_Init(DMA1_Stream1,&dma);
	
	Free_BUFF_NO=BUFF_NO2;
	
	DMA_Cmd(DMA1_Stream1,ENABLE);	    //使能DMA1_Stream1
}

/**
 * @Function : 裁判系统初始化
 * @Input    : void
 * @Output   : void
 * @Notes    : USART3_TX-->PD9      USART3_RX-->PD8 ，初始化串口3与DMA，开启DAM数据流
 * @Copyright: Aword
 **/
void JudgeSys_Init(void)
{
  USART3_InitConfig();                          //串口初始化
	JudgeSys_DMA_InitConfig();                    //DMA初始化
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);  //开启串口DMA接收
}



/**
  * @brief  裁判系统数据解码函数
  * @param  UsartRx_Info：接收的一帧数据的头地址
  * @retval void
  * @notes  在裁判系统数据校验函数中被调用，最后其实是在串口3接收中断中被调用
            对裁判系统发来的信息进行解码，结合下板发过来的数据对哨兵的底盘模式进行设置。

            机器人ID，用于判断阵营、
            剩余血量，用于判断受到什么打击、
            底盘输出，用于判断底盘是否被断电

            底盘缓冲能量，用于设置设置 最大功率、游走速度、以及其他速度

            受打击的装甲序号加上下板传来的Down_To_Up_TypeStruct.Yaw_Angle_Real，用于判断是否被许多敌人打击，
            设置底盘模式，同时设置Up_To_Down_TypeStruct.Get_Hit_flag

            弹丸速度的更新，用于判断发弹数number_1++;

            无人机标志，用于切换应对无人机

  */
void RefereeInfo_Decode(uint8_t *UsartRx_Info)
{
	static int Get_Hit_Armor_Last=1; //上一次被打击的装甲板
	static int Bullet_Speed_Last=0;  //上一次弹丸射速
	int Angle_Direction=0;           //云台角度位置
	int Pander_ID=0;                 

	
	
	//判断信息类型
	RefereeInfor.CmdID = bit8TObit16(&UsartRx_Info[5]);
	
	
	switch(RefereeInfor.CmdID)
	{
		/*比赛信息进程*/
		case 0x0001:          
		{		                
			 Game_State.game_type               = UsartRx_Info[7]&0x0f;                   //当前比赛类型，低四位			
		   Game_State.game_progress           = UsartRx_Info[7]>>4;			                //当前比赛阶段，高四位
			 Game_State.stage_remain_time       = bit8TObit16(&UsartRx_Info[8]);		      //当前阶段剩余时间
			//4：对战中
			if( Game_State.game_progress==4)
			{
				Game_Allow_Shoot_Flag=1;
			}
			else 
			{
				Game_Allow_Shoot_Flag=0;
				//Chassis_Mode=Chassis_Normal;
			}
	  }
    break ;
		
		
		/*比赛结果  获胜方*/
		case 0x0002:
		{
			GameResult.winner=UsartRx_Info[7];                    
		}
		break;
		
		
		case 0x0003://1s
		{
			GameRobotHP.red_1_robot_HP=UsartRx_Info[7];
			GameRobotHP.red_2_robot_HP=UsartRx_Info[9];
			GameRobotHP.red_3_robot_HP=UsartRx_Info[11];
			GameRobotHP.red_4_robot_HP=UsartRx_Info[13];
			GameRobotHP.red_5_robot_HP=UsartRx_Info[15];
			GameRobotHP.red_7_robot_HP=UsartRx_Info[17];
			GameRobotHP.red_outpost_HP=UsartRx_Info[19];
			GameRobotHP.red_base_HP=UsartRx_Info[21];
			GameRobotHP.blue_1_robot_HP=UsartRx_Info[23];
			GameRobotHP.blue_2_robot_HP=UsartRx_Info[25];
			GameRobotHP.blue_3_robot_HP=UsartRx_Info[27];	
			GameRobotHP.blue_4_robot_HP=UsartRx_Info[29];
			GameRobotHP.blue_5_robot_HP=UsartRx_Info[31];	
      GameRobotHP.blue_7_robot_HP=UsartRx_Info[33];			
      GameRobotHP.blue_outpost_HP	=UsartRx_Info[35];	
      GameRobotHP.blue_base_HP	=UsartRx_Info[37];	
		
    //获得敌方血量			
    if(Up_To_Down_TypeStruct.Which_Color_I_Am==1)
		{
		  My_Enemy_HP.Hero_1         =GameRobotHP.red_1_robot_HP;
			My_Enemy_HP.Engineering_2  =GameRobotHP.red_2_robot_HP;
		  My_Enemy_HP.Standar_3      =GameRobotHP.red_3_robot_HP;
		  My_Enemy_HP.Standar_4      =GameRobotHP.red_4_robot_HP;
		  My_Enemy_HP.Standar_5      =GameRobotHP.red_5_robot_HP;
		  My_Enemy_HP.Sentry_7       =GameRobotHP.red_7_robot_HP;
		
		}
		else
    {
		  My_Enemy_HP.Hero_1         =GameRobotHP.blue_1_robot_HP;
			My_Enemy_HP.Engineering_2  =GameRobotHP.blue_2_robot_HP;
		  My_Enemy_HP.Standar_3      =GameRobotHP.blue_3_robot_HP;
		  My_Enemy_HP.Standar_4      =GameRobotHP.blue_4_robot_HP;
		  My_Enemy_HP.Standar_5      =GameRobotHP.blue_5_robot_HP;
		  My_Enemy_HP.Sentry_7       =GameRobotHP.blue_7_robot_HP;
		}
		
		
		//获取存活数据
		Up_To_Down_TypeStruct.Enemy_State=Enemy_Alive_Judge();
		//血量优先级
		Up_To_Down_TypeStruct.Enemy_HP_HL=Enemy_Sort();

			
		}
		break;
	
		
		
		/*机器人状态数据*/
		//用到的主要是ID用于判断阵营、剩余血量用于判断受到什么打击、底盘输出用于判断底盘是否被断电
		case 0x0201:
		{
			 GameRobotState.robot_id                      =UsartRx_Info[7];                  //机器人ID
			 GameRobotState.robot_level                   =UsartRx_Info[8];                  //机器人等级
			 GameRobotState.remain_HP                     =bit8TObit16(&UsartRx_Info[9]);    //机器人剩余血量
			 GameRobotState.max_HP                        =bit8TObit16(&UsartRx_Info[11]);   //机器人上限血量
			 GameRobotState.shooter_id1_17mm_cooling_rate =bit8TObit16(&UsartRx_Info[13]);   //6 2  机器人 1 号 17mm 枪口每秒冷却值
			 GameRobotState.shooter_id1_17mm_cooling_limit=bit8TObit16(&UsartRx_Info[15]);   //8 2  机器人 1 号 17mm 枪口热量上限
			 GameRobotState.shooter_id1_17mm_speed_limit  =bit8TObit16(&UsartRx_Info[17]);   //10 2 机器人 1 号 17mm 枪口上限速度 单位 m/s
			 GameRobotState.shooter_id2_17mm_cooling_rate =bit8TObit16(&UsartRx_Info[19]);   //12 2 机器人 2 号 17mm 枪口每秒冷却值
			 GameRobotState.shooter_id2_17mm_cooling_limit=bit8TObit16(&UsartRx_Info[21]);   //14 2 机器人 2 号 17mm 枪口热量上限
			 GameRobotState.shooter_id2_17mm_speed_limit  =bit8TObit16(&UsartRx_Info[23]);   //16 2 机器人 2 号 17mm 枪口上限速度 单位 m/s
			 GameRobotState.shooter_id1_42mm_cooling_rate =bit8TObit16(&UsartRx_Info[25]);   //18 2 机器人 42mm 枪口每秒冷却值
			 GameRobotState.shooter_id1_42mm_cooling_limit=bit8TObit16(&UsartRx_Info[27]);   //20 2 机器人 42mm 枪口热量上限
			 GameRobotState.shooter_id1_42mm_speed_limit  =bit8TObit16(&UsartRx_Info[29]);   //22 2 机器人 42mm 枪口上限速度 单位 m/s
			 GameRobotState.chassis_power_limit           =bit8TObit16(&UsartRx_Info[31]);   //24 2 机器人底盘功率限制上限
			 GameRobotState.mains_power_gimbal_output     =(UsartRx_Info[33]&0x01);          //26 1 主控电源输出情况：0 bit：gimbal 口输出： 1 为有 24V 输出，0 为无 24v 输出；
			 GameRobotState.mains_power_chassis_output    =((UsartRx_Info[33]&0x02)>>1);     //                       1 bit：chassis 口输出：1 为有 24V 输出，0 为无 24v 输出；
			 GameRobotState.mains_power_shooter_output    =((UsartRx_Info[33]&0x04)>>1);     //                       2 bit：shooter 口输出：1 为有 24V 输出，0 为无 24v 输出；
			
			
			//颜色判断，阵营判断
			if(GameRobotState.robot_id>99)//如果机器人的ID>99则1，也就是说机器人是蓝方的时候就是1，红方时候是0
			{
				Up_To_Down_TypeStruct.Which_Color_I_Am=1;//我方为蓝方
				My_Outpost_HP=GameRobotHP.blue_outpost_HP;
					
			}
			else 
			{
			  Up_To_Down_TypeStruct.Which_Color_I_Am=0;
				My_Outpost_HP=GameRobotHP.red_outpost_HP;
			}
			
			
			
			//血量判断
			//如果剩余血量发生了变化
			if(Up_To_Down_TypeStruct.Remain_HP!=GameRobotState.remain_HP)
			{
				//若打击标志位，不为2
				if(Up_To_Down_TypeStruct.Get_Hit_flag!=2)
				{
					
					//若减少的血量大于100，说明受到大弹丸攻击
					if((Up_To_Down_TypeStruct.Remain_HP-GameRobotState.remain_HP)>=100) 
					{
						
						BeHurt_10s+=5;
						//设置弹丸类型为1
						Up_To_Down_TypeStruct.Bullet_Type=1;
						//底盘模式设置为受到大弹丸攻击
						if(Chassis_Mode!=Chassis_Up_Find_Target)
						Chassis_Mode=Chassis_Get_Big_Bullet;
						//模式持续时间为0
						Mode_Time=0;
						

						if(!Position_Collet_Flag)
						{
							if(Chassis_Motor_201_Number<SECOND_NOTE_NUM)
							{
								Formar_Place=1;
							}
							else if(Chassis_Motor_201_Number<THIRD_NOTE_NUM)
							{
								Formar_Place=2;
							}
							else Formar_Place=3;
							
							Position_Collet_Flag=1;
							Position_Collet_Time=0;
						}
						
					}
					//受到小弹丸打击
					else
					{
					 BeHurt_10s+=1;
					
					
					}
					
					
					
					//若血量减少为10~100，说明受到小弹丸攻击
					if(((Up_To_Down_TypeStruct.Remain_HP-GameRobotState.remain_HP)<50)&&((Up_To_Down_TypeStruct.Remain_HP-GameRobotState.remain_HP)>=10))  
					{
						//弹丸类型为2
						Up_To_Down_TypeStruct.Bullet_Type=2;
						//若底盘模式大于等于Chassis_Get_Small_Hurt，即为Chassis_Limite或Chassis_Normal
						if(Chassis_Mode>=Chassis_Get_Small_Hurt)
						{
							//将底盘模式设置为Chassis_Get_Small_Hurt
							Chassis_Mode=Chassis_Get_Small_Hurt;
							//模式设置时间为0
							Mode_Time=0;
							

							
							//位置收集，2019规则
							if(!Position_Collet_Flag)
							{
								if(Chassis_Motor_201_Number<SECOND_NOTE_NUM)
								{
									Formar_Place=1;
								}
								else if(Chassis_Motor_201_Number<THIRD_NOTE_NUM)
								{
									Formar_Place=2;
								}
								else Formar_Place=3;
								Position_Collet_Flag=1;
								Position_Collet_Time=0;
							}
							
							
							
						}
					}	
				}
			}
			
			//更新剩余血量
			Up_To_Down_TypeStruct.Remain_HP=GameRobotState.remain_HP;
			
			//底盘是否被断电
			if(GameRobotState.mains_power_chassis_output==1)
			{
				Up_To_Down_TypeStruct.Sentry_Satus=1;
			}
			else Up_To_Down_TypeStruct.Sentry_Satus=0;
		}
		break;
		
		
		/*功率热量信息*/
		//主要使用缓冲能量，根据它设置最大功率、游走速度、以及其他速度，每0.02秒发送一次
		case 0x0202:
		{
		  PowerHeatData.chassis_volt                 = bit8TObit16(&UsartRx_Info[7]); 		         //底盘输出电压
			PowerHeatData.chassis_current              = bit8TObit16(&UsartRx_Info[9]); 	           //底盘输出电流
			PowerHeatData.chassis_power                = bit8TObit32(&UsartRx_Info[11]); 	           //底盘输出功率
			PowerHeatData.chassis_power_buffer         = bit8TObit16(&UsartRx_Info[15]);             //底盘功率缓冲能量
			PowerHeatData.shooter_id1_17mm_cooling_heat= bit8TObit16(&UsartRx_Info[17]);		         // 1 号 17mm枪口热量
			PowerHeatData.shooter_id2_17mm_cooling_heat= bit8TObit16(&UsartRx_Info[19]);		         // 2 号 17mm枪口热量
			PowerHeatData.shooter_id1_42mm_cooling_heat= bit8TObit16(&UsartRx_Info[21]);             // 42mm 枪口热量
			
			
			
			
			//底盘功率设置
			if(PowerHeatData.chassis_power_buffer<=Power_Danger)//30
			{
				//最大功率为30
				Power_Max=25.0f;
        Swag_Speed=220;
			}
			else//否则如果缓冲能量大于30，瞬时
			{		
				Power_Max=((float)PowerHeatData.chassis_power_buffer-(float)Power_Danger)*0.55f;
				if(PowerHeatData.chassis_power_buffer>=120)Swag_Speed=320;
			}
			
			if(Power_Max>POWER_LIMIT) Power_Max=POWER_LIMIT;
			if(Power_Max<15) Power_Max=15;
			
		}
		break ;
		
			/*伤害类型\哪一块装甲受到伤害、*/
		//根据受到打击的装甲的序号与下板传来的数据判断是否受到许多敌人打击
		case 0x0206:
		{
			RobotHurt.armor_id         = UsartRx_Info[7]&0x0f;		  //受到伤害的装甲ID，0x0-0号装甲，0x1-1号装甲，0x2-2号装甲，0x3-3号装甲，0x4-4号装甲，0x5-5号装甲
			RobotHurt.hurt_type        = UsartRx_Info[7]>>4;        //血量变化类型，0x0受到攻击，0x1模块掉线

			//判断是否有许多敌人
			if(RobotHurt.hurt_type==0x0)
			{
				//根据下主板传回的云台角度值判断，云台是否面对前方，若面对前方则为0，不面对前方则为1
				Angle_Direction=(  (Down_To_Up_TypeStruct.Yaw_Angle_Real>90)&&(Down_To_Up_TypeStruct.Yaw_Angle_Real<270) ) ;
				//如果此时下云台反馈已经锁定敌人
				if(Down_To_Up_TypeStruct.Target_Locked)
				{
					//关于装甲板，初始化时云台在前方，前方装甲板ID设置为1
					//若云台面对的方向与被打击得装甲板朝向不同，则说明有很多敌人在打哨兵
					if(Angle_Direction == RobotHurt.armor_id)
					{
						//给下板发送打击标志位为3
						Up_To_Down_TypeStruct.Get_Hit_flag=3;	
						//设置云台模式为有许多敌人
						if(Chassis_Mode>=Chassis_Many_Enemy)
						{
							Chassis_Mode=Chassis_Many_Enemy;
							Mode_Time=0;
						}
					}
				}
				
				//上一次被打的装甲板与这次不同并且危险时间小于500且危险时间不为0，即在很短时间两块装甲板都被打击，Danger_Time每10毫秒增加1
				//说明还是有很多敌人
				if( (Get_Hit_Armor_Last!=RobotHurt.armor_id) &&(Danger_Time<500)&&(Danger_Time!=0) )
				{
					Up_To_Down_TypeStruct.Get_Hit_flag=3;	
					if(Chassis_Mode>=Chassis_Many_Enemy)
					{
						Chassis_Mode=Chassis_Many_Enemy;
						Mode_Time=0;
					}
				}	
			  //否则打击标注为等于被打击的装甲序号，0或者1
				else Up_To_Down_TypeStruct.Get_Hit_flag=RobotHurt.armor_id;
				Danger_Time=0;
				//更新上一次被打击的装甲序号
				Get_Hit_Armor_Last=RobotHurt.armor_id;
			}
		}
		break ;
		
		/*弹丸射击*/
		//用弹丸速度的更新判断发弹数
		case 0x0207:
		{
			ShootData.bullet_type   = UsartRx_Info[7];//弹丸类型，1:17mm弹丸；	2：42mm弹丸
		  ShootData.shooter_id    = UsartRx_Info[8];//发射机构 ID： 1：1 号 17mm 发射机构   2：2 号 17mm 发射机构   3：42mm 发射机构
			ShootData.bullet_freq   = UsartRx_Info[9];//弹丸射频
			ShootData.bullet_speed  = bit8TObit32(&UsartRx_Info[10]); //弹丸射速 
			
			//上
			if(ShootData.shooter_id==2)
			{
			  Up_Bullet_Speed=ShootData.bullet_speed;
			}
			//下
			else //if(ShootData.shooter_id==DOWN_SHOOT_ID)
			{
			  Down_Bullet_Speed=ShootData.bullet_speed;
			}
			
			
			//如果遥控器开启自动模式
			 if(DBUS.RC.Switch_Right==RC_SW_MID)
			 {
				 //弹丸速度更新，则说明发弹数量增加
				 if(Bullet_Speed_Last!=ShootData.bullet_speed)
				 {
					number_1++;
				 }
			 }
			 //更新弹丸速度
			 Bullet_Speed_Last=ShootData.bullet_speed;
		}
		break ;
		
		
		
		
		
			
		/*增益类型*/
		case 0x0204:
		{
			BuffMusk.power_rune_buff=bit8TObit16(&UsartRx_Info[7]);
		}
		break ;
		
			
		/*机器人位置信息*/
		case 0x0203:
		{
			GameRobotPos.x          =bit8TObit32(&UsartRx_Info[7]);		  			//位置X坐标值
			GameRobotPos.y          =bit8TObit32(&UsartRx_Info[11]);					//位置Y坐标值
			GameRobotPos.z          =bit8TObit32(&UsartRx_Info[15]);					//位置Z坐标值
			GameRobotPos.yaw        =bit8TObit32(&UsartRx_Info[19]);					//枪口朝向角度值
		}
		break ;
	
		/*用户自定义*/
		//应对无人机起飞
		case 0x0301:
		{
			Pander_ID=bit8TObit16(&UsartRx_Info[7]);
			if(Pander_ID==0x0233)
			{
				Plane_Fly_Flag=UsartRx_Info[13];
			}
			//如果无人机起飞，将底盘模式变为应对无人机起飞
			if(Plane_Fly_Flag==1)
			{
				if(Chassis_Mode!=Chassis_Plane_Fly)
				{
					Chassis_Mode=Chassis_Plane_Fly;
					Mode_Time=0;
				}
			}
			//否则恢复正常
			else 
			{
				if(Chassis_Mode==Chassis_Plane_Fly)
				{
					Chassis_Mode=Chassis_Normal;
				}
			}
		}
		
		/*小地图信息*/
		case 0x0303:
		{
			Small_Map.target_position_x           =bit8TObit32(&UsartRx_Info[7]);		  			//位置X坐标值
			Small_Map.target_position_y           =bit8TObit32(&UsartRx_Info[11]);					//位置Y坐标值
			Small_Map.target_position_z           =bit8TObit32(&UsartRx_Info[15]);					//位置Z坐标值
			Small_Map.commd_keyboard              =UsartRx_Info[19];					              //键盘信息
			Small_Map.target_robot_ID             =bit8TObit16(&UsartRx_Info[20]);          //目标机器人ID
			
//			if(KEY_ON_FLAG==0)
//			{
//			   if(Small_Map.commd_keyboard==)
//			
//			
//			
//			}
//					
			
		}
		break ;
		
		default:
		{}   
  }
}
/**
 * @Function : 裁判系统数据校验函数
 * @Input    : void
 * @Output   : void
 * @Notes    : 在串口3中断中被调用，找到空闲数组的数据帧头然后进行crc校验，
               通过之后调用裁判系统数据解码函数进行数据解码，解读当前一帧数据
 * @Copyright: Aword
 **/
void Judge_Data_Process(void)
{
	u16 i=0;
	u16 RxLength = 0;
	//如果空闲数组为数组1
	if(Free_BUFF_NO==BUFF_NO1)
	{
		//找到接收数组中的0xA5,即每一帧数据的起始点
		//因为程序是两个数组切换不断接收裁判系统的数据，一个数组中可能有多帧数据，
		//所以需要进行查找校验确定数据完整与准确
		while(JUDGE_RX_BUFF1[i]==0XA5)
		{
			//本帧数据长度
			RxLength=(JUDGE_RX_BUFF1[i+2]<<8)|JUDGE_RX_BUFF1[i+1]+9;
			//如果CRC校验通过，则调用裁判系统数据解码函数
			if (Verify_CRC16_Check_Sum(&JUDGE_RX_BUFF1[i], RxLength ))  RefereeInfo_Decode(&JUDGE_RX_BUFF1[i]);
			//清空数组1
			JUDGE_RX_BUFF1[i]=0;
			//下帧数据起始位置
			i+=RxLength;
		}
	}
	//如果空闲数组为2，同样找到帧头，进行校验，通过之后则解码
	else
	{
		while(JUDGE_RX_BUFF2[i]==0XA5)
		{
			RxLength=(JUDGE_RX_BUFF2[i+2]<<8)|JUDGE_RX_BUFF2[i+1]+9;//本帧数据长度
			if (Verify_CRC16_Check_Sum(&JUDGE_RX_BUFF2[i], RxLength ))  RefereeInfo_Decode(&JUDGE_RX_BUFF2[i]); 
			JUDGE_RX_BUFF1[i]=0;
			i+=RxLength;//下帧数据起始位置
		}
	}
}






/**
  * @brief  串口3接收中断服务函数
  * @param  void
  * @retval void
  * @notes  切换裁判系统数据接收数组，调用裁判系统数据校验函数，通过校验以后再调用
  */
void USART3_IRQHandler(void)    
{
	BaseType_t pxHigherPriorityTaskWoken;
	//用于清除串口空闲中断标志位 
	uint32_t temp=temp;
	//串口3切换标志位取反
	usart3_toggle_flag=!usart3_toggle_flag;
	
	if(USART_GetITStatus(USART3,USART_IT_IDLE)!=RESET)
	{ 
		 temp=USART3->SR;                //清除串口空闲中断标志位 
	 	 temp=USART3->DR;  
		 
		//向裁判系统数据解码任务发送通知，将其移至就绪列表
		 vTaskNotifyGiveFromISR(Referee_System_Task_Handler,&pxHigherPriorityTaskWoken);
		//进行任务切换
		 portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
 
	}
}
/**
  * @brief  裁判系统任务函数
  * @param  void
  * @retval void
  * @notes  初始化裁判系统，优先级为6，被串口3中断唤醒，中断优先级为6，关闭DMA数据流，切换裁判系统数据接收数组
            进行数据校验与解码
  */
void Referee_System_Task(void *pvParameters)
{
	uint32_t err;
	//裁判系统初始化
	JudgeSys_Init();
	vTaskDelay(200);
	
 while(1)
 {
//等待任务通知
	err=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);

	if(err==1)
	{
		DMA_Cmd(DMA1_Stream1,DISABLE);  //失能DMA1
	 //如果当前空闲数组为接收数组1
	 if(Free_BUFF_NO==BUFF_NO1)
	 {
		 //设置数组2接收到的数据个数为裁判系统最大数据长度减去剩余的数据
		 //获得DMA当前收到的字节数，因为DMA中断只有接收长度满时才会触发，所以要把剩余数据读出来
		 JUDGE_BUFF2_RX_NUMBER=JUDGE_MAX_LENGTH-DMA_GetCurrDataCounter(DMA1_Stream1);
		 //切换存储数组为数组1
		 dma.DMA_Memory0BaseAddr     =(uint32_t)JUDGE_RX_BUFF1;
		 //打开DMA数据流
		 DMA_Init(DMA1_Stream1,&dma);
		 //将空闲数组设为数组2
		 Free_BUFF_NO=BUFF_NO2;
	 }
	 //否则当空闲数组为数组2时，切换为数组1
	 else
	 {
		 JUDGE_BUFF1_RX_NUMBER=JUDGE_MAX_LENGTH-DMA_GetCurrDataCounter(DMA1_Stream1);
		 dma.DMA_Memory0BaseAddr     =(uint32_t)JUDGE_RX_BUFF2;
		 DMA_Init(DMA1_Stream1,&dma);
		 Free_BUFF_NO=BUFF_NO1;
	 }	
		 
	 DMA_SetCurrDataCounter(DMA1_Stream1,JUDGE_MAX_LENGTH);      //重新设置接收数据长度
	 DMA_Cmd(DMA1_Stream1,ENABLE);
	 
	 //裁判系统数据校验函数		 
	 Judge_Data_Process(); 
	}
 }		
}






/**
  * @brief  自定义数据上传函数
  * @param  Studentdata，用户定义的9字节数组指针
  * @retval void
  * @notes  使用串口3将用户传入的9字节数据上传
  */
void Student_Date_Upload( robot_interactive_data_t * Studentdata)
{
  unsigned char ucExpected = 0;
  uint16_t wExpected = 0;
  int i;
  //帧头结构  
  referee_upload[0] = 0xA5;//SOF   0xA5
  referee_upload[1] = 0x0D;//DataLength 13字节
  referee_upload[2] = 0x00;
  referee_upload[3] = 0x00;//Seq  !!!!!!!!!!
	
	//帧头8位一字节crc校验
  ucExpected = Get_CRC8_Check_Sum (referee_upload, 5-1, 0xff) ;//CRC8
  referee_upload[4] = ucExpected;//CRC8 
	
  //命令码 ID  
  referee_upload[5] = 0x03;
  referee_upload[6] = 0x01;
  //Data(12-Byte)
  referee_upload[ 7] = 0x02;
  referee_upload[ 8] = 0x00;
  referee_upload[ 9] = 0x00;
  referee_upload[10] = GameRobotState.robot_id;//机器人ID
  referee_upload[11] = 0x00;
  referee_upload[12] = 0x0e;
	//用户的9个字节的数据
  referee_upload[11] = Studentdata->data[0];
  referee_upload[12] = Studentdata->data[1];
  referee_upload[13] = Studentdata->data[2];
  referee_upload[14] = Studentdata->data[3];
  referee_upload[15] = Studentdata->data[4];
  referee_upload[16] = Studentdata->data[5];
  referee_upload[17] = Studentdata->data[6];
  referee_upload[18] = Studentdata->data[7];
  referee_upload[19] = Studentdata->data[8];	//(0<<5)|(1<<4)|(0<<3)|(0<<2)|(0<<1)|0;对应红、红、红、红、绿、红
	
  //FrameTail(2-Byte, CRC16)，帧尾16位两字节crc校验
  wExpected = Get_CRC16_Check_Sum ( referee_upload, 22 - 2, 0xffff);//CRC16 
  referee_upload[20] = wExpected & 0xFF;//CRC16  
  referee_upload[21] = (wExpected >> 8) & 0xFF;//CRC16 
	
   //清除标志位防止第一字节丢失
	USART_ClearFlag(USART3,USART_FLAG_TC);   
	
	//使用串口3发送22个字节的数据
  for (i=0;i<22;i++)
  {
		USART_SendData(USART3, referee_upload[i]);
		//判断每一个字节是否发出
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET){}
  }
}



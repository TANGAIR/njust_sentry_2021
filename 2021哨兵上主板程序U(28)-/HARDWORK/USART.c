/****************头文件包含区******************************/
#include "main.h"

/**********************************数据定义区*****************************/
int NUC_Lost_Time=0;

NUC_Data_Typedef    Up_NUC_Data_TypeStruct;
Up_To_Down_Typedef  Up_To_Down_TypeStruct;
Down_To_Up_Typedef  Down_To_Up_TypeStruct;

int8_t Up_To_DOWN_Tx[17];
int8_t Down_To_Up_Rx[10];
char   USART6Rx_Info[10];//串口三接收数组
char        Send_NUC[23];//串口三发送数组

int View_Enemy_ID=0;
char Bullet_Speed=0;

short Yaw_Limit =65*40;
short Pitch_Limit=35*20;


//串口7接收控制信息，
uint16_t My_Usart_Recieve_Data=2;
/*********************************************串口初始化区**********************************************/
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
// 发送数据
int fputc(int ch, FILE *f)
{
	while (USART_GetFlagStatus(UART7,USART_FLAG_TC) == RESET);//等待之前的字符发送完成
	USART_SendData(UART7, (uint8_t)ch);
	return (ch);
}
#endif
/**
  * @brief  初始化UART7
  * @param  void
  * @retval void
  * @notes  串口7打印初始化  UART7_RX -PE7  UART7_TX-PE8      
            打印中断优先级 7
  */
void UART7_Init(void)
{
	GPIO_InitTypeDef       GPIO_InitSturct;
	USART_InitTypeDef      USART_InitStruct;
	NVIC_InitTypeDef       NVIC_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7,ENABLE);
	
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_UART7);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_UART7);
	
	GPIO_InitSturct.GPIO_Pin=GPIO_Pin_7|GPIO_Pin_8;
	GPIO_InitSturct.GPIO_OType=GPIO_OType_PP;                                      //推挽输出
	GPIO_InitSturct.GPIO_Mode=GPIO_Mode_AF;                                        //复用模式
	GPIO_InitSturct.GPIO_PuPd=GPIO_PuPd_NOPULL;                                    //不上拉 
	GPIO_InitSturct.GPIO_Speed=GPIO_High_Speed;                                    //100MHZ
	GPIO_Init(GPIOE,&GPIO_InitSturct);
	
	USART_InitStruct.USART_BaudRate=9600;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;     //硬件流控制无
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;                       //收发模式
	USART_InitStruct.USART_Parity=USART_Parity_No;                                 //无奇偶校验位
	USART_InitStruct.USART_StopBits=USART_StopBits_1;                              //字节
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;                         //数据长度
	USART_Init(UART7,&USART_InitStruct);
	USART_Cmd(UART7,ENABLE);
	USART_ITConfig(UART7,USART_IT_RXNE,ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel=UART7_IRQn;                                    //中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;                                     //使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=7;                           //抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;                                  //子优先级
	NVIC_Init(&NVIC_InitStruct);	
}

/**
  * @brief  串口7打印中断
  * @param  void
  * @retval void
  * @notes  暂时没用
  */
void UART7_IRQHandler(void)
{
  if ( USART_GetITStatus( UART7, USART_IT_RXNE ) != RESET )
  { 
		My_Usart_Recieve_Data=UART7->DR;//(UART7);

  }
 
  USART_ClearFlag ( UART7,USART_IT_RXNE);
}






//

















/*********************************************上板与下板数据通信**********************************************************************/
/**
  * @brief  USART8初始化函数
  * @param  void
  * @retval void
  * @notes  USART8_TX-PE1      USART8_RX -----PE0   波特率115200
            发送用DMA1_Stream0，DMA_Channel_5，使用DMA1_Stream0_IRQHandler 优先级为 9
            接收用DMA1_Stream6，DMA_Channel_5，使用UART8_IRQHandler 优先级为 9
            上板发送给下板的数据赋初值，下板发送的值的初始化
  */
void USART8_Init(void)
{
   GPIO_InitTypeDef       GPIO_InitSturct;
	 USART_InitTypeDef      USART_InitStruct;
	 NVIC_InitTypeDef       NVIC_InitStruct;
   DMA_InitTypeDef        DMA_InitStruct;

/*******************************************时钟使能*******************************/	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART8,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);

/*******************************************IO口复用*******************************/		
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource0,GPIO_AF_UART8);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource1,GPIO_AF_UART8);

/*******************************************IO口配置******************************/		
	GPIO_InitSturct.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitSturct.GPIO_OType=GPIO_OType_PP;                                      //推挽输出
	GPIO_InitSturct.GPIO_Mode=GPIO_Mode_AF;                                        //复用模式
	GPIO_InitSturct.GPIO_PuPd=GPIO_PuPd_NOPULL;                                    //不上拉 
	GPIO_InitSturct.GPIO_Speed=GPIO_High_Speed;                                    //100MHZ
	GPIO_Init(GPIOE,&GPIO_InitSturct);

/*******************************************串口配置*******************************/	
	USART_InitStruct.USART_BaudRate=115200;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;     //硬件流控制无
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;                       //收发模式
	USART_InitStruct.USART_Parity=USART_Parity_No;                                 //无奇偶校验位
	USART_InitStruct.USART_StopBits=USART_StopBits_1;                              //字节
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;                         //数据长度
	USART_Init(UART8,&USART_InitStruct);
	USART_Cmd(UART8,ENABLE);
	
	USART_ITConfig(UART8,USART_IT_IDLE,ENABLE);

/*******************************************中断配置******************************/	
	NVIC_InitStruct.NVIC_IRQChannel=UART8_IRQn;                                    //中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;                                     //使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=9;                           //抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;                                  //子优先级
	NVIC_Init(&NVIC_InitStruct);	
	
	NVIC_InitStruct.NVIC_IRQChannel=DMA1_Stream0_IRQn;                             //中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;                                     //使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=9;                           //抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;                                  //子优先级
	NVIC_Init(&NVIC_InitStruct);	
	
	
/************************************************DMA发送********************************/
	USART_DMACmd(UART8,USART_DMAReq_Tx,ENABLE);                                    //串口DMA使能
	
	DMA_Cmd(DMA1_Stream0,DISABLE);                                                 //DMA失能（只有在DMA失能的情况才能对DMA寄存器进行操作）
	DMA_DeInit(DMA1_Stream0);                                                      //把DMA配置寄存器设置为缺省值
	
  DMA_InitStruct.DMA_Channel=DMA_Channel_5;                                      //通道配置（通道5）
	DMA_InitStruct.DMA_BufferSize=17;                                              //传输数据数目                
	DMA_InitStruct.DMA_DIR=DMA_DIR_MemoryToPeripheral;                             //数据传输方向(存储器到外设）                   
	DMA_InitStruct.DMA_FIFOMode=ENABLE;                                            //使能FIFO模式
	DMA_InitStruct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;               //阈值1/4      
	DMA_InitStruct.DMA_Memory0BaseAddr=(uint32_t)Up_To_DOWN_Tx;                    //存储器数据地址
	DMA_InitStruct.DMA_MemoryBurst=DMA_MemoryBurst_Single;                         //存储器突发配置
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;                     //存储器数据格式（字节）
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;                             //内存地址递增
	DMA_InitStruct.DMA_Mode=DMA_Mode_Normal;                                       //是否循环发送（不循环）
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&(UART8->DR);                 //外设地址 
	DMA_InitStruct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;                 //外设突发配置         
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;             //外设数据格式
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;                    //外设地址不递增
	DMA_InitStruct.DMA_Priority=DMA_Priority_VeryHigh;                             //优先级非常高      
	DMA_Init(DMA1_Stream0,&DMA_InitStruct);
	
	DMA_ITConfig(DMA1_Stream0,DMA_IT_TC,ENABLE);
/****************************************************DMA接收******************************************/	
	USART_DMACmd(UART8,USART_DMAReq_Rx,ENABLE);
	
	DMA_Cmd(DMA1_Stream6,DISABLE);
	DMA_DeInit(DMA1_Stream6);
	
	DMA_InitStruct.DMA_Channel=DMA_Channel_5;                                      //通道配置（通道5）
	DMA_InitStruct.DMA_BufferSize=15;                                              //传输数据数目                
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralToMemory;                             //数据传输方向（外设——存储器）                   
	DMA_InitStruct.DMA_FIFOMode=ENABLE;                                            //使能FIFO模式
	DMA_InitStruct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;               //阈值1/4      
	DMA_InitStruct.DMA_Memory0BaseAddr=(uint32_t)Down_To_Up_Rx;                    //存储器数据地址
	DMA_InitStruct.DMA_MemoryBurst=DMA_MemoryBurst_Single;                         //存储器突发配置
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;                     //存储器数据格式（字节）
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;                             //内存地址递增
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;                                     //是否循环发送（循环，普通模式只能接受一次）
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&(UART8->DR);                  //外设地址 
	DMA_InitStruct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;                 //外设突发配置         
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;             //外设数据格式
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;                    //外设地址不递增
	DMA_InitStruct.DMA_Priority=DMA_Priority_VeryHigh;                             //优先级非常高      
	DMA_Init(DMA1_Stream6,&DMA_InitStruct);
	DMA_Cmd(DMA1_Stream6,ENABLE);
	
	
	
  Up_To_Down_TypeStruct.Sentry_Satus=1;                                          //1-底盘未断电，0-底盘断电                                      
	Up_To_Down_TypeStruct.Chassis_Real_Position=0;                                 //以基地为第一视角，从左到右，-3~3
	Up_To_Down_TypeStruct.Get_Hit_flag=2;                                          //2-没有受到攻击，0-装甲0受到攻击，1-装甲1受到攻击，3-两个装甲都受到了攻击
	Up_To_Down_TypeStruct.Bass_Satus=2;                                           
	Up_To_Down_TypeStruct.Remain_HP=600;                                           //剩余血量除以50
	Up_To_Down_TypeStruct.Chassis_Real_Number=0;                                   
	Up_To_Down_TypeStruct.Shoot_Allow_Flag=0;                                      //0-不允许发射，1-允许发射 
	Up_To_Down_TypeStruct.Chassis_Speed_Real=0;
	Up_To_Down_TypeStruct.Locked_Flag=0;
	Up_To_Down_TypeStruct.Bullet_Speed=0;
	
	Down_To_Up_TypeStruct.Target_Locked=0;                                         //0-没有找到目标，1-找到目标，2-锁定目标
	Down_To_Up_TypeStruct.Enemy_Armor_ID=0;
	Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag=0;
	Down_To_Up_TypeStruct.Yaw_Angle_Real=0;
	Down_To_Up_TypeStruct.Distace =250;
}



/**
  * @brief  USART8 DMA发送中断
  * @param  void
  * @retval void
  * @notes  优先级为2
            每次发送完一次数据后产生此中断，关闭dma流，同时将发送数据的长度重新设置
  */
void DMA1_Stream0_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream0,DMA_IT_TCIF0)==SET)
	{
	 DMA_Cmd(DMA1_Stream0,DISABLE);
	 DMA_SetCurrDataCounter(DMA1_Stream0,17);
	}
	DMA_ClearITPendingBit(DMA1_Stream0,DMA_IT_TCIF0);
	DMA_ClearFlag(DMA1_Stream0,DMA_FLAG_TEIF0);
}
/**
  * @brief  USART8接收中断
  * @param  void
  * @retval void
  * @notes  优先级为2
            串口8的数据寄存器接收到数据之后产生中断，在此中断中将接收数据长度重新设置，
            然后调用接收数据解码函数，40ms一次
  */
void UART8_IRQHandler(void)
{
	BaseType_t pxHigherPriorityTaskWoken;
	char num=0;
	if(USART_GetITStatus(UART8,USART_IT_IDLE)==SET)
	{
		//清除串口空闲中断（中文手册712页）
		 num = UART8->SR;
     num = UART8->DR; 
		 num = 10-DMA_GetCurrDataCounter(DMA1_Stream6);
		 
		 //重新设置接收数据长度
		 DMA_Cmd(DMA1_Stream6,DISABLE); 
		 DMA_SetCurrDataCounter(DMA1_Stream6,10);     
		 DMA_Cmd(DMA1_Stream6,ENABLE);
		
	   vTaskNotifyGiveFromISR(Down_To_Up_Decode_Task_Handler,&pxHigherPriorityTaskWoken);		 
		 portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);	 	
	}
}
//








/**************************************************上板到下板通信数据分析***********************************

Get_Hit_flag：装甲板被打击标志位，0，即面向基地的0号装甲板被打，1，即面向外侧的1号装甲板被打击，2，即没有被打击，3，即两块装甲板都被打击
              在上主板程序，裁判系统解码函数中根据裁判系统给出的被打装甲板ID以及下板传来的云台Yaw_Angle_Real进行设置；
              在上主板程序，定时器7中断服务函数中，12没有受到攻击则设置为2
              在上主板程序，定时器7中断服务函数中，用于判断敌方危险等级
              在上主板程序，底盘自动控制应对系列函数中，用于判断底盘往哪一边跑
              在下主板程序，串口8接收解码函数中，用于设置云台模式

Which_Color_I_Am：己方的颜色，0，表示红方，1表示蓝方
                  在上主板程序，裁判系统解码函数中，根据裁判系统给出的机器人ID进行设置；
                  在下主板程序，使用NUC发送函数，发给NUC；

Shoot_Allow_Flag：发射允许标志位，0，不允许发射，1允许发射
                  在上主板程序，拨盘控制函数中，遥控器模式以及裁判系统的热量、裁判系统比赛开始标志位、遥控器允许、NUC允许、硬件没有堵弹进行设置；
                  在下主板程序，摩擦轮控制任务函数中使用，用于控制舵机是否打开；

Chassis_Real_Number：202电机的圈数，来自于Chassis_Motor_201_Number，哨兵在最左侧时圈数为0，向右移动电机圈数计数增加
										 在上主板程序，CAN1接收中断服务函数中，根据对202电机的角度判断进行增加或者减少；                     
										 在上主板程序，底盘转向系列函数中，根据左右红外传感器的值进行最左侧值和最右侧值校准消除累计误差；  
										 在上主板程序，定时器7中断服务函数中，每10ms根据左右红外传感器的值进行最左侧值和最右侧值校准消除累计误差；										 
										 在上主板程序，底盘转向系列函数中，用于判断是否到了临界值；
										 在上主板程序，定时器7中断服务函数中，用于判断底盘在哪一个位置被打击；
										 在上主板程序，裁判系统解码函数中，用于判断底盘在哪一个位置被打击；
                     在下主板程序，并没有被使用；
										 
Sentry_Satus：哨兵状态标志位，1，说明底盘没有被断电，0，说明底盘被断电
              在上主板程序，裁判系统解码函数中，根据裁判系统给出的机器人底盘输出电压进行设置；            
              在下主板程序，并没有被使用；
							
Bullet_Speed：弹丸速度							
              在上主板程序，USART8_Send函数中，根据=(ShootData.bulletSpeed-20.0f)*10.0f进行设置；
					    在下主板程序，并没有被使用；
							
Locked_Flag：拨弹盘锁死标志位，1，说明拨弹盘锁死，0，说明拨弹盘没有被锁死
             在上主板程序，定时器7中断服务函数中，每7秒判断一次是否卡弹设置；  
						 在下主板程序，并没有被使用；					 
   
*********************************************************************************************************************************/  
/**
  * @brief  USART8发送任务函数 
  * @param  void
  * @retval void
  * @notes  10ms一次
	          优先级为5，在定时器7中被调用，每10ms发送一次，将一些数据经过处理之后发送给下板，共10个数据，帧头为0x7A，帧尾为0X7B  
						
            每10毫秒，底盘模式恢复、位置采集标志位恢复、攻击标志位恢复、向下主板发送消息、红外限位 右侧PH12-B,左侧PH11-C         
  */
int Left_Sensor_Number=1;
int Right_Sensor_Number=1;	

void USART8_Send_Task(void *pvParameters)
{
   USART8_Init();
   uint32_t err;
	
	vTaskDelay(10);
	
	//红外传感器
  while(1)
	{
		//等待任务通知
		err=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		if(err==1)
		{
			//发送数据处理
			//底盘,底盘摩擦轮周长235.62mm
			Up_To_Down_TypeStruct.Chassis_Real_Position=(float)Chassis_Motor_201_Number/19.0f*235.62f+100;	//单位mm
			
			Right_Ultrasonic_Distance=3823-Up_To_Down_TypeStruct.Chassis_Real_Position;
		
			Up_To_Down_TypeStruct.Chassis_Speed_Real=(235.62f/19.0f/60.0f*(float)Chassis_Motor_201.Real_Speed);//单位mm/s
			
			Up_To_Down_TypeStruct.Bullet_Speed=Down_Bullet_Speed;
			
			
			//数据帧结构
			//帧头
			Up_To_DOWN_Tx[0]=0x7A;
			Up_To_DOWN_Tx[1]=Up_To_Down_TypeStruct.Get_Hit_flag;                       
			Up_To_DOWN_Tx[2]=(Up_To_Down_TypeStruct.Which_Color_I_Am)|(Output_On<<1)|(Swing_Flag<<2);
			Up_To_DOWN_Tx[3]=Up_To_Down_TypeStruct.Shoot_Allow_Flag;
			Up_To_DOWN_Tx[4]=shortTou8(0,Up_To_Down_TypeStruct.Chassis_Real_Position);
			Up_To_DOWN_Tx[5]=shortTou8(1,Up_To_Down_TypeStruct.Chassis_Real_Position);
			Up_To_DOWN_Tx[6]=Up_To_Down_TypeStruct.Sentry_Satus;                       
			Up_To_DOWN_Tx[7]=Up_To_Down_TypeStruct.Bullet_Speed;
			Up_To_DOWN_Tx[8]=Up_To_Down_TypeStruct.Locked_Flag;
      Up_To_DOWN_Tx[9]=shortTou8(0,Up_To_Down_TypeStruct.Chassis_Speed_Real);
			Up_To_DOWN_Tx[10]=shortTou8(1,Up_To_Down_TypeStruct.Chassis_Speed_Real);		
			Up_To_DOWN_Tx[11]=Up_NUC_Data_TypeStruct.Armor_ID;
			Up_To_DOWN_Tx[12]=Up_To_Down_TypeStruct.Enemy_State;
			Up_To_DOWN_Tx[13]=Up_To_Down_TypeStruct.Enemy_HP_HL;		
			Up_To_DOWN_Tx[14]=shortTou8(0,Chassis_Accel);
			Up_To_DOWN_Tx[15]=shortTou8(1,Chassis_Accel);
			
			
			//帧尾
			Up_To_DOWN_Tx[16]=0x7B;
			
			//启动DMA数据流发送数据
			DMA_Cmd(DMA1_Stream0,ENABLE);
			
/////////////////////////////////////////////////////////////////////////////////////////			
			//底盘各模式持续时间
			if( Chassis_Mode!=Chassis_Normal )
			{
				Mode_Time++;
			
				if(Chassis_Mode==Chassis_Get_Big_Fire)
				{
					if(Mode_Time>3000)
					{
						Chassis_Mode=Chassis_Normal;					
					}
				}
				else if(Chassis_Mode==Chassis_Get_Big_Bullet||(Chassis_Mode==Chassis_Get_Small_Hurt))
				{
					if(Mode_Time>3000)
					{
						Chassis_Mode=Chassis_Normal;					
					}
				}
				else if((Chassis_Mode==Chassis_Up_Find_Target)||(Chassis_Mode==Chassis_Find_Target))
				{
					if(Mode_Time>100)
					{
						Chassis_Mode=Chassis_Normal;					
					}
				}
				
				
				//其他模式如,Chassis_Get_Big_Fire,Chassis_Get_Small_Hurt,Chassis_Limite,持续9s
				else if(Chassis_Mode!=Chassis_Normal)
				{
					if(Mode_Time>400)
					{
						Chassis_Mode=Chassis_Normal;					
					}
				}
				
				
				
				
				
				else if(Chassis_Mode==Chassis_Lock_Target)
				{
					if(Mode_Time>300)
					{
						Chassis_Mode=Chassis_Normal;					
					}
				}
				
				
				
				//发现敌人持续5s
				else if(Chassis_Mode==Chassis_Find_Target)
				{
					if(Mode_Time>500)
					{
						Chassis_Mode=Chassis_Normal;					
					}
				}
				//刚停了3s模式，持续10秒
				else if(Chassis_Mode==Chassis_Stop_3S)
				{
					if(Mode_Time>1000)
					{
						Chassis_Mode=Chassis_Normal;					
					}
				}
			
				
				//被很多敌人进攻，持续25s
				else
				{
					if(Mode_Time>2500)
					{
						Chassis_Mode=Chassis_Normal;	
					}
				}
			}
			//当底盘模式为正常模式时，模式时间为0
			else Mode_Time=0;
			
			//位置采集
			//开机前5s进行位置收集
			if(Position_Collet_Flag)
			{
				Position_Collet_Time++;
				if(Position_Collet_Time>500)
				{
					Position_Collet_Flag=0;
				}
			}
			
			//如果在12s内没受到攻击，设置受攻击标志位为2即没受到攻击
			if(Up_To_Down_TypeStruct.Get_Hit_flag!=2)
			{
				Danger_Time++;
				if(Danger_Time>1200)
				{
					Danger_Time=0;
					Up_To_Down_TypeStruct.Get_Hit_flag=2;
				}
			}
			
			
			
			
			//每10ms读取红外传感器的值
			Left_Sensor_Number=GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_11);
			Right_Sensor_Number=GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_12);
	
	    //从己方基地往外看，若左侧传感器的值为0，即检测到
			if(Left_Sensor_Number==0)
			{
				//位置限制标志位为-1
				Position_Limit_Flag=-1;
	
				Chassis_Position=0;
				Chassis_Motor_201_Number=0;
			}
			else if(Right_Sensor_Number==0)
			{
				Position_Limit_Flag=1;
				Chassis_Position=4;
				Chassis_Motor_201_Number=RIGHT_LIMIT_NUM;
			}
			else
			{
				Position_Limit_Flag=0;
			}	
			
			
			
			//每10ms向超声波传感器发送11us的高电平
			GPIO_SetBits(GPIOF,GPIO_Pin_1|GPIO_Pin_10 );
			delay_us(11);
			GPIO_ResetBits(GPIOF,GPIO_Pin_1|GPIO_Pin_10 );
			
			
			
			
		}	
	}
}

/***********************************下板到上板通信数据分析*************************************

Target_Locked：NUC目标锁定标志位，0，即没有找到目标，1，识别到目标，2，锁定目标，初始值在串口8初始化函数中为0
               在下板程序，NUC数据解码任务函数中，根据NUC发过来的距离进行设置；
               在上板程序，串口8数据解码函数中，用于设置底盘模式

Distance：敌方距离
          在下板程序，NUC数据解码任务函数中，根据NUC发过来的敌方距离进行设置；
          在上板程序，拨盘控制函数中，使用自动模式时用于设置拨盘转速
                 
Enemy_Armor_ID：敌方装甲板的ID号
                在下板程序，NUC数据解码任务函数中，根据NUC发过来的=NUC_Data_TypeStruct.Armor_Number进行设置；
                在上板程序，并没有使用。

Yaw_Angle_Real：电机朝向角，范围0-360，来自于Yaw_Real_Angle
                在下板程序，CAN1电机位置判断函数中，根据电机反馈的角度值进行设置；
                在上板程序，串口8数据解码函数和裁判系统数据解码函数中，使用，用于判断是否被很多敌人打击
								
NUC_Shoot_Allow_Flag：自动模式下NUC是否要求射击，0，即允许射击，1，即不允许射击
                      在下板程序，NUC数据解码任务函数中，根据NUC发过来的第一个数据进行设置；
                      在下板程序，定时器7中断服务函数中，根据是否成功接收到NUC数据进行设置；
                      在上板程序，拨盘控制函数中，使用自动模式时判断是否允许射击

******************************************************************************************************/

/**
  * @brief  下主板到上主板数据解码任务函数 
  * @param  void
  * @retval void
  * @notes 接收下云台数据，根据装甲板是否被打击，以及敌方距离设置底盘模式         
  */
void Down_To_Up_Decode_Task(void *pvParameters)
{
	//云台方向
	int Angle_Direction=0;
  uint32_t err;
	while(1)
	{
		//等待任务通知
		err=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		if(err==1)
		{
			//数据校验
			if( (Down_To_Up_Rx[0]==0x7A) && (Down_To_Up_Rx[9]==0x7B) )
			{
				//数据获取
				Down_To_Up_TypeStruct.Target_Locked       =Down_To_Up_Rx[1];
				Down_To_Up_TypeStruct.Distace             =int8ToShort(Down_To_Up_Rx,2);
				Down_To_Up_TypeStruct.Enemy_Armor_ID      =Down_To_Up_Rx[4];
				Down_To_Up_TypeStruct.Yaw_Angle_Real      =int8ToShort(Down_To_Up_Rx,5);
				Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag=Down_To_Up_Rx[7];
				
				
				//根据下主板传回的云台角度值判断，云台是否面对前方，若面对前方则为0，不面对前方而面对基地侧则为1
				Angle_Direction=( (Down_To_Up_TypeStruct.Yaw_Angle_Real>90)&&(Down_To_Up_TypeStruct.Yaw_Angle_Real<270) ) ;
				
				//如果敌方的距离小于50则目标锁定等于2否则识别到敌人之后目标锁定应该等于1
				//敌方距离在50以内，说明已经识别到一个很近的敌人
				if(Down_To_Up_TypeStruct.Target_Locked==2)
				{ 
					//说明：0号装甲板在基地侧，1号装甲板在外侧
					//若云台面对的方向与被打击得装甲板朝向不同，则说明有很多敌人在打哨兵
					if(Angle_Direction == Up_To_Down_TypeStruct.Get_Hit_flag )
					{
						//前后两块装甲板都被打击
						Up_To_Down_TypeStruct.Get_Hit_flag=3;	
						if(Chassis_Mode!=Chassis_Many_Enemy)
						{
							Chassis_Mode=Chassis_Many_Enemy;
							Mode_Time=0;
						}
					}
					else Chassis_Mode=Chassis_Lock_Target;	
				}
				//当下板反馈回来的敌方距离还比较远时，当底盘模式为正常模式时切换底盘模式为发现敌人
				else if(Down_To_Up_TypeStruct.Target_Locked==1)
				{
					if(Chassis_Mode>=Chassis_Find_Target)
					{
						Chassis_Mode=Chassis_Find_Target;
						Mode_Time=0;
					}			
				}




				
	    }
	  }
 }
}

//
















/*********************************************上板与NUC数据通信**********************************************************************/
/**
  * @brief  USART6-NUC通信初始化
  * @param  void
  * @retval void
  * @notes  USART6_RX -----PG9    USART6_TX-----PG14     
            发送用DMA2_Stream6，DMA_Channel_5，使用DMA2_Stream6_IRQHandler
            接收用DMA2_Stream1，DMA_Channel_5，使用USART6_IRQHandler
  */
void NUC_Init(void)
{
  GPIO_InitTypeDef   GPIO_InitStruct;
	USART_InitTypeDef  USART_InitStruct;
	DMA_InitTypeDef    DMA_InitStruct;
	NVIC_InitTypeDef   NVIC_InitStruct;
	
	/*enabe clocks*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG| RCC_AHB1Periph_DMA2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
	
	/*open the alternative function*/
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6);
	
	/*Configure PB10,PB11 as GPIO_InitStruct1 input*/
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9|GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOG,&GPIO_InitStruct);
	
	USART_InitStruct.USART_BaudRate            = 115200;
	USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits            = USART_StopBits_1;
	USART_InitStruct.USART_Parity              = USART_Parity_No;
	USART_InitStruct.USART_Mode                = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART6,&USART_InitStruct);
	
	USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);
	
	USART_Cmd(USART6,ENABLE);		
	USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);
	
/* -------------- Configure NVIC ---------------------------------------*/
//接收中断
	NVIC_InitStruct.NVIC_IRQChannel                   =USART6_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =6;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        =0;
	NVIC_InitStruct.NVIC_IRQChannelCmd                =ENABLE ;
	NVIC_Init(&NVIC_InitStruct);
//发送后中断
	NVIC_InitStruct.NVIC_IRQChannel                   =DMA2_Stream7_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =9;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        =0;
	NVIC_InitStruct.NVIC_IRQChannelCmd                =ENABLE ;
	NVIC_Init(&NVIC_InitStruct);
	
/* ------------------------------------- Configure DMA  接收---------------------------------------*/

	USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);
	
	DMA_Cmd(DMA2_Stream1,DISABLE);
	DMA_DeInit(DMA2_Stream1);
	
	DMA_InitStruct.DMA_Channel=DMA_Channel_5;                                      //通道配置（通道5）
	DMA_InitStruct.DMA_BufferSize=15;                                              //传输数据数目                
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralToMemory;                             //数据传输方向（外设——存储器）                   
	DMA_InitStruct.DMA_FIFOMode=ENABLE;                                            //使能FIFO模式
	DMA_InitStruct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;               //阈值1/4      
	DMA_InitStruct.DMA_Memory0BaseAddr=(uint32_t)USART6Rx_Info;                    //存储器数据地址
	DMA_InitStruct.DMA_MemoryBurst=DMA_MemoryBurst_Single;                         //存储器突发配置
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;                     //存储器数据格式（字节）
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;                             //内存地址递增
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;                                     //是否循环发送（循环，普通模式只能接受一次）
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&(USART6->DR);                 //外设地址 
	DMA_InitStruct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;                 //外设突发配置         
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;             //外设数据格式
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;                    //外设地址不递增
	DMA_InitStruct.DMA_Priority=DMA_Priority_VeryHigh;                             //优先级非常高      
	DMA_Init(DMA2_Stream1,&DMA_InitStruct);
	DMA_Cmd(DMA2_Stream1,ENABLE);

///*********************************************DMA发送************************************/
	
	USART_DMACmd(USART6,USART_DMAReq_Tx,ENABLE);
	
  DMA_Cmd(DMA2_Stream7,DISABLE);
	
  DMA_DeInit(DMA2_Stream7);                                                 //为DMA配置通道
	DMA_InitStruct.DMA_Channel           =DMA_Channel_5;
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&(USART6->DR);          //起始地址
	DMA_InitStruct.DMA_Memory0BaseAddr   =(uint32_t)Send_NUC;                 //存储变量
	DMA_InitStruct.DMA_DIR               =DMA_DIR_MemoryToPeripheral;         //传输方向
	DMA_InitStruct.DMA_BufferSize        =27; //缓冲区长度
	DMA_InitStruct.DMA_PeripheralInc     =DMA_PeripheralInc_Disable;          //外设递增模式
	DMA_InitStruct.DMA_MemoryInc         =DMA_MemoryInc_Enable;               //内存递增模式
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;        //DMA访问时每次操作的数据长度
	DMA_InitStruct.DMA_MemoryDataSize    =DMA_PeripheralDataSize_Byte;        //DMA访问时每次操作的数据长度
	DMA_InitStruct.DMA_Mode              =DMA_Mode_Normal;                    //传输模式：连续不断
	DMA_InitStruct.DMA_Priority          =DMA_Priority_VeryHigh;              //优先级别
  DMA_InitStruct.DMA_FIFOMode			     =DMA_FIFOMode_Enable;
	DMA_InitStruct.DMA_FIFOThreshold     =DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStruct.DMA_MemoryBurst       =DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_PeripheralBurst   =DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream7,&DMA_InitStruct);
	
	DMA_ITConfig(DMA2_Stream7,DMA_IT_TC,ENABLE);
	
  Up_NUC_Data_TypeStruct.Armor_Type=0;
	Up_NUC_Data_TypeStruct.Enemy_distance=0;
	Up_NUC_Data_TypeStruct.Pitch_Angle=0;
	Up_NUC_Data_TypeStruct.Yaw_Angle=0;
	Up_NUC_Data_TypeStruct.Armor_ID=0;
	delay_ms(3);
}

/**
  * @brief  NUC DMA数据流发送中断服务函数
  * @param  void
  * @retval void
  * @notes  每次启动发送任务函数之后启动这个中断服务函数，每1ms执行一次
            中断中将DMA数据流关闭并且重新设置DNA数据长度
            
  */
void DMA2_Stream7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream7,DMA_IT_TCIF7)==SET)
	{
		
	 //关闭dma数据流
	 DMA_Cmd(DMA2_Stream7,DISABLE);
	 //重新设置dma数据长度
	 DMA_SetCurrDataCounter(DMA2_Stream7,23);
	
	 	
	}
	DMA_ClearITPendingBit(DMA2_Stream7,DMA_IT_TCIF7);
	DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TEIF7);
}

/**
  * @brief  NUC 串口6数据接收中断服务函数
  * @param  void
  * @retval void
  * @notes  串口六接收到nuc发来的数据之后先将dma数据流关闭重新设置数据长度之后再打开数据流
            然后转到nuc数据处理任务，DMA：从USART6->DR到USART6Rx_Info
  */
void USART6_IRQHandler(void)
{
	
	BaseType_t pxHigherPriorityTaskWoken;
	char num=0;
	if(USART_GetITStatus(USART6,USART_IT_IDLE)==SET)
	{
		 num = USART6->SR;
     num = USART6->DR; //清除串口空闲中断（中文手册712页）
		 num++;            //没有意义，只是消除警告
		
		 DMA_Cmd(DMA2_Stream1,DISABLE); 
		 DMA_SetCurrDataCounter(DMA2_Stream1,10);      //重新设置接收数据长度
		 DMA_Cmd(DMA2_Stream1,ENABLE);                 //传输，将串口3数据寄存器的数据传输到USART6Rx_Info
		
		//向NUC数据解码任务发送通知，将其移至就绪列表
		 vTaskNotifyGiveFromISR(NUC_Decode_Task_Handler,&pxHigherPriorityTaskWoken);
		//进行任务切换
		 portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
	}	
}

//






/**
  * @brief  串口六数据发送任务函数
  * @param  void
  * @retval void
  * @notes  将云台yaw轴和pitch轴的数据经过处理之后储存Send_NUC数组中
            然后启动dma数据流将数据发送出去，在陀螺仪数据解码函数中被调用
            每1ms发送一次Send_NUC到Send_NUC，再发出
            每500毫秒，用0.5秒损失的血量判断是否被大弹丸打击，同时进行位置判断
            每1000毫秒，根据1s内减少的血量与受攻击状态，判断危险等级
            每7秒，判断一次是否卡弹，拨弹盘反转时间超过560ms说明卡弹了，这时将发射锁定标志位设为1
            每22秒，判断是否长时间卡弹，如果卡弹则Locked_Flag=0
            
  */
/******************************上板到上NUC通信数据分析****************************


Yaw_Angle：YAW轴的角度
           电机加上圈数计算的真实角度
           在云台自动控制函数和云台手动控制函数中被使用，用于控制云台YAW轴转动

Pitch_Angle：PITCH轴的角度，
             在串口3发送函数中由(Pitch_Motor_206.Real_MechanicalAngle-5900)*(1.46502259f)+500得到

Yaw_Speed：YAW轴的速度，单位为陀螺仪原始单位
           在串口3发送函数中由IMU_Real_Data.Gyro_Y*100;得到
					 
Pitch_Speed：PITCH轴的速度，由电机速度转化为°/s之后再除以3乘以100得到

Find_Target_1：云台是否处于找到敌人的状态，0，说明没有处于找到敌人的模式，1，说明处于找到敌人的模式

Which_Color_I_Am：己方颜色，0，为红色，1为蓝色
                  
Enemy_Armor_ID：下NUC识别到的敌人的装甲板序号
*************************************************************************************/
	int     Yaw_Angle;
	short   Pitch_Angle,
				  Pitch_Speed,
			  	Yaw_Speed;

void UP_Send_NUC_Task(void *pvParameters)
{ 
	
	Tick_TIM7_Init (100);  //定时器7初始化，热量判断，底盘位置判断
	uint32_t err;

	char  Find_Target_1=0;
	
  //剩余血量
	static u32 TimeTick1ms=0;
	static int HP_Last_500ms=600;
	static int HP_Last_1000ms=600;
	
 while(1)
	{
		//等待任务通知
		err=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		if(err==1)
		{
		//printf("123123123");
		//	USART6->DR=1;
			//内部计数++
			TimeTick1ms++;

			//用于确认云台是否处于找到敌人的状态
			if(PTZ_Mode==PTZ_Find_Enemy)
			{
				Find_Target_1=0;
			}
			else Find_Target_1=1;
			
			// Yaw轴的角度就是电机编码器的真实角度,让初始时，yaw为0   
			Yaw_Angle=(float)(Yaw_Real_MechanicalAngle-6120)/8192.0f*180.0f*10;
			
			// Pitch轴的角度就是真实的机械角处理之后发送过去.8192对应180度，单位：度*10
	    Pitch_Angle=(float)(Pitch_Motor_206.Real_MechanicalAngle)/8192.0f*180.0f*10;
	
	     //单位：度/s*10
			Yaw_Speed=((float)Yaw_Motor_205.Real_Speed*360.0f)/(60.0f)*10.0f;
	
	     //单位：度/s*10
	    Pitch_Speed=((float)Pitch_Motor_206.Real_Speed*360.0f)/(60.0f)*10.0f;
			
			Bullet_Speed =Up_Bullet_Speed;
			
			Send_NUC[0]=0x7a;
			Send_NUC[1]=Bullet_Speed;
			Send_NUC[2]=bit32TObit8(0,(short)Yaw_Angle);
			Send_NUC[3]=bit32TObit8(1,(short)Yaw_Angle);	
			Send_NUC[4]=bit32TObit8(2,(short)Yaw_Angle);
			Send_NUC[5]=bit32TObit8(3,(short)Yaw_Angle);	
			Send_NUC[6]=shortTou8(0,(short)Pitch_Speed);//以下速度数据视觉都没有使用
			Send_NUC[7]=shortTou8(1,(short)Pitch_Speed);
			Send_NUC[8]=shortTou8(0,(short)Yaw_Speed);
			Send_NUC[9]=shortTou8(1,(short)Yaw_Speed);	
			//第9号数据用于判断现在哨兵云台是不是处于找到敌人的状态以及自身的颜色是什么，00表示红方，已找到敌人
		  Send_NUC[10]=shortTou8(0,(short)Up_To_Down_TypeStruct.Chassis_Real_Position );//位置，单位mm
	    Send_NUC[11]=shortTou8(1,(short)Up_To_Down_TypeStruct.Chassis_Real_Position );	
			Send_NUC[12]=(Find_Target_1)|(Up_To_Down_TypeStruct.Which_Color_I_Am<<1)|(Output_On<<2)|(Swing_Flag<<3);//己方颜色从上主板中发送过来的，机器人是红方时候是0，蓝方的时候就是1。
			Send_NUC[13]=Down_To_Up_TypeStruct.Enemy_Armor_ID;
    	Send_NUC[14]=shortTou8(0,(short)Up_To_Down_TypeStruct.Chassis_Speed_Real );//速度，单位mm/s
	    Send_NUC[15]=shortTou8(1,(short)Up_To_Down_TypeStruct.Chassis_Speed_Real );
			Send_NUC[16]=Up_To_Down_TypeStruct.Enemy_State;  
	    Send_NUC[17]=Up_To_Down_TypeStruct.Enemy_HP_HL;
			Send_NUC[18]=shortTou8(0,(short)Pitch_Angle);
			Send_NUC[19]=shortTou8(1,(short)Pitch_Angle);
			Send_NUC[20]=shortTou8(0,Chassis_Accel);
			Send_NUC[21]=shortTou8(1,Chassis_Accel);
			
			
			
			Send_NUC[22]=0x7b;

	
			
		  DMA_Cmd(DMA2_Stream7,ENABLE);		//******************************************************************************************


     /*每500ms*/ 
		//用0.5秒损失的血量判断是否被大弹丸打击，同时进行位置判断
		if(TimeTick1ms%500==0)
		{


			//如果在0.5s之内损失了大于40的血量，则切换底盘模式为受到大弹丸打击
			 if(HP_Last_500ms-GameRobotState.remain_HP>=40)
			 {
				if(Chassis_Mode>=Chassis_Get_Big_Fire)
				{
					Chassis_Mode=Chassis_Get_Big_Fire;
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
			}	
			 //更新0.5s前血量
       HP_Last_500ms=GameRobotState.remain_HP;			
		}	
		/*每1s*/
		//根据1s内减少的血量与受攻击状态，判断危险等级
		if(TimeTick1ms%1000==0)
		{
			//在1秒内损失血量大于30且观测到敌人ID且没有腹背受敌
			if((HP_Last_1000ms-GameRobotState.remain_HP>=30)&&(View_Enemy_ID)&&(Up_To_Down_TypeStruct.Get_Hit_flag!=3))
			{
				//敌人危险等级为1
				Enemy_Danger_Level[View_Enemy_ID]=1;
			}	
			//更新1秒的剩余血量
			HP_Last_1000ms=GameRobotState.remain_HP;
		}
		/*每7s*/
		//每7秒判断一次是否卡弹，拨弹盘反转时间超过560ms说明卡弹了，这时将发射锁定标志位设为1
		if(TimeTick1ms%7000==0)
		{
			if(Locked_Time>8)
			{
				//长时间锁定时间增加1
				LongTime_Locked_Times++;
				Locked_Flag=0;
				//发射锁定标志位为1
				Up_To_Down_TypeStruct.Locked_Flag=1;
			}
			
			//锁定时间归零
			Locked_Time=0;
			
			
			BeHurt_10s=0;
			
			
		}
		/*每22s*/
		//判断是否长时间卡弹，如果卡弹则Locked_Flag=0
		if(TimeTick1ms%22000==0)
		{
			//长时间锁定标志位为小于10，即累计小于10个7秒有卡弹现象，让锁定Locked_Flag等于1
			if(LongTime_Locked_Times<=10)
			{
				if(!Locked_Flag)
				{
					Locked_Flag=1;
				}
			}
			//否则有超过10个7秒有卡弹现象
			else Locked_Flag=0;
		}
		
				/*每30s*/

		if(TimeTick1ms%30000==0)
		{
			Sway_Buttle=1;
		}
		
		
	}
 }
}
/*******************************上NUC到上板通信数据分析*****************************

USART6Rx_Info[0]：允许发射数据标志位，0x1f，说明允许发射，其他，则不允许发射
                  在NUC数据解码任务函数中使用，用于给Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag赋值

USART6Rx_Info[1]、USART6Rx_Info[2]：Yaw_Angle，YAW轴控制移动角度，相当于遥控器的ch2通道值，是控制云台要移动的值
                                    在云台自动控制函数中用于控制云台移动

USART6Rx_Info[3]、USART6Rx_Info[4]：Pitch_Angle，PITCH轴控制移动角度，相当于遥控器的ch3通道值，是控制云台要移动的值
                                    在云台自动控制函数中用于控制云台移动

USART6Rx_Info[5]：Armor_Number，敌方装甲ID用于区分不同种类的机器人
									
USART6Rx_Info[6]：Armor_Type，装甲板类型，0，没识别到装甲板，1，识别到小装甲板，2，识别到大装甲板
                  在NUC数据解码任务函数中使用，用于切换云台模式
									
USART6Rx_Info[7]、USART6Rx_Info[8]：Enemy_distance
                                    敌方距离,在发射控制函数中用于设置拨盘转速
 
USART6Rx_Info[9]：NUC数据校验位，
                  在在NUC数据解码任务函数中使用，用于USART6Rx_Info[1] + USART6Rx_Info[3] + USART6Rx_Info[5])  % 255 == USART6Rx_Info[9]校验								

**********************************************************************************/
/**
  * @brief  Nuc数据解码任务
  * @param  void
  * @retval void
  * @notes  每次接收完NUC发过来的数据时执行一次
            Nuc初始化：初始化串口6进行与nuc的通讯，进入NUC解码任务，当数据经过校验之后说明nuc已经连接上了，这个时候nuc丢失时间等于0；
            当NUC发过来的数据中的装甲板类型不为0的时候说明已经识别到了敌人，此时将云台模式切换为识别到敌人，
            如果发过来的装甲板类型是0的话就说明没有检测到装甲板，这个时候将云台控制模式切换为没有识别到敌人            
  */
	int NUC_Get_Time=0;
	short Last_Pitch_Angle=0;
void NUC_Decode_Task(void *pvParameters)
{
//  static	uint16_t NUC_Start_Time=0;
	uint32_t err;
	int Lost_Times=0;     //丢失次数
	NUC_Init();
	vTaskDelay(200);
	while(1)
	{
		//等待任务通知
		err=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);

		if(err==1)
		{//数据校验
			if( ( USART6Rx_Info[0] + USART6Rx_Info[3] + USART6Rx_Info[5]) % 255 == USART6Rx_Info[9] ) 
			{
				// Nuc丢失时间等于0说明已经接受到了nuc的数据如果很长时间没有接收到nuc的数据,蜂鸣器会报警
				NUC_Lost_Time=0;
				
	
				//第0号数据为0xfe则告诉上板可以发射
				if( USART6Rx_Info[0]==(char)0x1f)    Up_NUC_Data_TypeStruct.NUC_Shoot_Allow_Flag=1;
				else                                 Up_NUC_Data_TypeStruct.NUC_Shoot_Allow_Flag=0;
				
				//装甲类型：0未识别到，1小装甲，2大装甲
				Up_NUC_Data_TypeStruct.Armor_Type     = USART6Rx_Info[6] ;
			
				//如果识别到
				if(Up_NUC_Data_TypeStruct.Armor_Type !=0)
				{
					NUC_Get_Time++;
					//Yaw轴数据解码
          Up_NUC_Data_TypeStruct.Yaw_Angle=u8ToShort(USART6Rx_Info,1);			
					//yaw限位
					if(Up_NUC_Data_TypeStruct.Yaw_Angle>=Yaw_Limit)Up_NUC_Data_TypeStruct.Yaw_Angle=Yaw_Limit;
					if(Up_NUC_Data_TypeStruct.Yaw_Angle<=-Yaw_Limit)Up_NUC_Data_TypeStruct.Yaw_Angle=-Yaw_Limit;
					
										
					//Pitch轴数据解码，
					Up_NUC_Data_TypeStruct.Pitch_Angle=u8ToShort(USART6Rx_Info,3); 
					if(Up_NUC_Data_TypeStruct.Pitch_Angle>=Pitch_Limit)Up_NUC_Data_TypeStruct.Pitch_Angle=Pitch_Limit;
					if(Up_NUC_Data_TypeStruct.Pitch_Angle<=-Pitch_Limit)Up_NUC_Data_TypeStruct.Pitch_Angle=-Pitch_Limit;
		
		
      	//	if(abs(Last_Pitch_Angle-Up_NUC_Data_TypeStruct.Pitch_Angle>=10))NUC_Get_Time=0;
					//跟随
//					if(NUC_Get_Time>=30)
//					{
//					  PID_Armor_Yaw_205_Mechanical.P=0.4; 
//					  PID_Armor_Yaw_205_Mechanical.I =0;
//						PID_Armor_Yaw_205_Mechanical.Iout =0;
//				

//            PID_Armor_Yaw_205_Mechanical.IMax=9;
//					  PID_Armor_Yaw_205_Mechanical.PMax=50;
////	          
////						
////						
////        
////	       
////	       


//						
//						
//						
//						
//						PID_Armor_Pitch_206_Mechanical.P=0.4; 
//            PID_Armor_Pitch_206_Mechanical.PIDOutMax=10;
//						
//						//5
//						PID_Armor_Pitch_206_Speed.P=300;                     //10
//						PID_Armor_Pitch_206_Speed.I=1;                      //0.01

//		        PID_Armor_Pitch_206_Speed.IMax=5000;
//						PID_Armor_Pitch_206_Speed.PIDOutMax=3000;


//						
//					}
//  				else//捕获
//					{
//              PID_Armor_Yaw_205_Mechanical.P=0.08; 
//              PID_Armor_Yaw_205_Mechanical.I=0.001;
//              PID_Armor_Yaw_205_Mechanical.PMax=30000;





//					   	PID_Armor_Pitch_206_Mechanical.P=0.23;                   //5
//              PID_Armor_Pitch_206_Mechanical.PIDOutMax=1000;
//					
//              PID_Armor_Pitch_206_Speed.P=270;
//						  PID_Armor_Pitch_206_Speed.I=2;                      //0.01
//              PID_Armor_Pitch_206_Speed.IMax=2250;
//						  PID_Armor_Pitch_206_Speed.PIDOutMax=30000;

//					  
//					}						
					
					Last_Pitch_Angle=Up_NUC_Data_TypeStruct.Pitch_Angle;
					
					
				
					

         	//敌方装甲板序号
					Up_NUC_Data_TypeStruct.Armor_ID= USART6Rx_Info[5];
					
					
					//敌方距离
					Up_NUC_Data_TypeStruct.Enemy_distance =u8ToShort(USART6Rx_Info,7);
								
					//目标丢失次数等于0
					Lost_Times=0;
					//设置云台模式为找到敌人
					PTZ_Mode=PTZ_Find_Enemy;									
				}
				
				//如果没有识别到装甲板，装甲类型为0，云台模式切换为没有找到敌人
				else  
				{
					NUC_Get_Time=0;
					Up_NUC_Data_TypeStruct.Yaw_Angle      = 0;
					Up_NUC_Data_TypeStruct.Pitch_Angle    = 0;
					Up_NUC_Data_TypeStruct.Enemy_distance = 0;
					
					Up_NUC_Data_TypeStruct.Armor_ID=0x00;
		
					//没有识别到装甲板说明说明目标已经丢失这个时候丢失时间增加
					Lost_Times++;
					if(Lost_Times>50)
					{
						//切换云台模式从找到敌人变成丢失敌人
						if(PTZ_Mode==PTZ_Find_Enemy)
						{
							//切换云台模式从找到敌人变成丢失敌人
							
							PTZ_Mode=PTZ_Lost_Enemy;
						}						
					}
				}
			}
		}
	}
}




//


/*********************************************数据类型转换函数**********************************************************************/



short u8ToShort(char a[],char b)
{
	union
	{
		short i;
		char byte[2];
	}u16val;
	u16val.byte[0]=a[b];
	u16val.byte[1]=a[b+1];
	return u16val.i;
}

char shortTou8(char bit,short data)
{
	union
	{
		short i;
		char byte[2];
	}u16val;
	u16val.i=data;
	return u16val.byte[bit];
}

int8_t shortToint8(char bit,short data)
{
	union
	{
		short i;
		int8_t byte[2];
	}u16val;
	u16val.i=data;
	return u16val.byte[bit];
}
short int8ToShort(int8_t a[],char b)
{
	union
	{
		short i;
		int8_t byte[2];
	}u16val;
	u16val.byte[0]=a[b];
	u16val.byte[1]=a[b+1];
	return u16val.i;
}

char floatTou8(char bit,float data)
{
	union
	{
		float i;
		char byte[4];
	}u16val;
	u16val.i=data;
	return u16val.byte[bit];
}





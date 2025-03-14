#include "main.h"

/**********************************数据定义区*****************************/
Up_To_Down_Typedef  Up_To_Down_TypeStruct;
Down_To_Up_Typedef  Down_To_Up_TypeStruct;
NUC_Data_Typedef    Down_NUC_Data_TypeStruct;

int8_t Up_To_DOWN_Rx[17];//上主板到下主板，接收数据数组
int8_t Down_To_Up_Tx[10];//下主板到上主板，发送数据数组
char   Usart3Rx_Info[10];//串口三接收数组
//char        Send_NUC[20];//串口三发送数组
char        Send_NUC[23];//串口三发送数组

short Chassis_Accel=0;

char Swing_Flag=0;
/**********************************串口6打印***************************/


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
//用于打印数据
int fputc(int ch, FILE *f)
{
	while (USART_GetFlagStatus(USART6,USART_FLAG_TC) == RESET);//等待之前的字符发送完成
	USART_SendData(USART6, (uint8_t)ch);
	return (ch);
}
#endif
/**
  * @brief  初始化USART6
  * @param  void
  * @retval void
  * @notes  串口6打印初始化    USART6_TX-PG14      USART6_RX -----PG9
  */
void USART6_Init(void)
{
	GPIO_InitTypeDef       GPIO_InitSturct;
	USART_InitTypeDef      USART_InitStruct;
	NVIC_InitTypeDef       NVIC_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
	
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6);
	
	GPIO_InitSturct.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_14;
	GPIO_InitSturct.GPIO_OType=GPIO_OType_PP;                                      //推挽输出
	GPIO_InitSturct.GPIO_Mode=GPIO_Mode_AF;                                        //复用模式
	GPIO_InitSturct.GPIO_PuPd=GPIO_PuPd_NOPULL;                                    //不上拉 
	GPIO_InitSturct.GPIO_Speed=GPIO_High_Speed;                                    //100MHZ
	GPIO_Init(GPIOG,&GPIO_InitSturct);
	
	USART_InitStruct.USART_BaudRate=115200;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;     //硬件流控制无
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;                       //收发模式
	USART_InitStruct.USART_Parity=USART_Parity_No;                                 //无奇偶校验位
	USART_InitStruct.USART_StopBits=USART_StopBits_1;                              //字节
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;                         //数据长度
	USART_Init(USART6,&USART_InitStruct);
	USART_Cmd(USART6,ENABLE);
	USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel=USART6_IRQn;                                   //中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;                                     //使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=15;                           //抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;                                  //子优先级
	NVIC_Init(&NVIC_InitStruct);	
}


/**
  * @brief  串口6打印中断
  * @param  void
  * @retval void
  * @notes  暂时没用
  */
void USART6_IRQHandler(void)
{
  if ( USART_GetITStatus( USART6, USART_IT_RXNE ) != RESET )
  { //串口六接收到的数据个数
  }
  if ( USART_GetITStatus( USART6, USART_IT_ORE_RX ) != RESET )
  { 
		
  }
  USART_ClearFlag ( USART6,USART_IT_RXNE | USART_IT_ORE_RX );
}




/*********************************************USART8上下板数据通信*************************************************/


/**
  * @brief  初始化USART8
  * @param  void
  * @retval void
  * @notes  用于上下主板之间的通信，将Down_To_Up_Tx中的10个数据发送给上主板，从上主板接受10个数据储存到Up_To_DOWN_Rx，
            发送用DMA1_Stream0，DMA_Channel_5，使用DMA1_Stream0_IRQHandler
            接收用DMA1_Stream6，DMA_Channel_5，使用UART8_IRQHandler
            优先级都为 8
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
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=8;                           //抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;                                  //子优先级
	NVIC_Init(&NVIC_InitStruct);	
	
	NVIC_InitStruct.NVIC_IRQChannel=DMA1_Stream0_IRQn;                             //中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;                                     //使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=8;                           //抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;                                  //子优先级
	NVIC_Init(&NVIC_InitStruct);	
	
	
/************************************************DMA发送********************************/
	USART_DMACmd(UART8,USART_DMAReq_Tx,ENABLE);                                    //串口DMA使能
	
	DMA_Cmd(DMA1_Stream0,DISABLE);                                                 //DMA失能（只有在DMA失能的情况才能对DMA寄存器进行操作）
	DMA_DeInit(DMA1_Stream0);                                                      //把DMA配置寄存器设置为缺省值
	
   DMA_InitStruct.DMA_Channel=DMA_Channel_5;                                     //通道配置（通道5）
	DMA_InitStruct.DMA_BufferSize=15;                                              //传输数据数目                
	DMA_InitStruct.DMA_DIR=DMA_DIR_MemoryToPeripheral;                             //数据传输方向(存储器到外设）                   
	DMA_InitStruct.DMA_FIFOMode=ENABLE;                                            //使能FIFO模式
	DMA_InitStruct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;               //阈值1/4      
	DMA_InitStruct.DMA_Memory0BaseAddr=(uint32_t)Down_To_Up_Tx;                    //存储器数据地址
	DMA_InitStruct.DMA_MemoryBurst=DMA_MemoryBurst_Single;                         //存储器突发配置
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;                     //存储器数据格式（字节）
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;                             //内存地址递增
	DMA_InitStruct.DMA_Mode=DMA_Mode_Normal;                                       //是否循环发送（不循环）
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&(UART8->DR);                  //外设地址 
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
	DMA_InitStruct.DMA_Memory0BaseAddr=(uint32_t)Up_To_DOWN_Rx;                    //存储器数据地址
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

  Up_To_Down_TypeStruct.Sentry_Satus=1;                                          //1-底盘有电，0-底盘断电                                       
	Up_To_Down_TypeStruct.Chassis_Real_Position=0;                                 //以基地为第一视角，从左到右，0-4
	Up_To_Down_TypeStruct.Get_Hit_flag=2;                                          //2-没有受到攻击，0-装甲0受到攻击，1-装甲1受到攻击，3-两个装甲都受到了攻击
	Up_To_Down_TypeStruct.Bass_Status=2;                                           //2-100%防御，1-50%防御 ， 0-无防御                                        
	Up_To_Down_TypeStruct.Chassis_Real_Number=0;                                   //每过10s减一个数	
	
	Down_To_Up_TypeStruct.Target_Locked=0;                                         //0-没有找到目标，1-找到目标，2-锁定目标
	Down_To_Up_TypeStruct.Target_Position=0;
	Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag=0;
	Down_To_Up_TypeStruct.Yaw_Angle_Real=0;
	Down_To_Up_TypeStruct.Enemy_Armor_ID=0;
}

/**
  * @brief  USART8 DMA发送中断
  * @param  void
  * @retval void
  * @notes  每次发送完一次数据后产生此中断，关闭dma流，同时将发送数据的长度重新设置
  */
void DMA1_Stream0_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream0,DMA_IT_TCIF0)==SET)
	{
	 DMA_Cmd(DMA1_Stream0,DISABLE);
	 DMA_SetCurrDataCounter(DMA1_Stream0,10);
	}
	 DMA_ClearITPendingBit(DMA1_Stream0,DMA_IT_TCIF0);
   DMA_ClearFlag(DMA1_Stream0,DMA_FLAG_TCIF0);	
}

/**
  * @brief  USART8接收中断
  * @param  void
  * @retval void
  * @notes  串口8的数据寄存器接收到数据之后产生中断，在此中断中将接收数据长度重新设置，然后启动接收数据解码任务
  */
void UART8_IRQHandler(void)
{
	char num=0;
	BaseType_t pxHigherPriorityTaskWoken;
	if(USART_GetITStatus(UART8,USART_IT_IDLE)==SET)
	{
		 num = UART8->SR;
     num = UART8->DR; //清除串口空闲中断（中文手册712页）
		 num++;           //没有意义，只是消除警告
		
		 DMA_Cmd(DMA1_Stream6,DISABLE); 
		 DMA_SetCurrDataCounter(DMA1_Stream6,17);      //重新设置接收数据长度
		 DMA_Cmd(DMA1_Stream6,ENABLE);
		//启动上板到下板的数据解码任务
		 vTaskNotifyGiveFromISR(Up_To_Down_Decode_Task_Handler,&pxHigherPriorityTaskWoken);	
		 portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
	}		
}

/***********************************下板到上板通信数据分析*************************************

Target_Locked：NUC目标锁定标志位，0，即没有找到目标，1，识别到目标，2，锁定目标，初始值在串口8初始化函数中为0
               在下板程序，NUC数据解码任务函数中，根据NUC发过来的距离进行设置；
               在上板程序，串口8数据解码函数中，用于设置底盘模式

Distance：敌方距离,单位cm
          在下板程序，NUC数据解码任务函数中，根据NUC发过来的敌方距离进行设置；
          在上板程序，拨盘控制函数中，使用自动模式时用于设置拨盘转速
                 
Enemy_Armor_ID：敌方装甲板的ID号
                在下板程序，NUC数据解码任务函数中，根据NUC发过来的=Down_NUC_Data_TypeStruct.Armor_Number进行设置；
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
  * @brief  USART8发送任务
  * @param  void
  * @retval void
  * @notes  初始化串口8，将数据存到Down_To_Up_Tx中，并启动dma流发送
  */
void USART8_Send_Task(void *pvParameters)
{	
	uint32_t err;
	USART8_Init();
	vTaskDelay(200);
	while(1)
	{
	
		err=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		if(err==1)
		{
			Down_To_Up_TypeStruct.Yaw_Angle_Real=Yaw_Real_Angle;
			
			Down_To_Up_Tx[0] = 0x7A;
			Down_To_Up_Tx[1] = Down_To_Up_TypeStruct.Target_Locked;
			Down_To_Up_Tx[2] = shortToint8(0,Down_To_Up_TypeStruct.Distance);
			Down_To_Up_Tx[3] = shortToint8(1,Down_To_Up_TypeStruct.Distance);
			Down_To_Up_Tx[4] = Down_To_Up_TypeStruct.Enemy_Armor_ID;
			Down_To_Up_Tx[5] = shortToint8(0,Down_To_Up_TypeStruct.Yaw_Angle_Real);
			Down_To_Up_Tx[6] = shortToint8(1,Down_To_Up_TypeStruct.Yaw_Angle_Real);
			Down_To_Up_Tx[7] = Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag;
			Down_To_Up_Tx[8] = 30;
			Down_To_Up_Tx[9] = 0x7B;

			DMA_Cmd(DMA1_Stream0,ENABLE);
		}
	}
}
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

Chassis_Real_Number：202电机的圈数，来自于Chassis_Motor_202_Number，哨兵在最左侧时圈数为0，向右移动电机圈数计数增加
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
  * @brief  USART8数据解码任务
  * @param  void
  * @retval void
  * @notes  被串口8接收中断唤醒，将Up_To_DOWN_Rx中数据解码，存储到Up_To_Down_TypeStruct结构体对象中，
            设置如果前后任意一块装甲板被打击并且没有检测到敌人，则切换云台模式。之后如果没有被击打，则
            将云台模式恢复为丢失敌人状态
  */
void Up_To_Down_Decode_Task(void *pvParameters)
{
	uint32_t err;
	while(1)
	{
		err=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		if(err==1)
		{
			if( (Up_To_DOWN_Rx[0]==0x7A) && (Up_To_DOWN_Rx[16]==0x7B) )
			{
				Up_To_Down_TypeStruct.Get_Hit_flag         = Up_To_DOWN_Rx[1];
				Up_To_Down_TypeStruct.Which_Color_I_Am     = (Up_To_DOWN_Rx[2]&0x01);
				Up_To_Down_TypeStruct.Output_On            =(Up_To_DOWN_Rx[2]>>1);
				Swing_Flag																 =(Up_To_DOWN_Rx[2]>>2);
				Up_To_Down_TypeStruct.Shoot_Allow_Flag     = Up_To_DOWN_Rx[3];
				Up_To_Down_TypeStruct.Chassis_Real_Position= int8ToShort(Up_To_DOWN_Rx,4);
				Up_To_Down_TypeStruct.Sentry_Satus         = Up_To_DOWN_Rx[6];
				Up_To_Down_TypeStruct.Bullet_Speed         = Up_To_DOWN_Rx[7];
				Up_To_Down_TypeStruct.Locked_Flag          = Up_To_DOWN_Rx[8];
				Up_To_Down_TypeStruct.Chassis_Speed_Real   = int8ToShort(Up_To_DOWN_Rx,9);
				Up_To_Down_TypeStruct.Enemy_Armor_ID       = Up_To_DOWN_Rx[11] ; 
        Up_To_Down_TypeStruct.Enemy_State          = Up_To_DOWN_Rx[12] ;
        Up_To_Down_TypeStruct.Enemy_HP_HL          = Up_To_DOWN_Rx[13] ;
        Chassis_Accel       										   = int8ToShort(Up_To_DOWN_Rx,14);
        										

				
				
			}	
			//如果装甲板1被攻击，并且没有识别到敌人
			if( (Up_To_Down_TypeStruct.Get_Hit_flag == 1) && (Down_To_Up_TypeStruct.Target_Locked==0) )
			{//如果云台模式大于等于装甲板0被攻击，即云台模式为PTZ_Armor_1_Be_Attacked或PTZ_Lost_Enemy_Within_5S或PTZ_Lost_Enemy
				if(PTZ_Mode>=PTZ_Armor_0_Be_Attacked)
				{
					PTZ_Mode=PTZ_Armor_1_Be_Attacked;
				}
			}
			//如果装甲板0被攻击且，没有识别到敌人
			else if( (Up_To_Down_TypeStruct.Get_Hit_flag == 0) && (Down_To_Up_TypeStruct.Target_Locked==0) )
			{//如果云台模式大于等于装甲板0被攻击，即云台模式为PTZ_Armor_1_Be_Attacked或PTZ_Lost_Enemy_Within_5S或PTZ_Lost_Enemy
				if(PTZ_Mode>=PTZ_Armor_0_Be_Attacked)
				{
					PTZ_Mode=PTZ_Armor_0_Be_Attacked;
				}
			}
			//如果云台没有找到敌人并且没有丢失敌人在5秒内，即云台处于PTZ_Mocalun_Locked或PTZ_Armor_0_Be_Attacked或PTZ_Armor_1_Be_Attacked时
			else if((PTZ_Mode!=PTZ_Find_Enemy)&&(PTZ_Mode!=PTZ_Lost_Enemy_Within_5S)&&(PTZ_Mode!=PTZ_Locked))
			{
				PTZ_Mode=PTZ_Lost_Enemy;
			}
		}
	}
}
/*********************************************下板与NUC数据通信**********************************************************************/


/**
  * @brief  USART3-NUC通信初始化
  * @param  void
  * @retval void
  * @notes  USART3_RX -----PD9    USART3_TX-----PD8     
            发送用DMA1_Stream3，DMA_Channel_4，使用DMA1_Stream3_IRQHandler
            接收用DMA1_Stream1，DMA_Channel_4，使用USART3_IRQHandler
  */
void NUC_Init(void)
{
  GPIO_InitTypeDef   GPIO_InitStruct;
	USART_InitTypeDef  USART_InitStruct;
	DMA_InitTypeDef    DMA_InitStruct;
	NVIC_InitTypeDef   NVIC_InitStruct;
	
	/*enabe clocks*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD| RCC_AHB1Periph_DMA1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	/*open the alternative function*/
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3);
	
	/*Configure PB10,PB11 as GPIO_InitStruct1 input*/
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9|GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	USART_InitStruct.USART_BaudRate            = 115200;
	USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits            = USART_StopBits_1;
	USART_InitStruct.USART_Parity              = USART_Parity_No;
	USART_InitStruct.USART_Mode                = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART3,&USART_InitStruct);
	
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
	
	USART_Cmd(USART3,ENABLE);		
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
	
/* -------------- Configure NVIC ---------------------------------------*/
//接收中断
	NVIC_InitStruct.NVIC_IRQChannel                   =USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =6;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        =0;
	NVIC_InitStruct.NVIC_IRQChannelCmd                =ENABLE ;
	NVIC_Init(&NVIC_InitStruct);
//发送后中断
	NVIC_InitStruct.NVIC_IRQChannel                   =DMA1_Stream3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =6;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        =0;
	NVIC_InitStruct.NVIC_IRQChannelCmd                =ENABLE ;
	NVIC_Init(&NVIC_InitStruct);
	
/* ------------------------------------- Configure DMA  接收---------------------------------------*/

	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
	
	DMA_Cmd(DMA1_Stream1,DISABLE);
	DMA_DeInit(DMA1_Stream1);
	
	DMA_InitStruct.DMA_Channel=DMA_Channel_4;                                      //通道配置（通道5）
	DMA_InitStruct.DMA_BufferSize=19;                                              //传输数据数目                
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralToMemory;                             //数据传输方向（外设——存储器）                   
	DMA_InitStruct.DMA_FIFOMode=ENABLE;                                            //使能FIFO模式
	DMA_InitStruct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;               //阈值1/4      
	DMA_InitStruct.DMA_Memory0BaseAddr=(uint32_t)Usart3Rx_Info;                    //存储器数据地址
	DMA_InitStruct.DMA_MemoryBurst=DMA_MemoryBurst_Single;                         //存储器突发配置
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;                     //存储器数据格式（字节）
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;                             //内存地址递增
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;                                     //是否循环发送（循环，普通模式只能接受一次）
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&(USART3->DR);                 //外设地址 
	DMA_InitStruct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;                 //外设突发配置         
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;             //外设数据格式
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;                    //外设地址不递增
	DMA_InitStruct.DMA_Priority=DMA_Priority_VeryHigh;                             //优先级非常高      
	DMA_Init(DMA1_Stream1,&DMA_InitStruct);
	DMA_Cmd(DMA1_Stream1,ENABLE);

///*********************************************DMA发送************************************/
	
	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);
	
	DMA_Cmd(DMA1_Stream3,DISABLE);
  DMA_DeInit(DMA1_Stream3);                                                 //为DMA配置通道
	DMA_InitStruct.DMA_Channel           =DMA_Channel_4;
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&(	USART3->DR);          //起始地址
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
	DMA_Init(DMA1_Stream3,&DMA_InitStruct);
	
	DMA_ITConfig(DMA1_Stream3,DMA_IT_TC,ENABLE);
	
  Down_NUC_Data_TypeStruct.Armor_Type=0;
	Down_NUC_Data_TypeStruct.Enemy_distance=0;
	Down_NUC_Data_TypeStruct.Pitch_Angle=0;
	Down_NUC_Data_TypeStruct.Yaw_Angle=0;
	Down_NUC_Data_TypeStruct.Armor_ID=0;
	delay_ms(3);
}
/**
  * @brief  NUC DMA数据流发送中断服务函数
  * @param  void
  * @retval void
  * @notes  每次启动发送任务函数之后启动这个中断服务函数，发送函数在陀螺仪获取任务中被调用，每1ms执行一次
            中断中将DMA数据流关闭并且重新设置DNA数据长度
            
  */
void DMA1_Stream3_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF3)==SET)
	{
	 //关闭dma数据流
	 DMA_Cmd(DMA1_Stream3,DISABLE);
	 //重新设置dma数据长度

	DMA_SetCurrDataCounter(DMA1_Stream3,23);
	}
	DMA_ClearITPendingBit(DMA1_Stream3,DMA_IT_TCIF3);	
}
/**
  * @brief  NUC 串口3数据接收中断服务函数
  * @param  void
  * @retval void
  * @notes  串口三接收到nuc发来的数据之后先将dma数据流关闭重新设置数据长度之后再打开数据流
            然后转到nuc数据处理任务，DMA：从USART3->DR到Usart3Rx_Info
  */
void USART3_IRQHandler(void)
{
	
	BaseType_t pxHigherPriorityTaskWoken;
	char num=0;
	if(USART_GetITStatus(USART3,USART_IT_IDLE)==SET)
	{
		 num = USART3->SR;
     num = USART3->DR; //清除串口空闲中断（中文手册712页）
		 num++;            //没有意义，只是消除警告
		
		 DMA_Cmd(DMA1_Stream1,DISABLE); 
		 DMA_SetCurrDataCounter(DMA1_Stream1,10);      //重新设置接收数据长度
		 DMA_Cmd(DMA1_Stream1,ENABLE);                 //传输，将串口3数据寄存器的数据传输到Usart3Rx_Info
		
		//向NUC数据解码任务发送通知，将其移至就绪列表
		 vTaskNotifyGiveFromISR(NUC_Decode_Task_Handler,&pxHigherPriorityTaskWoken);
		//进行任务切换
		 portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
	}	
}
/******************************下板到NUC通信数据分析****************************
Bullet_Speed：弹丸速度
              从上板发过来=(ShootData.bulletSpeed-20.0f)*10.0f

Yaw_Angle：YAW轴的角度
           在陀螺仪数据获取函数中被设置+=IMU_Real_Data.Gyro_Y*0.003f;
           在云台自动控制函数和云台手动控制函数中被使用，用于控制云台YAW轴转动

Pitch_Angle：PITCH轴的角度，
             在串口3发送函数中由(Pitch_Motor_206.Real_MechanicalAngle-5900)*(1.46502259f)+500得到

Yaw_Speed：YAW轴的速度，单位为陀螺仪原始单位
           在串口3发送函数中由IMU_Real_Data.Gyro_Y*100;得到
					 
Pitch_Speed：PITCH轴的速度，由电机速度转化为°/s之后再除以3乘以100得到

Find_Target_1：云台是否处于找到敌人的状态，0，说明没有处于找到敌人的模式，1，说明处于找到敌人的模式

Which_Color_I_Am：己方颜色，0，为红色，1为蓝色
                  在串口8数据解码函数中由上主板发过来的数据确定

Armor_ID:装甲ID，无实际意义只是为了保持上下发送给NUC数据的统一性

第九号数据：0即00，红方，已找到敌人；1即01，红方，未找到敌人；2即10，蓝方，已经找到敌人；3即11，蓝方，位找到敌人

*************************************************************************************/
/**
  * @brief  串口三数据发送函数
  * @param  void
  * @retval void
  * @notes  将云台yaw轴和pitch轴的数据经过处理之后储存Send_NUC数组中
            然后启动dma数据流将数据发送出去，在陀螺仪数据解码函数中被调用
            每1ms发送一次Send_NUC到Send_NUC，再发出
  */
int Yaw_Angle;
short		  Pitch_Angle;
short	Pitch_Speed,
			Yaw_Speed;

short Output_On=0;

//u8 change_info[4];
//int Yaw_Angle_1;
void USART3_Send_NUC(void)
{
	
	char  Bullet_Speed=0;
	char  Find_Target_1=0;

	//弹丸的速度是从上主板发送过来的
	Bullet_Speed=Up_To_Down_TypeStruct.Bullet_Speed;

  //单位度*100
	//Yaw轴的角度就是陀螺仪的数值进行简单的累加之后取负数，Y轴向右转角度增大，速度为负   
	Yaw_Angle=(-Yaw_Send_NUC_Angle_IMU*10);
	
	
	// Pitch轴的角度就是真实的机械角处理之后发送过去.8192对应180度，单位：度*10
	Pitch_Angle=((float)(Pitch_Motor_206.Real_MechanicalAngle)/8192.0f*360.0f*10)-1875;
	
	
	// 
	Yaw_Speed=IMU_Real_Data.Gyro_Y*100; 
	
	//单位：度/s*100
	Pitch_Speed=((float)Pitch_Motor_206.Real_Speed*360.0f)/(60.0f)*100.0f;
	
	
	//用于确认云台是否处于找到敌人的状态
	if(PTZ_Mode==PTZ_Find_Enemy)
	{
		Find_Target_1=0;
	}
	else Find_Target_1=1;
  
	Up_To_Down_TypeStruct.Output_On=Output_On;

	
	Send_NUC[0]=0x7a;
	Send_NUC[1]=Bullet_Speed;

	Send_NUC[2]=bit32TObit8(0,(short)Yaw_Angle);
	Send_NUC[3]=bit32TObit8(1,(short)Yaw_Angle);	
	Send_NUC[4]=bit32TObit8(2,(short)Yaw_Angle);
	Send_NUC[5]=bit32TObit8(3,(short)Yaw_Angle);		
	Send_NUC[6]=shortTou8(0,(short)Pitch_Speed);
	Send_NUC[7]=shortTou8(1,(short)Pitch_Speed);
	Send_NUC[8]=shortTou8(0,(short)Yaw_Speed);
	Send_NUC[9]=shortTou8(1,(short)Yaw_Speed);
	//第10号数据用于判断现在哨兵云台是不是处于找到敌人的状态以及自身的颜色是什么，00表示红方，已找到敌人
	Send_NUC[10]=shortTou8(0,(short)Up_To_Down_TypeStruct.Chassis_Real_Position );//(Find_Target_1)|(Up_To_Down_TypeStruct.Which_Color_I_Am<<1);//己方颜色从上主板中发送过来的，机器人是红方时候是0，蓝方的时候就是1。
	Send_NUC[11]=shortTou8(1,(short)Up_To_Down_TypeStruct.Chassis_Real_Position );
	Send_NUC[12]=(Find_Target_1)|(Up_To_Down_TypeStruct.Which_Color_I_Am<<1)|(Up_To_Down_TypeStruct.Output_On<<2)|(Swing_Flag);
	Send_NUC[13]=Up_To_Down_TypeStruct.Enemy_Armor_ID;
	Send_NUC[14]=shortTou8(0,(short)Up_To_Down_TypeStruct.Chassis_Speed_Real );//  mm/s
	Send_NUC[15]=shortTou8(1,(short)Up_To_Down_TypeStruct.Chassis_Speed_Real );
	Send_NUC[16]=Up_To_Down_TypeStruct.Enemy_State;
	Send_NUC[17]=Up_To_Down_TypeStruct.Enemy_HP_HL;
	Send_NUC[18]=shortTou8(0,(short)Pitch_Angle);
	Send_NUC[19]=shortTou8(1,(short)Pitch_Angle);
	Send_NUC[20]=shortTou8(0,Chassis_Accel);
	Send_NUC[21]=shortTou8(1,Chassis_Accel);
			
			
			
	Send_NUC[22]=0x7b;


//  change_info[0]=Send_NUC[2];
//	change_info[1]=Send_NUC[3];
//	change_info[2]=Send_NUC[4];
//  change_info	[3]=Send_NUC[5];
//	
//	 Yaw_Angle_1=bit8TObit32(change_info);
	
	
	//从Send_NUC到Send_NUC
	DMA_Cmd(DMA1_Stream3,ENABLE);
}




/*******************************NUC到下板通信数据分析*****************************

Usart3Rx_Info[0]：允许发射数据标志位，0x1f，说明允许发射，其他，则不允许发射
                  在NUC数据解码任务函数中使用，用于给Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag赋值

Usart3Rx_Info[1]、Usart3Rx_Info[2]：Yaw_Angle，YAW轴控制移动角度，相当于遥控器的ch2通道值，是控制云台要移动的值
                                    在云台自动控制函数中用于控制云台移动

Usart3Rx_Info[3]、Usart3Rx_Info[4]：Pitch_Angle，PITCH轴控制移动角度，相当于遥控器的ch3通道值，是控制云台要移动的值
                                    在云台自动控制函数中用于控制云台移动

Usart3Rx_Info[5]：Armor_ID，敌方装甲ID用于区分不同种类的机器人
									
Usart3Rx_Info[6]：Armor_Type，装甲板类型，0，没识别到装甲板，1，识别到小装甲板，2，识别到大装甲板
                  在NUC数据解码任务函数中使用，用于切换云台模式
									
Usart3Rx_Info[7]、Usart3Rx_Info[8]：Enemy_distance
                                    敌方距离
 
Usart3Rx_Info[9]：NUC数据校验位，
                  在在NUC数据解码任务函数中使用，用于Usart3Rx_Info[1] + Usart3Rx_Info[3] + Usart3Rx_Info[5])  % 255 == Usart3Rx_Info[9]校验								

**********************************************************************************/
/**
  * @brief  Nuc数据解码任务
  * @param  void
  * @retval void
  * @notes  每次接收完NUC发过来的数据时执行一次
            Nuc初始化：初始化串口3进行与nuc的通讯，进入NUC解码任务，当数据经过校验之后说明nuc已经连接上了，这个时候nuc丢失时间等于0；
            当NUC发过来的数据中的装甲板类型不为0的时候说明已经识别到了敌人，此时将云台模式切换为识别到敌人，
            如果发过来的装甲板类型是0的话就说明没有检测到装甲板，这个时候将云台控制模式切换为没有识别到敌人            
  */
		int NUC_Get_Time=0;
		short Last_Pitch_Angle=0;
void NUC_Decode_Task(void *pvParameters)
{
  static	uint16_t NUC_Start_Time=0;
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
			if(( Usart3Rx_Info[0] + Usart3Rx_Info[3] + Usart3Rx_Info[5]) % 255 == Usart3Rx_Info[9] ) 
			{
				// Nuc丢失时间等于0说明已经接受到了nuc的数据如果很长时间没有接收到nuc的数据,蜂鸣器会报警
				NUC_Lost_Time=0;
				
				//第一次接收到NUC数据之后会响一段时间
//				if(NUC_Start_Time<400)
//				{
//					NUC_Start_Time++;
//					Beep_ON();
//				}
//				else Beep_OFF();

				//第0号数据为0x1f则告诉上板可以发射
				if( Usart3Rx_Info[0]==0x1f)            Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag=1;//(char)0x1f
				else                                   Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag=0;
				
				//装甲类型：0未识别到，1小装甲，2大装甲
				Down_NUC_Data_TypeStruct.Armor_Type     = Usart3Rx_Info[6] ;
				//如果识别到
				if(Down_NUC_Data_TypeStruct.Armor_Type !=0)
				{
					NUC_Get_Time++;
					//
					//Yaw轴数据解码，1.2号，视觉数据short64555,-11~11,*2000,NUC-4000~4000,
          Down_NUC_Data_TypeStruct.Yaw_Angle=u8ToShort(Usart3Rx_Info,1);	//-0.2~0.2	

						
					
					
					
					//Pitch轴数据解码，3.4号视觉数据short64555,-7~7,*2000,NUC-4000~4000,
					Down_NUC_Data_TypeStruct.Pitch_Angle=u8ToShort(Usart3Rx_Info,3);//-600~600    
					
							
      		if(abs(Last_Pitch_Angle-Down_NUC_Data_TypeStruct.Pitch_Angle>=10))NUC_Get_Time=0;
					//跟随
//					if(NUC_Get_Time>=25)
//					{
//					  PID_Armor_Yaw_205_Mechanical.P=15; 
//					  PID_Armor_Yaw_205_Mechanical.IMax=100;
//	          PID_Armor_Yaw_205_Mechanical.PIDOutMax=50;
//					
//            PID_Armor_Yaw_205_Speed.P=300;  //500
//						PID_Armor_Yaw_205_Speed.PMax=10000;
//	          PID_Armor_Yaw_205_Speed.PIDOutMax=15000;
//					
//					
//						
//					    PID_Armor_Pitch_206_Mechanical.P=0.6; 
//              PID_Armor_Pitch_206_Mechanical.I=0.01; 					
//	            PID_Armor_Pitch_206_Mechanical.PIDOutMax=8;
////							
//							PID_Armor_Pitch_206_Speed.P=300;     
//	            PID_Armor_Pitch_206_Speed.I=0.5; //用于抵抗弹簧，有最大值限制
//              PID_Armor_Pitch_206_Speed.D=1000; 						
//					    PID_Armor_Pitch_206_Speed.IMax=13000;
//	            PID_Armor_Pitch_206_Speed.PIDOutMax=13000;
//					    
//					
////					
//					  
////					
//					
//////						
//					}
//  				else//跳变
//					{
//            PID_Armor_Yaw_205_Mechanical.P=12;

//            PID_Armor_Yaw_205_Mechanical.IMax=12;
//							
//	          PID_Armor_Yaw_205_Mechanical.PIDOutMax=500;

//            PID_Armor_Yaw_205_Speed.P=200;  //500
//            PID_Armor_Yaw_205_Speed.PMax=30000;
//	          PID_Armor_Yaw_205_Speed.PIDOutMax=30000;

//						PID_Armor_Pitch_206_Mechanical.P=0.25;  
//						PID_Armor_Pitch_206_Mechanical.I=0; 
//						PID_Armor_Pitch_206_Mechanical.Iout =0;
//						PID_Armor_Pitch_206_Mechanical.PIDOutMax=10000;
//						
//						PID_Armor_Pitch_206_Speed.P=450;     
//						PID_Armor_Pitch_206_Speed.I=1; //用于抵抗弹簧，有最大值限制 
//						PID_Armor_Pitch_206_Speed.D=0;
//						PID_Armor_Pitch_206_Speed.IMax=9000;
//						PID_Armor_Pitch_206_Speed.PIDOutMax=30000;
//					    
////			
////					  
//					}
         
					
	
					
					Last_Pitch_Angle=Down_NUC_Data_TypeStruct.Pitch_Angle;
         	//敌方装甲板序号
					Down_NUC_Data_TypeStruct.Armor_ID= Usart3Rx_Info[5];
					Down_To_Up_TypeStruct.Enemy_Armor_ID=Down_NUC_Data_TypeStruct.Armor_ID;
					
					
					//敌方距离
					Down_NUC_Data_TypeStruct.Enemy_distance =u8ToShort(Usart3Rx_Info,7);
					Down_To_Up_TypeStruct.Distance=Down_NUC_Data_TypeStruct.Enemy_distance;						
					//如果敌方的距离小于50则目标锁定否则只是识别到敌人
					 Down_To_Up_TypeStruct.Target_Locked=1;
					//目标丢失次数等于0
					Lost_Times=0;
					//设置云台模式为找到敌人
					PTZ_Mode=PTZ_Find_Enemy;									
				}
				
				//如果没有识别到装甲板，装甲类型为0，云台模式切换为没有找到敌人
				else  
				{
					NUC_Get_Time=0;
					Down_NUC_Data_TypeStruct.Yaw_Angle      = 0;
					Down_NUC_Data_TypeStruct.Pitch_Angle    = 0;
					Down_NUC_Data_TypeStruct.Enemy_distance = 0;
					
					Down_To_Up_TypeStruct.Enemy_Armor_ID=0x00;
					Down_To_Up_TypeStruct.Target_Position=0;
					//没有识别到装甲板说明说明目标已经丢失这个时候丢失时间增加
					Lost_Times++;
					if(Lost_Times>50)
					{
						//目标丢失时间大于50说明已经找不到敌人了这个时候目标锁定等于0
						Down_To_Up_TypeStruct.Target_Locked=0;
						//切换云台模式从找到敌人变成丢失敌人
						if(PTZ_Mode==PTZ_Find_Enemy)
						{
							
							
							//丢失的角度就等于当前yaw的角度
							Lost_Angle=Yaw_Real_Angle;
							//切换云台模式从找到敌人变成丢失敌人
							PTZ_Mode=PTZ_Lost_Enemy;
						}						
					}
				}
			}
		}
	}
}
/**
  * @brief  串口初始化
  * @param  void
  * @retval void
  * @notes  将下主板用到的三个串口初始化         
  */
void USART_InitConfig(void)
{
	USART6_Init();
	USART8_Init();
	NUC_Init();//USART3
}
/*********************************************数据类型转换函数**********************************************************************/
int bit8TObit32(uint8_t *change_info)
{
	union
	{
    int f;
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



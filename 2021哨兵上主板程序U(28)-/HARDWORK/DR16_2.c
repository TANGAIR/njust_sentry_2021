#include "DR16.h"

/***************************************数据定义区***************************************/
DBUS_DecodingDate_TypeDef DBUS;

#define SBUS_RX_BUF_NUM 36u
#define RC_FRAME_LENGTH 18u

static uint8_t SBUS_rx_buf[2][SBUS_RX_BUF_NUM];


short DR16_Flash_Val=0;

void RC_Init(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num)
{
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_DMA2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, DISABLE);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1); //PB7  usart1 rx
																														/* -------------- Configure GPIO ---------------------------------------*/
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		USART_DeInit(USART1);

		USART_InitStructure.USART_BaudRate = 100000;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_Even;
		USART_InitStructure.USART_Mode = USART_Mode_Rx;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_Init(USART1, &USART_InitStructure);

		USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);

		USART_ClearFlag(USART1, USART_FLAG_IDLE);
		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

		USART_Cmd(USART1, ENABLE);
	}

	/* -------------- Configure NVIC ---------------------------------------*/
	{
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}

	//DMA2 stream5 ch4  or (DMA2 stream2 ch4)    !!!!!!! P206 of the datasheet
	/* -------------- Configure DMA -----------------------------------------*/
	{
		DMA_InitTypeDef DMA_InitStructure;
		DMA_DeInit(DMA2_Stream2);

		DMA_InitStructure.DMA_Channel            = DMA_Channel_4;
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART1->DR);
		DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)rx1_buf;
		DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
		DMA_InitStructure.DMA_BufferSize         = dma_buf_num;
		DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;//外设地址寄存器不变
		DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
		
		DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
		DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_1QuarterFull;
		DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		
		DMA_Init(DMA2_Stream2, &DMA_InitStructure);
		DMA_DoubleBufferModeConfig(DMA2_Stream2, (uint32_t)rx2_buf, DMA_Memory_0);
		
		DMA_DoubleBufferModeCmd(DMA2_Stream2, ENABLE);
		DMA_Cmd(DMA2_Stream2, DISABLE); //Add a disable
		DMA_Cmd(DMA2_Stream2, ENABLE);
	}
}
/***************************************函数处理区***************************************/
/**
  * @brief  遥控器初始化
  * @param  void
  * @retval void
  * @notes  USART1_RX-->PB7    DMA接收
  */
void DR16_InitConfig(void)
{
	
	RC_Init(SBUS_rx_buf[0], SBUS_rx_buf[1], SBUS_RX_BUF_NUM);

}

/**
  * @brief  遥控器数据解码
  * @param  void
  * @retval void
  * @notes  ch0、ch2向右为正,ch1、ch3向上为正,鼠标X轴向右为正,Y轴向下为正
  */
static void SBUS_TO_RC(volatile const uint8_t *sbus_buf)
{
	DR16_Flash_Val=DR_DECENT_TIM;	//信号衰减

	DBUS.RC.ch0          = ((( sbus_buf[0] | (sbus_buf [1]<<8) ) & 0x07FF)-1024);                       //channel-0
	DBUS.RC.ch1          = ((( sbus_buf[1]>>3 | sbus_buf[2]<<5 ) & 0x07FF)-1024);                       //channel-1
	DBUS.RC.ch2          = ((( sbus_buf[2]>>6 | sbus_buf[3]<<2 | sbus_buf[4]<<10 ) & 0x07FF)-1024);  //channel-2
	DBUS.RC.ch3          = (( sbus_buf[4]>>1 | sbus_buf[5]<<7 ) & 0x07FF)-1024;                          //channel-3	
	DBUS.RC.Switch_Left  = (( sbus_buf[5]>>4 ) & 0x00C )>>2;                                                //Switch_Left
	DBUS.RC.Switch_Right = ( sbus_buf[5]>>4 ) & 0x003;                                                      //Switch_Right
	
	DBUS.PC.X    = sbus_buf[6] | (sbus_buf[7] << 8);                                             //Mouse X axis 
//	DBUS.PC.X=0;
//	for(k=0;k<5;k++)
//	{
//		DBUS.PC.X_Real[k]=DBUS.PC.X_Real[k+1];
//		DBUS.PC.X+=DBUS.PC.X_Real[k];
//	}
//	DBUS.PC.X/=5;
//	if(DBUS.PC.X>50) DBUS.PC.X=50;
//	if(DBUS.PC.X<-50) DBUS.PC.X=-50;
	
	DBUS.PC.Y       = sbus_buf[8] | (sbus_buf[9] << 8);                                             //Mouse Y axis
//	DBUS.PC.Y=0;
//	for(k=0;k<5;k++)
//	{
//		DBUS.PC.Y_Real[k]=DBUS.PC.Y_Real[k+1];
//		DBUS.PC.Y+=DBUS.PC.Y_Real[k];
//	}
//	DBUS.PC.Y/=5;
	if(DBUS.PC.Y>50) DBUS.PC.Y=50;
	if(DBUS.PC.Y<-50) DBUS.PC.Y=-50;
	DBUS.PC.Press_Left   = sbus_buf[12];                                                                    //Mouse Left Is Press ?	
	DBUS.PC.Press_Right  = sbus_buf[13];                                                                    //Mouse Right Is Press ?	
	DBUS.PC.Keyboard     = sbus_buf[14] | sbus_buf[15] << 8;                                             //Keyborad Value
	
	if(abs(DBUS.RC.ch0)<10)  DBUS.RC.ch0=0;
	if(abs(DBUS.RC.ch1)<10)  DBUS.RC.ch1=0;
	if(abs(DBUS.RC.ch2)<10)  DBUS.RC.ch2=0;
	if(abs(DBUS.RC.ch3)<10)  DBUS.RC.ch3=0;
				 Control_Data_Process();

}
//串口中断
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        USART_ReceiveData(USART1);
    }
    else if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        static uint16_t this_time_rx_len = 0;
        USART_ReceiveData(USART1);

        if(DMA_GetCurrentMemoryTarget(DMA2_Stream2) == 0)
        {
            //重新设置DMA
            DMA_Cmd(DMA2_Stream2, DISABLE);
            this_time_rx_len = SBUS_RX_BUF_NUM - DMA_GetCurrDataCounter(DMA2_Stream2);
            DMA_SetCurrDataCounter(DMA2_Stream2, SBUS_RX_BUF_NUM);
            DMA2_Stream2->CR |= DMA_SxCR_CT;
            //清DMA中断标志
            DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2 | DMA_FLAG_HTIF2);
            DMA_Cmd(DMA2_Stream2, ENABLE);
            if(this_time_rx_len == RC_FRAME_LENGTH)
            {
                //处理遥控器数据
                SBUS_TO_RC(SBUS_rx_buf[0]);
            }
        }
        else
        {
            //重新设置DMA
            DMA_Cmd(DMA2_Stream2, DISABLE);
            this_time_rx_len = SBUS_RX_BUF_NUM - DMA_GetCurrDataCounter(DMA2_Stream2);
            DMA_SetCurrDataCounter(DMA2_Stream2, SBUS_RX_BUF_NUM);
            DMA2_Stream2->CR &= ~(DMA_SxCR_CT);
            //清DMA中断标志
            DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2 | DMA_FLAG_HTIF2);
            DMA_Cmd(DMA2_Stream2, ENABLE);
            if(this_time_rx_len == RC_FRAME_LENGTH)
            {
                //处理遥控器数据
                SBUS_TO_RC(SBUS_rx_buf[1]);
            }

        }
    }
}

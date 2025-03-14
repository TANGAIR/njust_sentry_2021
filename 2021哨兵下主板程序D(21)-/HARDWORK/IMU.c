#include "main.h"

/***************************************数据定义区***************************************/

//陀螺仪原始数据
IMU_RAW_DATA_Typedef   IMU_Raw_Data;
//陀螺仪真实数据
IMU_REAL_DATA_Typedef  IMU_Real_Data;
//陀螺仪校准数据
IMU_REAL_DATA_Typedef  IMU_Offset_Data;


short PTZ_MPU_Ignore=3;
float Yaw_Send_NUC_Angle_IMU=0;

/***************************************函数处理区***************************************/
/**
  * @brief  MPU6600配置写入  新板
  * @param  void
  * @retval void
  * @notes  Write a register to MPU6600     
  */
uint8_t MPU6500_Write_Reg(uint8_t const reg, uint8_t const data)
{
  MPU6500_NSS_Low();
  SPI5_ReadWriteByte(reg&0x7f);
  SPI5_ReadWriteByte(data);
  MPU6500_NSS_High(); 
  return 0;
}

/**
  * @brief  MPU6500角度获取
  * @param  void
  * @retval void
  * @notes  Read a register from MPU6500      
  */
uint8_t MPU6500_Read_Reg(uint8_t  reg)
{
  uint8_t MPU_Rx;
  
  MPU6500_NSS_Low();
  SPI5_ReadWriteByte(reg|0x80);
  MPU_Rx=SPI5_ReadWriteByte(0xff);
  MPU6500_NSS_High();
  
  return MPU_Rx;
}

/**
  * @brief  MPU6500角度获取
  * @param  void
  * @retval void
  * @notes  Read registers from MPU6500,address begin with regAddr      
  */
uint8_t MPU6500_Read_Regs(uint8_t  regAddr, uint8_t *pData, uint8_t len)
{
  int i; 
  int a;
		
  for (i=0;i<len;i++)
  {
    *pData = MPU6500_Read_Reg(regAddr+i);
    pData++;
    a=10;
    while(a--);

  }
  return 0;
}

/**
  * @brief  陀螺仪数据获取函数
  * @param  void
  * @retval void
  * @notes  Get 6 axis data from MPU6500  ，在IMU_Get_Data_Task中被调用
            将陀螺仪数据经过简单处理之后存入IMU_Real_Data
            将yaw轴角速度乘以一个比例系数再进行简单相加后存入Yaw_Send_NUC_Angle_IMU     
  */
void IMU_Get_Data(void)
{
	uint8_t mpu_buff[6];
	
  //读取陀螺仪寄存器的值将其储存到mpu_buff数组中
	MPU6500_Read_Regs(MPU6500_GYRO_XOUT_H, mpu_buff, 6);
	
	//获取陀螺仪原始数据
	IMU_Raw_Data.Gyro_X = mpu_buff[0]<<8 |mpu_buff[1];
  IMU_Raw_Data.Gyro_Y = mpu_buff[2]<<8 |mpu_buff[3] ;
	IMU_Raw_Data.Gyro_Z = mpu_buff[4]<<8 |mpu_buff[5] ;
	
  
	IMU_Real_Data.Gyro_X = (float)(IMU_Raw_Data.Gyro_X - IMU_Offset_Data.Gyro_X )/65.5f;
	if(fabs(IMU_Real_Data.Gyro_X)<PTZ_MPU_Ignore) IMU_Real_Data.Gyro_X=0;
	
  IMU_Real_Data.Gyro_Y = (float)(IMU_Raw_Data.Gyro_Y - IMU_Offset_Data.Gyro_Y )/65.5f;
	if(fabs(IMU_Real_Data.Gyro_Y)<PTZ_MPU_Ignore) IMU_Real_Data.Gyro_Y=0;
	
	IMU_Real_Data.Gyro_Z = (float)(IMU_Raw_Data.Gyro_Z - IMU_Offset_Data.Gyro_Z )/65.5f;
	if(fabs(IMU_Real_Data.Gyro_Z)<PTZ_MPU_Ignore) IMU_Real_Data.Gyro_Z=0;
	
	//发送给NUC的数据就是将当前陀螺仪的数据进行简单的累加，同时乘以一个比例系数
	Yaw_Send_NUC_Angle_IMU +=IMU_Real_Data.Gyro_Y*0.003f;
	
}
//char Send_TIM=0;
/**
  * @brief  陀螺仪角度获取任务
  * @param  void
  * @retval void
  * @notes  每1毫秒被PB8外部中断唤醒一次，获取陀螺仪数据，将其存储到IMU_Real_Data成员变量中
            同时在USART3_Send_NUC函数中开启DMA数据流，给NUC发送消息，
  */
void IMU_Get_Data_Task(void *pvParameters)
{
	uint32_t err;
	while(1)
	{
		//等待中断任务通知
		err=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		if(err==1)
		{
//			Send_TIM++;
//			if(Send_TIM%10==0)
//			{
				USART3_Send_NUC();
//			  Send_TIM=0;
//			}
			  IMU_Get_Data();
			
		}
	}
}

/**
  * @brief  MPU6500初始化角度处理
  * @param  void
  * @retval void
  * @notes  获取陀螺仪最初的角度，用于之后的校准       
  */
void MPU6500_Gyro_Cali(void)
{
  u16 i;
  int num=300;
  float Gyro_X_temp=0,Gyro_Y_temp=0,Gyro_Z_temp=0;
  
  IMU_Offset_Data.Gyro_X = 0;
  IMU_Offset_Data.Gyro_Y = 0;
  IMU_Offset_Data.Gyro_Z = 0;
  for (i=0;i<num;i++)
  {
    IMU_Get_Data();
    Gyro_X_temp += IMU_Raw_Data.Gyro_X;
    Gyro_Y_temp += IMU_Raw_Data.Gyro_Y;
    Gyro_Z_temp += IMU_Raw_Data.Gyro_Z;
		delay_ms(2);
  }
  IMU_Offset_Data.Gyro_X =  Gyro_X_temp/num;
  IMU_Offset_Data.Gyro_Y =  Gyro_Y_temp/num;
  IMU_Offset_Data.Gyro_Z =  Gyro_Z_temp/num;
}

/**
  * @brief  IMU初始化
  * @param  void
  * @retval void
  * @notes  Initialize the MPU6500  1000Hz采样率  
            在云台初始化函数中调用
  */
uint8_t IMU_Init(void)
{

	uint8_t index = 0;
	//陀螺仪初始化配置
	uint8_t MPU6500_Init_Data[10][2] = 
	{
		{MPU6500_PWR_MGMT_1,    0x83},      // 6500第41页   Reset Device       Auto selects the best available clock source – PLL if ready, else use the Internal oscillator
//		{MPU6500_SMPLRT_DIV,    0x07},      // Clock Source - Gyro-Z
//		{MPU6500_SMPLRT_DIV,    0x01},
//		{MPU6500_CONFIG,        0x03},      // Enable Acc & Gyro
		{MPU6500_SMPLRT_DIV,    0x02},      // 6500第12页   采样频率分频器 SAMPLE_RATE =INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)   INTERNAL_SAMPLE_RATE = 1kHz
		{MPU6500_CONFIG,        0x03},      // 6500第13页   配置寄存器
		{MPU6500_ACCEL_CONFIG,  0x08},      // 6500第14页   加速度量程±4g
		{MPU6500_ACCEL_CONFIG_2,0x01},      // 6500第15页   enable LowPassFilter  Set Acc LPF
		{MPU6500_GYRO_CONFIG,   0x08},      // 6500第14页   陀螺仪量程±500dps
		{MPU6500_INT_PIN_CFG,   0x02},      // 6500第29页   enable I2C_MASTER interface pins (ES_CL and ES_DA)
		{MPU6500_INT_ENABLE,    0x01},      // 6500第29页   Enable Raw Sensor Data Ready interrupt to propagate to interrupt pin.
		{MPU6500_USER_CTRL,     0x20},      // 6500第40页   Enable AUX
   //  {MPU6500_PWR_MGMT_2,    0x00},      // Enable Acc & Gyro
    {MPU6500_PWR_MGMT_2,    0x38},      // 6500第42页
	};  
	
  //SPI外设初始化
	SPI_GPIO_InitConfig();
	
	//判断陀螺仪ID
	while(MPU6500_ID != MPU6500_Read_Reg(MPU6500_WHO_AM_I))
    {
		  LED_RED_ON;	   
    } 
	
		LED_RED_OFF;
  
	//将陀螺仪配置写入
	for(index = 0; index < 10; index++)//配置6500
	{
		MPU6500_Write_Reg(MPU6500_Init_Data[index][0], MPU6500_Init_Data[index][1]);
		delay_ms(1);
	}
	
	delay_ms(15);
	
	//陀螺仪角度初始化
	MPU6500_Gyro_Cali();
	
	//  IST8310_Init();
	
	MPU_6500_Interrupt_InitConfig();

	return 0;
}

/**
  * @brief  6500数据溢出中断对应的输入捕获初始化
  * @param  void
  * @retval void
  * @notes  EXTI_Line8-->PB8，陀螺仪内部每次解算出数据之后会产生一个电平变化，从PB8输入
            从GPIO8读取其电平，产生一个外部中断，中断优先级为7，频率大概为1000Hz
  */
void MPU_6500_Interrupt_InitConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXIT_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE );
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE );

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init (GPIOB,&GPIO_InitStruct);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB ,GPIO_PinSource8);

	EXIT_InitStruct.EXTI_Line = EXTI_Line8;
	EXIT_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXIT_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXIT_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXIT_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct); 
}

/**
  * @brief  定时器4外部中断处理函数()输入捕获  1KHz
  * @param  void
  * @retval void
  * @notes  向陀螺仪数据获取任务发送通知将其添加到就绪列表，同时进行一次任务切换
  */
void EXTI9_5_IRQHandler(void)
{
	BaseType_t pxHigherPriorityTaskWoken;
   if (EXTI_GetITStatus (EXTI_Line8 ) == SET)
   {
     vTaskNotifyGiveFromISR(IMU_Get_Data_Task_Handler,&pxHigherPriorityTaskWoken);

   }
   EXTI_ClearITPendingBit(EXTI_Line8);
	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
}


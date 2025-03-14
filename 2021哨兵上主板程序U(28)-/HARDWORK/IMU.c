#include "main.h"

/***************************************���ݶ�����***************************************/

//������ԭʼ����
IMU_RAW_DATA_Typedef   IMU_Raw_Data;
//��������ʵ����
IMU_REAL_DATA_Typedef  IMU_Real_Data;
//������У׼����
IMU_REAL_DATA_Typedef  IMU_Offset_Data;

//���̼��ٶ�
short   Chassis_Accel=0;

float PTZ_MPU_Ignore=0.01286f;


/***************************************����������***************************************/
/**
  * @brief  MPU6600����д��  �°�
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
  * @brief  MPU6500�ǶȻ�ȡ
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
  * @brief  MPU6500�ǶȻ�ȡ
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
  * @brief  ���������ݻ�ȡ����
  * @param  void
  * @retval void
  * @notes  Get 6 axis data from MPU6500  ����IMU_Get_Data_Task�б�����
            �����������ݾ����򵥴���֮�����IMU_Real_Data
            ��yaw����ٶȳ���һ������ϵ���ٽ��м���Ӻ����Yaw_Send_NUC_Angle_IMU     
  */
void IMU_Get_Data(void)
{
	uint8_t mpu_buff[14];
	
  //��ȡ�����ǼĴ�����ֵ���䴢�浽mpu_buff������
	MPU6500_Read_Regs(MPU6500_ACCEL_XOUT_H, mpu_buff, 14);

	IMU_Raw_Data.Accel_X = mpu_buff[0]<<8 |mpu_buff[1]; 
	IMU_Raw_Data.Accel_Y = mpu_buff[2]<<8 |mpu_buff[3];
	IMU_Raw_Data.Accel_Z = mpu_buff[4]<<8 |mpu_buff[5]; //���ٶ�
	IMU_Raw_Data.Temp    = mpu_buff[6]<<8 |mpu_buff[7];	//�¶�
	IMU_Raw_Data.Gyro_X  = mpu_buff[8]<<8 |mpu_buff[9];
	IMU_Raw_Data.Gyro_Y = mpu_buff[10]<<8 |mpu_buff[11] ;
	IMU_Raw_Data.Gyro_Z = mpu_buff[12]<<8 |mpu_buff[13] ; //���ٶ�
	

	IMU_Real_Data.Accel_X=IMU_Raw_Data.Accel_X;
	
  
	
	IMU_Real_Data.Accel_X =IMU_Real_Data.Accel_X/2048/9.8f-PTZ_MPU_Ignore;//����λ�������ÿƽ����
	
	if(fabs(IMU_Real_Data.Accel_X)<0.06f)IMU_Real_Data.Accel_X=0;
	
	//���̼��ٶȼ��㣬30Ϊ�����ٶ����ֱ��б����������ƽ�����ٶȵı���
	Chassis_Accel=-1000*IMU_Real_Data.Accel_X*30;//��λ������/ƽ����
	
//	IMU_Real_Data.Speed_X +=IMU_Real_Data.Accel_X*0.001;//����ÿ1ms��ȡһ��ֵ�������ٶȵ��ڼ��ٶȳ���ʱ�䣨��λΪ�룩���ۼ�
	
	
}
//char Send_TIM=0;
/**
  * @brief  �����ǽǶȻ�ȡ����
  * @param  void
  * @retval void
  * @notes  ÿ1���뱻PB8�ⲿ�жϻ���һ�Σ���ȡ���������ݣ�����洢��IMU_Real_Data��Ա������
            ͬʱ��USART3_Send_NUC�����п���DMA����������NUC������Ϣ��
  */
void IMU_Get_Data_Task(void *pvParameters)
{
	
	IMU_Init();       //�����ǳ�ʼ�����������������ݻ�ȡ�жϣ���ȡ�����ǵ�����
	vTaskDelay(100);
	uint32_t err;
	while(1)
	{
		//�ȴ��ж�����֪ͨ
		err=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		if(err==1)
		{

			  IMU_Get_Data();
			
		}
	}
}

/**
  * @brief  MPU6500��ʼ���Ƕȴ���
  * @param  void
  * @retval void
  * @notes  ��ȡ����������ĽǶȣ�����֮���У׼       
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
  * @brief  IMU��ʼ��
  * @param  void
  * @retval void
  * @notes  Initialize the MPU6500  1000Hz������  
            ����̨��ʼ�������е���
  */
uint8_t IMU_Init(void)
{

	uint8_t index = 0;
	//�����ǳ�ʼ������
	uint8_t MPU6500_Init_Data[10][2] = 
	{
		{MPU6500_PWR_MGMT_1,    0x83},      // 6500��41ҳ   Reset Device       Auto selects the best available clock source �C PLL if ready, else use the Internal oscillator
//		{MPU6500_SMPLRT_DIV,    0x07},      // Clock Source - Gyro-Z
//		{MPU6500_SMPLRT_DIV,    0x01},
		{MPU6500_SMPLRT_DIV,    0x02},      // 6500��12ҳ   ����Ƶ�ʷ�Ƶ�� SAMPLE_RATE =INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)   INTERNAL_SAMPLE_RATE = 1kHz
		{MPU6500_CONFIG,        0x03},      // 6500��13ҳ   ���üĴ���
		{MPU6500_ACCEL_CONFIG,  0x08},      // 6500��14ҳ   ���ٶ����̡�4g
		{MPU6500_ACCEL_CONFIG_2,0x01},      // 6500��15ҳ   enable LowPassFilter  Set Acc LPF
		{MPU6500_GYRO_CONFIG,   0x08},      // 6500��14ҳ   ���������̡�500dps
		{MPU6500_INT_PIN_CFG,   0x02},      // 6500��29ҳ   enable I2C_MASTER interface pins (ES_CL and ES_DA)
		{MPU6500_INT_ENABLE,    0x01},      // 6500��29ҳ   Enable Raw Sensor Data Ready interrupt to propagate to interrupt pin.
		{MPU6500_USER_CTRL,     0x20},      // 6500��40ҳ   Enable AUX
  // {MPU6500_PWR_MGMT_2,    0x00},      // Enable Acc & Gyro 0000 0000�����ٶȽ��ٶȶ���ȡ
		 {MPU6500_PWR_MGMT_2,    0x07},      // Enable Acc & Gyro 0000 0111��ֻ��ȡ���ٶ�
   // {MPU6500_PWR_MGMT_2,    0x38},      // 6500��42ҳ,ֻ�����ٶȣ��������ٶ�0011 1000
	};  
	
  //SPI�����ʼ��
	SPI_GPIO_InitConfig();
	
	//�ж�������ID
	while(MPU6500_ID != MPU6500_Read_Reg(MPU6500_WHO_AM_I))
    {
		  LED_RED_ON;	   
    } 
	
		LED_RED_OFF;
  
	//������������д��
	for(index = 0; index < 10; index++)//����6500
	{
		MPU6500_Write_Reg(MPU6500_Init_Data[index][0], MPU6500_Init_Data[index][1]);
		delay_ms(1);
	}
	
	delay_ms(15);
	
	//�����ǽǶȳ�ʼ��
	MPU6500_Gyro_Cali();
	
	//  IST8310_Init();
	
	MPU_6500_Interrupt_InitConfig();
	
		IMU_Real_Data.Speed_X =0;
	  IMU_Real_Data.Speed_Y =0;

	return 0;
}

/**
  * @brief  6500��������ж϶�Ӧ�����벶���ʼ��
  * @param  void
  * @retval void
  * @notes  EXTI_Line8-->PB8���������ڲ�ÿ�ν��������֮������һ����ƽ�仯����PB8����
            ��GPIO8��ȡ���ƽ������һ���ⲿ�жϣ��ж����ȼ�Ϊ7��Ƶ�ʴ��Ϊ1000Hz
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
  * @brief  ��ʱ��4�ⲿ�жϴ�����()���벶��  1KHz
  * @param  void
  * @retval void
  * @notes  �����������ݻ�ȡ������֪ͨ������ӵ������б�ͬʱ����һ�������л�
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


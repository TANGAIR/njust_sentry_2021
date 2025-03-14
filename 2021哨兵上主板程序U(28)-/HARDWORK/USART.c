/****************ͷ�ļ�������******************************/
#include "main.h"

/**********************************���ݶ�����*****************************/
int NUC_Lost_Time=0;

NUC_Data_Typedef    Up_NUC_Data_TypeStruct;
Up_To_Down_Typedef  Up_To_Down_TypeStruct;
Down_To_Up_Typedef  Down_To_Up_TypeStruct;

int8_t Up_To_DOWN_Tx[17];
int8_t Down_To_Up_Rx[10];
char   USART6Rx_Info[10];//��������������
char        Send_NUC[23];//��������������

int View_Enemy_ID=0;
char Bullet_Speed=0;

short Yaw_Limit =65*40;
short Pitch_Limit=35*20;


//����7���տ�����Ϣ��
uint16_t My_Usart_Recieve_Data=2;
/*********************************************���ڳ�ʼ����**********************************************/
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
// ��������
int fputc(int ch, FILE *f)
{
	while (USART_GetFlagStatus(UART7,USART_FLAG_TC) == RESET);//�ȴ�֮ǰ���ַ��������
	USART_SendData(UART7, (uint8_t)ch);
	return (ch);
}
#endif
/**
  * @brief  ��ʼ��UART7
  * @param  void
  * @retval void
  * @notes  ����7��ӡ��ʼ��  UART7_RX -PE7  UART7_TX-PE8      
            ��ӡ�ж����ȼ� 7
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
	GPIO_InitSturct.GPIO_OType=GPIO_OType_PP;                                      //�������
	GPIO_InitSturct.GPIO_Mode=GPIO_Mode_AF;                                        //����ģʽ
	GPIO_InitSturct.GPIO_PuPd=GPIO_PuPd_NOPULL;                                    //������ 
	GPIO_InitSturct.GPIO_Speed=GPIO_High_Speed;                                    //100MHZ
	GPIO_Init(GPIOE,&GPIO_InitSturct);
	
	USART_InitStruct.USART_BaudRate=9600;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;     //Ӳ����������
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;                       //�շ�ģʽ
	USART_InitStruct.USART_Parity=USART_Parity_No;                                 //����żУ��λ
	USART_InitStruct.USART_StopBits=USART_StopBits_1;                              //�ֽ�
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;                         //���ݳ���
	USART_Init(UART7,&USART_InitStruct);
	USART_Cmd(UART7,ENABLE);
	USART_ITConfig(UART7,USART_IT_RXNE,ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel=UART7_IRQn;                                    //�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;                                     //ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=7;                           //��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;                                  //�����ȼ�
	NVIC_Init(&NVIC_InitStruct);	
}

/**
  * @brief  ����7��ӡ�ж�
  * @param  void
  * @retval void
  * @notes  ��ʱû��
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

















/*********************************************�ϰ����°�����ͨ��**********************************************************************/
/**
  * @brief  USART8��ʼ������
  * @param  void
  * @retval void
  * @notes  USART8_TX-PE1      USART8_RX -----PE0   ������115200
            ������DMA1_Stream0��DMA_Channel_5��ʹ��DMA1_Stream0_IRQHandler ���ȼ�Ϊ 9
            ������DMA1_Stream6��DMA_Channel_5��ʹ��UART8_IRQHandler ���ȼ�Ϊ 9
            �ϰ巢�͸��°�����ݸ���ֵ���°巢�͵�ֵ�ĳ�ʼ��
  */
void USART8_Init(void)
{
   GPIO_InitTypeDef       GPIO_InitSturct;
	 USART_InitTypeDef      USART_InitStruct;
	 NVIC_InitTypeDef       NVIC_InitStruct;
   DMA_InitTypeDef        DMA_InitStruct;

/*******************************************ʱ��ʹ��*******************************/	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART8,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);

/*******************************************IO�ڸ���*******************************/		
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource0,GPIO_AF_UART8);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource1,GPIO_AF_UART8);

/*******************************************IO������******************************/		
	GPIO_InitSturct.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitSturct.GPIO_OType=GPIO_OType_PP;                                      //�������
	GPIO_InitSturct.GPIO_Mode=GPIO_Mode_AF;                                        //����ģʽ
	GPIO_InitSturct.GPIO_PuPd=GPIO_PuPd_NOPULL;                                    //������ 
	GPIO_InitSturct.GPIO_Speed=GPIO_High_Speed;                                    //100MHZ
	GPIO_Init(GPIOE,&GPIO_InitSturct);

/*******************************************��������*******************************/	
	USART_InitStruct.USART_BaudRate=115200;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;     //Ӳ����������
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;                       //�շ�ģʽ
	USART_InitStruct.USART_Parity=USART_Parity_No;                                 //����żУ��λ
	USART_InitStruct.USART_StopBits=USART_StopBits_1;                              //�ֽ�
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;                         //���ݳ���
	USART_Init(UART8,&USART_InitStruct);
	USART_Cmd(UART8,ENABLE);
	
	USART_ITConfig(UART8,USART_IT_IDLE,ENABLE);

/*******************************************�ж�����******************************/	
	NVIC_InitStruct.NVIC_IRQChannel=UART8_IRQn;                                    //�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;                                     //ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=9;                           //��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;                                  //�����ȼ�
	NVIC_Init(&NVIC_InitStruct);	
	
	NVIC_InitStruct.NVIC_IRQChannel=DMA1_Stream0_IRQn;                             //�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;                                     //ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=9;                           //��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;                                  //�����ȼ�
	NVIC_Init(&NVIC_InitStruct);	
	
	
/************************************************DMA����********************************/
	USART_DMACmd(UART8,USART_DMAReq_Tx,ENABLE);                                    //����DMAʹ��
	
	DMA_Cmd(DMA1_Stream0,DISABLE);                                                 //DMAʧ�ܣ�ֻ����DMAʧ�ܵ�������ܶ�DMA�Ĵ������в�����
	DMA_DeInit(DMA1_Stream0);                                                      //��DMA���üĴ�������Ϊȱʡֵ
	
  DMA_InitStruct.DMA_Channel=DMA_Channel_5;                                      //ͨ�����ã�ͨ��5��
	DMA_InitStruct.DMA_BufferSize=17;                                              //����������Ŀ                
	DMA_InitStruct.DMA_DIR=DMA_DIR_MemoryToPeripheral;                             //���ݴ��䷽��(�洢�������裩                   
	DMA_InitStruct.DMA_FIFOMode=ENABLE;                                            //ʹ��FIFOģʽ
	DMA_InitStruct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;               //��ֵ1/4      
	DMA_InitStruct.DMA_Memory0BaseAddr=(uint32_t)Up_To_DOWN_Tx;                    //�洢�����ݵ�ַ
	DMA_InitStruct.DMA_MemoryBurst=DMA_MemoryBurst_Single;                         //�洢��ͻ������
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;                     //�洢�����ݸ�ʽ���ֽڣ�
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;                             //�ڴ��ַ����
	DMA_InitStruct.DMA_Mode=DMA_Mode_Normal;                                       //�Ƿ�ѭ�����ͣ���ѭ����
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&(UART8->DR);                 //�����ַ 
	DMA_InitStruct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;                 //����ͻ������         
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;             //�������ݸ�ʽ
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;                    //�����ַ������
	DMA_InitStruct.DMA_Priority=DMA_Priority_VeryHigh;                             //���ȼ��ǳ���      
	DMA_Init(DMA1_Stream0,&DMA_InitStruct);
	
	DMA_ITConfig(DMA1_Stream0,DMA_IT_TC,ENABLE);
/****************************************************DMA����******************************************/	
	USART_DMACmd(UART8,USART_DMAReq_Rx,ENABLE);
	
	DMA_Cmd(DMA1_Stream6,DISABLE);
	DMA_DeInit(DMA1_Stream6);
	
	DMA_InitStruct.DMA_Channel=DMA_Channel_5;                                      //ͨ�����ã�ͨ��5��
	DMA_InitStruct.DMA_BufferSize=15;                                              //����������Ŀ                
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralToMemory;                             //���ݴ��䷽�����衪���洢����                   
	DMA_InitStruct.DMA_FIFOMode=ENABLE;                                            //ʹ��FIFOģʽ
	DMA_InitStruct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;               //��ֵ1/4      
	DMA_InitStruct.DMA_Memory0BaseAddr=(uint32_t)Down_To_Up_Rx;                    //�洢�����ݵ�ַ
	DMA_InitStruct.DMA_MemoryBurst=DMA_MemoryBurst_Single;                         //�洢��ͻ������
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;                     //�洢�����ݸ�ʽ���ֽڣ�
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;                             //�ڴ��ַ����
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;                                     //�Ƿ�ѭ�����ͣ�ѭ������ͨģʽֻ�ܽ���һ�Σ�
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&(UART8->DR);                  //�����ַ 
	DMA_InitStruct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;                 //����ͻ������         
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;             //�������ݸ�ʽ
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;                    //�����ַ������
	DMA_InitStruct.DMA_Priority=DMA_Priority_VeryHigh;                             //���ȼ��ǳ���      
	DMA_Init(DMA1_Stream6,&DMA_InitStruct);
	DMA_Cmd(DMA1_Stream6,ENABLE);
	
	
	
  Up_To_Down_TypeStruct.Sentry_Satus=1;                                          //1-����δ�ϵ磬0-���̶ϵ�                                      
	Up_To_Down_TypeStruct.Chassis_Real_Position=0;                                 //�Ի���Ϊ��һ�ӽǣ������ң�-3~3
	Up_To_Down_TypeStruct.Get_Hit_flag=2;                                          //2-û���ܵ�������0-װ��0�ܵ�������1-װ��1�ܵ�������3-����װ�׶��ܵ��˹���
	Up_To_Down_TypeStruct.Bass_Satus=2;                                           
	Up_To_Down_TypeStruct.Remain_HP=600;                                           //ʣ��Ѫ������50
	Up_To_Down_TypeStruct.Chassis_Real_Number=0;                                   
	Up_To_Down_TypeStruct.Shoot_Allow_Flag=0;                                      //0-�������䣬1-������ 
	Up_To_Down_TypeStruct.Chassis_Speed_Real=0;
	Up_To_Down_TypeStruct.Locked_Flag=0;
	Up_To_Down_TypeStruct.Bullet_Speed=0;
	
	Down_To_Up_TypeStruct.Target_Locked=0;                                         //0-û���ҵ�Ŀ�꣬1-�ҵ�Ŀ�꣬2-����Ŀ��
	Down_To_Up_TypeStruct.Enemy_Armor_ID=0;
	Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag=0;
	Down_To_Up_TypeStruct.Yaw_Angle_Real=0;
	Down_To_Up_TypeStruct.Distace =250;
}



/**
  * @brief  USART8 DMA�����ж�
  * @param  void
  * @retval void
  * @notes  ���ȼ�Ϊ2
            ÿ�η�����һ�����ݺ�������жϣ��ر�dma����ͬʱ���������ݵĳ�����������
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
  * @brief  USART8�����ж�
  * @param  void
  * @retval void
  * @notes  ���ȼ�Ϊ2
            ����8�����ݼĴ������յ�����֮������жϣ��ڴ��ж��н��������ݳ����������ã�
            Ȼ����ý������ݽ��뺯����40msһ��
  */
void UART8_IRQHandler(void)
{
	BaseType_t pxHigherPriorityTaskWoken;
	char num=0;
	if(USART_GetITStatus(UART8,USART_IT_IDLE)==SET)
	{
		//������ڿ����жϣ������ֲ�712ҳ��
		 num = UART8->SR;
     num = UART8->DR; 
		 num = 10-DMA_GetCurrDataCounter(DMA1_Stream6);
		 
		 //�������ý������ݳ���
		 DMA_Cmd(DMA1_Stream6,DISABLE); 
		 DMA_SetCurrDataCounter(DMA1_Stream6,10);     
		 DMA_Cmd(DMA1_Stream6,ENABLE);
		
	   vTaskNotifyGiveFromISR(Down_To_Up_Decode_Task_Handler,&pxHigherPriorityTaskWoken);		 
		 portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);	 	
	}
}
//








/**************************************************�ϰ嵽�°�ͨ�����ݷ���***********************************

Get_Hit_flag��װ�װ屻�����־λ��0����������ص�0��װ�װ屻��1������������1��װ�װ屻�����2����û�б������3��������װ�װ嶼�����
              ����������򣬲���ϵͳ���뺯���и��ݲ���ϵͳ�����ı���װ�װ�ID�Լ��°崫������̨Yaw_Angle_Real�������ã�
              ����������򣬶�ʱ��7�жϷ������У�12û���ܵ�����������Ϊ2
              ����������򣬶�ʱ��7�жϷ������У������жϵз�Σ�յȼ�
              ����������򣬵����Զ�����Ӧ��ϵ�к����У������жϵ�������һ����
              ����������򣬴���8���ս��뺯���У�����������̨ģʽ

Which_Color_I_Am����������ɫ��0����ʾ�췽��1��ʾ����
                  ����������򣬲���ϵͳ���뺯���У����ݲ���ϵͳ�����Ļ�����ID�������ã�
                  �����������ʹ��NUC���ͺ���������NUC��

Shoot_Allow_Flag�����������־λ��0���������䣬1������
                  ����������򣬲��̿��ƺ����У�ң����ģʽ�Լ�����ϵͳ������������ϵͳ������ʼ��־λ��ң��������NUC����Ӳ��û�жµ��������ã�
                  �����������Ħ���ֿ�����������ʹ�ã����ڿ��ƶ���Ƿ�򿪣�

Chassis_Real_Number��202�����Ȧ����������Chassis_Motor_201_Number���ڱ��������ʱȦ��Ϊ0�������ƶ����Ȧ����������
										 �����������CAN1�����жϷ������У����ݶ�202����ĽǶ��жϽ������ӻ��߼��٣�                     
										 ����������򣬵���ת��ϵ�к����У��������Һ��⴫������ֵ���������ֵ�����Ҳ�ֵУ׼�����ۼ���  
										 ����������򣬶�ʱ��7�жϷ������У�ÿ10ms�������Һ��⴫������ֵ���������ֵ�����Ҳ�ֵУ׼�����ۼ���										 
										 ����������򣬵���ת��ϵ�к����У������ж��Ƿ����ٽ�ֵ��
										 ����������򣬶�ʱ��7�жϷ������У������жϵ�������һ��λ�ñ������
										 ����������򣬲���ϵͳ���뺯���У������жϵ�������һ��λ�ñ������
                     ����������򣬲�û�б�ʹ�ã�
										 
Sentry_Satus���ڱ�״̬��־λ��1��˵������û�б��ϵ磬0��˵�����̱��ϵ�
              ����������򣬲���ϵͳ���뺯���У����ݲ���ϵͳ�����Ļ����˵��������ѹ�������ã�            
              ����������򣬲�û�б�ʹ�ã�
							
Bullet_Speed�������ٶ�							
              �����������USART8_Send�����У�����=(ShootData.bulletSpeed-20.0f)*10.0f�������ã�
					    ����������򣬲�û�б�ʹ�ã�
							
Locked_Flag��������������־λ��1��˵��������������0��˵��������û�б�����
             ����������򣬶�ʱ��7�жϷ������У�ÿ7���ж�һ���Ƿ񿨵����ã�  
						 ����������򣬲�û�б�ʹ�ã�					 
   
*********************************************************************************************************************************/  
/**
  * @brief  USART8���������� 
  * @param  void
  * @retval void
  * @notes  10msһ��
	          ���ȼ�Ϊ5���ڶ�ʱ��7�б����ã�ÿ10ms����һ�Σ���һЩ���ݾ�������֮���͸��°壬��10�����ݣ�֡ͷΪ0x7A��֡βΪ0X7B  
						
            ÿ10���룬����ģʽ�ָ���λ�òɼ���־λ�ָ���������־λ�ָ����������巢����Ϣ��������λ �Ҳ�PH12-B,���PH11-C         
  */
int Left_Sensor_Number=1;
int Right_Sensor_Number=1;	

void USART8_Send_Task(void *pvParameters)
{
   USART8_Init();
   uint32_t err;
	
	vTaskDelay(10);
	
	//���⴫����
  while(1)
	{
		//�ȴ�����֪ͨ
		err=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		if(err==1)
		{
			//�������ݴ���
			//����,����Ħ�����ܳ�235.62mm
			Up_To_Down_TypeStruct.Chassis_Real_Position=(float)Chassis_Motor_201_Number/19.0f*235.62f+100;	//��λmm
			
			Right_Ultrasonic_Distance=3823-Up_To_Down_TypeStruct.Chassis_Real_Position;
		
			Up_To_Down_TypeStruct.Chassis_Speed_Real=(235.62f/19.0f/60.0f*(float)Chassis_Motor_201.Real_Speed);//��λmm/s
			
			Up_To_Down_TypeStruct.Bullet_Speed=Down_Bullet_Speed;
			
			
			//����֡�ṹ
			//֡ͷ
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
			
			
			//֡β
			Up_To_DOWN_Tx[16]=0x7B;
			
			//����DMA��������������
			DMA_Cmd(DMA1_Stream0,ENABLE);
			
/////////////////////////////////////////////////////////////////////////////////////////			
			//���̸�ģʽ����ʱ��
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
				
				
				//����ģʽ��,Chassis_Get_Big_Fire,Chassis_Get_Small_Hurt,Chassis_Limite,����9s
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
				
				
				
				//���ֵ��˳���5s
				else if(Chassis_Mode==Chassis_Find_Target)
				{
					if(Mode_Time>500)
					{
						Chassis_Mode=Chassis_Normal;					
					}
				}
				//��ͣ��3sģʽ������10��
				else if(Chassis_Mode==Chassis_Stop_3S)
				{
					if(Mode_Time>1000)
					{
						Chassis_Mode=Chassis_Normal;					
					}
				}
			
				
				//���ܶ���˽���������25s
				else
				{
					if(Mode_Time>2500)
					{
						Chassis_Mode=Chassis_Normal;	
					}
				}
			}
			//������ģʽΪ����ģʽʱ��ģʽʱ��Ϊ0
			else Mode_Time=0;
			
			//λ�òɼ�
			//����ǰ5s����λ���ռ�
			if(Position_Collet_Flag)
			{
				Position_Collet_Time++;
				if(Position_Collet_Time>500)
				{
					Position_Collet_Flag=0;
				}
			}
			
			//�����12s��û�ܵ������������ܹ�����־λΪ2��û�ܵ�����
			if(Up_To_Down_TypeStruct.Get_Hit_flag!=2)
			{
				Danger_Time++;
				if(Danger_Time>1200)
				{
					Danger_Time=0;
					Up_To_Down_TypeStruct.Get_Hit_flag=2;
				}
			}
			
			
			
			
			//ÿ10ms��ȡ���⴫������ֵ
			Left_Sensor_Number=GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_11);
			Right_Sensor_Number=GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_12);
	
	    //�Ӽ����������⿴������ഫ������ֵΪ0������⵽
			if(Left_Sensor_Number==0)
			{
				//λ�����Ʊ�־λΪ-1
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
			
			
			
			//ÿ10ms����������������11us�ĸߵ�ƽ
			GPIO_SetBits(GPIOF,GPIO_Pin_1|GPIO_Pin_10 );
			delay_us(11);
			GPIO_ResetBits(GPIOF,GPIO_Pin_1|GPIO_Pin_10 );
			
			
			
			
		}	
	}
}

/***********************************�°嵽�ϰ�ͨ�����ݷ���*************************************

Target_Locked��NUCĿ��������־λ��0����û���ҵ�Ŀ�꣬1��ʶ��Ŀ�꣬2������Ŀ�꣬��ʼֵ�ڴ���8��ʼ��������Ϊ0
               ���°����NUC���ݽ����������У�����NUC�������ľ���������ã�
               ���ϰ���򣬴���8���ݽ��뺯���У��������õ���ģʽ

Distance���з�����
          ���°����NUC���ݽ����������У�����NUC�������ĵз�����������ã�
          ���ϰ���򣬲��̿��ƺ����У�ʹ���Զ�ģʽʱ�������ò���ת��
                 
Enemy_Armor_ID���з�װ�װ��ID��
                ���°����NUC���ݽ����������У�����NUC��������=NUC_Data_TypeStruct.Armor_Number�������ã�
                ���ϰ���򣬲�û��ʹ�á�

Yaw_Angle_Real���������ǣ���Χ0-360��������Yaw_Real_Angle
                ���°����CAN1���λ���жϺ����У����ݵ�������ĽǶ�ֵ�������ã�
                ���ϰ���򣬴���8���ݽ��뺯���Ͳ���ϵͳ���ݽ��뺯���У�ʹ�ã������ж��Ƿ񱻺ܶ���˴��
								
NUC_Shoot_Allow_Flag���Զ�ģʽ��NUC�Ƿ�Ҫ�������0�������������1�������������
                      ���°����NUC���ݽ����������У�����NUC�������ĵ�һ�����ݽ������ã�
                      ���°���򣬶�ʱ��7�жϷ������У������Ƿ�ɹ����յ�NUC���ݽ������ã�
                      ���ϰ���򣬲��̿��ƺ����У�ʹ���Զ�ģʽʱ�ж��Ƿ��������

******************************************************************************************************/

/**
  * @brief  �����嵽���������ݽ��������� 
  * @param  void
  * @retval void
  * @notes ��������̨���ݣ�����װ�װ��Ƿ񱻴�����Լ��з��������õ���ģʽ         
  */
void Down_To_Up_Decode_Task(void *pvParameters)
{
	//��̨����
	int Angle_Direction=0;
  uint32_t err;
	while(1)
	{
		//�ȴ�����֪ͨ
		err=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		if(err==1)
		{
			//����У��
			if( (Down_To_Up_Rx[0]==0x7A) && (Down_To_Up_Rx[9]==0x7B) )
			{
				//���ݻ�ȡ
				Down_To_Up_TypeStruct.Target_Locked       =Down_To_Up_Rx[1];
				Down_To_Up_TypeStruct.Distace             =int8ToShort(Down_To_Up_Rx,2);
				Down_To_Up_TypeStruct.Enemy_Armor_ID      =Down_To_Up_Rx[4];
				Down_To_Up_TypeStruct.Yaw_Angle_Real      =int8ToShort(Down_To_Up_Rx,5);
				Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag=Down_To_Up_Rx[7];
				
				
				//���������崫�ص���̨�Ƕ�ֵ�жϣ���̨�Ƿ����ǰ���������ǰ����Ϊ0�������ǰ������Ի��ز���Ϊ1
				Angle_Direction=( (Down_To_Up_TypeStruct.Yaw_Angle_Real>90)&&(Down_To_Up_TypeStruct.Yaw_Angle_Real<270) ) ;
				
				//����з��ľ���С��50��Ŀ����������2����ʶ�𵽵���֮��Ŀ������Ӧ�õ���1
				//�з�������50���ڣ�˵���Ѿ�ʶ��һ���ܽ��ĵ���
				if(Down_To_Up_TypeStruct.Target_Locked==2)
				{ 
					//˵����0��װ�װ��ڻ��ز࣬1��װ�װ������
					//����̨��Եķ����뱻�����װ�װ峯��ͬ����˵���кܶ�����ڴ��ڱ�
					if(Angle_Direction == Up_To_Down_TypeStruct.Get_Hit_flag )
					{
						//ǰ������װ�װ嶼�����
						Up_To_Down_TypeStruct.Get_Hit_flag=3;	
						if(Chassis_Mode!=Chassis_Many_Enemy)
						{
							Chassis_Mode=Chassis_Many_Enemy;
							Mode_Time=0;
						}
					}
					else Chassis_Mode=Chassis_Lock_Target;	
				}
				//���°巴�������ĵз����뻹�Ƚ�Զʱ��������ģʽΪ����ģʽʱ�л�����ģʽΪ���ֵ���
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
















/*********************************************�ϰ���NUC����ͨ��**********************************************************************/
/**
  * @brief  USART6-NUCͨ�ų�ʼ��
  * @param  void
  * @retval void
  * @notes  USART6_RX -----PG9    USART6_TX-----PG14     
            ������DMA2_Stream6��DMA_Channel_5��ʹ��DMA2_Stream6_IRQHandler
            ������DMA2_Stream1��DMA_Channel_5��ʹ��USART6_IRQHandler
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
//�����ж�
	NVIC_InitStruct.NVIC_IRQChannel                   =USART6_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =6;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        =0;
	NVIC_InitStruct.NVIC_IRQChannelCmd                =ENABLE ;
	NVIC_Init(&NVIC_InitStruct);
//���ͺ��ж�
	NVIC_InitStruct.NVIC_IRQChannel                   =DMA2_Stream7_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =9;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        =0;
	NVIC_InitStruct.NVIC_IRQChannelCmd                =ENABLE ;
	NVIC_Init(&NVIC_InitStruct);
	
/* ------------------------------------- Configure DMA  ����---------------------------------------*/

	USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);
	
	DMA_Cmd(DMA2_Stream1,DISABLE);
	DMA_DeInit(DMA2_Stream1);
	
	DMA_InitStruct.DMA_Channel=DMA_Channel_5;                                      //ͨ�����ã�ͨ��5��
	DMA_InitStruct.DMA_BufferSize=15;                                              //����������Ŀ                
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralToMemory;                             //���ݴ��䷽�����衪���洢����                   
	DMA_InitStruct.DMA_FIFOMode=ENABLE;                                            //ʹ��FIFOģʽ
	DMA_InitStruct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;               //��ֵ1/4      
	DMA_InitStruct.DMA_Memory0BaseAddr=(uint32_t)USART6Rx_Info;                    //�洢�����ݵ�ַ
	DMA_InitStruct.DMA_MemoryBurst=DMA_MemoryBurst_Single;                         //�洢��ͻ������
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;                     //�洢�����ݸ�ʽ���ֽڣ�
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;                             //�ڴ��ַ����
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;                                     //�Ƿ�ѭ�����ͣ�ѭ������ͨģʽֻ�ܽ���һ�Σ�
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&(USART6->DR);                 //�����ַ 
	DMA_InitStruct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;                 //����ͻ������         
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;             //�������ݸ�ʽ
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;                    //�����ַ������
	DMA_InitStruct.DMA_Priority=DMA_Priority_VeryHigh;                             //���ȼ��ǳ���      
	DMA_Init(DMA2_Stream1,&DMA_InitStruct);
	DMA_Cmd(DMA2_Stream1,ENABLE);

///*********************************************DMA����************************************/
	
	USART_DMACmd(USART6,USART_DMAReq_Tx,ENABLE);
	
  DMA_Cmd(DMA2_Stream7,DISABLE);
	
  DMA_DeInit(DMA2_Stream7);                                                 //ΪDMA����ͨ��
	DMA_InitStruct.DMA_Channel           =DMA_Channel_5;
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&(USART6->DR);          //��ʼ��ַ
	DMA_InitStruct.DMA_Memory0BaseAddr   =(uint32_t)Send_NUC;                 //�洢����
	DMA_InitStruct.DMA_DIR               =DMA_DIR_MemoryToPeripheral;         //���䷽��
	DMA_InitStruct.DMA_BufferSize        =27; //����������
	DMA_InitStruct.DMA_PeripheralInc     =DMA_PeripheralInc_Disable;          //�������ģʽ
	DMA_InitStruct.DMA_MemoryInc         =DMA_MemoryInc_Enable;               //�ڴ����ģʽ
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;        //DMA����ʱÿ�β��������ݳ���
	DMA_InitStruct.DMA_MemoryDataSize    =DMA_PeripheralDataSize_Byte;        //DMA����ʱÿ�β��������ݳ���
	DMA_InitStruct.DMA_Mode              =DMA_Mode_Normal;                    //����ģʽ����������
	DMA_InitStruct.DMA_Priority          =DMA_Priority_VeryHigh;              //���ȼ���
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
  * @brief  NUC DMA�����������жϷ�����
  * @param  void
  * @retval void
  * @notes  ÿ����������������֮����������жϷ�������ÿ1msִ��һ��
            �ж��н�DMA�������رղ�����������DNA���ݳ���
            
  */
void DMA2_Stream7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream7,DMA_IT_TCIF7)==SET)
	{
		
	 //�ر�dma������
	 DMA_Cmd(DMA2_Stream7,DISABLE);
	 //��������dma���ݳ���
	 DMA_SetCurrDataCounter(DMA2_Stream7,23);
	
	 	
	}
	DMA_ClearITPendingBit(DMA2_Stream7,DMA_IT_TCIF7);
	DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TEIF7);
}

/**
  * @brief  NUC ����6���ݽ����жϷ�����
  * @param  void
  * @retval void
  * @notes  ���������յ�nuc����������֮���Ƚ�dma�������ر������������ݳ���֮���ٴ�������
            Ȼ��ת��nuc���ݴ�������DMA����USART6->DR��USART6Rx_Info
  */
void USART6_IRQHandler(void)
{
	
	BaseType_t pxHigherPriorityTaskWoken;
	char num=0;
	if(USART_GetITStatus(USART6,USART_IT_IDLE)==SET)
	{
		 num = USART6->SR;
     num = USART6->DR; //������ڿ����жϣ������ֲ�712ҳ��
		 num++;            //û�����壬ֻ����������
		
		 DMA_Cmd(DMA2_Stream1,DISABLE); 
		 DMA_SetCurrDataCounter(DMA2_Stream1,10);      //�������ý������ݳ���
		 DMA_Cmd(DMA2_Stream1,ENABLE);                 //���䣬������3���ݼĴ��������ݴ��䵽USART6Rx_Info
		
		//��NUC���ݽ���������֪ͨ���������������б�
		 vTaskNotifyGiveFromISR(NUC_Decode_Task_Handler,&pxHigherPriorityTaskWoken);
		//���������л�
		 portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
	}	
}

//






/**
  * @brief  ���������ݷ���������
  * @param  void
  * @retval void
  * @notes  ����̨yaw���pitch������ݾ�������֮�󴢴�Send_NUC������
            Ȼ������dma�����������ݷ��ͳ�ȥ�������������ݽ��뺯���б�����
            ÿ1ms����һ��Send_NUC��Send_NUC���ٷ���
            ÿ500���룬��0.5����ʧ��Ѫ���ж��Ƿ񱻴�������ͬʱ����λ���ж�
            ÿ1000���룬����1s�ڼ��ٵ�Ѫ�����ܹ���״̬���ж�Σ�յȼ�
            ÿ7�룬�ж�һ���Ƿ񿨵��������̷�תʱ�䳬��560ms˵�������ˣ���ʱ������������־λ��Ϊ1
            ÿ22�룬�ж��Ƿ�ʱ�俨�������������Locked_Flag=0
            
  */
/******************************�ϰ嵽��NUCͨ�����ݷ���****************************


Yaw_Angle��YAW��ĽǶ�
           �������Ȧ���������ʵ�Ƕ�
           ����̨�Զ����ƺ�������̨�ֶ����ƺ����б�ʹ�ã����ڿ�����̨YAW��ת��

Pitch_Angle��PITCH��ĽǶȣ�
             �ڴ���3���ͺ�������(Pitch_Motor_206.Real_MechanicalAngle-5900)*(1.46502259f)+500�õ�

Yaw_Speed��YAW����ٶȣ���λΪ������ԭʼ��λ
           �ڴ���3���ͺ�������IMU_Real_Data.Gyro_Y*100;�õ�
					 
Pitch_Speed��PITCH����ٶȣ��ɵ���ٶ�ת��Ϊ��/s֮���ٳ���3����100�õ�

Find_Target_1����̨�Ƿ����ҵ����˵�״̬��0��˵��û�д����ҵ����˵�ģʽ��1��˵�������ҵ����˵�ģʽ

Which_Color_I_Am��������ɫ��0��Ϊ��ɫ��1Ϊ��ɫ
                  
Enemy_Armor_ID����NUCʶ�𵽵ĵ��˵�װ�װ����
*************************************************************************************/
	int     Yaw_Angle;
	short   Pitch_Angle,
				  Pitch_Speed,
			  	Yaw_Speed;

void UP_Send_NUC_Task(void *pvParameters)
{ 
	
	Tick_TIM7_Init (100);  //��ʱ��7��ʼ���������жϣ�����λ���ж�
	uint32_t err;

	char  Find_Target_1=0;
	
  //ʣ��Ѫ��
	static u32 TimeTick1ms=0;
	static int HP_Last_500ms=600;
	static int HP_Last_1000ms=600;
	
 while(1)
	{
		//�ȴ�����֪ͨ
		err=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		if(err==1)
		{
		//printf("123123123");
		//	USART6->DR=1;
			//�ڲ�����++
			TimeTick1ms++;

			//����ȷ����̨�Ƿ����ҵ����˵�״̬
			if(PTZ_Mode==PTZ_Find_Enemy)
			{
				Find_Target_1=0;
			}
			else Find_Target_1=1;
			
			// Yaw��ĽǶȾ��ǵ������������ʵ�Ƕ�,�ó�ʼʱ��yawΪ0   
			Yaw_Angle=(float)(Yaw_Real_MechanicalAngle-6120)/8192.0f*180.0f*10;
			
			// Pitch��ĽǶȾ�����ʵ�Ļ�е�Ǵ���֮���͹�ȥ.8192��Ӧ180�ȣ���λ����*10
	    Pitch_Angle=(float)(Pitch_Motor_206.Real_MechanicalAngle)/8192.0f*180.0f*10;
	
	     //��λ����/s*10
			Yaw_Speed=((float)Yaw_Motor_205.Real_Speed*360.0f)/(60.0f)*10.0f;
	
	     //��λ����/s*10
	    Pitch_Speed=((float)Pitch_Motor_206.Real_Speed*360.0f)/(60.0f)*10.0f;
			
			Bullet_Speed =Up_Bullet_Speed;
			
			Send_NUC[0]=0x7a;
			Send_NUC[1]=Bullet_Speed;
			Send_NUC[2]=bit32TObit8(0,(short)Yaw_Angle);
			Send_NUC[3]=bit32TObit8(1,(short)Yaw_Angle);	
			Send_NUC[4]=bit32TObit8(2,(short)Yaw_Angle);
			Send_NUC[5]=bit32TObit8(3,(short)Yaw_Angle);	
			Send_NUC[6]=shortTou8(0,(short)Pitch_Speed);//�����ٶ������Ӿ���û��ʹ��
			Send_NUC[7]=shortTou8(1,(short)Pitch_Speed);
			Send_NUC[8]=shortTou8(0,(short)Yaw_Speed);
			Send_NUC[9]=shortTou8(1,(short)Yaw_Speed);	
			//��9�����������ж������ڱ���̨�ǲ��Ǵ����ҵ����˵�״̬�Լ��������ɫ��ʲô��00��ʾ�췽�����ҵ�����
		  Send_NUC[10]=shortTou8(0,(short)Up_To_Down_TypeStruct.Chassis_Real_Position );//λ�ã���λmm
	    Send_NUC[11]=shortTou8(1,(short)Up_To_Down_TypeStruct.Chassis_Real_Position );	
			Send_NUC[12]=(Find_Target_1)|(Up_To_Down_TypeStruct.Which_Color_I_Am<<1)|(Output_On<<2)|(Swing_Flag<<3);//������ɫ���������з��͹����ģ��������Ǻ췽ʱ����0��������ʱ�����1��
			Send_NUC[13]=Down_To_Up_TypeStruct.Enemy_Armor_ID;
    	Send_NUC[14]=shortTou8(0,(short)Up_To_Down_TypeStruct.Chassis_Speed_Real );//�ٶȣ���λmm/s
	    Send_NUC[15]=shortTou8(1,(short)Up_To_Down_TypeStruct.Chassis_Speed_Real );
			Send_NUC[16]=Up_To_Down_TypeStruct.Enemy_State;  
	    Send_NUC[17]=Up_To_Down_TypeStruct.Enemy_HP_HL;
			Send_NUC[18]=shortTou8(0,(short)Pitch_Angle);
			Send_NUC[19]=shortTou8(1,(short)Pitch_Angle);
			Send_NUC[20]=shortTou8(0,Chassis_Accel);
			Send_NUC[21]=shortTou8(1,Chassis_Accel);
			
			
			
			Send_NUC[22]=0x7b;

	
			
		  DMA_Cmd(DMA2_Stream7,ENABLE);		//******************************************************************************************


     /*ÿ500ms*/ 
		//��0.5����ʧ��Ѫ���ж��Ƿ񱻴�������ͬʱ����λ���ж�
		if(TimeTick1ms%500==0)
		{


			//�����0.5s֮����ʧ�˴���40��Ѫ�������л�����ģʽΪ�ܵ�������
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
			 //����0.5sǰѪ��
       HP_Last_500ms=GameRobotState.remain_HP;			
		}	
		/*ÿ1s*/
		//����1s�ڼ��ٵ�Ѫ�����ܹ���״̬���ж�Σ�յȼ�
		if(TimeTick1ms%1000==0)
		{
			//��1������ʧѪ������30�ҹ۲⵽����ID��û�и����ܵ�
			if((HP_Last_1000ms-GameRobotState.remain_HP>=30)&&(View_Enemy_ID)&&(Up_To_Down_TypeStruct.Get_Hit_flag!=3))
			{
				//����Σ�յȼ�Ϊ1
				Enemy_Danger_Level[View_Enemy_ID]=1;
			}	
			//����1���ʣ��Ѫ��
			HP_Last_1000ms=GameRobotState.remain_HP;
		}
		/*ÿ7s*/
		//ÿ7���ж�һ���Ƿ񿨵��������̷�תʱ�䳬��560ms˵�������ˣ���ʱ������������־λ��Ϊ1
		if(TimeTick1ms%7000==0)
		{
			if(Locked_Time>8)
			{
				//��ʱ������ʱ������1
				LongTime_Locked_Times++;
				Locked_Flag=0;
				//����������־λΪ1
				Up_To_Down_TypeStruct.Locked_Flag=1;
			}
			
			//����ʱ�����
			Locked_Time=0;
			
			
			BeHurt_10s=0;
			
			
		}
		/*ÿ22s*/
		//�ж��Ƿ�ʱ�俨�������������Locked_Flag=0
		if(TimeTick1ms%22000==0)
		{
			//��ʱ��������־λΪС��10�����ۼ�С��10��7���п�������������Locked_Flag����1
			if(LongTime_Locked_Times<=10)
			{
				if(!Locked_Flag)
				{
					Locked_Flag=1;
				}
			}
			//�����г���10��7���п�������
			else Locked_Flag=0;
		}
		
				/*ÿ30s*/

		if(TimeTick1ms%30000==0)
		{
			Sway_Buttle=1;
		}
		
		
	}
 }
}
/*******************************��NUC���ϰ�ͨ�����ݷ���*****************************

USART6Rx_Info[0]�����������ݱ�־λ��0x1f��˵�������䣬��������������
                  ��NUC���ݽ�����������ʹ�ã����ڸ�Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag��ֵ

USART6Rx_Info[1]��USART6Rx_Info[2]��Yaw_Angle��YAW������ƶ��Ƕȣ��൱��ң������ch2ͨ��ֵ���ǿ�����̨Ҫ�ƶ���ֵ
                                    ����̨�Զ����ƺ��������ڿ�����̨�ƶ�

USART6Rx_Info[3]��USART6Rx_Info[4]��Pitch_Angle��PITCH������ƶ��Ƕȣ��൱��ң������ch3ͨ��ֵ���ǿ�����̨Ҫ�ƶ���ֵ
                                    ����̨�Զ����ƺ��������ڿ�����̨�ƶ�

USART6Rx_Info[5]��Armor_Number���з�װ��ID�������ֲ�ͬ����Ļ�����
									
USART6Rx_Info[6]��Armor_Type��װ�װ����ͣ�0��ûʶ��װ�װ壬1��ʶ��Сװ�װ壬2��ʶ�𵽴�װ�װ�
                  ��NUC���ݽ�����������ʹ�ã������л���̨ģʽ
									
USART6Rx_Info[7]��USART6Rx_Info[8]��Enemy_distance
                                    �з�����,�ڷ�����ƺ������������ò���ת��
 
USART6Rx_Info[9]��NUC����У��λ��
                  ����NUC���ݽ�����������ʹ�ã�����USART6Rx_Info[1] + USART6Rx_Info[3] + USART6Rx_Info[5])  % 255 == USART6Rx_Info[9]У��								

**********************************************************************************/
/**
  * @brief  Nuc���ݽ�������
  * @param  void
  * @retval void
  * @notes  ÿ�ν�����NUC������������ʱִ��һ��
            Nuc��ʼ������ʼ������6������nuc��ͨѶ������NUC�������񣬵����ݾ���У��֮��˵��nuc�Ѿ��������ˣ����ʱ��nuc��ʧʱ�����0��
            ��NUC�������������е�װ�װ����Ͳ�Ϊ0��ʱ��˵���Ѿ�ʶ���˵��ˣ���ʱ����̨ģʽ�л�Ϊʶ�𵽵��ˣ�
            �����������װ�װ�������0�Ļ���˵��û�м�⵽װ�װ壬���ʱ����̨����ģʽ�л�Ϊû��ʶ�𵽵���            
  */
	int NUC_Get_Time=0;
	short Last_Pitch_Angle=0;
void NUC_Decode_Task(void *pvParameters)
{
//  static	uint16_t NUC_Start_Time=0;
	uint32_t err;
	int Lost_Times=0;     //��ʧ����
	NUC_Init();
	vTaskDelay(200);
	while(1)
	{
		//�ȴ�����֪ͨ
		err=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);

		if(err==1)
		{//����У��
			if( ( USART6Rx_Info[0] + USART6Rx_Info[3] + USART6Rx_Info[5]) % 255 == USART6Rx_Info[9] ) 
			{
				// Nuc��ʧʱ�����0˵���Ѿ����ܵ���nuc����������ܳ�ʱ��û�н��յ�nuc������,�������ᱨ��
				NUC_Lost_Time=0;
				
	
				//��0������Ϊ0xfe������ϰ���Է���
				if( USART6Rx_Info[0]==(char)0x1f)    Up_NUC_Data_TypeStruct.NUC_Shoot_Allow_Flag=1;
				else                                 Up_NUC_Data_TypeStruct.NUC_Shoot_Allow_Flag=0;
				
				//װ�����ͣ�0δʶ�𵽣�1Сװ�ף�2��װ��
				Up_NUC_Data_TypeStruct.Armor_Type     = USART6Rx_Info[6] ;
			
				//���ʶ��
				if(Up_NUC_Data_TypeStruct.Armor_Type !=0)
				{
					NUC_Get_Time++;
					//Yaw�����ݽ���
          Up_NUC_Data_TypeStruct.Yaw_Angle=u8ToShort(USART6Rx_Info,1);			
					//yaw��λ
					if(Up_NUC_Data_TypeStruct.Yaw_Angle>=Yaw_Limit)Up_NUC_Data_TypeStruct.Yaw_Angle=Yaw_Limit;
					if(Up_NUC_Data_TypeStruct.Yaw_Angle<=-Yaw_Limit)Up_NUC_Data_TypeStruct.Yaw_Angle=-Yaw_Limit;
					
										
					//Pitch�����ݽ��룬
					Up_NUC_Data_TypeStruct.Pitch_Angle=u8ToShort(USART6Rx_Info,3); 
					if(Up_NUC_Data_TypeStruct.Pitch_Angle>=Pitch_Limit)Up_NUC_Data_TypeStruct.Pitch_Angle=Pitch_Limit;
					if(Up_NUC_Data_TypeStruct.Pitch_Angle<=-Pitch_Limit)Up_NUC_Data_TypeStruct.Pitch_Angle=-Pitch_Limit;
		
		
      	//	if(abs(Last_Pitch_Angle-Up_NUC_Data_TypeStruct.Pitch_Angle>=10))NUC_Get_Time=0;
					//����
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
//  				else//����
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
					
					
				
					

         	//�з�װ�װ����
					Up_NUC_Data_TypeStruct.Armor_ID= USART6Rx_Info[5];
					
					
					//�з�����
					Up_NUC_Data_TypeStruct.Enemy_distance =u8ToShort(USART6Rx_Info,7);
								
					//Ŀ�궪ʧ��������0
					Lost_Times=0;
					//������̨ģʽΪ�ҵ�����
					PTZ_Mode=PTZ_Find_Enemy;									
				}
				
				//���û��ʶ��װ�װ壬װ������Ϊ0����̨ģʽ�л�Ϊû���ҵ�����
				else  
				{
					NUC_Get_Time=0;
					Up_NUC_Data_TypeStruct.Yaw_Angle      = 0;
					Up_NUC_Data_TypeStruct.Pitch_Angle    = 0;
					Up_NUC_Data_TypeStruct.Enemy_distance = 0;
					
					Up_NUC_Data_TypeStruct.Armor_ID=0x00;
		
					//û��ʶ��װ�װ�˵��˵��Ŀ���Ѿ���ʧ���ʱ��ʧʱ������
					Lost_Times++;
					if(Lost_Times>50)
					{
						//�л���̨ģʽ���ҵ����˱�ɶ�ʧ����
						if(PTZ_Mode==PTZ_Find_Enemy)
						{
							//�л���̨ģʽ���ҵ����˱�ɶ�ʧ����
							
							PTZ_Mode=PTZ_Lost_Enemy;
						}						
					}
				}
			}
		}
	}
}




//


/*********************************************��������ת������**********************************************************************/



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





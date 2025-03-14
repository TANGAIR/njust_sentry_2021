#include "main.h"

/**********************************���ݶ�����*****************************/
Up_To_Down_Typedef  Up_To_Down_TypeStruct;
Down_To_Up_Typedef  Down_To_Up_TypeStruct;
NUC_Data_Typedef    Down_NUC_Data_TypeStruct;

int8_t Up_To_DOWN_Rx[17];//�����嵽�����壬������������
int8_t Down_To_Up_Tx[10];//�����嵽�����壬������������
char   Usart3Rx_Info[10];//��������������
//char        Send_NUC[20];//��������������
char        Send_NUC[23];//��������������

short Chassis_Accel=0;

char Swing_Flag=0;
/**********************************����6��ӡ***************************/


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
//���ڴ�ӡ����
int fputc(int ch, FILE *f)
{
	while (USART_GetFlagStatus(USART6,USART_FLAG_TC) == RESET);//�ȴ�֮ǰ���ַ��������
	USART_SendData(USART6, (uint8_t)ch);
	return (ch);
}
#endif
/**
  * @brief  ��ʼ��USART6
  * @param  void
  * @retval void
  * @notes  ����6��ӡ��ʼ��    USART6_TX-PG14      USART6_RX -----PG9
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
	GPIO_InitSturct.GPIO_OType=GPIO_OType_PP;                                      //�������
	GPIO_InitSturct.GPIO_Mode=GPIO_Mode_AF;                                        //����ģʽ
	GPIO_InitSturct.GPIO_PuPd=GPIO_PuPd_NOPULL;                                    //������ 
	GPIO_InitSturct.GPIO_Speed=GPIO_High_Speed;                                    //100MHZ
	GPIO_Init(GPIOG,&GPIO_InitSturct);
	
	USART_InitStruct.USART_BaudRate=115200;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;     //Ӳ����������
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;                       //�շ�ģʽ
	USART_InitStruct.USART_Parity=USART_Parity_No;                                 //����żУ��λ
	USART_InitStruct.USART_StopBits=USART_StopBits_1;                              //�ֽ�
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;                         //���ݳ���
	USART_Init(USART6,&USART_InitStruct);
	USART_Cmd(USART6,ENABLE);
	USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel=USART6_IRQn;                                   //�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;                                     //ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=15;                           //��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;                                  //�����ȼ�
	NVIC_Init(&NVIC_InitStruct);	
}


/**
  * @brief  ����6��ӡ�ж�
  * @param  void
  * @retval void
  * @notes  ��ʱû��
  */
void USART6_IRQHandler(void)
{
  if ( USART_GetITStatus( USART6, USART_IT_RXNE ) != RESET )
  { //���������յ������ݸ���
  }
  if ( USART_GetITStatus( USART6, USART_IT_ORE_RX ) != RESET )
  { 
		
  }
  USART_ClearFlag ( USART6,USART_IT_RXNE | USART_IT_ORE_RX );
}




/*********************************************USART8���°�����ͨ��*************************************************/


/**
  * @brief  ��ʼ��USART8
  * @param  void
  * @retval void
  * @notes  ������������֮���ͨ�ţ���Down_To_Up_Tx�е�10�����ݷ��͸������壬�����������10�����ݴ��浽Up_To_DOWN_Rx��
            ������DMA1_Stream0��DMA_Channel_5��ʹ��DMA1_Stream0_IRQHandler
            ������DMA1_Stream6��DMA_Channel_5��ʹ��UART8_IRQHandler
            ���ȼ���Ϊ 8
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
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=8;                           //��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;                                  //�����ȼ�
	NVIC_Init(&NVIC_InitStruct);	
	
	NVIC_InitStruct.NVIC_IRQChannel=DMA1_Stream0_IRQn;                             //�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;                                     //ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=8;                           //��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;                                  //�����ȼ�
	NVIC_Init(&NVIC_InitStruct);	
	
	
/************************************************DMA����********************************/
	USART_DMACmd(UART8,USART_DMAReq_Tx,ENABLE);                                    //����DMAʹ��
	
	DMA_Cmd(DMA1_Stream0,DISABLE);                                                 //DMAʧ�ܣ�ֻ����DMAʧ�ܵ�������ܶ�DMA�Ĵ������в�����
	DMA_DeInit(DMA1_Stream0);                                                      //��DMA���üĴ�������Ϊȱʡֵ
	
   DMA_InitStruct.DMA_Channel=DMA_Channel_5;                                     //ͨ�����ã�ͨ��5��
	DMA_InitStruct.DMA_BufferSize=15;                                              //����������Ŀ                
	DMA_InitStruct.DMA_DIR=DMA_DIR_MemoryToPeripheral;                             //���ݴ��䷽��(�洢�������裩                   
	DMA_InitStruct.DMA_FIFOMode=ENABLE;                                            //ʹ��FIFOģʽ
	DMA_InitStruct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;               //��ֵ1/4      
	DMA_InitStruct.DMA_Memory0BaseAddr=(uint32_t)Down_To_Up_Tx;                    //�洢�����ݵ�ַ
	DMA_InitStruct.DMA_MemoryBurst=DMA_MemoryBurst_Single;                         //�洢��ͻ������
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;                     //�洢�����ݸ�ʽ���ֽڣ�
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;                             //�ڴ��ַ����
	DMA_InitStruct.DMA_Mode=DMA_Mode_Normal;                                       //�Ƿ�ѭ�����ͣ���ѭ����
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&(UART8->DR);                  //�����ַ 
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
	DMA_InitStruct.DMA_Memory0BaseAddr=(uint32_t)Up_To_DOWN_Rx;                    //�洢�����ݵ�ַ
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

  Up_To_Down_TypeStruct.Sentry_Satus=1;                                          //1-�����е磬0-���̶ϵ�                                       
	Up_To_Down_TypeStruct.Chassis_Real_Position=0;                                 //�Ի���Ϊ��һ�ӽǣ������ң�0-4
	Up_To_Down_TypeStruct.Get_Hit_flag=2;                                          //2-û���ܵ�������0-װ��0�ܵ�������1-װ��1�ܵ�������3-����װ�׶��ܵ��˹���
	Up_To_Down_TypeStruct.Bass_Status=2;                                           //2-100%������1-50%���� �� 0-�޷���                                        
	Up_To_Down_TypeStruct.Chassis_Real_Number=0;                                   //ÿ��10s��һ����	
	
	Down_To_Up_TypeStruct.Target_Locked=0;                                         //0-û���ҵ�Ŀ�꣬1-�ҵ�Ŀ�꣬2-����Ŀ��
	Down_To_Up_TypeStruct.Target_Position=0;
	Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag=0;
	Down_To_Up_TypeStruct.Yaw_Angle_Real=0;
	Down_To_Up_TypeStruct.Enemy_Armor_ID=0;
}

/**
  * @brief  USART8 DMA�����ж�
  * @param  void
  * @retval void
  * @notes  ÿ�η�����һ�����ݺ�������жϣ��ر�dma����ͬʱ���������ݵĳ�����������
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
  * @brief  USART8�����ж�
  * @param  void
  * @retval void
  * @notes  ����8�����ݼĴ������յ�����֮������жϣ��ڴ��ж��н��������ݳ����������ã�Ȼ�������������ݽ�������
  */
void UART8_IRQHandler(void)
{
	char num=0;
	BaseType_t pxHigherPriorityTaskWoken;
	if(USART_GetITStatus(UART8,USART_IT_IDLE)==SET)
	{
		 num = UART8->SR;
     num = UART8->DR; //������ڿ����жϣ������ֲ�712ҳ��
		 num++;           //û�����壬ֻ����������
		
		 DMA_Cmd(DMA1_Stream6,DISABLE); 
		 DMA_SetCurrDataCounter(DMA1_Stream6,17);      //�������ý������ݳ���
		 DMA_Cmd(DMA1_Stream6,ENABLE);
		//�����ϰ嵽�°�����ݽ�������
		 vTaskNotifyGiveFromISR(Up_To_Down_Decode_Task_Handler,&pxHigherPriorityTaskWoken);	
		 portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
	}		
}

/***********************************�°嵽�ϰ�ͨ�����ݷ���*************************************

Target_Locked��NUCĿ��������־λ��0����û���ҵ�Ŀ�꣬1��ʶ��Ŀ�꣬2������Ŀ�꣬��ʼֵ�ڴ���8��ʼ��������Ϊ0
               ���°����NUC���ݽ����������У�����NUC�������ľ���������ã�
               ���ϰ���򣬴���8���ݽ��뺯���У��������õ���ģʽ

Distance���з�����,��λcm
          ���°����NUC���ݽ����������У�����NUC�������ĵз�����������ã�
          ���ϰ���򣬲��̿��ƺ����У�ʹ���Զ�ģʽʱ�������ò���ת��
                 
Enemy_Armor_ID���з�װ�װ��ID��
                ���°����NUC���ݽ����������У�����NUC��������=Down_NUC_Data_TypeStruct.Armor_Number�������ã�
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
  * @brief  USART8��������
  * @param  void
  * @retval void
  * @notes  ��ʼ������8�������ݴ浽Down_To_Up_Tx�У�������dma������
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

Chassis_Real_Number��202�����Ȧ����������Chassis_Motor_202_Number���ڱ��������ʱȦ��Ϊ0�������ƶ����Ȧ����������
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
  * @brief  USART8���ݽ�������
  * @param  void
  * @retval void
  * @notes  ������8�����жϻ��ѣ���Up_To_DOWN_Rx�����ݽ��룬�洢��Up_To_Down_TypeStruct�ṹ������У�
            �������ǰ������һ��װ�װ屻�������û�м�⵽���ˣ����л���̨ģʽ��֮�����û�б�������
            ����̨ģʽ�ָ�Ϊ��ʧ����״̬
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
			//���װ�װ�1������������û��ʶ�𵽵���
			if( (Up_To_Down_TypeStruct.Get_Hit_flag == 1) && (Down_To_Up_TypeStruct.Target_Locked==0) )
			{//�����̨ģʽ���ڵ���װ�װ�0������������̨ģʽΪPTZ_Armor_1_Be_Attacked��PTZ_Lost_Enemy_Within_5S��PTZ_Lost_Enemy
				if(PTZ_Mode>=PTZ_Armor_0_Be_Attacked)
				{
					PTZ_Mode=PTZ_Armor_1_Be_Attacked;
				}
			}
			//���װ�װ�0�������ң�û��ʶ�𵽵���
			else if( (Up_To_Down_TypeStruct.Get_Hit_flag == 0) && (Down_To_Up_TypeStruct.Target_Locked==0) )
			{//�����̨ģʽ���ڵ���װ�װ�0������������̨ģʽΪPTZ_Armor_1_Be_Attacked��PTZ_Lost_Enemy_Within_5S��PTZ_Lost_Enemy
				if(PTZ_Mode>=PTZ_Armor_0_Be_Attacked)
				{
					PTZ_Mode=PTZ_Armor_0_Be_Attacked;
				}
			}
			//�����̨û���ҵ����˲���û�ж�ʧ������5���ڣ�����̨����PTZ_Mocalun_Locked��PTZ_Armor_0_Be_Attacked��PTZ_Armor_1_Be_Attackedʱ
			else if((PTZ_Mode!=PTZ_Find_Enemy)&&(PTZ_Mode!=PTZ_Lost_Enemy_Within_5S)&&(PTZ_Mode!=PTZ_Locked))
			{
				PTZ_Mode=PTZ_Lost_Enemy;
			}
		}
	}
}
/*********************************************�°���NUC����ͨ��**********************************************************************/


/**
  * @brief  USART3-NUCͨ�ų�ʼ��
  * @param  void
  * @retval void
  * @notes  USART3_RX -----PD9    USART3_TX-----PD8     
            ������DMA1_Stream3��DMA_Channel_4��ʹ��DMA1_Stream3_IRQHandler
            ������DMA1_Stream1��DMA_Channel_4��ʹ��USART3_IRQHandler
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
//�����ж�
	NVIC_InitStruct.NVIC_IRQChannel                   =USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =6;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        =0;
	NVIC_InitStruct.NVIC_IRQChannelCmd                =ENABLE ;
	NVIC_Init(&NVIC_InitStruct);
//���ͺ��ж�
	NVIC_InitStruct.NVIC_IRQChannel                   =DMA1_Stream3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =6;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        =0;
	NVIC_InitStruct.NVIC_IRQChannelCmd                =ENABLE ;
	NVIC_Init(&NVIC_InitStruct);
	
/* ------------------------------------- Configure DMA  ����---------------------------------------*/

	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
	
	DMA_Cmd(DMA1_Stream1,DISABLE);
	DMA_DeInit(DMA1_Stream1);
	
	DMA_InitStruct.DMA_Channel=DMA_Channel_4;                                      //ͨ�����ã�ͨ��5��
	DMA_InitStruct.DMA_BufferSize=19;                                              //����������Ŀ                
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralToMemory;                             //���ݴ��䷽�����衪���洢����                   
	DMA_InitStruct.DMA_FIFOMode=ENABLE;                                            //ʹ��FIFOģʽ
	DMA_InitStruct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;               //��ֵ1/4      
	DMA_InitStruct.DMA_Memory0BaseAddr=(uint32_t)Usart3Rx_Info;                    //�洢�����ݵ�ַ
	DMA_InitStruct.DMA_MemoryBurst=DMA_MemoryBurst_Single;                         //�洢��ͻ������
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;                     //�洢�����ݸ�ʽ���ֽڣ�
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;                             //�ڴ��ַ����
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;                                     //�Ƿ�ѭ�����ͣ�ѭ������ͨģʽֻ�ܽ���һ�Σ�
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&(USART3->DR);                 //�����ַ 
	DMA_InitStruct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;                 //����ͻ������         
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;             //�������ݸ�ʽ
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;                    //�����ַ������
	DMA_InitStruct.DMA_Priority=DMA_Priority_VeryHigh;                             //���ȼ��ǳ���      
	DMA_Init(DMA1_Stream1,&DMA_InitStruct);
	DMA_Cmd(DMA1_Stream1,ENABLE);

///*********************************************DMA����************************************/
	
	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);
	
	DMA_Cmd(DMA1_Stream3,DISABLE);
  DMA_DeInit(DMA1_Stream3);                                                 //ΪDMA����ͨ��
	DMA_InitStruct.DMA_Channel           =DMA_Channel_4;
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&(	USART3->DR);          //��ʼ��ַ
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
  * @brief  NUC DMA�����������жϷ�����
  * @param  void
  * @retval void
  * @notes  ÿ����������������֮����������жϷ����������ͺ����������ǻ�ȡ�����б����ã�ÿ1msִ��һ��
            �ж��н�DMA�������رղ�����������DNA���ݳ���
            
  */
void DMA1_Stream3_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF3)==SET)
	{
	 //�ر�dma������
	 DMA_Cmd(DMA1_Stream3,DISABLE);
	 //��������dma���ݳ���

	DMA_SetCurrDataCounter(DMA1_Stream3,23);
	}
	DMA_ClearITPendingBit(DMA1_Stream3,DMA_IT_TCIF3);	
}
/**
  * @brief  NUC ����3���ݽ����жϷ�����
  * @param  void
  * @retval void
  * @notes  ���������յ�nuc����������֮���Ƚ�dma�������ر������������ݳ���֮���ٴ�������
            Ȼ��ת��nuc���ݴ�������DMA����USART3->DR��Usart3Rx_Info
  */
void USART3_IRQHandler(void)
{
	
	BaseType_t pxHigherPriorityTaskWoken;
	char num=0;
	if(USART_GetITStatus(USART3,USART_IT_IDLE)==SET)
	{
		 num = USART3->SR;
     num = USART3->DR; //������ڿ����жϣ������ֲ�712ҳ��
		 num++;            //û�����壬ֻ����������
		
		 DMA_Cmd(DMA1_Stream1,DISABLE); 
		 DMA_SetCurrDataCounter(DMA1_Stream1,10);      //�������ý������ݳ���
		 DMA_Cmd(DMA1_Stream1,ENABLE);                 //���䣬������3���ݼĴ��������ݴ��䵽Usart3Rx_Info
		
		//��NUC���ݽ���������֪ͨ���������������б�
		 vTaskNotifyGiveFromISR(NUC_Decode_Task_Handler,&pxHigherPriorityTaskWoken);
		//���������л�
		 portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
	}	
}
/******************************�°嵽NUCͨ�����ݷ���****************************
Bullet_Speed�������ٶ�
              ���ϰ巢����=(ShootData.bulletSpeed-20.0f)*10.0f

Yaw_Angle��YAW��ĽǶ�
           �����������ݻ�ȡ�����б�����+=IMU_Real_Data.Gyro_Y*0.003f;
           ����̨�Զ����ƺ�������̨�ֶ����ƺ����б�ʹ�ã����ڿ�����̨YAW��ת��

Pitch_Angle��PITCH��ĽǶȣ�
             �ڴ���3���ͺ�������(Pitch_Motor_206.Real_MechanicalAngle-5900)*(1.46502259f)+500�õ�

Yaw_Speed��YAW����ٶȣ���λΪ������ԭʼ��λ
           �ڴ���3���ͺ�������IMU_Real_Data.Gyro_Y*100;�õ�
					 
Pitch_Speed��PITCH����ٶȣ��ɵ���ٶ�ת��Ϊ��/s֮���ٳ���3����100�õ�

Find_Target_1����̨�Ƿ����ҵ����˵�״̬��0��˵��û�д����ҵ����˵�ģʽ��1��˵�������ҵ����˵�ģʽ

Which_Color_I_Am��������ɫ��0��Ϊ��ɫ��1Ϊ��ɫ
                  �ڴ���8���ݽ��뺯�����������巢����������ȷ��

Armor_ID:װ��ID����ʵ������ֻ��Ϊ�˱������·��͸�NUC���ݵ�ͳһ��

�ھź����ݣ�0��00���췽�����ҵ����ˣ�1��01���췽��δ�ҵ����ˣ�2��10���������Ѿ��ҵ����ˣ�3��11��������λ�ҵ�����

*************************************************************************************/
/**
  * @brief  ���������ݷ��ͺ���
  * @param  void
  * @retval void
  * @notes  ����̨yaw���pitch������ݾ�������֮�󴢴�Send_NUC������
            Ȼ������dma�����������ݷ��ͳ�ȥ�������������ݽ��뺯���б�����
            ÿ1ms����һ��Send_NUC��Send_NUC���ٷ���
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

	//������ٶ��Ǵ������巢�͹�����
	Bullet_Speed=Up_To_Down_TypeStruct.Bullet_Speed;

  //��λ��*100
	//Yaw��ĽǶȾ��������ǵ���ֵ���м򵥵��ۼ�֮��ȡ������Y������ת�Ƕ������ٶ�Ϊ��   
	Yaw_Angle=(-Yaw_Send_NUC_Angle_IMU*10);
	
	
	// Pitch��ĽǶȾ�����ʵ�Ļ�е�Ǵ���֮���͹�ȥ.8192��Ӧ180�ȣ���λ����*10
	Pitch_Angle=((float)(Pitch_Motor_206.Real_MechanicalAngle)/8192.0f*360.0f*10)-1875;
	
	
	// 
	Yaw_Speed=IMU_Real_Data.Gyro_Y*100; 
	
	//��λ����/s*100
	Pitch_Speed=((float)Pitch_Motor_206.Real_Speed*360.0f)/(60.0f)*100.0f;
	
	
	//����ȷ����̨�Ƿ����ҵ����˵�״̬
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
	//��10�����������ж������ڱ���̨�ǲ��Ǵ����ҵ����˵�״̬�Լ��������ɫ��ʲô��00��ʾ�췽�����ҵ�����
	Send_NUC[10]=shortTou8(0,(short)Up_To_Down_TypeStruct.Chassis_Real_Position );//(Find_Target_1)|(Up_To_Down_TypeStruct.Which_Color_I_Am<<1);//������ɫ���������з��͹����ģ��������Ǻ췽ʱ����0��������ʱ�����1��
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
	
	
	//��Send_NUC��Send_NUC
	DMA_Cmd(DMA1_Stream3,ENABLE);
}




/*******************************NUC���°�ͨ�����ݷ���*****************************

Usart3Rx_Info[0]�����������ݱ�־λ��0x1f��˵�������䣬��������������
                  ��NUC���ݽ�����������ʹ�ã����ڸ�Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag��ֵ

Usart3Rx_Info[1]��Usart3Rx_Info[2]��Yaw_Angle��YAW������ƶ��Ƕȣ��൱��ң������ch2ͨ��ֵ���ǿ�����̨Ҫ�ƶ���ֵ
                                    ����̨�Զ����ƺ��������ڿ�����̨�ƶ�

Usart3Rx_Info[3]��Usart3Rx_Info[4]��Pitch_Angle��PITCH������ƶ��Ƕȣ��൱��ң������ch3ͨ��ֵ���ǿ�����̨Ҫ�ƶ���ֵ
                                    ����̨�Զ����ƺ��������ڿ�����̨�ƶ�

Usart3Rx_Info[5]��Armor_ID���з�װ��ID�������ֲ�ͬ����Ļ�����
									
Usart3Rx_Info[6]��Armor_Type��װ�װ����ͣ�0��ûʶ��װ�װ壬1��ʶ��Сװ�װ壬2��ʶ�𵽴�װ�װ�
                  ��NUC���ݽ�����������ʹ�ã������л���̨ģʽ
									
Usart3Rx_Info[7]��Usart3Rx_Info[8]��Enemy_distance
                                    �з�����
 
Usart3Rx_Info[9]��NUC����У��λ��
                  ����NUC���ݽ�����������ʹ�ã�����Usart3Rx_Info[1] + Usart3Rx_Info[3] + Usart3Rx_Info[5])  % 255 == Usart3Rx_Info[9]У��								

**********************************************************************************/
/**
  * @brief  Nuc���ݽ�������
  * @param  void
  * @retval void
  * @notes  ÿ�ν�����NUC������������ʱִ��һ��
            Nuc��ʼ������ʼ������3������nuc��ͨѶ������NUC�������񣬵����ݾ���У��֮��˵��nuc�Ѿ��������ˣ����ʱ��nuc��ʧʱ�����0��
            ��NUC�������������е�װ�װ����Ͳ�Ϊ0��ʱ��˵���Ѿ�ʶ���˵��ˣ���ʱ����̨ģʽ�л�Ϊʶ�𵽵��ˣ�
            �����������װ�װ�������0�Ļ���˵��û�м�⵽װ�װ壬���ʱ����̨����ģʽ�л�Ϊû��ʶ�𵽵���            
  */
		int NUC_Get_Time=0;
		short Last_Pitch_Angle=0;
void NUC_Decode_Task(void *pvParameters)
{
  static	uint16_t NUC_Start_Time=0;
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
			if(( Usart3Rx_Info[0] + Usart3Rx_Info[3] + Usart3Rx_Info[5]) % 255 == Usart3Rx_Info[9] ) 
			{
				// Nuc��ʧʱ�����0˵���Ѿ����ܵ���nuc����������ܳ�ʱ��û�н��յ�nuc������,�������ᱨ��
				NUC_Lost_Time=0;
				
				//��һ�ν��յ�NUC����֮�����һ��ʱ��
//				if(NUC_Start_Time<400)
//				{
//					NUC_Start_Time++;
//					Beep_ON();
//				}
//				else Beep_OFF();

				//��0������Ϊ0x1f������ϰ���Է���
				if( Usart3Rx_Info[0]==0x1f)            Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag=1;//(char)0x1f
				else                                   Down_To_Up_TypeStruct.NUC_Shoot_Allow_Flag=0;
				
				//װ�����ͣ�0δʶ�𵽣�1Сװ�ף�2��װ��
				Down_NUC_Data_TypeStruct.Armor_Type     = Usart3Rx_Info[6] ;
				//���ʶ��
				if(Down_NUC_Data_TypeStruct.Armor_Type !=0)
				{
					NUC_Get_Time++;
					//
					//Yaw�����ݽ��룬1.2�ţ��Ӿ�����short64555,-11~11,*2000,NUC-4000~4000,
          Down_NUC_Data_TypeStruct.Yaw_Angle=u8ToShort(Usart3Rx_Info,1);	//-0.2~0.2	

						
					
					
					
					//Pitch�����ݽ��룬3.4���Ӿ�����short64555,-7~7,*2000,NUC-4000~4000,
					Down_NUC_Data_TypeStruct.Pitch_Angle=u8ToShort(Usart3Rx_Info,3);//-600~600    
					
							
      		if(abs(Last_Pitch_Angle-Down_NUC_Data_TypeStruct.Pitch_Angle>=10))NUC_Get_Time=0;
					//����
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
//	            PID_Armor_Pitch_206_Speed.I=0.5; //���ڵֿ����ɣ������ֵ����
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
//  				else//����
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
//						PID_Armor_Pitch_206_Speed.I=1; //���ڵֿ����ɣ������ֵ���� 
//						PID_Armor_Pitch_206_Speed.D=0;
//						PID_Armor_Pitch_206_Speed.IMax=9000;
//						PID_Armor_Pitch_206_Speed.PIDOutMax=30000;
//					    
////			
////					  
//					}
         
					
	
					
					Last_Pitch_Angle=Down_NUC_Data_TypeStruct.Pitch_Angle;
         	//�з�װ�װ����
					Down_NUC_Data_TypeStruct.Armor_ID= Usart3Rx_Info[5];
					Down_To_Up_TypeStruct.Enemy_Armor_ID=Down_NUC_Data_TypeStruct.Armor_ID;
					
					
					//�з�����
					Down_NUC_Data_TypeStruct.Enemy_distance =u8ToShort(Usart3Rx_Info,7);
					Down_To_Up_TypeStruct.Distance=Down_NUC_Data_TypeStruct.Enemy_distance;						
					//����з��ľ���С��50��Ŀ����������ֻ��ʶ�𵽵���
					 Down_To_Up_TypeStruct.Target_Locked=1;
					//Ŀ�궪ʧ��������0
					Lost_Times=0;
					//������̨ģʽΪ�ҵ�����
					PTZ_Mode=PTZ_Find_Enemy;									
				}
				
				//���û��ʶ��װ�װ壬װ������Ϊ0����̨ģʽ�л�Ϊû���ҵ�����
				else  
				{
					NUC_Get_Time=0;
					Down_NUC_Data_TypeStruct.Yaw_Angle      = 0;
					Down_NUC_Data_TypeStruct.Pitch_Angle    = 0;
					Down_NUC_Data_TypeStruct.Enemy_distance = 0;
					
					Down_To_Up_TypeStruct.Enemy_Armor_ID=0x00;
					Down_To_Up_TypeStruct.Target_Position=0;
					//û��ʶ��װ�װ�˵��˵��Ŀ���Ѿ���ʧ���ʱ��ʧʱ������
					Lost_Times++;
					if(Lost_Times>50)
					{
						//Ŀ�궪ʧʱ�����50˵���Ѿ��Ҳ������������ʱ��Ŀ����������0
						Down_To_Up_TypeStruct.Target_Locked=0;
						//�л���̨ģʽ���ҵ����˱�ɶ�ʧ����
						if(PTZ_Mode==PTZ_Find_Enemy)
						{
							
							
							//��ʧ�ĽǶȾ͵��ڵ�ǰyaw�ĽǶ�
							Lost_Angle=Yaw_Real_Angle;
							//�л���̨ģʽ���ҵ����˱�ɶ�ʧ����
							PTZ_Mode=PTZ_Lost_Enemy;
						}						
					}
				}
			}
		}
	}
}
/**
  * @brief  ���ڳ�ʼ��
  * @param  void
  * @retval void
  * @notes  ���������õ����������ڳ�ʼ��         
  */
void USART_InitConfig(void)
{
	USART6_Init();
	USART8_Init();
	NUC_Init();//USART3
}
/*********************************************��������ת������**********************************************************************/
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



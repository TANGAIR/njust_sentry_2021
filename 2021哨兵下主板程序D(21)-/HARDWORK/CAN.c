#include "CAN.h"

/****************************************���ݶ�����*********************************/
CAN1_Data_TypeDef Friction_Wheel_203,
                  Friction_Wheel_204;

CAN1_Data_TypeDef Pitch_Motor_206,
                  Yaw_Motor_205;
									
static int  CAN1_Tx_Flag;                         //CAN������ɱ�־λ

int Yaw_Motor_205_Number=0;      //���205��Ȧ��
int64_t Yaw_Real_MechanicalAngle=0;               //yawʵ�ʽǶ�      �������¿�˳ʱ��Ϊ��  ��ʱ�롤Ϊ��
int64_t Pitch_Real_MechanicalAngle=0;               //Pitchʵ�ʽǶ�   ���ϼ�С����������


float Yaw_Real_Angle=0;                           //�������� 
float Yaw_Send_NUC_Angle=0;                       //���͸�NUC�ĽǶ�

	
/*****************************************����������********************************/
/**
  * @brief  CANͨ�ų�ʼ������
  * @param  void
  * @retval void
  * @notes  �����ж����ȼ�Ϊ8�������ж����ȼ�Ϊ9
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
	
	NVIC_InitStruct.NVIC_IRQChannel                    = CAN1_RX0_IRQn;             //ע�����ж�����������  refer to stm32f4xx.h file
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  = 9;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority         = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd                 = ENABLE ;
	NVIC_Init(&NVIC_InitStruct);
	
   NVIC_InitStruct.NVIC_IRQChannel                    = CAN1_TX_IRQn;              //ע�����ж�����������  refer to stm32f4xx.h file
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  = 8;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority         = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd                 = ENABLE ;
	NVIC_Init(&NVIC_InitStruct);
  
   CAN_DeInit(CAN1);
   CAN_StructInit(&CAN_InitStruct);
    
	CAN_InitStruct.CAN_Prescaler = 3;                                               //CAN BaudRate 42/(1+9+4)/3=1Mbps  //��Ƶϵ��(Fdiv)Ϊ3+1
	CAN_InitStruct.CAN_Mode      = CAN_Mode_Normal;                                 //ģʽ���� 
	CAN_InitStruct.CAN_SJW       = CAN_SJW_1tq;                                     //����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStruct.CAN_BS1       = CAN_BS1_9tq;                                     //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStruct.CAN_BS2       = CAN_BS2_4tq;                                     //Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStruct.CAN_TTCM      = DISABLE;                                         //��ʱ�䴥��ͨ��ģʽ
	CAN_InitStruct.CAN_ABOM      = DISABLE;                                         //����Զ����߹���	  
	CAN_InitStruct.CAN_AWUM      = DISABLE;                                         //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStruct.CAN_NART      = DISABLE;	                                        //��ֹ�����Զ�����
	CAN_InitStruct.CAN_RFLM      = DISABLE;                                         //���Ĳ�����,�µĸ��Ǿɵ�
	CAN_InitStruct.CAN_TXFP      = ENABLE;	                                        //���ȼ��ɱ��ı�ʶ������
	CAN_Init(CAN1,&CAN_InitStruct);
	
	CAN_FilterInitStruct.CAN_FilterIdHigh         = 0x0000;                         //32λID
	CAN_FilterInitStruct.CAN_FilterIdLow          = 0x0000;                         //32λID
	CAN_FilterInitStruct.CAN_FilterMaskIdHigh     = 0x0000;
	CAN_FilterInitStruct.CAN_FilterMaskIdLow      = 0x0000;
	CAN_FilterInitStruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;               //������0������FIFO0;
	CAN_FilterInitStruct.CAN_FilterNumber         = 0;                              //������0,��Χ0---13
	CAN_FilterInitStruct.CAN_FilterMode           = CAN_FilterMode_IdMask;
	CAN_FilterInitStruct.CAN_FilterScale          = CAN_FilterScale_32bit;
	CAN_FilterInitStruct.CAN_FilterActivation     = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStruct);
	
   CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);                                          //FIFO0��Ϣ�Һ��ж������򿪽����ж�
	CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE);                                           //�򿪷����ж�	
}


/**
  * @brief  Ħ����CAN1���ͺ���
  * @param  void
  * @retval void
  * @notes  �������ݷ��ͣ��㲥ID 0x200����������������ֵ��Χ��-16384~+16384
  */
void CAN1_TX_Friction (void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //��׼֡
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //����֡
	CAN1_Tx_Message.DLC = 0x08;                                                     //֡����Ϊ8
	CAN1_Tx_Message.StdId = CAN1_TRANSMIT_ID_200;                                   //֡IDΪ���������CAN_ID


	CAN1_Tx_Message.Data[0] = 0x00;             
	CAN1_Tx_Message.Data[1] = 0x00;                
	CAN1_Tx_Message.Data[2] = 0x00;           
	CAN1_Tx_Message.Data[3] = 0x00;                
	CAN1_Tx_Message.Data[4] = (Friction_Wheel_203.Target_Speed>>8)&0xff;               //203���յ�����8λ
	CAN1_Tx_Message.Data[5] = (Friction_Wheel_203.Target_Speed)&0xff;                  //203���յ�����8λ
	CAN1_Tx_Message.Data[6] = (Friction_Wheel_204.Target_Speed>>8)&0xff;               //204���յ�����8λ
	CAN1_Tx_Message.Data[7] = (Friction_Wheel_204.Target_Speed)&0xff;                  //204���յ�����8λ
	
	CAN1_Tx_Flag = 0; 
	CAN_Transmit(CAN1,&CAN1_Tx_Message);

	while(CAN1_Tx_Flag == 0); 
}

/**
  * @brief  ��̨CAN1���ͺ���
  * @param  void
  * @retval void
  * @notes  ��̨���ݷ��ͣ��㲥ID 0x1ff����������������ֵ��Χ��-30000~+30000
  */
void CAN1_TX_PTZ (void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //��׼֡
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //����֡
	CAN1_Tx_Message.DLC = 0x08;                                                     //֡����Ϊ8
	CAN1_Tx_Message.StdId = CAN1_TRANSMIT_ID_PTZ;                                   //֡IDΪ���������CAN_ID

	//��̨����
  //Yaw_Motor_205.Target_Speed=0;
 // Pitch_Motor_206.Target_Speed=0;
	
	CAN1_Tx_Message.Data[0] = (Yaw_Motor_205.Target_Speed>>8)&0xff;               //205���յ�����8λ
	CAN1_Tx_Message.Data[1] = (Yaw_Motor_205.Target_Speed)&0xff;                  //205���յ�����8λ
	CAN1_Tx_Message.Data[2] = (Pitch_Motor_206.Target_Speed>>8)&0xff;             //206���յ�����8λ
	CAN1_Tx_Message.Data[3] = (Pitch_Motor_206.Target_Speed)&0xff;                //206���յ�����8λ
	CAN1_Tx_Message.Data[4] = 0x00;             
	CAN1_Tx_Message.Data[5] = 0x00;                
	CAN1_Tx_Message.Data[6] = 0x00;           
	CAN1_Tx_Message.Data[7] = 0x00;                

	CAN1_Tx_Flag = 0; 
	CAN_Transmit(CAN1,&CAN1_Tx_Message);

	while(CAN1_Tx_Flag == 0); 
}


/**
  * @brief  CAN1�����жϺ���
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
  * @brief  YAW��λ���жϺ���
  * @param  void
  * @retval void
  * @notes  �ж���̨ת����Ȧ��������̨ʵ��ת���Ľ�·�̴���Yaw_Real_MechanicalAngle��
            �ж���̨ʱ��������״̬�����������̨ģʽ
  */
void Position_Direction_Judge(void)
{
/***********************************λ���ж�*****************************/
	static int Yaw_Last_Angle_Init=0;
	static int Yaw_Last_Angle=0;
	static int Yaw_Locked_Time=0;
	static int64_t  Yaw_Last_MechanicalAngle=0;
	
	if(Yaw_Last_Angle_Init==0)//���ǵ�һ�ν��뺯��������ճ�ʼ����ɣ���ֻ������������ֵ
	{
		Yaw_Last_Angle=Yaw_Motor_205.Real_MechanicalAngle;
		Yaw_Last_Angle_Init=1;
	}
	else//�����ǵ�һ���뺯��
	{
		//���ת�������ʱȦ�����㣬���ڼ�ȥ֮ǰ���涨��ʱ��Ϊ��������8191���䵽0ʱ�����ת����Ȧ������
		//��0ת��8192����ֵΪ8192��˳ʱ��ת����Ȧ����һ
		if(Yaw_Motor_205.Real_MechanicalAngle-Yaw_Last_Angle>4000)
		{
			Yaw_Motor_205_Number--;		
		}
		//��8192ת��0����ֵΪ-8192����ʱ��ת����Ȧ����һ
		if(Yaw_Motor_205.Real_MechanicalAngle-Yaw_Last_Angle<-4000)
		{
			Yaw_Motor_205_Number++;
		}
		//����Ȧ��ͳ��֮��ĵ������ֵYaw_Real_MechanicalAngle
		Yaw_Real_MechanicalAngle=Yaw_Motor_205.Real_MechanicalAngle+Yaw_Motor_205_Number*8192;
		
		
		
		
		
		//yaw��������
    //����Ȧ�ı���ֵȡ�࣬�ٳ�0.02197f��180/8192��ת��Ϊ��
		Yaw_Real_Angle=(float)((Yaw_Real_MechanicalAngle-PTZ_Init_Value.Initial_Yaw_Angle)%(8192*2))*0.02197f;
		//���͸�NUC��ֵ������ǰ�ĽǶȼ�ȥ��ʼ�ĽǶ�*180/8192
		Yaw_Send_NUC_Angle=(float)(Yaw_Real_MechanicalAngle-PTZ_Init_Value.Initial_Yaw_Angle)*0.02197f;
		
		
		if(Yaw_Real_Angle<0) Yaw_Real_Angle+=360;
		
		//��ң�������Ҳದ�����м䣬���������Զ�ģʽ
		if(DBUS.RC.Switch_Right == RC_SW_MID)
		{
			if(PTZ_Mode!=PTZ_Locked)//����̨ģʽ����������ģʽ
			{
				if(Yaw_Last_MechanicalAngle==Yaw_Real_MechanicalAngle)//����һ�νǶȵ��ڵ�ǰ�Ƕȣ�����̨����ʱ������
				{
					Yaw_Locked_Time++;
					
				}
				else Yaw_Locked_Time=0;//������̨����ʱ��Ϊ0
			}
			if(Yaw_Locked_Time>5000)//����̨����ʱ�����Ӵ���5000����ԼΪ5s�����л���̨ģʽΪ��ת������
			{
				PTZ_Mode=PTZ_Locked;
				Yaw_Locked_Time=0;
			}	
		}
		//����Yaw_Last_Angle�������ж�Ȧ��
		Yaw_Last_Angle=Yaw_Motor_205.Real_MechanicalAngle;
		
		//����Yaw_Last_MechanicalAngle�������ж���̨�Ƿ�����
		Yaw_Last_MechanicalAngle=Yaw_Real_MechanicalAngle;
	}
}
/**
  * @brief  Pitch��λ���жϺ���
  * @param  void
  * @retval void
  * @notes  �ж���̨ת����Ȧ��������̨ʵ��ת���Ľ�·�̴���Pitch_Real_MechanicalAngle��
           
  */
void Position_Direction_Judge_Pitch(void)
{
/***********************************λ���ж�*****************************/
	static int Pitch_Last_Angle_Init=0;
	static int Pitch_Last_Angle=0;
	static int Pitch_Motor_206_Number=0;      //���205��Ȧ��
	
	
	if(Pitch_Last_Angle_Init==0)//���ǵ�һ�ν��뺯��������ճ�ʼ����ɣ���ֻ������������ֵ
	{
		Pitch_Last_Angle=Pitch_Motor_206.Real_MechanicalAngle;
		Pitch_Last_Angle_Init=1;
	}
	else//�����ǵ�һ���뺯��
	{
		//���ת�������ʱȦ�����㣬���ڼ�ȥ֮ǰ���涨����Ϊ��������8191���䵽0ʱ�����ת����Ȧ������
		//��0ת��8192����ֵΪ8192����ת����Ȧ����һ
		if(Pitch_Motor_206.Real_MechanicalAngle-Pitch_Last_Angle>4000)
		{
			Pitch_Motor_206_Number--;		
		}
		//��8192ת��0����ֵΪ-8192������ת����Ȧ����һ
		if(Pitch_Motor_206.Real_MechanicalAngle-Pitch_Last_Angle<-4000)
		{
			Pitch_Motor_206_Number++;
		}
		//����Ȧ��ͳ��֮��ĵ������ֵYaw_Real_MechanicalAngle
		Pitch_Real_MechanicalAngle=Pitch_Motor_206.Real_MechanicalAngle+Pitch_Motor_206_Number*8192;
		
		//����Yaw_Last_Angle�������ж�Ȧ��
		Pitch_Last_Angle=Pitch_Motor_206.Real_MechanicalAngle;
	
	}
}
/**
  * @brief  CAN1�����жϺ���
  * @param  void
  * @retval void
  * @notes  ÿ1ms����һ�Σ�������֮��ֱ����̨����PITCH�Ƕȴ���Ϊ�˷�Խ�����䣩��
            ��YAW����д�����Ҫ���ж���̨�Ƿ�������״̬
  */
void CAN1_RX0_IRQHandler(void)                                          
{   
	CanRxMsg CAN1_Rx_Message;
	if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
	{
		  CAN_Receive(CAN1,CAN_FIFO0,&CAN1_Rx_Message);
		
			if ( (CAN1_Rx_Message.IDE == CAN_Id_Standard) && (CAN1_Rx_Message.RTR == CAN_RTR_Data) && (CAN1_Rx_Message.DLC == 8) )//��׼֡������֡�����ݳ���Ϊ8�ֽ�
			{
				switch (CAN1_Rx_Message.StdId)
				{			
	
					//Ħ����
          case CAN1_FEEDBACK_ID_203:                                                                                
					{
						Friction_Wheel_203.Real_Speed            =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]); 					      //ת��
					}break ;
					
					case CAN1_FEEDBACK_ID_204:                                                                               
					{
						Friction_Wheel_204.Real_Speed          =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]);              //ת��	
					}break ;
					
					
					//��̨
					case CAN1_FEEDBACK_ID_Pitch:                                                                                
					{
						Pitch_Motor_206.Real_MechanicalAngle =(CAN1_Rx_Message.Data[0]<<8)|(CAN1_Rx_Message.Data[1]);              //��е��
						Pitch_Motor_206.Real_Speed            =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]); 					      //ת��
            
						Position_Direction_Judge_Pitch();

					}break ;
					
					case CAN1_FEEDBACK_ID_Yaw:                                                                               
					{
						Yaw_Motor_205.Real_MechanicalAngle=(CAN1_Rx_Message.Data[0]<<8)|(CAN1_Rx_Message.Data[1]);              //��е��
						Yaw_Motor_205.Real_Speed          =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]);              //ת��

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

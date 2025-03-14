#include "main.h"

/****************************************���ݶ�����*********************************/
CAN1_Data_TypeDef Chassis_Motor_201;
//               
CAN1_Data_TypeDef Friction_Wheel_203,
                  Friction_Wheel_204;									
									
									

CAN1_Data_TypeDef Pitch_Motor_206,
                  Yaw_Motor_205,
									Shoot_Motor_207,
									Shoot_Motor_208;
									
short   Chassis_Motor_201_Number=100;                    //���202��Ȧ��
int8_t  Chassis_Direction=0;                           //���̷���     -1����������    1----��     0----ֹͣ
																

static int  CAN1_Tx_Flag;                         //CAN������ɱ�־λ


int64_t Yaw_Real_MechanicalAngle=0;               //yawʵ�ʽǶ�      �������¿�˳ʱ��Ϊ��  ��ʱ�롤Ϊ��
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
  * @brief  CAN1���ͺ���
  * @param  void
  * @retval void
  * @notes  ���̡��Ϸ���Ħ�������ݷ��ͣ��㲥ID 0x200��
            201��202����������������ֵ��Χ��-16384~+16384
  */
void CAN1_TX_Chassis(void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //��׼֡
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //����֡
	CAN1_Tx_Message.DLC = 0x08;                                                     //֡����Ϊ8
	CAN1_Tx_Message.StdId = CAN1_TRANSMIT_ID_TRAVEL;                              	//֡IDΪ���������CAN_ID
	
	//���̵��ʧȥ����
	//Chassis_Motor_201.Target_Speed=0;

	CAN1_Tx_Message.Data[0] = (Chassis_Motor_201.Target_Speed>>8)&0xff;                //201���յ�����8λ
	CAN1_Tx_Message.Data[1] =  Chassis_Motor_201.Target_Speed&0xff;                    //201���յ�����8λ
	CAN1_Tx_Message.Data[2] = 0;                                                       //202���յ�����8λ
	CAN1_Tx_Message.Data[3] = 0;                                                       //202���յ�����8λ
	CAN1_Tx_Message.Data[4] = (Friction_Wheel_203.Target_Speed>>8)&0xff;               //203���յ�����8λ
	CAN1_Tx_Message.Data[5] = (Friction_Wheel_203.Target_Speed)&0xff;                  //203���յ�����8λ
	CAN1_Tx_Message.Data[6] = (Friction_Wheel_204.Target_Speed>>8)&0xff;               //204���յ�����8λ
	CAN1_Tx_Message.Data[7] = (Friction_Wheel_204.Target_Speed)&0xff;                  //204���յ�����8λ
	            
	
	CAN1_Tx_Flag = 0;
	CAN_Transmit(CAN1,&CAN1_Tx_Message);

	while(CAN1_Tx_Flag == 0); 
}

/**
  * @brief  CAN1���ͺ���
  * @param  void
  * @retval void
  * @notes  ��̨���ݷ��ͣ��㲥ID 0x1ff����������������ֵ��Χ��-16384~+16384
  */
void CAN1_TX_PTZ (void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //��׼֡
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //����֡
	CAN1_Tx_Message.DLC = 0x08;                                                     //֡����Ϊ8
	CAN1_Tx_Message.StdId = CAN1_TRANSMIT_ID_PTZ;                                   //֡IDΪ���������CAN_ID

	//����̨ʧȥ����
	//Yaw_Motor_205.Target_Speed=0;
	//Pitch_Motor_206.Target_Speed=0;
	
	CAN1_Tx_Message.Data[0] = (Yaw_Motor_205.Target_Speed>>8)&0xff;               //205���յ�����8λ
	CAN1_Tx_Message.Data[1] = (Yaw_Motor_205.Target_Speed)&0xff;                  //205���յ�����8λ
	CAN1_Tx_Message.Data[2] = (Pitch_Motor_206.Target_Speed>>8)&0xff;             //206���յ�����8λ
	CAN1_Tx_Message.Data[3] = (Pitch_Motor_206.Target_Speed)&0xff;                //206���յ�����8λ
	CAN1_Tx_Message.Data[4] = (Shoot_Motor_207.Target_Speed>>8)&0xff;             
	CAN1_Tx_Message.Data[5] = (Shoot_Motor_207.Target_Speed)&0xff;                
	CAN1_Tx_Message.Data[6] = (Shoot_Motor_208.Target_Speed>>8)&0xff;           
	CAN1_Tx_Message.Data[7] = (Shoot_Motor_208.Target_Speed)&0xff;                

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
  * @brief  201λ���жϺ���
  * @param  void
  * @retval void
  * @notes  ����ۼ�ת���ĽǶȣ��жϵ����ƶ�������ͣ��3S�ұ�����ʼ��ر�ң�������ݽ���ͬʱ�л�Ϊ�Զ�ģʽ
  */
void Position_2_Direction_Judge(void)
{
	static short   Chassis_201_Last_Angle=0;                   //���202�ϴνǶȼ���
	static int8_t  Chassis_201_Last_Angle_Init=0;              //���202�ϴνǶȳ�ʼ��
	static int     Chassis_201_Stop_Time=0;
/***********************************λ���ж�*****************************/
	//����һ�νǶ�Ϊ0������һ�η����Ƕ�
	if(Chassis_201_Last_Angle_Init==0)
	{
		//��һ�νǶȵ�����ʵ�Ƕ�
		Chassis_201_Last_Angle=Chassis_Motor_201.Real_MechanicalAngle;
		Chassis_201_Last_Angle_Init=1;
	}
	//�����ǵ�һ�ν���
	else
	{
		//Ȧ������
		//Ȧ�����㣬�涨˳ʱ��Ϊ���ӣ�����0˳ʱ��ת����8192����ֵΪ8192��Ȧ����1����ǰ�����ؿ� �ڱ�������ʱȦ������
		if(Chassis_Motor_201.Real_MechanicalAngle-Chassis_201_Last_Angle>4000)
		{
			Chassis_Motor_201_Number--;
		}
		//��8192��ʱ��ת��0����ֵΪ-8192��Ȧ����1
		if(Chassis_Motor_201.Real_MechanicalAngle-Chassis_201_Last_Angle<-4000)
		{
			Chassis_Motor_201_Number++;
		}
		//������һ�νǶ�
		Chassis_201_Last_Angle=Chassis_Motor_201.Real_MechanicalAngle;
		
	}
/***********************************�ٶ��ж�****************************/
	//�ж��ٶȷ��򣬴ӻ��ؿ�����������Ϊ-1������Ϊ1������Ϊ0��202�����װ�ڻ��ط����ٶ�Ϊ��ʱ�ڱ������ƶ�
	if(Chassis_Motor_201.Real_Speed>20)
	{
		Chassis_Direction=1;
	}
	else if(Chassis_Motor_201.Real_Speed<-20)
	{
		Chassis_Direction=-1;
	}
	else 
	{
		Chassis_Direction=0;
	}
	
	
//	//������̲�������3�룬�ж��Ƿ������ʼ
//	if(abs(Chassis_Motor_202.Real_Speed)<20)
//	{
//		//��������׶�Ϊ4���ҵ���û�б��ϵ�
////		if( (Game_State.game_progress==4)&&(GameRobotState.mains_power_chassis_output) )
////		{
////			//��ȡ����ϵͳ���ݣ��õ�������ʼ����Ϣ��ͬʱͣ��3s
////			//��ر�ң�������������ٴ�ң�����������ݣ�ͬʱ�����Զ�ģʽ
////			if(Chassis_202_Stop_Time<3500)
////			{
////				Chassis_202_Stop_Time++;
////			}
////			if(Chassis_202_Stop_Time>3000)
////			{
////				DMA_Cmd(DMA2_Stream5,DISABLE);
////				DBUS.RC.Switch_Right=RC_SW_MID;
////				Chassis_202_Stop_Time=0;
////				Chassis_Mode=Chassis_Stop_3S;
////				Mode_Time=0;
////			}
////		}
//	}
//	else Chassis_202_Stop_Time=0;
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
	static int Yaw_Motor_205_Number=0;      //���205��Ȧ��
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
  * @brief  CAN1�����жϺ���
  * @param  void
  * @retval void
  * @notes  ÿ1ms����һ�Σ�������֮��ֱ����̨����PITCH�Ƕȴ���Ϊ�˷�Խ�����䣩��
            ��YAW����д�����Ҫ���ж���̨�Ƿ�������״̬
  */
	int Count=0;
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
					//����
					case CAN1_FEEDBACK_ID_TRAVEL_1:                                                                               //���̵��--���-->0x201
					{
						Chassis_Motor_201.Real_MechanicalAngle=(CAN1_Rx_Message.Data[0]<<8)|(CAN1_Rx_Message.Data[1]);              //��е��
						Chassis_Motor_201.Real_Speed          =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]); 					    //ת��
						//201λ���ж�
						Position_2_Direction_Judge();
						
					}break ;

					
					//����̨
					case CAN1_FEEDBACK_ID_Pitch:                                                                                
					{
					  Count++;
						Pitch_Motor_206.Real_MechanicalAngle  =(CAN1_Rx_Message.Data[0]<<8)|(CAN1_Rx_Message.Data[1]);              //��е��
						Pitch_Motor_206.Real_Speed            =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]); 					      //ת��

					}break ;
					
					
					case CAN1_FEEDBACK_ID_Yaw:                                                                               
					{
						Yaw_Motor_205.Real_MechanicalAngle=(CAN1_Rx_Message.Data[0]<<8)|(CAN1_Rx_Message.Data[1]);              //��е��
						Yaw_Motor_205.Real_Speed          =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]);              //ת��

						Position_Direction_Judge();
					}break ;		
					
					//���̵��
					case CAN1_FEEDBACK_ID_SHOOT_DOWN:                                                                               //Shoot-->0x207
					{
						Shoot_Motor_207.Real_MechanicalAngle   =(CAN1_Rx_Message.Data[0]<<8)|(CAN1_Rx_Message.Data[1]);             //��е��
						Shoot_Motor_207.Real_Speed             =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]);             //ת��
					}break;
						
					
					case CAN1_FEEDBACK_ID_SHOOT_UP:                                                                               //Shoot-->0x208
					{
						Shoot_Motor_208.Real_MechanicalAngle   =(CAN1_Rx_Message.Data[0]<<8)|(CAN1_Rx_Message.Data[1]);             //��е��
						Shoot_Motor_208.Real_Speed             =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]);             //ת��
					}break;	
						
					
					//Ħ���ֵ��
          case CAN1_FEEDBACK_ID_203:                                                                                
					{
						Friction_Wheel_203.Real_Speed            =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]); 					      //ת��
					}break ;
					
					case CAN1_FEEDBACK_ID_204:                                                                               
					{
						Friction_Wheel_204.Real_Speed          =(CAN1_Rx_Message.Data[2]<<8)|(CAN1_Rx_Message.Data[3]);              //ת��	
					}break ;
					
					
					
					
					default:
					{}
					break ;	   
				}
			}
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);		
	}
}

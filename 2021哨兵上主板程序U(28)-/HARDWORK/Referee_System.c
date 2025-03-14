 #include "main.h"
/***********************����ϵͳ������ݽṹ�嶨��***********************/
 //����ϵͳ���ݸ�ʽ
 RefereeInfo_TypeDef 				   RefereeInfor;
 
 
 
 //������Ϣ
 ext_game_state_t              Game_State; 
 //�������
 extGameResult_t     				   GameResult; 
 
 
 
 //������״̬
 ext_game_robot_status_t       GameRobotState;
 //ʵʱ��������
 ext_power_heat_data_t  			 PowerHeatData; 
 //�˺�״̬
 ext_robot_hurt_t      				 RobotHurt;
 //ʵʱ�����Ϣ
 ext_shoot_data_t      				 ShootData;



 //����������Ѫ������
 ext_game_robot_HP_t           GameRobotHP;
 //���ڷ���״̬
 ext_dart_status_t             DartStatus;
 //���ڷ���ڵ���ʱ
 ext_dart_remaining_time_t     DartRemainingTime;
 
 
 
 //����������
 ext_buff_t       				     BuffMusk;
 //������ RFID ״̬
 ext_rfid_status_t     				 RfidDetect;
 //�����¼�����
 ext_event_data_t              EventData;
 
 
 
 //������λ��
 ext_game_robot_pos_t    			 GameRobotPos;
 //ѧ�������˼�ͨ��
 robot_interactive_data_t      ShowData;


 //С��ͼ
 ext_robot_command_t Small_Map;


//
Enemy_HP My_Enemy_HP;



//ÿ�ν����жϣ���־λȡ������һ����־λ����ʱ��ܳ�������Ϊû�����жϣ���ʱ������Ƶ
short usart3_toggle_flag=0; 
//Σ�ջ�������
float Power_Danger=30;
//�����̹���
float Power_Max=200;


//���跢������
int number_1=0; 

//����������Ʊ�־
int Game_Allow_Shoot_Flag=0;

//���˻���ɱ�־//1��ɣ�0δ��
int Plane_Fly_Flag=0;                            

//����ϵͳ���ݽ������鳤��
#define JUDGE_MAX_LENGTH  200

//����ϵͳ��������
uint8_t JUDGE_RX_BUFF1[JUDGE_MAX_LENGTH];
uint8_t JUDGE_RX_BUFF2[JUDGE_MAX_LENGTH];

//����������յ����ݸ���
uint16_t JUDGE_BUFF1_RX_NUMBER=0;
uint16_t JUDGE_BUFF2_RX_NUMBER=0;

//�û����ݷ������鳤��
#define referee_tx_len  200
//�û������ϴ�֡����
unsigned char referee_upload[referee_tx_len];

//ǹ����Ϣ
#define UP_SHOOT_ID 2
#define DOWN_SHOOT_ID 0


//�޵�ʱ��,��λ��
#define INVINCEBLE_TIME 11

float Up_Bullet_Speed=26;
float Down_Bullet_Speed=26;

int My_Outpost_HP=1000;
char Output_On=1;



int YAW_RIGHT=0;
char KEY_ON_FLAG=0;



/*����BUFFö��*/
typedef enum{
      BUFF_NO1=0,
      BUFF_NO2=1
}BUFF_NO;
//����BUFF
BUFF_NO  Free_BUFF_NO;   

//DMA�ṹ�壬��Ϊ�������ط���ʹ�����Բ��ú궨��
DMA_InitTypeDef   dma;


/*************************************���ݸ�ʽת������*************************************/ 
/**
 * @Function : λ��ת��
 * @Input    : void
 * @Output   : void
 * @Notes    : 8λ-32λ ���ù�����ṹ����������������������һ���ڴ�����һ������
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


/*************************************�������㺯������*************************************/ 
/**
 * @Function : �з�����жϺ���
 * @Input    : void
 * @Output   : void
 * @Notes    : ÿ��ִ��һ�Σ�0�����������޵У�1������ӵ�λ����λ������1~5�ţ�7��
 * @Copyright: Aword
 **/
u8 Enemy_Alive_Judge(void)
{
	//1��
	if(My_Enemy_HP.Hero_1==0)My_Enemy_HP.Hero_1_Invinceble_Time=INVINCEBLE_TIME;
	if(My_Enemy_HP.Hero_1_Invinceble_Time>0)My_Enemy_HP.Hero_1_Invinceble_Time--;
	//2��
	if(My_Enemy_HP.Engineering_2==0)My_Enemy_HP.Engineering_2_Invinceble_Time=INVINCEBLE_TIME;
	if(My_Enemy_HP.Engineering_2_Invinceble_Time>0)My_Enemy_HP.Engineering_2_Invinceble_Time--;
	//3��
	if(My_Enemy_HP.Standar_3==0)My_Enemy_HP.Standar_3_Invinceble_Time=INVINCEBLE_TIME;
	if(My_Enemy_HP.Standar_3_Invinceble_Time>0)My_Enemy_HP.Standar_3_Invinceble_Time--;
	//4��
	if(My_Enemy_HP.Standar_4==0)My_Enemy_HP.Standar_4_Invinceble_Time=INVINCEBLE_TIME;
	if(My_Enemy_HP.Standar_4_Invinceble_Time>0)My_Enemy_HP.Standar_4_Invinceble_Time--;
	//5��
	if(My_Enemy_HP.Standar_5==0)My_Enemy_HP.Standar_5_Invinceble_Time=INVINCEBLE_TIME;
	if(My_Enemy_HP.Standar_5_Invinceble_Time>0)My_Enemy_HP.Standar_5_Invinceble_Time--;
	
	//ֻ�е�Ѫ������0�Ҳ��ڸ����10���ʱ����ܱ���
	return (((My_Enemy_HP.Hero_1>0)&&(My_Enemy_HP.Hero_1_Invinceble_Time==0)?1:0)||\
		      (((My_Enemy_HP.Engineering_2)&&(!My_Enemy_HP.Engineering_2_Invinceble_Time)?1:0)<<1)||\
		      (((My_Enemy_HP.Standar_3)&&(!My_Enemy_HP.Standar_3_Invinceble_Time)?1:0)<<2)||\
		      (((My_Enemy_HP.Standar_4)&&(!My_Enemy_HP.Standar_4_Invinceble_Time)?1:0)<<3)||\
		      (((My_Enemy_HP.Standar_5)&&(!My_Enemy_HP.Standar_5_Invinceble_Time)?1:0)<<4)||\
			    (((My_Enemy_HP.Sentry_7)?1:0)<<5)\
	       );

}

/**
 * @Function : �з���ս����Ѫ��������
 * @Input    : void
 * @Output   : void
 * @Notes    : Ӣ�۴���01��3�Ų���10���ĺŲ���11����Ų���00���������ȼ�1>3>4>5
 * @Copyright: Aword
 **/
u8 Enemy_Sort(void)
{
   char Hero_1_Remove=0;//0��2��4��6��0��2��2��6��0��2��4��4
	 char Standar_3_Remove=0;
   char Standar_4_Remove=0;
   char Standar_5_Remove=-2;

   //���ĸ������˵�Ѫ���Աȣ��������һ������λ
	 for(char j=0;j<=3;j++)
	 {
		 if(My_Enemy_HP.Hero_1>*(&My_Enemy_HP.Hero_1+2*j))Hero_1_Remove+=2;
		 if(My_Enemy_HP.Standar_3>*(&My_Enemy_HP.Hero_1+2*j))Standar_3_Remove+=2;
		 if(My_Enemy_HP.Standar_4>*(&My_Enemy_HP.Hero_1+2*j))Standar_4_Remove+=2;
		 if(My_Enemy_HP.Standar_5>=*(&My_Enemy_HP.Hero_1+2*j))Standar_5_Remove+=2;//��ź󱻴�
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
	





/***********************************************����ϵͳ���ݺ���***********************************************/ 
/**
  * @brief  ����3��ʼ������
  * @param  void
  * @retval void
  * @notes  ��������3�����жϣ��ж����ȼ�Ϊ 6
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
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);  //ʹ�ܴ��ڿ����ж�


}

/**
  * @brief  ����3����DMA��ʼ��
  * @param  void
  * @retval void
  * @notes  �ṹ�嶨��ʹ�ú궨����Ϊ�жϺ����л�Ҫʹ�ã�ʹ��DMA1_Stream1_DMA_Channel_4,
            ���ʱ��������ϵͳ���ص���Ϣ����JUDGE_RX_BUFF1
  */
void JudgeSys_DMA_InitConfig(void)
{
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	
	DMA_DeInit(DMA1_Stream1);
	DMA_StructInit(&dma);
	
	dma.DMA_Channel             =DMA_Channel_4;             //ͨ��4
	dma.DMA_PeripheralBaseAddr  =(uint32_t)&(USART3->DR);
	dma.DMA_Memory0BaseAddr     =(uint32_t)JUDGE_RX_BUFF1;
	dma.DMA_DIR                 =DMA_DIR_PeripheralToMemory;
	dma.DMA_BufferSize          =JUDGE_MAX_LENGTH;          //һ�δ�����������Ĵ�С
	dma.DMA_PeripheralInc       =DMA_PeripheralInc_Disable; 
	dma.DMA_MemoryInc           =DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize  =DMA_PeripheralDataSize_Byte;
	dma.DMA_MemoryDataSize      =DMA_MemoryDataSize_Byte;
	dma.DMA_Mode                =DMA_Mode_Circular;         //ModeӦ��ѡ��Circularģʽ��Normalģʽֻ�ܽ���һ������
	dma.DMA_Priority            =DMA_Priority_VeryHigh;     //DMAͨ�����ȼ�
  dma.DMA_FIFOMode            =DMA_FIFOMode_Disable;      //����FIFO
	dma.DMA_FIFOThreshold       =DMA_FIFOThreshold_Full;
	dma.DMA_MemoryBurst         =DMA_MemoryBurst_Single;    //����ͻ����������
	dma.DMA_PeripheralBurst     =DMA_PeripheralBurst_Single;//����ͻ����������
	DMA_Init(DMA1_Stream1,&dma);
	
	Free_BUFF_NO=BUFF_NO2;
	
	DMA_Cmd(DMA1_Stream1,ENABLE);	    //ʹ��DMA1_Stream1
}

/**
 * @Function : ����ϵͳ��ʼ��
 * @Input    : void
 * @Output   : void
 * @Notes    : USART3_TX-->PD9      USART3_RX-->PD8 ����ʼ������3��DMA������DAM������
 * @Copyright: Aword
 **/
void JudgeSys_Init(void)
{
  USART3_InitConfig();                          //���ڳ�ʼ��
	JudgeSys_DMA_InitConfig();                    //DMA��ʼ��
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);  //��������DMA����
}



/**
  * @brief  ����ϵͳ���ݽ��뺯��
  * @param  UsartRx_Info�����յ�һ֡���ݵ�ͷ��ַ
  * @retval void
  * @notes  �ڲ���ϵͳ����У�麯���б����ã������ʵ���ڴ���3�����ж��б�����
            �Բ���ϵͳ��������Ϣ���н��룬����°巢���������ݶ��ڱ��ĵ���ģʽ�������á�

            ������ID�������ж���Ӫ��
            ʣ��Ѫ���������ж��ܵ�ʲô�����
            ��������������жϵ����Ƿ񱻶ϵ�

            ���̻��������������������� ����ʡ������ٶȡ��Լ������ٶ�

            �ܴ����װ����ż����°崫����Down_To_Up_TypeStruct.Yaw_Angle_Real�������ж��Ƿ������˴����
            ���õ���ģʽ��ͬʱ����Up_To_Down_TypeStruct.Get_Hit_flag

            �����ٶȵĸ��£������жϷ�����number_1++;

            ���˻���־�������л�Ӧ�����˻�

  */
void RefereeInfo_Decode(uint8_t *UsartRx_Info)
{
	static int Get_Hit_Armor_Last=1; //��һ�α������װ�װ�
	static int Bullet_Speed_Last=0;  //��һ�ε�������
	int Angle_Direction=0;           //��̨�Ƕ�λ��
	int Pander_ID=0;                 

	
	
	//�ж���Ϣ����
	RefereeInfor.CmdID = bit8TObit16(&UsartRx_Info[5]);
	
	
	switch(RefereeInfor.CmdID)
	{
		/*������Ϣ����*/
		case 0x0001:          
		{		                
			 Game_State.game_type               = UsartRx_Info[7]&0x0f;                   //��ǰ�������ͣ�����λ			
		   Game_State.game_progress           = UsartRx_Info[7]>>4;			                //��ǰ�����׶Σ�����λ
			 Game_State.stage_remain_time       = bit8TObit16(&UsartRx_Info[8]);		      //��ǰ�׶�ʣ��ʱ��
			//4����ս��
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
		
		
		/*�������  ��ʤ��*/
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
		
    //��õз�Ѫ��			
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
		
		
		//��ȡ�������
		Up_To_Down_TypeStruct.Enemy_State=Enemy_Alive_Judge();
		//Ѫ�����ȼ�
		Up_To_Down_TypeStruct.Enemy_HP_HL=Enemy_Sort();

			
		}
		break;
	
		
		
		/*������״̬����*/
		//�õ�����Ҫ��ID�����ж���Ӫ��ʣ��Ѫ�������ж��ܵ�ʲô�����������������жϵ����Ƿ񱻶ϵ�
		case 0x0201:
		{
			 GameRobotState.robot_id                      =UsartRx_Info[7];                  //������ID
			 GameRobotState.robot_level                   =UsartRx_Info[8];                  //�����˵ȼ�
			 GameRobotState.remain_HP                     =bit8TObit16(&UsartRx_Info[9]);    //������ʣ��Ѫ��
			 GameRobotState.max_HP                        =bit8TObit16(&UsartRx_Info[11]);   //����������Ѫ��
			 GameRobotState.shooter_id1_17mm_cooling_rate =bit8TObit16(&UsartRx_Info[13]);   //6 2  ������ 1 �� 17mm ǹ��ÿ����ȴֵ
			 GameRobotState.shooter_id1_17mm_cooling_limit=bit8TObit16(&UsartRx_Info[15]);   //8 2  ������ 1 �� 17mm ǹ����������
			 GameRobotState.shooter_id1_17mm_speed_limit  =bit8TObit16(&UsartRx_Info[17]);   //10 2 ������ 1 �� 17mm ǹ�������ٶ� ��λ m/s
			 GameRobotState.shooter_id2_17mm_cooling_rate =bit8TObit16(&UsartRx_Info[19]);   //12 2 ������ 2 �� 17mm ǹ��ÿ����ȴֵ
			 GameRobotState.shooter_id2_17mm_cooling_limit=bit8TObit16(&UsartRx_Info[21]);   //14 2 ������ 2 �� 17mm ǹ����������
			 GameRobotState.shooter_id2_17mm_speed_limit  =bit8TObit16(&UsartRx_Info[23]);   //16 2 ������ 2 �� 17mm ǹ�������ٶ� ��λ m/s
			 GameRobotState.shooter_id1_42mm_cooling_rate =bit8TObit16(&UsartRx_Info[25]);   //18 2 ������ 42mm ǹ��ÿ����ȴֵ
			 GameRobotState.shooter_id1_42mm_cooling_limit=bit8TObit16(&UsartRx_Info[27]);   //20 2 ������ 42mm ǹ����������
			 GameRobotState.shooter_id1_42mm_speed_limit  =bit8TObit16(&UsartRx_Info[29]);   //22 2 ������ 42mm ǹ�������ٶ� ��λ m/s
			 GameRobotState.chassis_power_limit           =bit8TObit16(&UsartRx_Info[31]);   //24 2 �����˵��̹�����������
			 GameRobotState.mains_power_gimbal_output     =(UsartRx_Info[33]&0x01);          //26 1 ���ص�Դ��������0 bit��gimbal ������� 1 Ϊ�� 24V �����0 Ϊ�� 24v �����
			 GameRobotState.mains_power_chassis_output    =((UsartRx_Info[33]&0x02)>>1);     //                       1 bit��chassis �������1 Ϊ�� 24V �����0 Ϊ�� 24v �����
			 GameRobotState.mains_power_shooter_output    =((UsartRx_Info[33]&0x04)>>1);     //                       2 bit��shooter �������1 Ϊ�� 24V �����0 Ϊ�� 24v �����
			
			
			//��ɫ�жϣ���Ӫ�ж�
			if(GameRobotState.robot_id>99)//��������˵�ID>99��1��Ҳ����˵��������������ʱ�����1���췽ʱ����0
			{
				Up_To_Down_TypeStruct.Which_Color_I_Am=1;//�ҷ�Ϊ����
				My_Outpost_HP=GameRobotHP.blue_outpost_HP;
					
			}
			else 
			{
			  Up_To_Down_TypeStruct.Which_Color_I_Am=0;
				My_Outpost_HP=GameRobotHP.red_outpost_HP;
			}
			
			
			
			//Ѫ���ж�
			//���ʣ��Ѫ�������˱仯
			if(Up_To_Down_TypeStruct.Remain_HP!=GameRobotState.remain_HP)
			{
				//�������־λ����Ϊ2
				if(Up_To_Down_TypeStruct.Get_Hit_flag!=2)
				{
					
					//�����ٵ�Ѫ������100��˵���ܵ����蹥��
					if((Up_To_Down_TypeStruct.Remain_HP-GameRobotState.remain_HP)>=100) 
					{
						
						BeHurt_10s+=5;
						//���õ�������Ϊ1
						Up_To_Down_TypeStruct.Bullet_Type=1;
						//����ģʽ����Ϊ�ܵ����蹥��
						if(Chassis_Mode!=Chassis_Up_Find_Target)
						Chassis_Mode=Chassis_Get_Big_Bullet;
						//ģʽ����ʱ��Ϊ0
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
					//�ܵ�С������
					else
					{
					 BeHurt_10s+=1;
					
					
					}
					
					
					
					//��Ѫ������Ϊ10~100��˵���ܵ�С���蹥��
					if(((Up_To_Down_TypeStruct.Remain_HP-GameRobotState.remain_HP)<50)&&((Up_To_Down_TypeStruct.Remain_HP-GameRobotState.remain_HP)>=10))  
					{
						//��������Ϊ2
						Up_To_Down_TypeStruct.Bullet_Type=2;
						//������ģʽ���ڵ���Chassis_Get_Small_Hurt����ΪChassis_Limite��Chassis_Normal
						if(Chassis_Mode>=Chassis_Get_Small_Hurt)
						{
							//������ģʽ����ΪChassis_Get_Small_Hurt
							Chassis_Mode=Chassis_Get_Small_Hurt;
							//ģʽ����ʱ��Ϊ0
							Mode_Time=0;
							

							
							//λ���ռ���2019����
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
			
			//����ʣ��Ѫ��
			Up_To_Down_TypeStruct.Remain_HP=GameRobotState.remain_HP;
			
			//�����Ƿ񱻶ϵ�
			if(GameRobotState.mains_power_chassis_output==1)
			{
				Up_To_Down_TypeStruct.Sentry_Satus=1;
			}
			else Up_To_Down_TypeStruct.Sentry_Satus=0;
		}
		break;
		
		
		/*����������Ϣ*/
		//��Ҫʹ�û�����������������������ʡ������ٶȡ��Լ������ٶȣ�ÿ0.02�뷢��һ��
		case 0x0202:
		{
		  PowerHeatData.chassis_volt                 = bit8TObit16(&UsartRx_Info[7]); 		         //���������ѹ
			PowerHeatData.chassis_current              = bit8TObit16(&UsartRx_Info[9]); 	           //�����������
			PowerHeatData.chassis_power                = bit8TObit32(&UsartRx_Info[11]); 	           //�����������
			PowerHeatData.chassis_power_buffer         = bit8TObit16(&UsartRx_Info[15]);             //���̹��ʻ�������
			PowerHeatData.shooter_id1_17mm_cooling_heat= bit8TObit16(&UsartRx_Info[17]);		         // 1 �� 17mmǹ������
			PowerHeatData.shooter_id2_17mm_cooling_heat= bit8TObit16(&UsartRx_Info[19]);		         // 2 �� 17mmǹ������
			PowerHeatData.shooter_id1_42mm_cooling_heat= bit8TObit16(&UsartRx_Info[21]);             // 42mm ǹ������
			
			
			
			
			//���̹�������
			if(PowerHeatData.chassis_power_buffer<=Power_Danger)//30
			{
				//�����Ϊ30
				Power_Max=25.0f;
        Swag_Speed=220;
			}
			else//�������������������30��˲ʱ
			{		
				Power_Max=((float)PowerHeatData.chassis_power_buffer-(float)Power_Danger)*0.55f;
				if(PowerHeatData.chassis_power_buffer>=120)Swag_Speed=320;
			}
			
			if(Power_Max>POWER_LIMIT) Power_Max=POWER_LIMIT;
			if(Power_Max<15) Power_Max=15;
			
		}
		break ;
		
			/*�˺�����\��һ��װ���ܵ��˺���*/
		//�����ܵ������װ�׵�������°崫���������ж��Ƿ��ܵ������˴��
		case 0x0206:
		{
			RobotHurt.armor_id         = UsartRx_Info[7]&0x0f;		  //�ܵ��˺���װ��ID��0x0-0��װ�ף�0x1-1��װ�ף�0x2-2��װ�ף�0x3-3��װ�ף�0x4-4��װ�ף�0x5-5��װ��
			RobotHurt.hurt_type        = UsartRx_Info[7]>>4;        //Ѫ���仯���ͣ�0x0�ܵ�������0x1ģ�����

			//�ж��Ƿ���������
			if(RobotHurt.hurt_type==0x0)
			{
				//���������崫�ص���̨�Ƕ�ֵ�жϣ���̨�Ƿ����ǰ���������ǰ����Ϊ0�������ǰ����Ϊ1
				Angle_Direction=(  (Down_To_Up_TypeStruct.Yaw_Angle_Real>90)&&(Down_To_Up_TypeStruct.Yaw_Angle_Real<270) ) ;
				//�����ʱ����̨�����Ѿ���������
				if(Down_To_Up_TypeStruct.Target_Locked)
				{
					//����װ�װ壬��ʼ��ʱ��̨��ǰ����ǰ��װ�װ�ID����Ϊ1
					//����̨��Եķ����뱻�����װ�װ峯��ͬ����˵���кܶ�����ڴ��ڱ�
					if(Angle_Direction == RobotHurt.armor_id)
					{
						//���°巢�ʹ����־λΪ3
						Up_To_Down_TypeStruct.Get_Hit_flag=3;	
						//������̨ģʽΪ��������
						if(Chassis_Mode>=Chassis_Many_Enemy)
						{
							Chassis_Mode=Chassis_Many_Enemy;
							Mode_Time=0;
						}
					}
				}
				
				//��һ�α����װ�װ�����β�ͬ����Σ��ʱ��С��500��Σ��ʱ�䲻Ϊ0�����ںܶ�ʱ������װ�װ嶼�������Danger_Timeÿ10��������1
				//˵�������кܶ����
				if( (Get_Hit_Armor_Last!=RobotHurt.armor_id) &&(Danger_Time<500)&&(Danger_Time!=0) )
				{
					Up_To_Down_TypeStruct.Get_Hit_flag=3;	
					if(Chassis_Mode>=Chassis_Many_Enemy)
					{
						Chassis_Mode=Chassis_Many_Enemy;
						Mode_Time=0;
					}
				}	
			  //��������עΪ���ڱ������װ����ţ�0����1
				else Up_To_Down_TypeStruct.Get_Hit_flag=RobotHurt.armor_id;
				Danger_Time=0;
				//������һ�α������װ�����
				Get_Hit_Armor_Last=RobotHurt.armor_id;
			}
		}
		break ;
		
		/*�������*/
		//�õ����ٶȵĸ����жϷ�����
		case 0x0207:
		{
			ShootData.bullet_type   = UsartRx_Info[7];//�������ͣ�1:17mm���裻	2��42mm����
		  ShootData.shooter_id    = UsartRx_Info[8];//������� ID�� 1��1 �� 17mm �������   2��2 �� 17mm �������   3��42mm �������
			ShootData.bullet_freq   = UsartRx_Info[9];//������Ƶ
			ShootData.bullet_speed  = bit8TObit32(&UsartRx_Info[10]); //�������� 
			
			//��
			if(ShootData.shooter_id==2)
			{
			  Up_Bullet_Speed=ShootData.bullet_speed;
			}
			//��
			else //if(ShootData.shooter_id==DOWN_SHOOT_ID)
			{
			  Down_Bullet_Speed=ShootData.bullet_speed;
			}
			
			
			//���ң���������Զ�ģʽ
			 if(DBUS.RC.Switch_Right==RC_SW_MID)
			 {
				 //�����ٶȸ��£���˵��������������
				 if(Bullet_Speed_Last!=ShootData.bullet_speed)
				 {
					number_1++;
				 }
			 }
			 //���µ����ٶ�
			 Bullet_Speed_Last=ShootData.bullet_speed;
		}
		break ;
		
		
		
		
		
			
		/*��������*/
		case 0x0204:
		{
			BuffMusk.power_rune_buff=bit8TObit16(&UsartRx_Info[7]);
		}
		break ;
		
			
		/*������λ����Ϣ*/
		case 0x0203:
		{
			GameRobotPos.x          =bit8TObit32(&UsartRx_Info[7]);		  			//λ��X����ֵ
			GameRobotPos.y          =bit8TObit32(&UsartRx_Info[11]);					//λ��Y����ֵ
			GameRobotPos.z          =bit8TObit32(&UsartRx_Info[15]);					//λ��Z����ֵ
			GameRobotPos.yaw        =bit8TObit32(&UsartRx_Info[19]);					//ǹ�ڳ���Ƕ�ֵ
		}
		break ;
	
		/*�û��Զ���*/
		//Ӧ�����˻����
		case 0x0301:
		{
			Pander_ID=bit8TObit16(&UsartRx_Info[7]);
			if(Pander_ID==0x0233)
			{
				Plane_Fly_Flag=UsartRx_Info[13];
			}
			//������˻���ɣ�������ģʽ��ΪӦ�����˻����
			if(Plane_Fly_Flag==1)
			{
				if(Chassis_Mode!=Chassis_Plane_Fly)
				{
					Chassis_Mode=Chassis_Plane_Fly;
					Mode_Time=0;
				}
			}
			//����ָ�����
			else 
			{
				if(Chassis_Mode==Chassis_Plane_Fly)
				{
					Chassis_Mode=Chassis_Normal;
				}
			}
		}
		
		/*С��ͼ��Ϣ*/
		case 0x0303:
		{
			Small_Map.target_position_x           =bit8TObit32(&UsartRx_Info[7]);		  			//λ��X����ֵ
			Small_Map.target_position_y           =bit8TObit32(&UsartRx_Info[11]);					//λ��Y����ֵ
			Small_Map.target_position_z           =bit8TObit32(&UsartRx_Info[15]);					//λ��Z����ֵ
			Small_Map.commd_keyboard              =UsartRx_Info[19];					              //������Ϣ
			Small_Map.target_robot_ID             =bit8TObit16(&UsartRx_Info[20]);          //Ŀ�������ID
			
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
 * @Function : ����ϵͳ����У�麯��
 * @Input    : void
 * @Output   : void
 * @Notes    : �ڴ���3�ж��б����ã��ҵ��������������֡ͷȻ�����crcУ�飬
               ͨ��֮����ò���ϵͳ���ݽ��뺯���������ݽ��룬�����ǰһ֡����
 * @Copyright: Aword
 **/
void Judge_Data_Process(void)
{
	u16 i=0;
	u16 RxLength = 0;
	//�����������Ϊ����1
	if(Free_BUFF_NO==BUFF_NO1)
	{
		//�ҵ����������е�0xA5,��ÿһ֡���ݵ���ʼ��
		//��Ϊ���������������л����Ͻ��ղ���ϵͳ�����ݣ�һ�������п����ж�֡���ݣ�
		//������Ҫ���в���У��ȷ������������׼ȷ
		while(JUDGE_RX_BUFF1[i]==0XA5)
		{
			//��֡���ݳ���
			RxLength=(JUDGE_RX_BUFF1[i+2]<<8)|JUDGE_RX_BUFF1[i+1]+9;
			//���CRCУ��ͨ��������ò���ϵͳ���ݽ��뺯��
			if (Verify_CRC16_Check_Sum(&JUDGE_RX_BUFF1[i], RxLength ))  RefereeInfo_Decode(&JUDGE_RX_BUFF1[i]);
			//�������1
			JUDGE_RX_BUFF1[i]=0;
			//��֡������ʼλ��
			i+=RxLength;
		}
	}
	//�����������Ϊ2��ͬ���ҵ�֡ͷ������У�飬ͨ��֮�������
	else
	{
		while(JUDGE_RX_BUFF2[i]==0XA5)
		{
			RxLength=(JUDGE_RX_BUFF2[i+2]<<8)|JUDGE_RX_BUFF2[i+1]+9;//��֡���ݳ���
			if (Verify_CRC16_Check_Sum(&JUDGE_RX_BUFF2[i], RxLength ))  RefereeInfo_Decode(&JUDGE_RX_BUFF2[i]); 
			JUDGE_RX_BUFF1[i]=0;
			i+=RxLength;//��֡������ʼλ��
		}
	}
}






/**
  * @brief  ����3�����жϷ�����
  * @param  void
  * @retval void
  * @notes  �л�����ϵͳ���ݽ������飬���ò���ϵͳ����У�麯����ͨ��У���Ժ��ٵ���
  */
void USART3_IRQHandler(void)    
{
	BaseType_t pxHigherPriorityTaskWoken;
	//����������ڿ����жϱ�־λ 
	uint32_t temp=temp;
	//����3�л���־λȡ��
	usart3_toggle_flag=!usart3_toggle_flag;
	
	if(USART_GetITStatus(USART3,USART_IT_IDLE)!=RESET)
	{ 
		 temp=USART3->SR;                //������ڿ����жϱ�־λ 
	 	 temp=USART3->DR;  
		 
		//�����ϵͳ���ݽ���������֪ͨ���������������б�
		 vTaskNotifyGiveFromISR(Referee_System_Task_Handler,&pxHigherPriorityTaskWoken);
		//���������л�
		 portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
 
	}
}
/**
  * @brief  ����ϵͳ������
  * @param  void
  * @retval void
  * @notes  ��ʼ������ϵͳ�����ȼ�Ϊ6��������3�жϻ��ѣ��ж����ȼ�Ϊ6���ر�DMA���������л�����ϵͳ���ݽ�������
            ��������У�������
  */
void Referee_System_Task(void *pvParameters)
{
	uint32_t err;
	//����ϵͳ��ʼ��
	JudgeSys_Init();
	vTaskDelay(200);
	
 while(1)
 {
//�ȴ�����֪ͨ
	err=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);

	if(err==1)
	{
		DMA_Cmd(DMA1_Stream1,DISABLE);  //ʧ��DMA1
	 //�����ǰ��������Ϊ��������1
	 if(Free_BUFF_NO==BUFF_NO1)
	 {
		 //��������2���յ������ݸ���Ϊ����ϵͳ������ݳ��ȼ�ȥʣ�������
		 //���DMA��ǰ�յ����ֽ�������ΪDMA�ж�ֻ�н��ճ�����ʱ�Żᴥ��������Ҫ��ʣ�����ݶ�����
		 JUDGE_BUFF2_RX_NUMBER=JUDGE_MAX_LENGTH-DMA_GetCurrDataCounter(DMA1_Stream1);
		 //�л��洢����Ϊ����1
		 dma.DMA_Memory0BaseAddr     =(uint32_t)JUDGE_RX_BUFF1;
		 //��DMA������
		 DMA_Init(DMA1_Stream1,&dma);
		 //������������Ϊ����2
		 Free_BUFF_NO=BUFF_NO2;
	 }
	 //���򵱿�������Ϊ����2ʱ���л�Ϊ����1
	 else
	 {
		 JUDGE_BUFF1_RX_NUMBER=JUDGE_MAX_LENGTH-DMA_GetCurrDataCounter(DMA1_Stream1);
		 dma.DMA_Memory0BaseAddr     =(uint32_t)JUDGE_RX_BUFF2;
		 DMA_Init(DMA1_Stream1,&dma);
		 Free_BUFF_NO=BUFF_NO1;
	 }	
		 
	 DMA_SetCurrDataCounter(DMA1_Stream1,JUDGE_MAX_LENGTH);      //�������ý������ݳ���
	 DMA_Cmd(DMA1_Stream1,ENABLE);
	 
	 //����ϵͳ����У�麯��		 
	 Judge_Data_Process(); 
	}
 }		
}






/**
  * @brief  �Զ��������ϴ�����
  * @param  Studentdata���û������9�ֽ�����ָ��
  * @retval void
  * @notes  ʹ�ô���3���û������9�ֽ������ϴ�
  */
void Student_Date_Upload( robot_interactive_data_t * Studentdata)
{
  unsigned char ucExpected = 0;
  uint16_t wExpected = 0;
  int i;
  //֡ͷ�ṹ  
  referee_upload[0] = 0xA5;//SOF   0xA5
  referee_upload[1] = 0x0D;//DataLength 13�ֽ�
  referee_upload[2] = 0x00;
  referee_upload[3] = 0x00;//Seq  !!!!!!!!!!
	
	//֡ͷ8λһ�ֽ�crcУ��
  ucExpected = Get_CRC8_Check_Sum (referee_upload, 5-1, 0xff) ;//CRC8
  referee_upload[4] = ucExpected;//CRC8 
	
  //������ ID  
  referee_upload[5] = 0x03;
  referee_upload[6] = 0x01;
  //Data(12-Byte)
  referee_upload[ 7] = 0x02;
  referee_upload[ 8] = 0x00;
  referee_upload[ 9] = 0x00;
  referee_upload[10] = GameRobotState.robot_id;//������ID
  referee_upload[11] = 0x00;
  referee_upload[12] = 0x0e;
	//�û���9���ֽڵ�����
  referee_upload[11] = Studentdata->data[0];
  referee_upload[12] = Studentdata->data[1];
  referee_upload[13] = Studentdata->data[2];
  referee_upload[14] = Studentdata->data[3];
  referee_upload[15] = Studentdata->data[4];
  referee_upload[16] = Studentdata->data[5];
  referee_upload[17] = Studentdata->data[6];
  referee_upload[18] = Studentdata->data[7];
  referee_upload[19] = Studentdata->data[8];	//(0<<5)|(1<<4)|(0<<3)|(0<<2)|(0<<1)|0;��Ӧ�졢�졢�졢�졢�̡���
	
  //FrameTail(2-Byte, CRC16)��֡β16λ���ֽ�crcУ��
  wExpected = Get_CRC16_Check_Sum ( referee_upload, 22 - 2, 0xffff);//CRC16 
  referee_upload[20] = wExpected & 0xFF;//CRC16  
  referee_upload[21] = (wExpected >> 8) & 0xFF;//CRC16 
	
   //�����־λ��ֹ��һ�ֽڶ�ʧ
	USART_ClearFlag(USART3,USART_FLAG_TC);   
	
	//ʹ�ô���3����22���ֽڵ�����
  for (i=0;i<22;i++)
  {
		USART_SendData(USART3, referee_upload[i]);
		//�ж�ÿһ���ֽ��Ƿ񷢳�
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET){}
  }
}



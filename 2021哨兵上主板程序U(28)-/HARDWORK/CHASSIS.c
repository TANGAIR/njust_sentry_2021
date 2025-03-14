#include "main.h"
/*�������ҵ��ж�
  ���ǴӶԷ����ؿ����ڱ�������Ҳ�Ϊ1�����Ϊ-1
*/



//�����ٶȿ���
short   Chassis_Speed=0;
//���������ٶ�
char Add_Speed=40;
 //���̷������   1--��   -1--��
int8_t  Chassis_Target_Direction=1;









//��ʲô������  
int Where_To_Go=0;

//�ܵ��˺�ʱ��λ�á�����Ŀ��ʱ��λ��
int Formar_Place=0;

//λ���ռ�����ʱ����֪����ʲô�ô�
int Position_Collet_Flag=0;   

//ҡһҡ����
char Sway_Buttle=0;





//����ģʽ������
enum Chassis_mode Chassis_Mode=Chassis_Normal;

//�з�Σ�յȼ������ھ����ܲ���
int Enemy_Danger_Level[8]={0,1,0,1,1,0,0,0};  //0,������Ӣ�ۣ����̣��������������ڱ�

                      
int Danger_Speed=380;

                    
int Attack_Speed=300;
int BeAttacked_Speed=300;

/*********************************�����ٶ����ݶ�����***********************/
/*
   �ٶ�
swing
   350    �������Ļ�������������ײ��  ,��Ӧ����27
   320    һ��ѭ������������          ����Ӧ����15
   300    ���Իָ���������                     

�ֶ�


*/
/*ҡ�����*/
int Serach_Speed=220;

int Run_Speed=350;
//ת���ٶ�
int Swag_Speed=320;  
//ҡ��������
int SWING_WIGHT=32;
//ҡ�α�־λ
char Swing_Flag=0;

//����Χ���ӵ��ٶ�
char OUT_Speed_Add=40;

/*�ٶ�����*/
//10s�ڱ�������˳̶�
char BeHurt_10s=0;
//�˺��ٶ�ת������
char HP_Speed_Roit=10;

/*************************************************���̻������ƺ���2��*************************************************/
/**
 * @Function : �����ƶ����ƺ���
 * @Input    : �ٶ�
 * @Output   : 
 * @Notes    : ���ֶ������б����ã�������һ����������ÿһ���˶����ƺ������й�������
 * @Copyright: Aword
 **/
void Chassis_Run(short chassis_Speed)
{

		Chassis_Motor_201.Target_Speed=Pid_Calc(&PID_Chassis_201,Chassis_Motor_201.Real_Speed,chassis_Speed);
		
	  CAN1_TX_Chassis();
}

/**
 * @Function : ���������ƶ�����
 * @Input    : �ٶ�,�����
 * @Output   : 
 * @Notes    : �ڼ����Ҫ����ĵ���ֵ֮����й��ʼ��㣬��������ʣ��Լ��������Ʊ���
               ������ĵ���ֵ�˹������ƱȽ�������
 * @Copyright: Aword
 **/
float Current_Power=0;

void Chassis_Run_PowerLimit(short chassis_Speed,float power_max)
{
	
	float ratio=0;
	
  Chassis_Motor_201.Target_Speed=Pid_Calc(&PID_Chassis_201,Chassis_Motor_201.Real_Speed,chassis_Speed);
	
	
	
  //�Ӳ���ϵͳ��ȡ��ǰ����
	if(PowerHeatData.chassis_power!=0)
	Current_Power=PowerHeatData.chassis_power ;
	else
		Current_Power=(Chassis_Motor_201.Target_Speed/16384*20*23);
	
	
	//��ǰ������λ��POWER_LIMIT
	if(Current_Power>POWER_LIMIT) Current_Power=POWER_LIMIT;

	//����������뵱ǰ���ʵı���
	ratio=power_max/Current_Power;
	//����ǰ���ʴ�������ʣ�������ĵ���ֵ���б�������
	if(Current_Power>power_max)
	{
		Chassis_Motor_201.Target_Speed=Chassis_Motor_201.Target_Speed*ratio;

	}
	CAN1_TX_Chassis();
 
}
/**/




/*************************************************���̸�������2��*************************************************/
/**
 * @Function : �ڱ����̻�λ����
 * @Input    : 
 * @Output   : 
 * @Notes    : �õ����ƶ��������ಢͣ��
 * @Copyright: Aword
 **/
void Chassis_Poisition_Init(void)
{
	//���ڱ�û�������ʱ��һֱ������ƶ�,�ٶ�Ϊ����ʱ���ڱ����󣨴���࿴����أ�
	while(Position_Limit_Flag!=-1)
	{
		Chassis_Speed=-250;
		Chassis_Run(Chassis_Speed);
	}
	//���������˼�ֹͣ
	if(Position_Limit_Flag==-1)
	{
		Chassis_Speed=0;
		Chassis_Run(Chassis_Speed);
	}
}
/**
 * @Function : ���̵�ָ��λ��
 * @Input    : Ŀ�ĵأ��ٶ�
 * @Output   : 
 * @Notes    : 2019�������һ����Ĺ������4������λ�ã�
 * @Copyright: Aword
 **/
void Run_To_Position(char position,short speed)
{
	if(Chassis_Position==position)
	{
		Chassis_Speed=0;	
	}
	else if(Chassis_Position>position)
	{
      Chassis_Speed=-speed;	
	}
	else  Chassis_Speed=speed;
	
	Chassis_Run(Chassis_Speed);
	
}


/**/




/*************************************************����ת����2��*************************************************/
/**
 * @Function : λ��ת����
 * @Input    : //2019���� ���ޣ��Ҽ���
 * @Output   : 
 * @Notes    : ��ȡ���Һ��⴫������ֵ�������ڱ��ﵽ���Ҽ���λ�û����㳬���ޣ���ɲ�����ұ���
               ����������λ��֮��ɲ������
 * @Copyright: Aword
 **/

void Chassis_Turn_Position(char chassis_Left_Position,char chassis_Right_Position)
{
	float   speed;
	uint8_t stop_num=100;
	
	static int Left_Sensor_Number=1;
	static int Right_Sensor_Number=1;
	

	//��ȡ���⴫������ֵ
	Left_Sensor_Number=GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_11);
	Right_Sensor_Number=GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_12);
	
	//�������⵽
	if(Left_Sensor_Number==0)
	{
		//�ȴ�10ms�Է��󴥷�
	  delay_ms(10);
		if(Left_Sensor_Number==0)
		{
			//λ������Ϊ�������
			Position_Limit_Flag=-1;
			Chassis_Position=0;
			//���Ȧ����0
			Chassis_Motor_201_Number=0;
		}
	}
	//��������Ҳ��⵽
	else if(Right_Sensor_Number==0)
	{
		//�ȴ�10ms�Է��󴥷�
		delay_ms(10);
		if(Right_Sensor_Number==0)
		{
			//λ������Ϊ�Ҳ�����
			Position_Limit_Flag=1;
			
			Chassis_Position=4;
			Chassis_Motor_201_Number=RIGHT_LIMIT_NUM;
		}
	}
	//��������
	else
	{
		Position_Limit_Flag=0;
	}
	
	//���������λ��С�����λ�û��������λ����202�����Ȧ��С��0�����ң����������ƶ����߲��������ң����̻����������ƶ���
	if(((Chassis_Position <= chassis_Left_Position) || (Position_Limit_Flag==-1)||(Chassis_Motor_201_Number<0))&&((Chassis_Direction==-1)||(Chassis_Direction==0))&&(Chassis_Target_Direction==-1)) 
	{  
    //ɲ�����õ���ͣ��   	
		for(char i=0;i<stop_num;i++)
		{
			Chassis_Run(0);
			delay_ms(1);
		}
		//������٣����õ��̷��򵽴ﵱǰ���ٶ�
		for(short i=0;i<(stop_num+300);i++)
		{
			speed=-((float)(Chassis_Speed+Add_Speed)*(((float)(i+1))/((float)stop_num)));
			Chassis_Run_PowerLimit(speed,Power_Max);
			delay_ms(1);
		}
		//������Ŀ�귽��תΪ����
		Chassis_Target_Direction=1;
	}
	//���������λ�ô����Ҳ༫��λ�û����Ҳ���λ����202�����Ȧ������RIGHT_LIMIT_NUM�����ң����������ƶ����߲��������ң����̻����������ƶ���
	if(((Chassis_Position >= chassis_Right_Position)|| (Position_Limit_Flag==1)||(Chassis_Motor_201_Number>RIGHT_LIMIT_NUM))&&((Chassis_Direction==1)||(Chassis_Direction==0))&&(Chassis_Target_Direction==1)) 
	{
		//ɲ��
		for(char i=0;i<stop_num;i++)
		{
			Chassis_Run(0);
			delay_ms(1);
		}
		//�������
		for(short i=0;i<(stop_num+300);i++)
		{
			speed=-((float)(Chassis_Speed+Add_Speed)*(((float)(i+1))/((float)stop_num)));
			Chassis_Run_PowerLimit(speed,Power_Max);
			delay_ms(1);
		}
		//������Ŀ�귽��תΪ����
		Chassis_Target_Direction=-1;
	}
}
/**
 * @Function : Ȧ��ת����
 * @Input    : ����Ȧ�����Ҽ���Ȧ��
 * @Output   : 
 * @Notes    : ��ȡ���Һ��⴫������ֵ�������ڱ��ﵽ���Ҽ���λ�û����㳬���ޣ���ɲ�����ұ���
               ʹ�õ��ת����Ȧ������λ               
 * @Copyright: Aword
 **/
void Chassis_Num_Turn(char chassis_Left_Position,char chassis_Right_Position)
{
	float speed;
	uint8_t stop_num=100;
	static int Left_Sensor_Number=1;
	static int Right_Sensor_Number=1;
	
	Left_Sensor_Number=GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_11);
	Right_Sensor_Number=GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_12);
	
	if(Left_Sensor_Number==0)
	{
		delay_ms(10);
		if(Left_Sensor_Number==0)
		{
			Position_Limit_Flag=-1;
			Chassis_Position=0;
			Chassis_Motor_201_Number=0;
		}
	}
	else if(Right_Sensor_Number==0)
	{
		delay_ms(10);
		if(Right_Sensor_Number==0)
		{
			Position_Limit_Flag=1;
			Chassis_Position=4;
			Chassis_Motor_201_Number=RIGHT_LIMIT_NUM;
		}
	}
	else
	{
		Position_Limit_Flag=0;
	}
	
	if(((Chassis_Motor_201_Number <= chassis_Left_Position) || (Position_Limit_Flag==-1))&&((Chassis_Direction==-1)||(Chassis_Direction==0))&&(Chassis_Target_Direction==-1)) 
	{   
		
		for(char i=0;i<stop_num;i++)
		{
			Chassis_Run(0);
			delay_ms(1);
		}
		for(char i=0;i<stop_num;i++)
		{
			speed=-((float)Chassis_Speed*(((float)(i+1))/((float)stop_num)));
			Chassis_Run_PowerLimit(speed,Power_Max);
			delay_ms(1);
		}
		Chassis_Target_Direction=1;
	}
	//�Ҳ�
	if(((Chassis_Motor_201_Number >= chassis_Right_Position)|| (Position_Limit_Flag==1))&&((Chassis_Direction==1)||(Chassis_Direction==0))&&(Chassis_Target_Direction==1)) 
	{
		for(char i=0;i<stop_num;i++)
		{
			Chassis_Run(0);
			delay_ms(1);
		}
		for(char i=0;i<stop_num;i++)
		{
			speed=-((float)Chassis_Speed*(((float)(i+1))/((float)stop_num)));
//			printf("%f\r\n",speed);
			Chassis_Run_PowerLimit(speed,Power_Max);
			delay_ms(1);
		}
		Chassis_Target_Direction=-1;
	}
}



/**/



/*************************************************�����ε�����4��*************************************************/
/**
 * @Function : �����ε�����
 * @Input    : ���ޣ��Ҽ��ޣ��ٶ�
 * @Output   : 
 * @Notes    : ���Ҽ��޿���ȷ�������ڹ������һ��λ���ƶ����й�������
 * @Copyright: Aword
 **/
void Chassis_Swing(char Left_Poisition,char Right_Poisition,short chassis_Speed)
{
	
	Swing_Flag=0;
	
	//��������λλ����ɲ���𽥼��ٷ���ͬʱChassis_Target_Directionȡ��
	Chassis_Turn_Position(Left_Poisition,Right_Poisition);
	
	//�ٶ�����Ϊ����
	Chassis_Speed=chassis_Speed*Chassis_Target_Direction;
	
	//�����ڲ������ʵ�������ƶ�
	  
	Chassis_Run_PowerLimit(Chassis_Speed,Power_Max);	
}

/**
 * @Function : ���ٶ��ε�����
 * @Input    : ���ޣ��Ҽ��ޣ�
               chassis_Speed1�������ڳ�������λ��ʱ���ƶ��ٶ�
               chassis_Speed2��������û�г�������λ��ʱ���ƶ��ٶ�
 * @Output   : 
 * @Notes    : ���Ҽ��޿���ȷ�������ڹ������һ��λ���ƶ��������˳�������λ�ú�û�г�������λ�õ��ƶ��ٶ�
 * @Copyright: Aword
 **/
void Chassis_Swing_Speed(char Left_Poisition,char Right_Poisition,short chassis_Speed1,short chassis_Speed2)
{
	//��������λλ����ɲ���𽥼��ٷ���ͬʱChassis_Target_Directionȡ��
	Chassis_Turn_Position(Left_Poisition,Right_Poisition);
	

	//chassis_Speed1����chassis_Speed2
	//���ǳ����˼���λ����ʹ��chassis_Speed1�����ƶ�
	if((Chassis_Position<=Left_Poisition)||(Chassis_Position>=Right_Poisition))
	{
		Chassis_Speed=chassis_Speed1*Chassis_Target_Direction;
	}
	//����û�г�������λ����ʹ��chassis_Speed1�����ƶ�
	else Chassis_Speed=chassis_Speed2*Chassis_Target_Direction;
	
	//�����ڲ������ʵ�������ƶ�
	Chassis_Run_PowerLimit(Chassis_Speed,Power_Max);	
}
/**
 * @Function : Ȧ����λ�ε�����
 * @Input    : ���ޣ��Ҽ��ޣ�
               chassis_Speed1�������ڳ�������λ��ʱ���ƶ��ٶ�
               chassis_Speed2��������û�г�������λ��ʱ���ƶ��ٶ�
 * @Output   : 
 * @Notes    : ���Ҽ��޿���ȷ�������ڹ������һ��λ���ƶ�������һ��������ͬ����ʹ����Ȧ��ת����2
               ��ʹ��Ȧ�����������Ҽ���
 * @Copyright: Aword
 **/
void Chassis_Swing_NumLimit(char Left_Poisition,short Right_Poisition,short chassis_Speed_Out,short chassis_Speed_In)
{
	//���ڷ���
	Chassis_Num_Turn(Left_Poisition,Right_Poisition);
  
	
	
	//�����ٶ�
	//���202�����Ȧ��С������λ�û��ߴ����Ҽ���λ��,���ڷ�Χ��
	if((Chassis_Motor_201_Number<=Left_Poisition)||(Chassis_Motor_201_Number>=Right_Poisition))
	{
		Chassis_Speed=chassis_Speed_Out*Chassis_Target_Direction;
	}
	//�ڼ�����
	else Chassis_Speed=chassis_Speed_In*Chassis_Target_Direction;
	
	if(Chassis_Speed>=450)Chassis_Speed=450;
	
	
	
	
	
	Chassis_Run_PowerLimit(Chassis_Speed,Power_Max);	
}
	/**
 * @Function : �ֶ����ߺ���
 * @Input    : chassis_Speed1�������ڳ�������λ��ʱ���ƶ��ٶ�
               chassis_Speed2��������û�г�������λ��ʱ���ƶ��ٶ�
 * @Output   : 
 * @Notes    : �����ڹ���Ϸֶ��ƶ�2019���򣬵����ڲ������ʵ�������ƶ�
 * @Copyright: Aword
 **/
void Chassis_Swing_Section(short chassis_Speed1,short chassis_Speed2)
{
	static int Start_Number=0;
	static int Left_Limite=0,
		         Right_Limite=2;
	
	static int Arrive_Right_Limite=0;
	int   Start_Position[2]={0,2};
	
	//���ǵ���λ�ô����Ҳ�����λ�û�������λ�ñ�־Ϊ1���Ҳ༫�޻��ߵ��Ȧ������RIGHT_LIMIT_NUM
	//�������Ҽ���λ��
	if( (Chassis_Position>Right_Limite) || (Position_Limit_Flag==1) || (Chassis_Motor_201_Number>RIGHT_LIMIT_NUM) )
	{
		//�����Ҳ��־λ��1
		Arrive_Right_Limite=1;
	}
	
	//������Ҳ��־λΪ1
	if(Arrive_Right_Limite)
	{
		//������λ��С��������ƻ���λ�����Ʊ�־λΪ1���ߵ��Ȧ��С��0
		//�������Ҽ���λ���ֵ�������λ��
		if( (Chassis_Position<Left_Limite)||(Position_Limit_Flag==-1) || (Chassis_Motor_201_Number<0) )
		{
			//��ʼ��������
			Start_Number++;
			//�����Ҳ����Ʊ�־λΪ0
			Arrive_Right_Limite=0;
		}
		//�����ʼ��������1�����������0
		if(Start_Number>1) Start_Number=0;
		
		//������Ƶ��������ֵ��
		Left_Limite=Start_Position[Start_Number];
		//�Ҳ����Ƶ������
		Right_Limite=Left_Limite+2;	
	}
	
	//���ǵ������Ҽ��޼�ת��
	Chassis_Turn_Position(Left_Limite,Right_Limite);
	
	//���ǳ����˼���λ����ʹ��chassis_Speed1�����ƶ�
	if((Chassis_Position<=Left_Limite)||(Chassis_Position>=Right_Limite))
	{
		Chassis_Speed=chassis_Speed1*Chassis_Target_Direction;
	}
	//����û�г�������λ����ʹ��chassis_Speed2�����ƶ�
	else Chassis_Speed=chassis_Speed2*Chassis_Target_Direction;
	
	//�����ڲ������ʵ�������ƶ�
	Chassis_Run_PowerLimit(Chassis_Speed,Power_Max);	
}




/**
  * @brief  Ȧ���̷ֶ�
  * @param  void
  * @retval void
  * @notes  С���䷴���𵴣������õ��˴���   
            
  */

void Chassis_Swing_Short_NumLimit(char Left_Poisition,short chassis_Speed_In)
{
	
	short Right_Poisition;
	short chassis_Speed_Out;
	float speed;
	uint8_t stop_num=100;
	
	
	
	
	//������λ
	static int Left_Sensor_Number=1;
	static int Right_Sensor_Number=1;
	
	Left_Sensor_Number=GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_11);
	Right_Sensor_Number=GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_12);
	
	if(Left_Sensor_Number==0)
	{
		delay_ms(10);
		if(Left_Sensor_Number==0)
		{
			Position_Limit_Flag=-1;
			Chassis_Position=0;
			Chassis_Motor_201_Number=0;
		}
	}
	else if(Right_Sensor_Number==0)
	{
		delay_ms(10);
		if(Right_Sensor_Number==0)
		{
			Position_Limit_Flag=1;
			Chassis_Position=4;
			Chassis_Motor_201_Number=RIGHT_LIMIT_NUM;
		}
	}
	else
	{
		Position_Limit_Flag=0;
	}
	
	//�ٶ���λ������
	Right_Poisition=Left_Poisition+SWING_WIGHT;
	chassis_Speed_Out=chassis_Speed_In+OUT_Speed_Add;
	
	//���緽�����
	//���Ȧ��С������Ϊ��һ
	//ƫ��
   if((Chassis_Motor_201_Number<=Left_Poisition)||(Position_Limit_Flag==-1))
   { 
		 //����
		 Chassis_Target_Direction=1;
		 Chassis_Speed= Chassis_Target_Direction*chassis_Speed_Out;
		 //���粢�ҽ�Զ
		if(Left_Poisition-Chassis_Motor_201_Number>=8)Swing_Flag=0;
		 
		 
			
	 }
	 else if((Chassis_Motor_201_Number>=Right_Poisition)||(Position_Limit_Flag==1))
   { 
		 //����
		 Chassis_Target_Direction=-1;
	   Chassis_Speed= Chassis_Target_Direction*chassis_Speed_Out;
    
    if(Chassis_Motor_201_Number-Right_Poisition>=8)Swing_Flag=0;
	 
	 }
	 //�����ٶ�
	 else
	 {	

		 Swing_Flag=1;
	  Chassis_Speed= Chassis_Target_Direction*chassis_Speed_In;
	 }
	 
	 
	 
	 //���練ת�����
	 //�����ˣ�ֹͣ,֮�������
	 if(Chassis_Direction==(-Chassis_Target_Direction))
	 {
		 //ֹͣ
		for(char i=0;i<stop_num;i++)
		{
			Chassis_Run(0);
			delay_ms(1);
		}
		//�������
		for(short i=0;i<(stop_num+100);i++)
		{
			speed=((float)(Chassis_Speed>0?(Chassis_Speed+Add_Speed):(Chassis_Speed-Add_Speed))*(((float)(i+1))/((float)stop_num)));
			Chassis_Run_PowerLimit(speed,Power_Max);
			delay_ms(1);
		}	
	 }
	 
	 //�ٶ�����
	 if(Chassis_Speed>=450)Chassis_Speed=450;
	 if(Chassis_Speed<=-450)Chassis_Speed=-450;
	 
	 
	 
	 
	 //û����
	 Chassis_Run_PowerLimit(Chassis_Speed,Power_Max);

}
//


/**
  * @brief  �ֶ�����
  * @param  void
  * @retval void
  * @notes     chassis_Speed1�������ڳ�������λ��ʱ���ƶ��ٶ�
               chassis_Speed2��������û�г�������λ��ʱ���ƶ��ٶ�
            
  */
int Swing_i=0;
char Swing_Position=0;
char Danger_Coefficient=10;
void Swing_Now_Section(void)
{
	
	//��������Ѫ�����ܴ������л�����ģʽ
	
	//Ѫ������300�����������Ͽ���
	if(Up_To_Down_TypeStruct.Remain_HP<=300)
	{
	  Danger_Coefficient=10;
	}
	//���򣬷���Ŀ�꣬��
	else if((Chassis_Mode==Chassis_Up_Find_Target)||(Chassis_Mode==Chassis_Find_Target))Danger_Coefficient=1; 
	//û�з���Ŀ�꣬ȴ�����ˣ�30s�ڣ�����
	else if((Chassis_Mode==Chassis_Get_Big_Bullet)||(Chassis_Mode==Chassis_Get_Small_Hurt)||(Chassis_Mode==Chassis_Get_Big_Fire))Danger_Coefficient=4; 
	//û�з���Ŀ��Ҳû�б���
	else Danger_Coefficient=0; 
	
	
	/*ģʽ����*/
	Swing_i++;
	if(Swing_i>=1200)
	{
		Swing_i=0;
	  Swing_Position++;
		
		if(Swing_Position>Danger_Coefficient)Swing_Position=0;
	}
	
	
	 if(Up_To_Down_TypeStruct.Remain_HP<=100)Swing_Position=3;
	
	
	/*�����ε��������ΧԽ��Խ����*/
	switch(Swing_Position)
	{
	//��û���ܵ����ʱ
	case 0:
		{
		  	Chassis_Swing(0,4,Swag_Speed);
			break;
		}
	case 1:
		{
			Chassis_Swing(0,4,Swag_Speed+30); 
			break;	
		}
	case 2:
		{
		  Chassis_Swing_Short_NumLimit(30,Swag_Speed);
		  break;
		}
	case 3:
		{
		   Chassis_Swing_Short_NumLimit(150,Swag_Speed);
		  break;
		}	
		case 4:
		{
		  Chassis_Swing_Short_NumLimit(200,Run_Speed);
			break;
		}
		
		
		
		
		
	case 5:
		{
		  Chassis_Swing_Short_NumLimit(150,Swag_Speed);
			break;
		}

		case 6:
	{
		Chassis_Swing_Short_NumLimit(208,Swag_Speed);
		break;
	}
	case 7:
	{
		Chassis_Swing(0,4,Swag_Speed);
		break;
	}
	case 8:
	{
		Chassis_Swing_Short_NumLimit(170,Swag_Speed);
		break;
	}
	case 9:
	{
		Chassis_Swing_Short_NumLimit(30,Swag_Speed);
		break;
	}
		
	case 10:
	{
		Chassis_Swing(0,4,Swag_Speed);
		break;
	}
  
	
	
	default :
	 {
		Chassis_Swing(0,4,Swag_Speed);
		break;
	 }
	
	
	
	
	}
	
	

}
/**/



/*************************************************�����Զ�����Ӧ�Ժ���6��*************************************************/
/**
  * @brief  Ӧ�Դ��躯��
  * @param  void
  * @retval void
  * @notes  ���ʾ��ͼ��  ��--0--1--2--3--4--5��
            ���Ҳ౻�����ܵ���࣬��������ε���
            ����౻�����ܵ��Ҳ࣬�������Ҳ�

            �õ��ٶ�ΪDanger_Speed��BeAttacked_Speed
  */
void Chassis_Deal_With_Big_Bullet(void)
{
		 		
	static short Sin_Speed=300;
	
	Sin_Speed=(sin((float)TimeTick1ms/1000)*100+250); 
	
	Chassis_Swing(0,4,Sin_Speed);
	

}
/**
  * @brief  �ҵ����˺���
  * @param  void
  * @retval void
  * @notes  ���ֵ���֮���жϵ���ǿ�����������̫ǿ���ܣ���ǿ����
            �õ��ٶ�ΪDanger_Speed��BeAttacked_Speed
  */
void Chassis_Deal_With_Found_Target(void)
{
	//�������û�����ڱ���1s֮����ʧ30Ѫ�����򵽹���м����
	if(!Enemy_Danger_Level[View_Enemy_ID])
	{
		Chassis_Swing_Speed(2,2,Attack_Speed,BeAttacked_Speed);
	}
	//���������������һ���ڴ���ڱ�30Ѫ�����ڱ��������м�λ�ñ�����������ܵ��������
	else
	{
		if((Formar_Place==0)||(Formar_Place==1))
		{
			Chassis_Swing_Speed(2,2,Attack_Speed,BeAttacked_Speed);
		}
		else if((Formar_Place==3)||(Formar_Place==4))
		{
			Chassis_Swing_Speed(2,2,Attack_Speed,BeAttacked_Speed);
		}
	}	
}
/**
  * @brief  Ӧ��ǿ����
  * @param  void
  * @retval void
  * @notes  ��Ӧ�Դ���һ����
            ���Ҳ౻�����ܵ���࣬��������ε���
            ����౻�����ܵ��Ҳ࣬�������Ҳ�
            �õ��ٶ�ΪDanger_Speed��BeAttacked_Speed
  */
void Chassis_Deal_With_Big_Fire(void)
{

	Chassis_Swing(0,4,Danger_Speed);
}
/**
  * @brief  Ӧ��С����
  * @param  void
  * @retval void
  * @notes  ��Ӧ�Դ���һ����
            ���Ҳ౻�����ܵ���࣬��������ε���
            ����౻�����ܵ��Ҳ࣬�������Ҳ�
            �õ��ٶ�ΪRun_Speed��BeAttacked_Speed
  */
void Chassis_Deal_With_Small_Fire(void)
{
//   	if(Up_To_Down_TypeStruct.Get_Hit_flag==0)
//		{
//			if(Formar_Place<2) Where_To_Go=4;
//			else Where_To_Go=0;
//		}
//		else if(Up_To_Down_TypeStruct.Get_Hit_flag==1)
//		{
//			if(Formar_Place<3) Where_To_Go=4;
//			else Where_To_Go=0;
//		}
//		if(Where_To_Go==4) Chassis_Swing_Speed(2,PART_FOUR,Run_Speed,BeAttacked_Speed);
//		else Chassis_Swing_Speed(PART_ZERO,2,Run_Speed,BeAttacked_Speed);

	
}
/**
  * @brief  ���ӵ���Ѫ������
  * @param  void
  * @retval void
  * @notes  ���ε��ڹ����������ȥ
            �õ��ٶ�ΪRun_Speed
  */
void Chassis_Deal_With_Limite(void)
{
	Chassis_Swing(PART_ZERO,PART_FOUR,Run_Speed);
}

//


/**
  * @brief  ��ͨ����ֶα����˶�����
  * @param  void
  * @retval void
  * @notes  ֱ���ڹ�������������ε������
            �õ��ٶ�ΪSwag_Speed

            ��������ؿ�ʱ���ٶ�����Ϊ��
            ���ڱ����Ҳ�,����Ϊ��
            Chassis_Position=4
            Chassis_Motor_201_Number=276
            Position_Limit_Flag=1

            ǰ��վ����ʱ�����ڹ����ߣ�ÿ20s��һ�Σ�ǰ��վ���ˣ�����Ѳ��         
  */
void Chassis_Auto_Normal(void)
{
  
	if(My_Outpost_HP>=10)
	{
		//��һ��
		if(Sway_Buttle==1)
		{ 
			while(Position_Limit_Flag!=1)
			{
				Chassis_Speed=250;
				Chassis_Run(Chassis_Speed);
			}
			//������Ҳ��˼�ֹͣ
			if(Position_Limit_Flag==1)
			{
				Chassis_Speed=0;
				Chassis_Run(Chassis_Speed);
			}
			
			Sway_Buttle=0;
		}
		//���ڹ�����
		else
		{
		  Chassis_Poisition_Init();
		}
	    Output_On=1;
	}
	//ǰ��վû��֮��
	else
	{
		Output_On=0;
		
	  //
		Swing_Now_Section();
		
	}
		

}


/**/





/*************************************************����ģʽ����3��*************************************************/
/**
 * @Function : �����ֶ�ң��������
 * @Input    : void
 * @Output   : void
 * @Notes    : ����������������
 * @Copyright: Aword
 **/
void Chassis_Control(void)
{ 
	uint16_t stop_num=200;
	float speed;
	
	
	Swing_Flag=0;
	
	//û�е��Ｋ��λ��
	if(Position_Limit_Flag==0)//&&(Chassis_Motor_201_Number<RIGHT_LIMIT_NUM-1) )
	{
		Chassis_Speed=DBUS.RC.ch0/2;
	}
	//������༫��������������ƶ�,�ٶ�Ϊ��ʱ����
	else if(Position_Limit_Flag==-1)
	{
		if(Chassis_Speed<0)
		{
			for(int i=0;i<stop_num;i++)
			{
				//��������
				speed=(float)Chassis_Speed*(1.0f-((float)(i+1))/((float)stop_num));
				Chassis_Run(speed);
				delay_ms(1);
			}
		}
		Chassis_Speed=DBUS.RC.ch0/2;
		Chassis_Speed=( Chassis_Speed<0?0:Chassis_Speed);		
	}
	//�����Ҳ����������Ҳ��ƶ�
	else if(Position_Limit_Flag==1)
	{
		if(Chassis_Speed>0)
		{
			for(int i=0;i<stop_num;i++)
			{
				speed=(float)Chassis_Speed*(1.0f-((float)(i+1))/((float)stop_num));
				Chassis_Run(0);
				delay_ms(1);
			}
		}
		Chassis_Speed=DBUS.RC.ch0/2;
		Chassis_Speed=( Chassis_Speed>0?0:Chassis_Speed);	
	}
	
  //ң�������Ƶ�ʱ��û�й������ƣ�ֱ��ʹ����ͨ���ƶ�����
	Chassis_Run( Chassis_Speed );
}

/**
 * @Function : �����Զ�����
 * @Input    : 
 * @Output   : 
 * @Notes    : ��ͬ��ģʽ���ò�ͬ��Ӧ�Է�������9�ַ���
               ���ȼ��Ӹߵ���
 * @Copyright: Aword
*/
void Chassis_Auto_Control(void)
{
	//��⵽���裬��������
  if(Chassis_Mode==Chassis_Get_Big_Bullet)
	{
		Chassis_Swing(0,4,340);
	}
	//�з�����̫ǿֱ���ܣ���������
	else if(Chassis_Mode==Chassis_Get_Big_Fire)
	{
		Chassis_Swing(0,4,340);
	}
	//ʶ�𵽵��ˣ�������ӵ���ͣ�´�û�о���
	else if(Chassis_Mode==Chassis_Up_Find_Target)
	{
		
		if(UP_Heat_Allow_Flag&&(number_1<=490)&&Up_NUC_Data_TypeStruct.NUC_Shoot_Allow_Flag)
		{	
			
		  Chassis_Run(0);
			
		}
		//�������
		else
		{
		  Swing_Now_Section();
		}	
	}	
	
  //�ܵ�С����������ͨ
	else if(Chassis_Mode==Chassis_Get_Small_Hurt)
	{
		 Swing_Now_Section();
	}	
	
	
	//���˻������
	else if(Chassis_Mode==Chassis_Plane_Fly)
	{
		Chassis_Swing(0,4,270);
	}
	//��ͣ�����룬���ڹ���Ϸֶ�����
	else if(Chassis_Mode==Chassis_Stop_3S)
	{
		Chassis_Swing(0,4,270);
	}
	
	
	//�кܶ���ˣ�
	else if(Chassis_Mode==Chassis_Many_Enemy)
	{
		Chassis_Swing(0,4,270);
		
	}
  //����ģʽ������Ѳ��
	else if(Chassis_Mode==Chassis_Normal)
	{
	  Chassis_Swing(0,4,Swag_Speed);	
	}
	else
  {
	  Chassis_Swing(0,4,270);	
	}
	
}
/**
 * @Function : �����Զ���������
 * @Input    : 
 * @Output   : 
 * @Notes    : �����ڱ�����ʼ֮ǰ���ڹ���Ҳ࣬��ʼ֮��30s�����Ҳ���ǰ��վ��֮��Ѳ��
 * @Copyright: Aword
*/
char First_Start_Flag=0;
u32 Start_Time=0;
void Chassis_Auto_Control_Advance(void)
{
	//����û��ʼ�����ڹ���Ҳ�,����ʱ���ֵ�һ��
	if(Game_State.game_progress!=4)
	{
	  Run_To_Position(4,220); 
	
	}
	//������ʼ��
	else
	{
		//��¼ʱ��
		if(First_Start_Flag==0)
		{
	  	Start_Time=TimeTick1ms;
			First_Start_Flag=1;
			
		}
	  //��ʼ֮��20�룬����
		if(TimeTick1ms<=Start_Time+20000)
		{
		
			Chassis_Run(0);
		}
		//��ʮ��֮��Ѳ��
		else
	  {
	
			
				
		  Chassis_Swing(0,4,270);	
		}
	
	
	}
	
	
	
	
	
		
	
}
/**/




/**
  * @brief  ���̿���������
  * @param  void
  * @retval void
  * @notes  ���ȼ�Ϊ 2��ÿ2msִ��һ��	
  */
void Chassis_Control_Task(void *pvParameters)
{
	Sensor_Init();//��������ʼ��
	PID_Init_CHASSIS();//����PID��ʼ��
  
	//Random_Num_Generator_Init();//�������ʼ��
	
	
	
	//��ֹң����������ֵ�����ʹ�õ��̱���
	Chassis_Motor_201.Target_Speed=0;
	CAN1_TX_Chassis();
	
   while(1)
	{

		 //�����ֶ����ƣ��Ҳದ�����ϻ�����
		if(DBUS.RC.Switch_Right==RC_SW_UP)
		{
			//ֻ�м���λ������û�й�������
			Chassis_Control();
		}
		else if( DBUS.RC.Switch_Right==RC_SW_DOWN)
		{
			
			Chassis_Control();
		}	
		//�����Զ����ƣ��Ҳದ�����м�
		else if(DBUS.RC.Switch_Right==RC_SW_MID)
		{ 
		
//			if(My_Usart_Recieve_Data==0)
//			{
//			 //�ֶ�����
		//	Chassis_Control();
//			}
//			else if (My_Usart_Recieve_Data==1)
//			{
//			  //�ڹ��С��Χҡ�Σ���һ��������λ�ã����������꣺0-276
			// Chassis_Swing(0,4,Swag_Speed);
       //Chassis_Swing_Short_NumLimit(100,Swag_Speed);
//			}
//		  else if (My_Usart_Recieve_Data==2)
//			{  
//			  //ȫ���ε�
		    Swing_Now_Section();
//			}
//      else
//			{  
//			 //����ǰ��վ��ģʽ
//			 Chassis_Auto_Normal();
//			}
				
		}
		 vTaskDelay(2);
	}
}













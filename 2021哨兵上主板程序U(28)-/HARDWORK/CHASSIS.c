#include "main.h"
/*关于左右的判断
  总是从对方基地看向哨兵轨道，右侧为1，左侧为-1
*/



//底盘速度控制
short   Chassis_Speed=0;
//反向增加速度
char Add_Speed=40;
 //底盘方向控制   1--右   -1--左
int8_t  Chassis_Target_Direction=1;









//往什么方向跑  
int Where_To_Go=0;

//受到伤害时的位置、锁定目标时的位置
int Formar_Place=0;

//位置收集，暂时还不知道有什么用处
int Position_Collet_Flag=0;   

//摇一摇弹丸
char Sway_Buttle=0;





//底盘模式共用体
enum Chassis_mode Chassis_Mode=Chassis_Normal;

//敌方危险等级，用于决定跑不跑
int Enemy_Danger_Level[8]={0,1,0,1,1,0,0,0};  //0,步兵，英雄，工程，步兵，步兵，哨兵

                      
int Danger_Speed=380;

                    
int Attack_Speed=300;
int BeAttacked_Speed=300;

/*********************************底盘速度数据定义区***********************/
/*
   速度
swing
   350    缓慢消耗缓冲能量，不会撞到  ,对应功率27
   320    一个循环，能量不变          ，对应功率15
   300    可以恢复缓冲能量                     

分段


*/
/*摇晃相关*/
int Serach_Speed=220;

int Run_Speed=350;
//转换速度
int Swag_Speed=320;  
//摇晃区间宽度
int SWING_WIGHT=32;
//摇晃标志位
char Swing_Flag=0;

//在外围增加的速度
char OUT_Speed_Add=40;

/*速度区间*/
//10s内被打击受伤程度
char BeHurt_10s=0;
//伤害速度转换比例
char HP_Speed_Roit=10;

/*************************************************底盘基本控制函数2个*************************************************/
/**
 * @Function : 底盘移动控制函数
 * @Input    : 速度
 * @Output   : 
 * @Notes    : 在手动控制中被调用，除了这一个，其他的每一个运动控制函数都有功率限制
 * @Copyright: Aword
 **/
void Chassis_Run(short chassis_Speed)
{

		Chassis_Motor_201.Target_Speed=Pid_Calc(&PID_Chassis_201,Chassis_Motor_201.Real_Speed,chassis_Speed);
		
	  CAN1_TX_Chassis();
}

/**
 * @Function : 功率限制移动函数
 * @Input    : 速度,最大功率
 * @Output   : 
 * @Notes    : 在计算出要输出的电流值之后进行功率计算，限制最大功率，以及功率限制比例
               对输出的电流值乘功率限制比进行缩放
 * @Copyright: Aword
 **/
float Current_Power=0;

void Chassis_Run_PowerLimit(short chassis_Speed,float power_max)
{
	
	float ratio=0;
	
  Chassis_Motor_201.Target_Speed=Pid_Calc(&PID_Chassis_201,Chassis_Motor_201.Real_Speed,chassis_Speed);
	
	
	
  //从裁判系统读取当前功率
	if(PowerHeatData.chassis_power!=0)
	Current_Power=PowerHeatData.chassis_power ;
	else
		Current_Power=(Chassis_Motor_201.Target_Speed/16384*20*23);
	
	
	//当前功率限位于POWER_LIMIT
	if(Current_Power>POWER_LIMIT) Current_Power=POWER_LIMIT;

	//计算最大功率与当前功率的比例
	ratio=power_max/Current_Power;
	//若当前功率大于最大功率，对输出的电流值进行比例缩放
	if(Current_Power>power_max)
	{
		Chassis_Motor_201.Target_Speed=Chassis_Motor_201.Target_Speed*ratio;

	}
	CAN1_TX_Chassis();
 
}
/**/




/*************************************************底盘辅助函数2个*************************************************/
/**
 * @Function : 哨兵底盘回位函数
 * @Input    : 
 * @Output   : 
 * @Notes    : 让底盘移动到轨道左侧并停下
 * @Copyright: Aword
 **/
void Chassis_Poisition_Init(void)
{
	//当哨兵没有在左边时，一直向左侧移动,速度为负的时候哨兵向左（从外侧看向基地）
	while(Position_Limit_Flag!=-1)
	{
		Chassis_Speed=-250;
		Chassis_Run(Chassis_Speed);
	}
	//如果到左侧了即停止
	if(Position_Limit_Flag==-1)
	{
		Chassis_Speed=0;
		Chassis_Run(Chassis_Speed);
	}
}
/**
 * @Function : 底盘到指定位置
 * @Input    : 目的地，速度
 * @Output   : 
 * @Notes    : 2019轨道有是一条弯的轨道，有4个特征位置，
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




/*************************************************底盘转向函数2个*************************************************/
/**
 * @Function : 位置转向函数
 * @Input    : //2019规则 左极限，右极限
 * @Output   : 
 * @Notes    : 获取左右红外传感器的值，若是哨兵达到左右极限位置还打算超极限，则刹车并且变向
               即到达限制位置之后刹车变向
 * @Copyright: Aword
 **/

void Chassis_Turn_Position(char chassis_Left_Position,char chassis_Right_Position)
{
	float   speed;
	uint8_t stop_num=100;
	
	static int Left_Sensor_Number=1;
	static int Right_Sensor_Number=1;
	

	//获取红外传感器的值
	Left_Sensor_Number=GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_11);
	Right_Sensor_Number=GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_12);
	
	//如果左侧检测到
	if(Left_Sensor_Number==0)
	{
		//等待10ms以防误触发
	  delay_ms(10);
		if(Left_Sensor_Number==0)
		{
			//位置限制为左侧限制
			Position_Limit_Flag=-1;
			Chassis_Position=0;
			//电机圈数置0
			Chassis_Motor_201_Number=0;
		}
	}
	//否则如果右侧检测到
	else if(Right_Sensor_Number==0)
	{
		//等待10ms以防误触发
		delay_ms(10);
		if(Right_Sensor_Number==0)
		{
			//位置限制为右侧限制
			Position_Limit_Flag=1;
			
			Chassis_Position=4;
			Chassis_Motor_201_Number=RIGHT_LIMIT_NUM;
		}
	}
	//否则不限制
	else
	{
		Position_Limit_Flag=0;
	}
	
	//如果（底盘位置小于左侧位置或者左侧限位或者202电机的圈数小于0）并且（底盘向左移动或者不动）并且（底盘还打算向左移动）
	if(((Chassis_Position <= chassis_Left_Position) || (Position_Limit_Flag==-1)||(Chassis_Motor_201_Number<0))&&((Chassis_Direction==-1)||(Chassis_Direction==0))&&(Chassis_Target_Direction==-1)) 
	{  
    //刹车，让底盘停下   	
		for(char i=0;i<stop_num;i++)
		{
			Chassis_Run(0);
			delay_ms(1);
		}
		//反向加速，逐渐让底盘反向到达当前的速度
		for(short i=0;i<(stop_num+300);i++)
		{
			speed=-((float)(Chassis_Speed+Add_Speed)*(((float)(i+1))/((float)stop_num)));
			Chassis_Run_PowerLimit(speed,Power_Max);
			delay_ms(1);
		}
		//将底盘目标方向转为向右
		Chassis_Target_Direction=1;
	}
	//如果（底盘位置大于右侧极限位置或者右侧限位或者202电机的圈数大于RIGHT_LIMIT_NUM）并且（底盘向右移动或者不动）并且（底盘还打算向右移动）
	if(((Chassis_Position >= chassis_Right_Position)|| (Position_Limit_Flag==1)||(Chassis_Motor_201_Number>RIGHT_LIMIT_NUM))&&((Chassis_Direction==1)||(Chassis_Direction==0))&&(Chassis_Target_Direction==1)) 
	{
		//刹车
		for(char i=0;i<stop_num;i++)
		{
			Chassis_Run(0);
			delay_ms(1);
		}
		//反向加速
		for(short i=0;i<(stop_num+300);i++)
		{
			speed=-((float)(Chassis_Speed+Add_Speed)*(((float)(i+1))/((float)stop_num)));
			Chassis_Run_PowerLimit(speed,Power_Max);
			delay_ms(1);
		}
		//将底盘目标方向转为向左
		Chassis_Target_Direction=-1;
	}
}
/**
 * @Function : 圈数转向函数
 * @Input    : 左极限圈数，右极限圈数
 * @Output   : 
 * @Notes    : 获取左右红外传感器的值，若是哨兵达到左右极限位置还打算超极限，则刹车并且变向
               使用电机转过的圈数来限位               
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
	//右侧
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



/*************************************************底盘游荡函数4个*************************************************/
/**
 * @Function : 底盘游荡函数
 * @Input    : 左极限，右极限，速度
 * @Output   : 
 * @Notes    : 左右极限可以确定底盘在轨道的哪一段位置移动，有功率限制
 * @Copyright: Aword
 **/
void Chassis_Swing(char Left_Poisition,char Right_Poisition,short chassis_Speed)
{
	
	Swing_Flag=0;
	
	//若到达限位位置则刹车逐渐加速反向、同时Chassis_Target_Direction取反
	Chassis_Turn_Position(Left_Poisition,Right_Poisition);
	
	//速度向右为正，
	Chassis_Speed=chassis_Speed*Chassis_Target_Direction;
	
	//底盘在不超功率的情况下移动
	  
	Chassis_Run_PowerLimit(Chassis_Speed,Power_Max);	
}

/**
 * @Function : 分速度游荡函数
 * @Input    : 左极限，右极限，
               chassis_Speed1：底盘在超出极限位置时的移动速度
               chassis_Speed2：底盘在没有超出极限位置时的移动速度
 * @Output   : 
 * @Notes    : 左右极限可以确定底盘在轨道的哪一段位置移动，区分了超出极限位置和没有超出极限位置的移动速度
 * @Copyright: Aword
 **/
void Chassis_Swing_Speed(char Left_Poisition,char Right_Poisition,short chassis_Speed1,short chassis_Speed2)
{
	//若到达限位位置则刹车逐渐加速反向、同时Chassis_Target_Direction取反
	Chassis_Turn_Position(Left_Poisition,Right_Poisition);
	

	//chassis_Speed1大于chassis_Speed2
	//若是超过了极限位置则使用chassis_Speed1进行移动
	if((Chassis_Position<=Left_Poisition)||(Chassis_Position>=Right_Poisition))
	{
		Chassis_Speed=chassis_Speed1*Chassis_Target_Direction;
	}
	//否则没有超过极限位置则使用chassis_Speed1进行移动
	else Chassis_Speed=chassis_Speed2*Chassis_Target_Direction;
	
	//底盘在不超功率的情况下移动
	Chassis_Run_PowerLimit(Chassis_Speed,Power_Max);	
}
/**
 * @Function : 圈数限位游荡函数
 * @Input    : 左极限，右极限，
               chassis_Speed1：底盘在超出极限位置时的移动速度
               chassis_Speed2：底盘在没有超出极限位置时的移动速度
 * @Output   : 
 * @Notes    : 左右极限可以确定底盘在轨道的哪一段位置移动，与上一个函数不同的是使用了圈数转向函数2
               即使用圈数来计算左右极限
 * @Copyright: Aword
 **/
void Chassis_Swing_NumLimit(char Left_Poisition,short Right_Poisition,short chassis_Speed_Out,short chassis_Speed_In)
{
	//关于方向
	Chassis_Num_Turn(Left_Poisition,Right_Poisition);
  
	
	
	//关于速度
	//如果202电机的圈数小于左极限位置或者大于右极限位置,不在范围内
	if((Chassis_Motor_201_Number<=Left_Poisition)||(Chassis_Motor_201_Number>=Right_Poisition))
	{
		Chassis_Speed=chassis_Speed_Out*Chassis_Target_Direction;
	}
	//在极限内
	else Chassis_Speed=chassis_Speed_In*Chassis_Target_Direction;
	
	if(Chassis_Speed>=450)Chassis_Speed=450;
	
	
	
	
	
	Chassis_Run_PowerLimit(Chassis_Speed,Power_Max);	
}
	/**
 * @Function : 分段游走函数
 * @Input    : chassis_Speed1：底盘在超出极限位置时的移动速度
               chassis_Speed2：底盘在没有超出极限位置时的移动速度
 * @Output   : 
 * @Notes    : 底盘在轨道上分段移动2019规则，底盘在不超功率的情况下移动
 * @Copyright: Aword
 **/
void Chassis_Swing_Section(short chassis_Speed1,short chassis_Speed2)
{
	static int Start_Number=0;
	static int Left_Limite=0,
		         Right_Limite=2;
	
	static int Arrive_Right_Limite=0;
	int   Start_Position[2]={0,2};
	
	//若是底盘位置大于右侧限制位置或者限制位置标志为1即右侧极限或者电机圈数大于RIGHT_LIMIT_NUM
	//即到达右极限位置
	if( (Chassis_Position>Right_Limite) || (Position_Limit_Flag==1) || (Chassis_Motor_201_Number>RIGHT_LIMIT_NUM) )
	{
		//到达右侧标志位置1
		Arrive_Right_Limite=1;
	}
	
	//如果达右侧标志位为1
	if(Arrive_Right_Limite)
	{
		//如果电机位置小于左侧限制或者位置限制标志位为1或者电机圈数小于0
		//即到完右极限位置又到了左极限位置
		if( (Chassis_Position<Left_Limite)||(Position_Limit_Flag==-1) || (Chassis_Motor_201_Number<0) )
		{
			//开始数量增加
			Start_Number++;
			//到达右侧限制标志位为0
			Arrive_Right_Limite=0;
		}
		//如果开始数量大于1，则令其等于0
		if(Start_Number>1) Start_Number=0;
		
		//左侧限制等于数组的值，
		Left_Limite=Start_Position[Start_Number];
		//右侧限制等于左侧
		Right_Limite=Left_Limite+2;	
	}
	
	//若是到了左、右极限即转向
	Chassis_Turn_Position(Left_Limite,Right_Limite);
	
	//若是超过了极限位置则使用chassis_Speed1进行移动
	if((Chassis_Position<=Left_Limite)||(Chassis_Position>=Right_Limite))
	{
		Chassis_Speed=chassis_Speed1*Chassis_Target_Direction;
	}
	//否则没有超过极限位置则使用chassis_Speed2进行移动
	else Chassis_Speed=chassis_Speed2*Chassis_Target_Direction;
	
	//底盘在不超功率的情况下移动
	Chassis_Run_PowerLimit(Chassis_Speed,Power_Max);	
}




/**
  * @brief  圈数短分段
  * @param  void
  * @retval void
  * @notes  小区间反复震荡，可以让敌人打不中   
            
  */

void Chassis_Swing_Short_NumLimit(char Left_Poisition,short chassis_Speed_In)
{
	
	short Right_Poisition;
	short chassis_Speed_Out;
	float speed;
	uint8_t stop_num=100;
	
	
	
	
	//红外限位
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
	
	//速度与位置区间
	Right_Poisition=Left_Poisition+SWING_WIGHT;
	chassis_Speed_Out=chassis_Speed_In+OUT_Speed_Add;
	
	//过界方向控制
	//左侧圈数小，限制为负一
	//偏左
   if((Chassis_Motor_201_Number<=Left_Poisition)||(Position_Limit_Flag==-1))
   { 
		 //向右
		 Chassis_Target_Direction=1;
		 Chassis_Speed= Chassis_Target_Direction*chassis_Speed_Out;
		 //过界并且较远
		if(Left_Poisition-Chassis_Motor_201_Number>=8)Swing_Flag=0;
		 
		 
			
	 }
	 else if((Chassis_Motor_201_Number>=Right_Poisition)||(Position_Limit_Flag==1))
   { 
		 //向左
		 Chassis_Target_Direction=-1;
	   Chassis_Speed= Chassis_Target_Direction*chassis_Speed_Out;
    
    if(Chassis_Motor_201_Number-Right_Poisition>=8)Swing_Flag=0;
	 
	 }
	 //界内速度
	 else
	 {	

		 Swing_Flag=1;
	  Chassis_Speed= Chassis_Target_Direction*chassis_Speed_In;
	 }
	 
	 
	 
	 //过界反转与加速
	 //方向反了，停止,之后反向加速
	 if(Chassis_Direction==(-Chassis_Target_Direction))
	 {
		 //停止
		for(char i=0;i<stop_num;i++)
		{
			Chassis_Run(0);
			delay_ms(1);
		}
		//反向加速
		for(short i=0;i<(stop_num+100);i++)
		{
			speed=((float)(Chassis_Speed>0?(Chassis_Speed+Add_Speed):(Chassis_Speed-Add_Speed))*(((float)(i+1))/((float)stop_num)));
			Chassis_Run_PowerLimit(speed,Power_Max);
			delay_ms(1);
		}	
	 }
	 
	 //速度限制
	 if(Chassis_Speed>=450)Chassis_Speed=450;
	 if(Chassis_Speed<=-450)Chassis_Speed=-450;
	 
	 
	 
	 
	 //没反则
	 Chassis_Run_PowerLimit(Chassis_Speed,Power_Max);

}
//


/**
  * @brief  分段游走
  * @param  void
  * @retval void
  * @notes     chassis_Speed1：底盘在超出极限位置时的移动速度
               chassis_Speed2：底盘在没有超出极限位置时的移动速度
            
  */
int Swing_i=0;
char Swing_Position=0;
char Danger_Coefficient=10;
void Swing_Now_Section(void)
{
	
	//根据自生血量和受打击情况切换底盘模式
	
	//血量少于300，不挣扎，赶快跑
	if(Up_To_Down_TypeStruct.Remain_HP<=300)
	{
	  Danger_Coefficient=10;
	}
	//否则，发现目标，干
	else if((Chassis_Mode==Chassis_Up_Find_Target)||(Chassis_Mode==Chassis_Find_Target))Danger_Coefficient=1; 
	//没有发现目标，却被打了，30s内，乱跑
	else if((Chassis_Mode==Chassis_Get_Big_Bullet)||(Chassis_Mode==Chassis_Get_Small_Hurt)||(Chassis_Mode==Chassis_Get_Big_Fire))Danger_Coefficient=4; 
	//没有发现目标也没有被打
	else Danger_Coefficient=0; 
	
	
	/*模式递增*/
	Swing_i++;
	if(Swing_i>=1200)
	{
		Swing_i=0;
	  Swing_Position++;
		
		if(Swing_Position>Danger_Coefficient)Swing_Position=0;
	}
	
	
	 if(Up_To_Down_TypeStruct.Remain_HP<=100)Swing_Position=3;
	
	
	/*底盘游荡情况，范围越广越复杂*/
	switch(Swing_Position)
	{
	//当没有受到打击时
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



/*************************************************底盘自动控制应对函数6个*************************************************/
/**
  * @brief  应对大弹丸函数
  * @param  void
  * @retval void
  * @notes  轨道示意图：  左--0--1--2--3--4--5右
            在右侧被打则跑到左侧，并在左侧游荡，
            在左侧被打则跑到右侧，并躲在右侧

            用的速度为Danger_Speed和BeAttacked_Speed
  */
void Chassis_Deal_With_Big_Bullet(void)
{
		 		
	static short Sin_Speed=300;
	
	Sin_Speed=(sin((float)TimeTick1ms/1000)*100+250); 
	
	Chassis_Swing(0,4,Sin_Speed);
	

}
/**
  * @brief  找到敌人函数
  * @param  void
  * @retval void
  * @notes  发现敌人之后判断敌人强弱，如果敌人太强则跑，不强则留
            用的速度为Danger_Speed和BeAttacked_Speed
  */
void Chassis_Deal_With_Found_Target(void)
{
	//如果敌人没有让哨兵在1s之内损失30血量，则到轨道中间打他
	if(!Enemy_Danger_Level[View_Enemy_ID])
	{
		Chassis_Swing_Speed(2,2,Attack_Speed,BeAttacked_Speed);
	}
	//否则如果敌人能在一秒内打掉哨兵30血，且哨兵不是在中间位置被打，则快速逃跑到轨道中央
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
  * @brief  应对强火力
  * @param  void
  * @retval void
  * @notes  与应对大弹丸一样跑
            在右侧被打则跑到左侧，并在左侧游荡，
            在左侧被打则跑到右侧，并躲在右侧
            用的速度为Danger_Speed和BeAttacked_Speed
  */
void Chassis_Deal_With_Big_Fire(void)
{

	Chassis_Swing(0,4,Danger_Speed);
}
/**
  * @brief  应对小弹丸
  * @param  void
  * @retval void
  * @notes  与应对大弹丸一样跑
            在右侧被打则跑到左侧，并在左侧游荡，
            在左侧被打则跑到右侧，并躲在右侧
            用的速度为Run_Speed和BeAttacked_Speed
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
  * @brief  无子弹与血量过低
  * @param  void
  * @retval void
  * @notes  无奈地在轨道上跑来跑去
            用的速度为Run_Speed
  */
void Chassis_Deal_With_Limite(void)
{
	Chassis_Swing(PART_ZERO,PART_FOUR,Run_Speed);
}

//


/**
  * @brief  普通随机分段变速运动函数
  * @param  void
  * @retval void
  * @notes  直接在轨道上正常左右游荡，随机
            用的速度为Swag_Speed

            从外向基地看时，速度向右为正
            当哨兵在右侧,向右为正
            Chassis_Position=4
            Chassis_Motor_201_Number=276
            Position_Limit_Flag=1

            前哨站还在时，待在轨道左边，每20s晃一晃，前哨站无了，开启巡航         
  */
void Chassis_Auto_Normal(void)
{
  
	if(My_Outpost_HP>=10)
	{
		//晃一晃
		if(Sway_Buttle==1)
		{ 
			while(Position_Limit_Flag!=1)
			{
				Chassis_Speed=250;
				Chassis_Run(Chassis_Speed);
			}
			//如果到右侧了即停止
			if(Position_Limit_Flag==1)
			{
				Chassis_Speed=0;
				Chassis_Run(Chassis_Speed);
			}
			
			Sway_Buttle=0;
		}
		//待在轨道左边
		else
		{
		  Chassis_Poisition_Init();
		}
	    Output_On=1;
	}
	//前哨站没了之后
	else
	{
		Output_On=0;
		
	  //
		Swing_Now_Section();
		
	}
		

}


/**/





/*************************************************底盘模式函数3个*************************************************/
/**
 * @Function : 底盘手动遥控器控制
 * @Input    : void
 * @Output   : void
 * @Notes    : 超过极限则缓慢减速
 * @Copyright: Aword
 **/
void Chassis_Control(void)
{ 
	uint16_t stop_num=200;
	float speed;
	
	
	Swing_Flag=0;
	
	//没有到达极限位置
	if(Position_Limit_Flag==0)//&&(Chassis_Motor_201_Number<RIGHT_LIMIT_NUM-1) )
	{
		Chassis_Speed=DBUS.RC.ch0/2;
	}
	//到达左侧极限则不能再往左侧移动,速度为负时向左，
	else if(Position_Limit_Flag==-1)
	{
		if(Chassis_Speed<0)
		{
			for(int i=0;i<stop_num;i++)
			{
				//缓慢减速
				speed=(float)Chassis_Speed*(1.0f-((float)(i+1))/((float)stop_num));
				Chassis_Run(speed);
				delay_ms(1);
			}
		}
		Chassis_Speed=DBUS.RC.ch0/2;
		Chassis_Speed=( Chassis_Speed<0?0:Chassis_Speed);		
	}
	//到达右侧则不能再往右侧移动
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
	
  //遥控器控制的时候没有功率限制，直接使用普通的移动函数
	Chassis_Run( Chassis_Speed );
}

/**
 * @Function : 底盘自动控制
 * @Input    : 
 * @Output   : 
 * @Notes    : 不同的模式采用不同的应对方法，共9种方法
               优先级从高到低
 * @Copyright: Aword
*/
void Chassis_Auto_Control(void)
{
	//检测到大弹丸，紧急加速
  if(Chassis_Mode==Chassis_Get_Big_Bullet)
	{
		Chassis_Swing(0,4,340);
	}
	//敌方火力太强直接跑，紧急加速
	else if(Chassis_Mode==Chassis_Get_Big_Fire)
	{
		Chassis_Swing(0,4,340);
	}
	//识别到敌人，如果有子弹就停下打，没有就跑
	else if(Chassis_Mode==Chassis_Up_Find_Target)
	{
		
		if(UP_Heat_Allow_Flag&&(number_1<=490)&&Up_NUC_Data_TypeStruct.NUC_Shoot_Allow_Flag)
		{	
			
		  Chassis_Run(0);
			
		}
		//否则就跑
		else
		{
		  Swing_Now_Section();
		}	
	}	
	
  //受到小弹丸打击，普通
	else if(Chassis_Mode==Chassis_Get_Small_Hurt)
	{
		 Swing_Now_Section();
	}	
	
	
	//无人机起飞跑
	else if(Chassis_Mode==Chassis_Plane_Fly)
	{
		Chassis_Swing(0,4,270);
	}
	//刚停了三秒，则在轨道上分段游走
	else if(Chassis_Mode==Chassis_Stop_3S)
	{
		Chassis_Swing(0,4,270);
	}
	
	
	//有很多敌人，
	else if(Chassis_Mode==Chassis_Many_Enemy)
	{
		Chassis_Swing(0,4,270);
		
	}
  //正常模式，开启巡航
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
 * @Function : 底盘自动控制升级
 * @Input    : 
 * @Output   : 
 * @Notes    : 首先在比赛开始之前待在轨道右侧，开始之后30s待在右侧打击前哨站，之后巡航
 * @Copyright: Aword
*/
char First_Start_Flag=0;
u32 Start_Time=0;
void Chassis_Auto_Control_Advance(void)
{
	//比赛没开始，待在轨道右侧,测试时用手挡一下
	if(Game_State.game_progress!=4)
	{
	  Run_To_Position(4,220); 
	
	}
	//比赛开始，
	else
	{
		//记录时间
		if(First_Start_Flag==0)
		{
	  	Start_Time=TimeTick1ms;
			First_Start_Flag=1;
			
		}
	  //开始之后20秒，不动
		if(TimeTick1ms<=Start_Time+20000)
		{
		
			Chassis_Run(0);
		}
		//三十秒之后，巡航
		else
	  {
	
			
				
		  Chassis_Swing(0,4,270);	
		}
	
	
	}
	
	
	
	
	
		
	
}
/**/




/**
  * @brief  底盘控制任务函数
  * @param  void
  * @retval void
  * @notes  优先级为 2，每2ms执行一次	
  */
void Chassis_Control_Task(void *pvParameters)
{
	Sensor_Init();//传感器初始化
	PID_Init_CHASSIS();//底盘PID初始化
  
	//Random_Num_Generator_Init();//随机数初始化
	
	
	
	//防止遥控器发出奇怪的数据使得底盘暴走
	Chassis_Motor_201.Target_Speed=0;
	CAN1_TX_Chassis();
	
   while(1)
	{

		 //底盘手动控制，右侧拨杆在上或者下
		if(DBUS.RC.Switch_Right==RC_SW_UP)
		{
			//只有极限位置限制没有功率限制
			Chassis_Control();
		}
		else if( DBUS.RC.Switch_Right==RC_SW_DOWN)
		{
			
			Chassis_Control();
		}	
		//底盘自动控制，右侧拨杆在中间
		else if(DBUS.RC.Switch_Right==RC_SW_MID)
		{ 
		
//			if(My_Usart_Recieve_Data==0)
//			{
//			 //手动控制
		//	Chassis_Control();
//			}
//			else if (My_Usart_Recieve_Data==1)
//			{
//			  //在轨道小范围摇晃，第一个参数是位置，从左到右坐标：0-276
			// Chassis_Swing(0,4,Swag_Speed);
       //Chassis_Swing_Short_NumLimit(100,Swag_Speed);
//			}
//		  else if (My_Usart_Recieve_Data==2)
//			{  
//			  //全段游荡
		    Swing_Now_Section();
//			}
//      else
//			{  
//			 //加入前哨站的模式
//			 Chassis_Auto_Normal();
//			}
				
		}
		 vTaskDelay(2);
	}
}













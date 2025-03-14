#include "main.h"
/*************************数据定义区**********************/
PTZ_TypeDef 		PTZ_Init_Value;            //云台初始信息结构体对象     
enum PTZ_MODE   PTZ_Mode=PTZ_Lost_Enemy;

int Lost_Angle=0;

//云台控制变量
int 			pitch=0;
int 		  yaw=0;






//PITCH轴的位置限定，上云台向上数值减小
short PITCH_UP=2200;//615
short PITCH_DOWN = 2660;  //
short PITCH_HIT = 2400 ;


#define YAW_MOVE_LIMIT  2000
//打前哨站，向右，向上减少
#define YAW_HIT    6472


/*************************函数定义区**********************/
/**
  * @brief  云台初始化函数
  * @param  void
  * @retval void
  * @notes  首先对云台进行防越界处理，然后控制云台回正
  */
void PTZ_Init(void)
{
	
	 PTZ_Angle_Value_Init();
   vTaskDelay(3);
	 PTZ_Angle_Init();	
   vTaskDelay(3);
	
}

/**
  * @brief  云台防越界函数
  * @param  void
  * @retval void
  * @notes  设置云台初始位置，对云台进行防越界处理，将跳变点移到回正点背后，
  */
void PTZ_Angle_Value_Init(void)
{

	PTZ_Init_Value.Initial_Pitch_Angle =PITCH_HIT;//；Flash_Array[0];
	PTZ_Init_Value.Initial_Yaw_Angle   =6120;//182

  
//	//Pitch 角度初始化
//	if ( (PTZ_Init_Value.Initial_Pitch_Angle+4000)>8191 )    
//	{   
//		PTZ_Init_Value.Critical_Pitch_Mechanical_Angle = PTZ_Init_Value.Initial_Pitch_Angle +4000-8192;
//	}
//	else  
//	{   
//		PTZ_Init_Value.Critical_Pitch_Mechanical_Angle = PTZ_Init_Value.Initial_Pitch_Angle +4000;
//	}
//	
//	if (PTZ_Init_Value.Initial_Pitch_Angle < PTZ_Init_Value.Critical_Pitch_Mechanical_Angle)  
//	{
//		PTZ_Init_Value.Initial_Pitch_Angle +=8192;
//	}
	
}

/**
  * @brief  云台回正
  * @param  void
  * @retval void
  * @notes  使用双环控制，在2000内将云台回正到初始位置，然后将用于控制的两个变量
            pitch和yaw设置为初始位置对应的数据
  */
void PTZ_Angle_Init(void)
{
	int 				   Current_Pitch_Angle,
		             Current_Yaw_Angle; 
	short					 Angle_Init_Num;

  //用于云台回正
	Current_Pitch_Angle =Pitch_Motor_206.Real_MechanicalAngle ;
	Current_Yaw_Angle   =Yaw_Real_MechanicalAngle;

	
	//云台回正操作，缓慢将云台回正到初始设定位置
  for (Angle_Init_Num=0;Angle_Init_Num<1000;Angle_Init_Num++)
  {	

		//YAW轴回正，每2ms变化一点，逐渐回到初始位置 
	 if ( Current_Yaw_Angle < (PTZ_Init_Value.Initial_Yaw_Angle -6))       Current_Yaw_Angle +=7;
	 if ( Current_Yaw_Angle > (PTZ_Init_Value.Initial_Yaw_Angle +6))       Current_Yaw_Angle -=7;
	 //角度位置环，Current_Yaw_Angle与电机反馈的角度比较
	 PID_Yaw_205_Mechanical.PIDout = (Pid_Calc(& PID_Yaw_205_Mechanical_1,Yaw_Real_MechanicalAngle,Current_Yaw_Angle));
	 //速度环，PID_Yaw_205_Mechanical.PIDout与电机反馈的真实速度比较
	 Yaw_Motor_205.Target_Speed = (Pid_Calc(&PID_Yaw_205_Speed_1,Yaw_Motor_205.Real_Speed,PID_Yaw_205_Mechanical.PIDout ));
		
		
		
	//PITCH轴回正，每2ms变化一点，逐渐回到初始位置
	 if ( Current_Pitch_Angle < (PTZ_Init_Value.Initial_Pitch_Angle -6))       Current_Pitch_Angle +=7;
	 if ( Current_Pitch_Angle > (PTZ_Init_Value.Initial_Pitch_Angle +6))       Current_Pitch_Angle -=7;
	



		//角度位置环，Current_Pitch_Angle与电机反馈的角度比较
	 PID_Pitch_206_Mechanical.PIDout = (Pid_Calc(& PID_Pitch_206_Mechanical_1,Pitch_Motor_206.Real_MechanicalAngle,Current_Pitch_Angle));
	
	//速度环，PID_Pitch_206_Mechanical.PIDout与电机反馈的真实速度比较
	 Pitch_Motor_206.Target_Speed = (Pid_Calc(&PID_Pitch_206_Speed_1,Pitch_Motor_206.Real_Speed,PID_Pitch_206_Mechanical.PIDout));
  
 

	 CAN1_TX_PTZ();
   vTaskDelay(2);
  }
	
  //yaw控制初始值设置
	yaw=Yaw_Real_MechanicalAngle;
	//pitch控制初始值设置，使用设置的初始值经过防越界处理之后的数据
	pitch=PTZ_Init_Value.Initial_Pitch_Angle;
}


/**
  * @brief  云台遥控器手动控制
  * @param  void
  * @retval void
  * @notes  直接使用遥控器的控制数据，遥控器原始数据范围为-600~600，
            ch2和ch3是增量，分别在yaw和pitch的基础上加或者减
            ch2控制yaw轴，数据范围为-0.2~0.2，从上向下看，正值顺时钟转动，负值逆时针转动，
            ch3控制pitch轴，数据范围-7~7，由于pitch轴抬头时电机的反馈角度值比低头角度值小
                            所以ch3正值时云台低头，负值时云台抬头
  */
void PTZ_Control_Hand(void)                                       
{
	//模拟遥控器
	//因为每次进入函数时ch都会被重新赋值，所以是否静态并无影响
	static float   ch2=0;
	static float   ch3=0;
	
	//如果右侧拨杆，拨到最下方或者中间，获取遥控器信息
	if(DBUS.RC.Switch_Right == RC_SW_DOWN||DBUS.RC.Switch_Right == RC_SW_MID)
	{
		ch2 = (float)DBUS.RC.ch2/80;     //-7~7
		ch3 = (float)DBUS.RC.ch3/100;            //-7~7
	}
	//如果右侧拨杆，拨到最上方或者没有收到遥控器信息，保持不动
	else
	{
	   ch2=0;
		 ch3=0;
	}
	
	yaw   += ch2; 
	pitch -= ch3;	
	
	//yaw限位，yaw轴使用的是圈数累加编码值计算，不需要转移跳变点，所以可以使用原始的编码器值
	if(yaw<PTZ_Init_Value.Initial_Yaw_Angle-YAW_MOVE_LIMIT)yaw=PTZ_Init_Value.Initial_Yaw_Angle-YAW_MOVE_LIMIT; 
	if(yaw>PTZ_Init_Value.Initial_Yaw_Angle+YAW_MOVE_LIMIT)yaw=PTZ_Init_Value.Initial_Yaw_Angle+YAW_MOVE_LIMIT;

	
	//pitch限位，由于PITCH_UP的值比PITCH_DOWN的值小，
	//所以与PITCH_UP比较时要用小于，与PITCH_DOWN比较时要用大于
	if(pitch<PITCH_UP)pitch=PITCH_UP; 
	if(pitch>PITCH_DOWN)pitch=PITCH_DOWN;
	
	
	//YAW轴使用的角度和角速度都是陀螺仪数据
  //yaw角度位置环，Yaw_Real_MechanicalAngle与yaw比较	
	PID_Yaw_205_Mechanical.PIDout = (Pid_Calc(& PID_Yaw_205_Mechanical,Yaw_Real_MechanicalAngle,yaw));
	//yaw速度环，Yaw_Motor_205.Real_Speed与PID_Yaw_205_Mechanical.PIDout比较
	//PID计算输入的参数和返回值都是浮点类型，电机can通讯结构体对象成员是shout类型
	Yaw_Motor_205.Target_Speed = (Pid_Calc(&PID_Yaw_205_Speed,Yaw_Motor_205.Real_Speed,PID_Yaw_205_Mechanical.PIDout));

	//PITCH轴使用的角度和角速度是电机反馈的角度和速度
  PID_Pitch_206_Mechanical.PIDout = (Pid_Calc(& PID_Pitch_206_Mechanical , Pitch_Motor_206.Real_MechanicalAngle , pitch));
	Pitch_Motor_206.Target_Speed = (Pid_Calc(&PID_Pitch_206_Speed , Pitch_Motor_206.Real_Speed ,PID_Pitch_206_Mechanical.PIDout ));

}
//

/**
  * @brief  云台NUC控制
  * @param  void
  * @retval void
  * @notes  使用视觉的数据进行控制
  */
float Yaw_Omiga_Last=0;
void PTZ_Auto_No_Limit(void)                                                        //自动控制  找到目标
{ 	
	float   ch2=0;
	float   ch3=0;

	//直接来自NUC数据，代表与当前角度的差值,视觉数据short64555,-11~11,*400,NUC-4400~4400,
	ch2=(float)Up_NUC_Data_TypeStruct.Yaw_Angle;//ch2的范围应是-0.2~0.2

	
  //直接来自NUC数据，代表与当前角度的差值,视觉数据short64555,-7~7,*600,NUC-4200~4200,
	ch3=(float)Up_NUC_Data_TypeStruct.Pitch_Angle;           //ch3的范围应是-600~600，正值往下移

	
//	if(fabs(ch2)>=1)//防抖
//	{
		yaw=Yaw_Real_MechanicalAngle-ch2;
		//将NUC发过来的数据清零
		//测试
		//Up_NUC_Data_TypeStruct.Yaw_Angle=0;
//	}
//	if(fabs(ch3)>=1)//防抖
//	{
		pitch	=Pitch_Motor_206.Real_MechanicalAngle+ch3;
		//将NUC发过来的数据清零
		
		//Up_NUC_Data_TypeStruct.Pitch_Angle=0;
		
//	}
	
	//yaw限位，yaw轴使用的是圈数累加编码值计算，不需要转移跳变点，所以可以使用原始的编码器值
	if(yaw<PTZ_Init_Value.Initial_Yaw_Angle-YAW_MOVE_LIMIT)yaw=PTZ_Init_Value.Initial_Yaw_Angle-YAW_MOVE_LIMIT; 
	if(yaw>PTZ_Init_Value.Initial_Yaw_Angle+YAW_MOVE_LIMIT)yaw=PTZ_Init_Value.Initial_Yaw_Angle+YAW_MOVE_LIMIT;

	//pitch限位，由于PITCH_UP的值比PITCH_DOWN的值小，
	//所以与PITCH_UP比较时要用小于，与PITCH_DOWN比较时要用大于
	if(pitch<PITCH_UP)pitch=PITCH_UP; 
	if(pitch>PITCH_DOWN)pitch=PITCH_DOWN;

	
	
	//在界内
//	if(Swing_Flag==1)
//	{
//		PID_Armor_Yaw_205_Mechanical.P=0.15;
//	
//	}
//	else
//	{
//		PID_Armor_Yaw_205_Mechanical.P=0.1;
//	}
	
	
	
	
  //YAW轴使用的角度和角速度都是陀螺仪数据
  //yaw角度位置环，Yaw_Real_MechanicalAngle与yaw比较	
	PID_Yaw_205_Mechanical.PIDout = (Pid_Calc(& PID_Armor_Yaw_205_Mechanical,Yaw_Real_MechanicalAngle,yaw));
	
//	if(fabs(Yaw_Omiga_Last-PID_Yaw_205_Mechanical.PIDout)>10)PID_Yaw_205_Mechanical.PIDout=(PID_Yaw_205_Mechanical.PIDout>Yaw_Omiga_Last)?(Yaw_Omiga_Last+10):(Yaw_Omiga_Last-10);
//	else Yaw_Omiga_Last=PID_Yaw_205_Mechanical.PIDout;
		
	
	Yaw_Motor_205.Target_Speed = (Pid_Calc(&PID_Armor_Yaw_205_Speed,Yaw_Motor_205.Real_Speed, PID_Yaw_205_Mechanical.PIDout));
	
	
	PID_Pitch_206_Mechanical.PIDout = (Pid_Calc(& PID_Armor_Pitch_206_Mechanical , Pitch_Motor_206.Real_MechanicalAngle , pitch));
	Pitch_Motor_206.Target_Speed = (Pid_Calc(&PID_Armor_Pitch_206_Speed , Pitch_Motor_206.Real_Speed ,PID_Pitch_206_Mechanical.PIDout  ));

}
/**
  * @brief  云台控制任务函数
  * @param  void
  * @retval void
  * @notes  对CAN1,PID,陀螺仪，云台进行初始化，初始化之后蜂鸣器响
            优先级为3，每1ms运行一次，控制时用的是增量模式，每1ms的时间变化很小的值
            由于云台回正操作需要用到CAN1的数据以及对电机进行操作所以CAN以及
            PID应该在云台之前被初始化
  */

void PTZ_Control_Task(void *pvParameters)
{

	//const TickType_t TimeIncrement=pdMS_TO_TICKS(1);
	//TickType_t	PreviousWakeTime;
	//PreviousWakeTime=xTaskGetTickCount();	
	
	CAN1_Init();      //CAN1通讯初始化
	vTaskDelay(500); //阻塞
	PID_Init_PTZ();   //PID初始化
	PTZ_Init();       //云台回正，初始化yaw=Yaw_Real_MechanicalAngle和pitch  
	Beep_ON();
	delay_ms(500);
	Beep_OFF();
 
	
	while(1)
	{
		
		
		
		if(DBUS.RC.Switch_Right == RC_SW_UP)//如果右侧拨杆，拨到最上方，上云台手动控制（不可控）
		{
			PTZ_Control_Hand();	
		}
		else if(DBUS.RC.Switch_Right == RC_SW_MID)//如果右侧拨杆，拨到最中间
		{
			

		
				if(PTZ_Mode==PTZ_Find_Enemy)//发现敌人自动控制
				{
					if(Chassis_Mode>Chassis_Up_Find_Target)
					{
					  Chassis_Mode=Chassis_Up_Find_Target;
					  Mode_Time=0;
					}
					
					PTZ_Auto_No_Limit();	
				}
				else//测试，如果视觉没识别到敌人则摇摆
				{	
					//测试
					//PTZ_Control_Hand();	
					Y_Swing(Yaw_Real_MechanicalAngle+1000,Yaw_Real_MechanicalAngle-1000,2);
					P_Swing(PITCH_UP+50,PITCH_DOWN-50,3);
				}	



	
			
	  }
		else//如果右侧拨杆，拨到最下方或者没有收到遥控器信息，上云台手动控制（可控）
		{
			PTZ_Control_Hand();	
		}
		//向云台发送数据
		CAN1_TX_PTZ();  
		vTaskDelay(1);
		//vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}
}
/**
  * @brief  PITCH轴点头函数
  * @param  Up_Limite：点头的上限，一般比PITCH_UP要小一些
            Down_Limite：点头的下角度限制，一般比PITCH_DOWN大一些
            Speed：点头的速度，每毫秒移动的编码器值，一般为22，
                   云台可移动的范围大小是5600，即大约2.5S可以从
                   最下移动到最上方。
  * @retval void
  * @notes  要改变云台pitch轴的点头速度只需要改变Speed的值就可以了
  */
void P_Swing(int Up_Limite,int Down_Limite,int Speed)
{
	//增量，相当于手动控制的ch3，当speed的值为正时云台向上移动，值为负时云台向下移动
	static int speed=6;
	
	//若到达上转换点，则增量递减为负，pitch变为增加，云台向下运动
	if(Pitch_Motor_206.Real_MechanicalAngle<=Up_Limite) 
	{
		speed-=1;
	}
	//若到达下转换点，则增量递增为正，pitch变为减小，云台向上运动
	else if(Pitch_Motor_206.Real_MechanicalAngle>=Down_Limite) 
	{
		speed+=1;
	}
	//限制到达转换点是变速的范围，防止猛点头
	if(speed<-Speed)     speed=-Speed;
	else if(speed>Speed) speed=Speed;
	
	//pitch减去增量
	pitch -= speed;
	//限位
//	if(pitch<PITCH_UP)pitch=PITCH_UP; 
//	if(pitch>PITCH_DOWN)pitch=PITCH_DOWN;
	
	PID_Pitch_206_Mechanical.PIDout = (Pid_Calc(& PID_Pitch_206_Mechanical , Pitch_Motor_206.Real_MechanicalAngle , pitch));
	Pitch_Motor_206.Target_Speed = (Pid_Calc(&PID_Pitch_206_Speed , Pitch_Motor_206.Real_Speed ,PID_Pitch_206_Mechanical.PIDout ));

}

/**
  * @brief  上云台YAW轴摇头函数
  * @param  Speed：云台旋转的速度，范围为-0.2~0.2
  * @retval void
  * @notes  左侧值大，右侧值小
  */
void Y_Swing(int Left_Limite,int Right_Limite,float Speed)
{
	//增量，相当于手动控制的ch3，当speed的值为正时云台向上移动，值为负时云台向下移动
	static int speed=6;
	
	//若到达左转换点，则增量递减为负，yaw变为减小，云台向右移动
	if(Yaw_Real_MechanicalAngle>=PTZ_Init_Value.Initial_Yaw_Angle+YAW_MOVE_LIMIT) 
	{
		speed-=1;
	}
	//若到达右转换点，则增量递增为正，yaw变为增加，云台向左运动
	else if(Yaw_Real_MechanicalAngle<=PTZ_Init_Value.Initial_Yaw_Angle-YAW_MOVE_LIMIT) 
	{
		speed+=1;
	}
	
	//限制到达转换点是变速的范围，防止摇头
	if(speed<-Speed)     speed=-Speed;
	else if(speed>Speed) speed=Speed;
	
	//yaw加上增量
   yaw += speed;
	
	PID_Yaw_205_Mechanical.PIDout = (Pid_Calc(& PID_Yaw_205_Mechanical,Yaw_Real_MechanicalAngle,yaw));
	Yaw_Motor_205.Target_Speed = (Pid_Calc(&PID_Yaw_205_Speed,Yaw_Motor_205.Real_Speed, PID_Yaw_205_Mechanical.PIDout));
	
}



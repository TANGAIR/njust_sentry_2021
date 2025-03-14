#include "main.h"
/*************************数据定义区**********************/
PTZ_TypeDef 		PTZ_Init_Value;            //云台初始信息结构体对象     
enum PTZ_MODE   PTZ_Mode=PTZ_Lost_Enemy;

int Lost_Angle=0;



//云台控制变量
int 			pitch=0;
float 		yaw=0;
//PITCH轴的位置限定
int PITCH_UP    = 4600;  //1439
int PITCH_DOWN  = 3600;
/*************************函数定义区**********************/
/**
  * @brief  云台YAW初始化回正
  * @param  void
  * @retval void
  * @notes  设置云台初始位置，使用双环控制，在2000内将云台回正到初始位置，然后将用于控制的两个变量
            pitch和yaw设置为初始位置对应的数据
  */
void PTZ_Init_Yaw(void)
{
	int 				   Current_Yaw_Angle=0;
		              
	short					 Angle_Init_Num=0;
	

	
	 PTZ_Init_Value.Initial_Yaw_Angle   =6672;//182
	
	 vTaskDelay(200);
	
	if(Yaw_Motor_205.Real_MechanicalAngle<=2200)
	{
	 PTZ_Init_Value.Initial_Yaw_Angle  -=8192;//182
	}
  
	
	
	Current_Yaw_Angle   =   Yaw_Real_MechanicalAngle;
	
 
	
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


	 CAN1_TX_PTZ();
   vTaskDelay(2);
	 
  }
	
  //yaw控制初始值设置，使用下主板板载陀螺仪数据
	yaw=Yaw_Send_NUC_Angle_IMU;

}
/**
  * @brief  云台PITCH初始化回正
  * @param  void
  * @retval void
  * @notes  设置云台初始位置，使用双环控制，在2000内将云台回正到初始位置，然后将用于控制的两个变量
            pitch和yaw设置为初始位置对应的数据
  */
void PTZ_Init_PITCH(void)
{
	int 				   Current_Pitch_Angle;
	
	short					 Angle_Init_Num;
	
	
	//设置初始回正位置
	PTZ_Init_Value.Initial_Pitch_Angle =4000;//；Flash_Array[0];
	
	

  //用于云台回正
	Current_Pitch_Angle =Pitch_Real_MechanicalAngle;

	

	//云台回正操作，缓慢将云台回正到初始设定位置
  for (Angle_Init_Num=0;Angle_Init_Num<1000;Angle_Init_Num++)
  {	
		
		
		
	//PITCH轴回正，每2ms变化一点，逐渐回到初始位置
	 if ( Current_Pitch_Angle < (PTZ_Init_Value.Initial_Pitch_Angle -6))       Current_Pitch_Angle +=7;
	 if ( Current_Pitch_Angle > (PTZ_Init_Value.Initial_Pitch_Angle +6))       Current_Pitch_Angle -=7;
	 //角度位置环，Current_Pitch_Angle与电机反馈的角度比较
	 PID_Pitch_206_Mechanical.PIDout = (Pid_Calc(& PID_Pitch_206_Mechanical_1,Pitch_Real_MechanicalAngle,Current_Pitch_Angle));
	 //速度环，PID_Pitch_206_Mechanical.PIDout与电机反馈的真实速度比较
	 Pitch_Motor_206.Target_Speed = (Pid_Calc(&PID_Pitch_206_Speed_1,Pitch_Motor_206.Real_Speed,PID_Pitch_206_Mechanical.PIDout));
  


	 CAN1_TX_PTZ();
   vTaskDelay(2);
	 
  }
	
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
	
	//如果右侧拨杆，拨到最上下方或者中间，获取遥控器信息
	if(DBUS.RC.Switch_Right == RC_SW_UP||DBUS.RC.Switch_Right == RC_SW_MID)
	{
		ch2 = (float)DBUS.RC.ch2/3000;     //-0.2~0.2
		ch3 = DBUS.RC.ch3/80;            //-7~7
	}
	//如果右侧拨杆，拨到最下方或者没有收到遥控器信息，保持不动
	else
	{
	   ch2=0;
		 ch3=0;
	}
	
	yaw   += ch2; 
	pitch += ch3;
	
	
	if(pitch>PITCH_UP)pitch=PITCH_UP; 
	if(pitch<PITCH_DOWN)pitch=PITCH_DOWN;
	
	
	
	//YAW轴使用的角度和角速度都是陀螺仪数据
  //yaw角度位置环，Yaw_Send_NUC_Angle_IMU与yaw比较	
	PID_Yaw_205_Mechanical.PIDout = -(Pid_Calc(& PID_Yaw_205_Mechanical,Yaw_Send_NUC_Angle_IMU,yaw));
	//yaw速度环，IMU_Real_Data.Gyro_Y与PID_Yaw_205_Mechanical.PIDout比较
	//PID计算输入的参数和返回值都是浮点类型，电机can通讯结构体对象成员是shout类型
	Yaw_Motor_205.Target_Speed = (Pid_Calc(&PID_Yaw_205_Speed,-IMU_Real_Data.Gyro_Y, PID_Yaw_205_Mechanical.PIDout));


	//PITCH轴使用的角度和角速度是电机反馈的角度和速度
  PID_Pitch_206_Mechanical.PIDout = (Pid_Calc(& PID_Pitch_206_Mechanical , Pitch_Real_MechanicalAngle , pitch));
	
	Pitch_Motor_206.Target_Speed =    (Pid_Calc(&PID_Pitch_206_Speed , Pitch_Motor_206.Real_Speed ,PID_Pitch_206_Mechanical.PIDout ));

}


/**
  * @brief  云台NUC控制
  * @param  void
  * @retval void
  * @notes  使用视觉的数据进行控制
  */
void PTZ_Auto_No_Limit(void)                                                        //自动控制  找到目标
{ 	
	float   ch2=0;
	float   ch3=0;

  //直接来自NUC数据，代表与当前角度的差值,视觉数据short64555,-11~11,*400,NUC-4400~4400,
	ch2=((float)Down_NUC_Data_TypeStruct.Yaw_Angle)/100;//ch2的范围应是-0.2~0.2
  //直接来自NUC数据，代表与当前角度的差值,视觉数据short64555,-7~7,*600,NUC-4200~4200,
	ch3=Down_NUC_Data_TypeStruct.Pitch_Angle/4;           //ch3的范围应是-600~600，正值往下移
	
//	if(fabs(ch2)>=0.05f)//防抖
//	{
		yaw=Yaw_Send_NUC_Angle_IMU-ch2;	
		//将NUC发过来的数据清零
		//Down_NUC_Data_TypeStruct.Yaw_Angle=0;
	//}
//	if(fabs(ch3)>=0.5)//防抖
//	{
		pitch	=Pitch_Real_MechanicalAngle-ch3;
		//将NUC发过来的数据清零
		//Down_NUC_Data_TypeStruct.Pitch_Angle=0;
//	}
	
  if(pitch>PITCH_UP)pitch=PITCH_UP; 
	if(pitch<PITCH_DOWN)pitch=PITCH_DOWN;
	

	
  //YAW轴使用的角度和角速度都是陀螺仪数据
  //yaw角度位置环，Yaw_Send_NUC_Angle_IMU与yaw比较	
	PID_Yaw_205_Mechanical.PIDout = -(Pid_Calc(& PID_Armor_Yaw_205_Mechanical,Yaw_Send_NUC_Angle_IMU,yaw));
	
	Yaw_Motor_205.Target_Speed = (Pid_Calc(&PID_Armor_Yaw_205_Speed,-IMU_Real_Data.Gyro_Y, PID_Yaw_205_Mechanical.PIDout));
	
	
	PID_Pitch_206_Mechanical.PIDout = (Pid_Calc(& PID_Armor_Pitch_206_Mechanical , Pitch_Real_MechanicalAngle , pitch));
	
	Pitch_Motor_206.Target_Speed = (Pid_Calc(&PID_Armor_Pitch_206_Speed , Pitch_Motor_206.Real_Speed ,PID_Pitch_206_Mechanical.PIDout  ));

}
/**
  * @brief  云台控制任务函数
  * @param  void
  * @retval void
  * @notes  对CAN1,PID,陀螺仪，云台进行初始化，初始化之后蜂鸣器响
            优先级为3，每1ms运行一次，控制时用的是增量模式，每1ms的时间变化很小的值
            由于云台回正操作需要用到陀螺仪的数据以及对电机进行操作所以陀螺仪和CAN以及
            PID应该在云台之前被初始化
  */
void PTZ_Control_Task(void *pvParameters)
{

	CAN1_Init();      //CAN1通讯初始化
	vTaskDelay(300);  //阻塞
	PID_Init();       //PID初始化
	PTZ_Init_Yaw();
	IMU_Init();       //陀螺仪初始化，开启陀螺仪数据获取中断，获取陀螺仪的数据
	vTaskDelay(100);	//阻塞
	PTZ_Init_PITCH();
	
//	Beep_ON();
//	delay_ms(300);
//	Beep_OFF();

	
	while(1)
	{
		if(DBUS.RC.Switch_Right == RC_SW_UP)//如果右侧拨杆，拨到最上方，下云台手动控制（可控）
		{
			PTZ_Control_Hand();	
		}
		else if(DBUS.RC.Switch_Right == RC_SW_MID)//如果右侧拨杆，拨到最中间
		{
			if(PTZ_Mode==PTZ_Find_Enemy)//发现敌人自动控制
			{
				PTZ_Auto_No_Limit();	
			}
			else//如果视觉没识别到敌人则下云台摆动
			{	
				//PTZ_Control_Hand();
				Y_Swing(0.08f);
				P_Swing(PITCH_UP-100,PITCH_DOWN+100,4);
			}

	  }
		else//如果右侧拨杆，拨到最下方或者没有遥控器信息，下云台手动控制（不可控）
		{
			PTZ_Control_Hand();		
		}
		//向云台发送数据
		CAN1_TX_PTZ();
		vTaskDelay(1);
	}
}
/**
  * @brief  PITCH轴点头函数
  * @param  Up_Limite：点头的上限，一般比PITCH_UP要大一些
            Down_Limite：点头的下角度限制，一般比PITCH_DOWN小一些
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
	
	//若到达上转换点，则增量递增为正，pitch变为减小，云台向下运动
	if(Pitch_Real_MechanicalAngle>=Up_Limite) 
	{
		speed+=1;
	}
	//若到达上转换点，，则增量递减为负，pitch变为增加云台向上运动
	else if(Pitch_Real_MechanicalAngle<=Down_Limite) 
	{
		speed-=1;
	}
	//限制到达转换点是变速的范围，防止猛点头
	if(speed<-Speed)     speed=-Speed;
	else if(speed>Speed) speed=Speed;
	
	//pitch加上增量
	pitch -= speed;
	
	if(pitch>PITCH_UP)pitch=PITCH_UP; 
	if(pitch<PITCH_DOWN)pitch=PITCH_DOWN;
	
	PID_Pitch_206_Mechanical.PIDout = (Pid_Calc(& PID_Pitch_206_Mechanical ,Pitch_Real_MechanicalAngle , pitch));
	Pitch_Motor_206.Target_Speed =    (Pid_Calc(&PID_Pitch_206_Speed , Pitch_Motor_206.Real_Speed ,PID_Pitch_206_Mechanical.PIDout));

}

/**
  * @brief  YAW轴旋转函数
  * @param  Speed：云台旋转的速度，范围为-0.2~0.2
  * @retval void
  * @notes  要改变云台yaw轴的点头速度只需要改变Speed的值就可以了
  */

void Y_Swing(float Speed)
{
	yaw+=Speed;
	PID_Yaw_205_Mechanical.PIDout = -(Pid_Calc(& PID_Yaw_205_Mechanical,Yaw_Send_NUC_Angle_IMU,yaw));
	Yaw_Motor_205.Target_Speed = (Pid_Calc(&PID_Yaw_205_Speed,-IMU_Real_Data.Gyro_Y, PID_Yaw_205_Mechanical.PIDout));
}



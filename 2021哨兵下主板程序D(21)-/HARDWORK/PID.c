#include "main.h"

PID_TypeDef PID_Yaw_205_Mechanical,
						PID_Yaw_205_Speed,
						PID_Pitch_206_Mechanical,
						PID_Pitch_206_Speed,
						PID_Pitch_206_Mechanical_1,
						PID_Pitch_206_Speed_1,
						PID_Yaw_205_Mechanical_1,
						PID_Yaw_205_Speed_1;

PID_TypeDef PID_Armor_Yaw_205_Mechanical,
						PID_Armor_Yaw_205_Speed,
						PID_Armor_Pitch_206_Mechanical,
						PID_Armor_Pitch_206_Speed;
						
PID_TypeDef PID_Friction_Wheel_203,
            PID_Friction_Wheel_204;
						
						

void PID_Init(void)
{                    
	
/*******************回正***************************/	
	{
		/******云台回正*********/
	PID_Yaw_205_Mechanical_1.P=1;
	PID_Yaw_205_Mechanical_1.I=0;
	PID_Yaw_205_Mechanical_1.D=0;

	PID_Yaw_205_Speed_1.P=30;
	PID_Yaw_205_Speed_1.I=0;
	PID_Yaw_205_Speed_1.D=0;
		
	PID_Pitch_206_Mechanical_1.P=0.6;
	PID_Pitch_206_Mechanical_1.I=0;
	PID_Pitch_206_Mechanical_1.D=0;
	
	PID_Pitch_206_Speed_1.P=300;
	PID_Pitch_206_Speed_1.I=0;
	PID_Pitch_206_Speed_1.D=0;
			
	PID_Yaw_205_Mechanical_1.CurrentError=0;
	PID_Yaw_205_Mechanical_1.LastError=0;
	PID_Yaw_205_Mechanical_1.ErrorIgnored=0;
	PID_Yaw_205_Mechanical_1.Pout=0;
	PID_Yaw_205_Mechanical_1.Iout=0;
	PID_Yaw_205_Mechanical_1.Dout=0;
	PID_Yaw_205_Mechanical_1.PIDout=0;
	PID_Yaw_205_Mechanical_1.PIDOutCompensate=0;
	PID_Yaw_205_Mechanical_1.PMax=2000;
	PID_Yaw_205_Mechanical_1.IMax=1000;
	PID_Yaw_205_Mechanical_1.DMax=1000;
	PID_Yaw_205_Mechanical_1.PIDOutMax=2000;
	PID_Yaw_205_Mechanical_1.PIDOutLast=0;
	PID_Yaw_205_Mechanical_1.Target_Speed_Last=0;
	PID_Yaw_205_Mechanical_1.Speed_Ratio=1;
	PID_Yaw_205_Mechanical_1.Acceleration=0;
	PID_Yaw_205_Mechanical_1.Err_Change=0;
	PID_Yaw_205_Mechanical_1.Ke=0;
	PID_Yaw_205_Mechanical_1.Kec=0;
	PID_Yaw_205_Mechanical_1.Ku_D=0;
	PID_Yaw_205_Mechanical_1.Ku_I=0;
	PID_Yaw_205_Mechanical_1.Ku_P=0;
	
	PID_Yaw_205_Speed_1.CurrentError=0;
	PID_Yaw_205_Speed_1.LastError=0;
	PID_Yaw_205_Speed_1.ErrorIgnored=0;
	PID_Yaw_205_Speed_1.Pout=0;
	PID_Yaw_205_Speed_1.Iout=0;
	PID_Yaw_205_Speed_1.Dout=0;
	PID_Yaw_205_Speed_1.PIDout=0;
	PID_Yaw_205_Speed_1.PIDOutCompensate=0;
	PID_Yaw_205_Speed_1.PMax=20000;
	PID_Yaw_205_Speed_1.IMax=4000;
	PID_Yaw_205_Speed_1.DMax=20000;
	PID_Yaw_205_Speed_1.PIDOutMax=20000;
	PID_Yaw_205_Speed_1.PIDOutLast=0;
	PID_Yaw_205_Speed_1.Target_Speed_Last=0;
	PID_Yaw_205_Speed_1.Speed_Ratio=1;
	PID_Yaw_205_Speed_1.Acceleration=0;
	PID_Yaw_205_Speed_1.Ke=0;
	PID_Yaw_205_Speed_1.Kec=0;
	PID_Yaw_205_Speed_1.Ku_D=0;
	PID_Yaw_205_Speed_1.Ku_I=0;
	PID_Yaw_205_Speed_1.Ku_P=0;
	
	
	PID_Pitch_206_Mechanical_1.CurrentError=0;
	PID_Pitch_206_Mechanical_1.LastError=0;
	PID_Pitch_206_Mechanical_1.ErrorIgnored=0;
	PID_Pitch_206_Mechanical_1.Pout=0;
	PID_Pitch_206_Mechanical_1.Iout=0;
	PID_Pitch_206_Mechanical_1.Dout=0;
	PID_Pitch_206_Mechanical_1.PIDout=0;
	PID_Pitch_206_Mechanical_1.PIDOutCompensate=0;
	PID_Pitch_206_Mechanical_1.PMax=2000;
	PID_Pitch_206_Mechanical_1.IMax=500;
	PID_Pitch_206_Mechanical_1.DMax=500;
	PID_Pitch_206_Mechanical_1.PIDOutMax=2000;
	PID_Pitch_206_Mechanical_1.PIDOutLast=0;
	PID_Pitch_206_Mechanical_1.Target_Speed_Last=0;
	PID_Pitch_206_Mechanical_1.Speed_Ratio=1;
	PID_Pitch_206_Mechanical_1.Acceleration=0;
	
	PID_Pitch_206_Speed_1.CurrentError=0;
	PID_Pitch_206_Speed_1.LastError=0;
	PID_Pitch_206_Speed_1.ErrorIgnored=0;
	PID_Pitch_206_Speed_1.Pout=0;
	PID_Pitch_206_Speed_1.Iout=0;
	PID_Pitch_206_Speed_1.Dout=0;
	PID_Pitch_206_Speed_1.PIDout=0;
	PID_Pitch_206_Speed_1.PIDOutCompensate=0;
	PID_Pitch_206_Speed_1.PMax=15000;
	PID_Pitch_206_Speed_1.IMax=1000;
	PID_Pitch_206_Speed_1.DMax=1000;
	PID_Pitch_206_Speed_1.PIDOutMax=10000;
	PID_Pitch_206_Speed_1.PIDOutLast=0;
	PID_Pitch_206_Speed_1.Target_Speed_Last=0;
	PID_Pitch_206_Speed_1.Speed_Ratio=1;
	PID_Pitch_206_Speed_1.Acceleration=0;
	PID_Pitch_206_Speed_1.LastLastError=0;
  }
		
/*******************云台控制***********************/
	{/*0x205*/
	/******云台控制*********/
	/*0x205*/
	PID_Yaw_205_Mechanical.P=20;
	PID_Yaw_205_Mechanical.I=0;
	PID_Yaw_205_Mechanical.D=0;
	
	PID_Yaw_205_Mechanical.CurrentError=0;
	PID_Yaw_205_Mechanical.LastError=0;
	PID_Yaw_205_Mechanical.ErrorIgnored=0;
	PID_Yaw_205_Mechanical.Pout=0;
	PID_Yaw_205_Mechanical.Iout=0;
	PID_Yaw_205_Mechanical.Dout=0;
	PID_Yaw_205_Mechanical.PIDout=0;
	PID_Yaw_205_Mechanical.PIDOutCompensate=0;
	PID_Yaw_205_Mechanical.PMax=20000;
	PID_Yaw_205_Mechanical.IMax=5000;
	PID_Yaw_205_Mechanical.DMax=20000;
	PID_Yaw_205_Mechanical.PIDOutMax=20000;
	PID_Yaw_205_Mechanical.PIDOutLast=0;
	PID_Yaw_205_Mechanical.Target_Speed_Last=0;
	PID_Yaw_205_Mechanical.Speed_Ratio=1;
	PID_Yaw_205_Mechanical.Acceleration=0;
	PID_Yaw_205_Mechanical.Err_Change=0;
	PID_Yaw_205_Mechanical.Ke=0;
	PID_Yaw_205_Mechanical.Kec=0;
	PID_Yaw_205_Mechanical.Ku_D=0;
	PID_Yaw_205_Mechanical.Ku_I=0;
	PID_Yaw_205_Mechanical.Ku_P=0;
	
	
	PID_Yaw_205_Speed.P=175;
	PID_Yaw_205_Speed.I=0;
	PID_Yaw_205_Speed.D=0;
	
	
	PID_Yaw_205_Speed.CurrentError=0;
	PID_Yaw_205_Speed.LastError=0;
	PID_Yaw_205_Speed.ErrorIgnored=0;
	PID_Yaw_205_Speed.Pout=0;
	PID_Yaw_205_Speed.Iout=0;
	PID_Yaw_205_Speed.Dout=0;
	PID_Yaw_205_Speed.PIDout=0;
	PID_Yaw_205_Speed.PIDOutCompensate=0;
	PID_Yaw_205_Speed.PMax=30000;
	PID_Yaw_205_Speed.IMax=5000;
	PID_Yaw_205_Speed.DMax=20000;
	PID_Yaw_205_Speed.PIDOutMax=30000;
	PID_Yaw_205_Speed.PIDOutLast=0;
	PID_Yaw_205_Speed.Target_Speed_Last=0;
	PID_Yaw_205_Speed.Speed_Ratio=1;
	PID_Yaw_205_Speed.Acceleration=0;
	PID_Yaw_205_Speed.Ke=0;
	PID_Yaw_205_Speed.Kec=0;
	PID_Yaw_205_Speed.Ku_D=0;
	PID_Yaw_205_Speed.Ku_I=0;
	PID_Yaw_205_Speed.Ku_P=0;



  /*0x206*/                                      //跳变  跟随
	PID_Pitch_206_Mechanical.P=0.6;                //1.5   0.3
	PID_Pitch_206_Mechanical.I=0;                  //0
	PID_Pitch_206_Mechanical.D=2;                  //3


	/*0x206*/
	PID_Pitch_206_Mechanical.CurrentError=0;
	PID_Pitch_206_Mechanical.LastError=0;
	PID_Pitch_206_Mechanical.ErrorIgnored=0;      //2     0
	PID_Pitch_206_Mechanical.Pout=0;
	PID_Pitch_206_Mechanical.Iout=0;
	PID_Pitch_206_Mechanical.Dout=0;
	PID_Pitch_206_Mechanical.PIDout=0;
	PID_Pitch_206_Mechanical.PIDOutCompensate=0;  //50    2000
	PID_Pitch_206_Mechanical.PMax=60;             //30
	PID_Pitch_206_Mechanical.IMax=30;             //500
	PID_Pitch_206_Mechanical.DMax=500;  
	PID_Pitch_206_Mechanical.PIDOutMax=650;        //65     
	PID_Pitch_206_Mechanical.PIDOutLast=0;
	PID_Pitch_206_Mechanical.Target_Speed_Last=0;
	PID_Pitch_206_Mechanical.Speed_Ratio=1;
	PID_Pitch_206_Mechanical.Acceleration=0;
	PID_Pitch_206_Mechanical.Ke=0;
	PID_Pitch_206_Mechanical.Kec=0;
	PID_Pitch_206_Mechanical.Ku_D=0;
	PID_Pitch_206_Mechanical.Ku_I=0;
	PID_Pitch_206_Mechanical.Ku_P=0;
	
	                                              //跳变      跟随
	PID_Pitch_206_Speed.P=300;                    //300       300
	PID_Pitch_206_Speed.I=0.1;                    //5         0.1
	PID_Pitch_206_Speed.D=0;                      //3
	
	
	PID_Pitch_206_Speed.CurrentError=0;
	PID_Pitch_206_Speed.LastError=0;
	PID_Pitch_206_Speed.ErrorIgnored=0;           //0
	PID_Pitch_206_Speed.Pout=0;
	PID_Pitch_206_Speed.Iout=0;
	PID_Pitch_206_Speed.Dout=0;
	PID_Pitch_206_Speed.PIDout=0;
	PID_Pitch_206_Speed.PIDOutCompensate=0;
	PID_Pitch_206_Speed.PMax=30000;              //30000  
	PID_Pitch_206_Speed.IMax=16000;              //16000
	PID_Pitch_206_Speed.DMax=20000;              //20000
	PID_Pitch_206_Speed.PIDOutMax=20000;         //20000
	PID_Pitch_206_Speed.PIDOutLast=0;
	PID_Pitch_206_Speed.Target_Speed_Last=0;
	PID_Pitch_206_Speed.Speed_Ratio=1;
	PID_Pitch_206_Speed.Acceleration=0;
	PID_Pitch_206_Speed.Ke=0;
	PID_Pitch_206_Speed.Kec=0;
	PID_Pitch_206_Speed.Ku_D=0;
	PID_Pitch_206_Speed.Ku_I=0;
	PID_Pitch_206_Speed.Ku_P=0;
  }
/*******************自瞄***************************/
	{/*0x205*/
	


	PID_Armor_Yaw_205_Mechanical.P=15;
	PID_Armor_Yaw_205_Mechanical.I=0.08;
	PID_Armor_Yaw_205_Mechanical.D=0;		
	PID_Armor_Yaw_205_Mechanical.CurrentError=0;
	PID_Armor_Yaw_205_Mechanical.LastError=0;
	PID_Armor_Yaw_205_Mechanical.ErrorIgnored=0;
	PID_Armor_Yaw_205_Mechanical.Pout=0;
	PID_Armor_Yaw_205_Mechanical.Iout=0;
	PID_Armor_Yaw_205_Mechanical.Dout=0;
	PID_Armor_Yaw_205_Mechanical.PIDout=0;
	PID_Armor_Yaw_205_Mechanical.PIDOutCompensate=0;
	PID_Armor_Yaw_205_Mechanical.PMax=10000;
	PID_Armor_Yaw_205_Mechanical.IMax=30;
	PID_Armor_Yaw_205_Mechanical.DMax=10000;
	PID_Armor_Yaw_205_Mechanical.PIDOutMax=80;
	PID_Armor_Yaw_205_Mechanical.PIDOutLast=0;
	PID_Armor_Yaw_205_Mechanical.Target_Speed_Last=0;
	PID_Armor_Yaw_205_Mechanical.Speed_Ratio=1;
	PID_Armor_Yaw_205_Mechanical.Acceleration=0;
	PID_Armor_Yaw_205_Mechanical.Ke=0;
	PID_Armor_Yaw_205_Mechanical.Kec=0;
	PID_Armor_Yaw_205_Mechanical.Ku_D=0;
	PID_Armor_Yaw_205_Mechanical.Ku_I=0;
	PID_Armor_Yaw_205_Mechanical.Ku_P=0;
	
	
	
	PID_Armor_Yaw_205_Speed.P=300;
	PID_Armor_Yaw_205_Speed.I=0.5;
	PID_Armor_Yaw_205_Speed.D=1000;
	PID_Armor_Yaw_205_Speed.CurrentError=0;
	PID_Armor_Yaw_205_Speed.LastError=0;
	PID_Armor_Yaw_205_Speed.ErrorIgnored=0;
	PID_Armor_Yaw_205_Speed.Pout=0;
	PID_Armor_Yaw_205_Speed.Iout=0;
	PID_Armor_Yaw_205_Speed.Dout=0;
	PID_Armor_Yaw_205_Speed.PIDout=0;
	PID_Armor_Yaw_205_Speed.PIDOutCompensate=0;
	PID_Armor_Yaw_205_Speed.PMax=10000;
	PID_Armor_Yaw_205_Speed.IMax=4000;
	PID_Armor_Yaw_205_Speed.DMax=10000;
	PID_Armor_Yaw_205_Speed.PIDOutMax=15000;
	PID_Armor_Yaw_205_Speed.PIDOutLast=0;
	PID_Armor_Yaw_205_Speed.Target_Speed_Last=0;
	PID_Armor_Yaw_205_Speed.Speed_Ratio=1;
	PID_Armor_Yaw_205_Speed.Acceleration=0;
	PID_Armor_Yaw_205_Speed.Ke=0;
	PID_Armor_Yaw_205_Speed.Kec=0;
	PID_Armor_Yaw_205_Speed.Ku_D=0;
	PID_Armor_Yaw_205_Speed.Ku_I=0;
	PID_Armor_Yaw_205_Speed.Ku_P=0;
	
	
	
	/*0x206*/
	PID_Armor_Pitch_206_Mechanical.P=0.6;  
	PID_Armor_Pitch_206_Mechanical.I=0.01; 
	PID_Armor_Pitch_206_Mechanical.D=0; 
	PID_Armor_Pitch_206_Mechanical.CurrentError=0;
	PID_Armor_Pitch_206_Mechanical.LastError=0;
	PID_Armor_Pitch_206_Mechanical.ErrorIgnored=0;
	PID_Armor_Pitch_206_Mechanical.Pout=0;
	PID_Armor_Pitch_206_Mechanical.Iout=0;
	PID_Armor_Pitch_206_Mechanical.Dout=0;
	PID_Armor_Pitch_206_Mechanical.PIDout=0;
	PID_Armor_Pitch_206_Mechanical.PIDOutCompensate=0;
	PID_Armor_Pitch_206_Mechanical.PMax=20;
	PID_Armor_Pitch_206_Mechanical.IMax=5;
	PID_Armor_Pitch_206_Mechanical.DMax=10;
	PID_Armor_Pitch_206_Mechanical.PIDOutMax=30;
	PID_Armor_Pitch_206_Mechanical.PIDOutLast=0;
	PID_Armor_Pitch_206_Mechanical.Target_Speed_Last=0;
	PID_Armor_Pitch_206_Mechanical.Speed_Ratio=1;
	PID_Armor_Pitch_206_Mechanical.Acceleration=0;
	PID_Armor_Pitch_206_Mechanical.Ke=0;
	PID_Armor_Pitch_206_Mechanical.Kec=0;
	PID_Armor_Pitch_206_Mechanical.Ku_D=0;
	PID_Armor_Pitch_206_Mechanical.Ku_I=0;
	PID_Armor_Pitch_206_Mechanical.Ku_P=0;
	
	
	PID_Armor_Pitch_206_Speed.P=160;     
	PID_Armor_Pitch_206_Speed.I=0.5; //用于抵抗弹簧，有最大值限制   
	PID_Armor_Pitch_206_Speed.D=0;  
	PID_Armor_Pitch_206_Speed.CurrentError=0;
	PID_Armor_Pitch_206_Speed.LastError=0;
	PID_Armor_Pitch_206_Speed.ErrorIgnored=0;
	PID_Armor_Pitch_206_Speed.Pout=0;
	PID_Armor_Pitch_206_Speed.Iout=8000;
	PID_Armor_Pitch_206_Speed.Dout=0;
	PID_Armor_Pitch_206_Speed.PIDout=0;
	PID_Armor_Pitch_206_Speed.PIDOutCompensate=0;
	PID_Armor_Pitch_206_Speed.PMax=20000;
	PID_Armor_Pitch_206_Speed.IMax=13000;
	PID_Armor_Pitch_206_Speed.DMax=5000;
	PID_Armor_Pitch_206_Speed.PIDOutMax=13000;
	PID_Armor_Pitch_206_Speed.PIDOutLast=0;
	PID_Armor_Pitch_206_Speed.Target_Speed_Last=0;
	PID_Armor_Pitch_206_Speed.Speed_Ratio=1;
	PID_Armor_Pitch_206_Speed.Acceleration=0;
	PID_Armor_Pitch_206_Speed.Ke=0;
	PID_Armor_Pitch_206_Speed.Kec=0;
	PID_Armor_Pitch_206_Speed.Ku_D=0;
	PID_Armor_Pitch_206_Speed.Ku_I=0;
	PID_Armor_Pitch_206_Speed.Ku_P=0;
  }

/*******************摩擦轮***************************/
  {
  //注意，如果电压不足，PIDout不能输出过大，否则会出现电机启动失败的情况，输出值最大值最好在5000
	PID_Friction_Wheel_203.P=6;
	PID_Friction_Wheel_203.I=0;
	PID_Friction_Wheel_203.D=1.5;
	
	PID_Friction_Wheel_204.P=6;                     //10
	PID_Friction_Wheel_204.I=0;                       //0.01
	PID_Friction_Wheel_204.D=1.5;  
		
	PID_Friction_Wheel_203.CurrentError=0;
	PID_Friction_Wheel_203.LastError=0;
	PID_Friction_Wheel_203.ErrorIgnored=0;
	PID_Friction_Wheel_203.Pout=0;
	PID_Friction_Wheel_203.Iout=0;
	PID_Friction_Wheel_203.Dout=0;
	PID_Friction_Wheel_203.PIDout=0;
	PID_Friction_Wheel_203.PIDOutCompensate=0;
	PID_Friction_Wheel_203.PMax=9000;
	PID_Friction_Wheel_203.IMax=4000;
	PID_Friction_Wheel_203.DMax=5000;
	PID_Friction_Wheel_203.PIDOutMax=12000;
	PID_Friction_Wheel_203.PIDOutLast=0;
	PID_Friction_Wheel_203.Target_Speed_Last=0;
	PID_Friction_Wheel_203.Speed_Ratio=1;
	PID_Friction_Wheel_203.Acceleration=0;
	PID_Friction_Wheel_203.Ke=0;
	PID_Friction_Wheel_203.Kec=0;
	PID_Friction_Wheel_203.Ku_D=0;
	PID_Friction_Wheel_203.Ku_I=0;
	PID_Friction_Wheel_203.Ku_P=0;
	
	
	PID_Friction_Wheel_204.CurrentError=0;
	PID_Friction_Wheel_204.LastError=0;
	PID_Friction_Wheel_204.ErrorIgnored=0;
	PID_Friction_Wheel_204.Pout=0;
	PID_Friction_Wheel_204.Iout=0;
	PID_Friction_Wheel_204.Dout=0;
	PID_Friction_Wheel_204.PIDout=0;
	PID_Friction_Wheel_204.PIDOutCompensate=0;
	PID_Friction_Wheel_204.PMax=9000;
	PID_Friction_Wheel_204.IMax=5000;
	PID_Friction_Wheel_204.DMax=5000;
	PID_Friction_Wheel_204.PIDOutMax=12000;
	PID_Friction_Wheel_204.PIDOutLast=0;
	PID_Friction_Wheel_204.Target_Speed_Last=0;
	PID_Friction_Wheel_204.Speed_Ratio=1;
	PID_Friction_Wheel_204.Acceleration=0;
	PID_Friction_Wheel_204.Ke=0;
	PID_Friction_Wheel_204.Kec=0;
	PID_Friction_Wheel_204.Ku_D=0;
	PID_Friction_Wheel_204.Ku_I=0;
	PID_Friction_Wheel_204.Ku_P=0;
	
	}

}

/**
  * @brief  PID计算
  * @param  void
  * @retval void
  * @notes  void  
*/
 float Pid_Calc(PID_TypeDef *PID,float Current_Speed,float Target_Speed)
{   	
	Current_Speed = Current_Speed/PID->Speed_Ratio;
	
	PID->Target_Speed_Last = Target_Speed;
	
	PID->CurrentError = Target_Speed - Current_Speed;
	
	PID->Err_Change=PID->CurrentError-PID->LastError;
	
	
	if ( fabs(PID->CurrentError)< PID->ErrorIgnored )  PID->CurrentError = 0;

	
	//P  
	PID->Pout = PID->P * PID->CurrentError;
	if(PID->Pout> PID->PMax) PID->Pout= PID->PMax;
	if(PID->Pout<(-PID->PMax)) PID->Pout=(-PID->PMax);
	//I
	PID->Iout += PID->I * PID->CurrentError;
	if(PID->Iout> PID->IMax)   PID->Iout= PID->IMax;
	if(PID->Iout<(-PID->IMax)) PID->Iout=(-PID->IMax);
	//D 
	PID->Dout = PID->D * ( PID->CurrentError - PID->LastError );
	if(PID->Dout> PID->DMax) PID->Dout= PID->DMax;
	if(PID->Dout<(-PID->DMax)) PID->Dout=(-PID->DMax);
	//PID
	if ( PID->CurrentError > 0 ) PID->PIDout = PID->Pout + PID->Iout + PID->Dout + PID->PIDOutCompensate;
	else  if(PID->CurrentError < 0) PID->PIDout = PID->Pout + PID->Iout + PID->Dout - PID->PIDOutCompensate;
  else  PID->PIDout = PID->Pout + PID->Iout + PID->Dout;

  
		 
	if ( (fabs( PID->PIDout - PID->PIDOutLast)>3000)  && (fabs(Current_Speed) !=0) )    
	{
	  PID->PIDout =( PID->PIDout>PID->PIDOutLast) ?  (PID->PIDOutLast+3000) : (PID->PIDOutLast-3000);
	}
 
	
	if(PID->PIDout> PID->PIDOutMax)   PID->PIDout= PID->PIDOutMax;
	if(PID->PIDout<(-PID->PIDOutMax)) PID->PIDout=(-PID->PIDOutMax);

	
  PID->PIDOutLast = PID->PIDout;
	PID->LastError = PID->CurrentError;
	
	return PID->PIDout;
}




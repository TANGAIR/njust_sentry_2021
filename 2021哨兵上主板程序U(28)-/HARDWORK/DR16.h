#ifndef _DR16_H
#define _DR16_H
#include "sys.h" 

#define DBUS_DATE_LENGTH  18
#define DBUS_SAFTY_LENGTH 0
/* ----------------------- RC Channel Definition---------------------------- */ 
#define RC_CH_VALUE_MIN              ((uint16_t)364 ) 
#define RC_CH_VALUE_OFFSET           ((uint16_t)1024) 
#define RC_CH_VALUE_MAX              ((uint16_t)1684) 
 
/* ----------------------- RC Switch Definition----------------------------- */ 
#define RC_SW_UP                     ((uint16_t)1) 
#define RC_SW_MID                    ((uint16_t)3) 
#define RC_SW_DOWN                   ((uint16_t)2) 
#define RC_NO_RECIEVE					 ((uint16_t)0)
 
/* ----------------------- Data Struct ------------------------------------- */ 	
typedef struct
{
	struct
	{
		int16_t ch0;            
		int16_t ch1;
		int16_t ch2;
		int16_t ch3;
		uint8_t Switch_Left;     
		uint8_t Switch_Right;
	}RC;
	struct 
	{
		int16_t ch0;
		int16_t ch1;
		int16_t ch2;
		int16_t ch3;
		int16_t omega;
		int16_t angle;
	}Control;
}DBUS_DecodingDate_TypeDef;

extern DBUS_DecodingDate_TypeDef DBUS;
	
void DR16_InitConfig(void);


#endif

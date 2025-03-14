#include "main.h"

int Random_Number=0;
int Random_Number_Last=0;
int Random_Number_Now=0;

void Random_Num_Generator_Init(void) //随机数发生器初始化
{
	/* 使能RNG时钟 */
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	
	/* 使能RNG外设 */
  RNG_Cmd(ENABLE);
}

void Random_Num_Generator_Produce(void)  //产生1-5的随机数
{
	    Random_Number_Last=Random_Number_Now;
      /* 等待随机数产生完毕 */
      while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET)
      {
      }		
      /*获取随机数*/       
      Random_Number_Now = (RNG_GetRandomNumber()%6);			
}

#include "Laser.h"

/***************************************数据定义区***************************************/


/***************************************函数处理区***************************************/
/**
  * @brief  激光初始化
  * @param  void
  * @retval void
  * @notes  PG13  
  */
void Laser_InitConfig(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOG,ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOG, &GPIO_InitStruct);//初始化GPIO
	LASER_ON;
}	

#include "main.h"

int Random_Number=0;
int Random_Number_Last=0;
int Random_Number_Now=0;

void Random_Num_Generator_Init(void) //�������������ʼ��
{
	/* ʹ��RNGʱ�� */
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	
	/* ʹ��RNG���� */
  RNG_Cmd(ENABLE);
}

void Random_Num_Generator_Produce(void)  //����1-5�������
{
	    Random_Number_Last=Random_Number_Now;
      /* �ȴ������������� */
      while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET)
      {
      }		
      /*��ȡ�����*/       
      Random_Number_Now = (RNG_GetRandomNumber()%6);			
}

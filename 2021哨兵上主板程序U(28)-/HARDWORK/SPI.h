#ifndef SPI_H
#define SPI_H
#include "stm32f4xx.h" 

void SPI_GPIO_InitConfig(void);
u8 SPI5_ReadWriteByte(u8 TxData);

#endif

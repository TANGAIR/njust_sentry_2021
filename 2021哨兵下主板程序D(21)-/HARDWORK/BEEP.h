#ifndef BEEP_H
#define BEEP_H
#include "stm32f4xx.h" 












//Òô½×ÆµÂÊ
 #define  ZERO    3000
 #define  H7       262         //µÍ1¡¡DO
 #define  half_H6  277         //#1¡¡DO#
 #define  H6       294
 #define  half_H5  311
 #define  H5       330
 #define  H4       349
 #define  half_H3  370
 #define  H3       392
 #define  half_H2  410
 #define  H2       440
 #define  half_H1  466
 #define  H1       494

 #define  M7       523        //ÖÐ1¡¡DO
 #define  half_M6  554        //#1¡¡ DO#
 #define  M6       587
 #define  half_M5  622
 #define  M5       659
 #define  M4       698
 #define  half_M3  740
 #define  M3       784
 #define  half_M2  831
 #define  M2       880
 #define  half_M1  932
 #define  M1       988

 #define  L7       1046        //¸ß1¡¡DO
 #define  half_L6  1109        //#1¡¡DO#
 #define  L6       1175
 #define  half_L5  1245
 #define  L5       1318
 #define  L4       1397
 #define  half_L3  1480
 #define  L3       1568
 #define  half_L2  1661
 #define  L2       1760
 #define  half_L1  1865
 #define  L1       1967


extern int tune[];

extern int delay[];

extern int Music_length;

void Beep_Change_Music(int pra);


void Beep_Init(void);
void Beep_OFF(void);
void Beep_ON(void);


#endif

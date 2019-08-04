#ifndef __ENCODER_H
#define __ENCODER_H

#include "sys.h"


#define ENCODER_TIM_PERIOD (u32)(65535) 

int Read_Encoder(u8 TIMX);
void Encoder_Init_TIM2(void);
void Encoder_Init_TIM3(void);

void TIM4_Int_Init(u16 arr,u16 psc);
void TIM4_IRQHandler(void); 



#endif


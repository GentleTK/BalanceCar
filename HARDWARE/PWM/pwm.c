#include "pwm.h"

void TIM9_PWM_Init(u32 arr,u32 psc)
{		 					 
	GPIO_InitTypeDef	GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
	TIM_OCInitTypeDef   TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);  			//TIM1时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 			//使能PORTF时钟	
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM9);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM9);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;//GPIOA8,A11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        			//复用功能           
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      			//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        			//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);              				//初始化PA8,PA11

	TIM_TimeBaseStructure.TIM_Prescaler=psc;  						//定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		//向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   						//自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);					//初始化定时器1
																	//初始化TIM1 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 		//输出极性:TIM输出比较极性低
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);  						//根据T指定的参数初始化外设TIM1 4OC1
	TIM_OC2Init(TIM9, &TIM_OCInitStructure);
	//TIM_CtrlPWMOutputs(TIM9, ENABLE);
	
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);  				//使能TIM1在CCR1上的预装载寄存器
	TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM9, ENABLE);								//ARPE使能 
	TIM_Cmd(TIM9, ENABLE);  										//使能TIM1								  
}  

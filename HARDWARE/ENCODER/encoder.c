#include "encoder.h"

/**************************************************************************
函数功能：把TIM2初始化为编码器接口模式
入口参数：无
返回  值：无
**************************************************************************/
void Encoder_Init_TIM2(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);               
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);                

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;       
	GPIO_Init(GPIOA, &GPIO_InitStructure);                                            

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0, GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1, GPIO_AF_TIM2);

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 10;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);   
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//Reset counter
	TIM_SetCounter(TIM2, 0);
	TIM_Cmd(TIM2, ENABLE); 
}
/**************************************************************************
函数功能：把TIM3初始化为编码器接口模式
入口参数：无
返回  值：无
**************************************************************************/
void Encoder_Init_TIM3(void)
{
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
        TIM_ICInitTypeDef TIM_ICInitStructure;  
        GPIO_InitTypeDef GPIO_InitStructure;
        
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

        GPIO_StructInit(&GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        
        GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;         
        GPIO_Init(GPIOA, &GPIO_InitStructure);                      

        
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);

        //TIM_DeInit(TIM3);
        TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
        
        TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
        
        TIM_TimeBaseStructure.TIM_Prescaler = 0x0;                                
        TIM_TimeBaseStructure.TIM_Period = 0xffff;
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                 
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 

        TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
        
        TIM_ICStructInit(&TIM_ICInitStructure);
        TIM_ICInitStructure.TIM_ICFilter = 10;
        TIM_ICInit(TIM3, &TIM_ICInitStructure);

        TIM_ClearFlag(TIM3, TIM_FLAG_Update);
        TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

        //Reset counter
        TIM_SetCounter(TIM3,0);
        TIM_Cmd(TIM3, ENABLE); 
}

/**************************************************************************
函数功能：单位时间读取编码器计数
入口参数：定时器
返回  值：速度值
**************************************************************************/
int Read_Encoder(u8 TIMX)
{
	int Encoder_TIM;    
	switch(TIMX)
	{
		case 2:  Encoder_TIM = (short)TIM2->CNT;TIM2->CNT = 0;break;
		case 3:  Encoder_TIM = (short)TIM3->CNT;TIM3->CNT = 0;break;	
		default: Encoder_TIM = 0;
	}
	return Encoder_TIM;
}


void TIM4_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM4, ENABLE);  //使能TIMx外设
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;  //先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	
					 
}

void TIM4_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) 			//检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  			//清除TIMx的中断待处理位:TIM 中断源.	
		Encoder_Left = -Read_Encoder(2);
		Encoder_Right = Read_Encoder(3);
	}
}





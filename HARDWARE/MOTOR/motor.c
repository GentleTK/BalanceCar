#include "motor.h"

extern int MotorA, MotorB;

void Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					//初始化
}

void Set_PWM_Val(int Motor_A, int Motor_B)
{
	if(Motor_A < 0)
	{
		AIN1 = 1;
		AIN2 = 0;
	}
	else if(Motor_A > 0)
	{
		AIN1 = 0;
		AIN2 = 1;
	}
	else
	{
		AIN1 = 0;
		AIN2 = 0;
	}
	PWMA = Myabs(Motor_A);
	if(Motor_B < 0)
	{
		BIN1 = 0;
		BIN2 = 1;
	}
	else if(Motor_B > 0)
	{
		BIN1 = 1;
		BIN2 = 0;
	}
	else
	{
		BIN1 = 0;
		BIN2 = 0;
	}
	PWMB = Myabs(Motor_B);
}

void forward(void)
{
	AIN1 = 0;
	AIN2 = 1;
	BIN1 = 1;
	BIN2 = 0;
	PWMA = PWMB = 6000;
}

void backward(void)
{
	AIN1 = 1;
	AIN2 = 0;
	BIN1 = 0;
	BIN2 = 1;
	PWMA = PWMB = 100;
}

int Myabs(int value)//绝对值函数
{
	if(value < 0)
		return (-value);
	else
		return value;
}

void Limit_PWM(void)//限制PWM
{
	int Limited_PWM = 8000;
	if(MotorA < -Limited_PWM)
		MotorA = -Limited_PWM;
	if(MotorB < -Limited_PWM)
		MotorB = -Limited_PWM;
	if(MotorA > Limited_PWM)
		MotorA = Limited_PWM;
	if(MotorB > Limited_PWM)
		MotorB = Limited_PWM;
}

void MotorStop(float angle)
{
	if((angle < -40) || (angle > 40) || (1 == Flag_Stop))
	{
		MotorA = 0;
		MotorB = 0;
		AIN1 = 0;
		AIN2 = 0;
		BIN1 = 0;
		BIN2 = 0;
	}

}


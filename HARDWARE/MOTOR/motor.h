#ifndef __MOTOR_H
#define __MOTOR_H

#include "sys.h"

#define PWMA TIM9->CCR1
#define PWMB TIM9->CCR2

#define AIN2 PBout(12)
#define AIN1 PBout(13)
#define BIN1 PBout(14)
#define BIN2 PBout(15)

void Motor_Init(void);
void Set_PWM_Val(int Motor_A, int Motor_B);
int Myabs(int value);
void Limit_PWM(void);
void MotorStop(float angle);

void forward(void);
void backward(void);

#endif


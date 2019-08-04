#ifndef __CONTROL_H
#define __CONTROL_H

#include "sys.h"

void PID_Control(void);
int balance_UP(float Angle,float Mechanical_balance,float Gyro);
int velocity(int encoder_left,int encoder_right);
int turn(int EncoderLeft, int EncoderRight, float gyro);
void Remote_Control(void);
void Buletooth_Control(void);
#endif


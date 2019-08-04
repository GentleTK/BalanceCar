#include "sys.h"

int MotorA, MotorB;
int Encoder_Left, Encoder_Right;
float pitch, roll, yaw; 							//欧拉角
short aacx, aacy, aacz;								//加速度传感器原始数据
short gyrox, gyroy, gyroz;							//陀螺仪原始数据
int Turn_Way, Velocity_Val, Flag_Left, Flag_Right, Flag_Front, Flag_Back, Flag_Stop;

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置系统中断优先级分组2
	delay_init(168);      							//初始化延时函数
	uart_init(115200);							   	//初始化串口波特率为115200
	Remote_Init();									//红外接收初始化
	HC05_Init();									//初始化ATK-HC05模块 
	Encoder_Init_TIM2();					 		//编码器2初始化
	Encoder_Init_TIM3();					 		//编码器3初始化		
	Motor_Init();									//初始化电机
  	TIM9_PWM_Init(8400-1, 0);						//PWM初始化
	TIM4_Int_Init(100-1,8400-1);					//10ms
	MPU_Init();		
	while(mpu_dmp_init())							//初始化MPU6050
	{
		printf("\nMPU_DMP_Init Error!\n");
		delay_ms(500);
	}    	
	while(1) 
	{		 
		PID_Control();
		Remote_Control();
		Buletooth_Control();												
		//printf("\nPitch = %.2f, Roll = %.2f, Yaw = %.2f\n", pitch, roll, yaw);
		//printf("\nEncoder_Left = %d, Encoder_Right = %d\n", Encoder_Left, Encoder_Right);
	} 
}




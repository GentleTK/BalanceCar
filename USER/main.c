#include "sys.h"

int MotorA, MotorB;
int Encoder_Left, Encoder_Right;
float pitch, roll, yaw; 							//ŷ����
short aacx, aacy, aacz;								//���ٶȴ�����ԭʼ����
short gyrox, gyroy, gyroz;							//������ԭʼ����
int Turn_Way, Velocity_Val, Flag_Left, Flag_Right, Flag_Front, Flag_Back, Flag_Stop;

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����ϵͳ�ж����ȼ�����2
	delay_init(168);      							//��ʼ����ʱ����
	uart_init(115200);							   	//��ʼ�����ڲ�����Ϊ115200
	Remote_Init();									//������ճ�ʼ��
	HC05_Init();									//��ʼ��ATK-HC05ģ�� 
	Encoder_Init_TIM2();					 		//������2��ʼ��
	Encoder_Init_TIM3();					 		//������3��ʼ��		
	Motor_Init();									//��ʼ�����
  	TIM9_PWM_Init(8400-1, 0);						//PWM��ʼ��
	TIM4_Int_Init(100-1,8400-1);					//10ms
	MPU_Init();		
	while(mpu_dmp_init())							//��ʼ��MPU6050
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




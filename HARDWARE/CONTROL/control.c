#include "control.h"

extern int MotorA, MotorB;

int Balance_Pwm, Velocity_Pwm, Turn_Pwm;

float Mechanical_angle = 0; 
float balance_UP_KP = -420; 	 											// С��ֱ����KP
float balance_UP_KD = -1.2;

float velocity_KP = -80;

float Turn_KP = -1;

void PID_Control(void)
{
	while(mpu_dmp_get_data(&pitch,&roll,&yaw))
	{
		MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);						//�õ�����������
		Balance_Pwm = balance_UP(pitch, Mechanical_angle, gyroy);		//===ƽ�⻷PID����
		Velocity_Pwm = velocity(Encoder_Left, Encoder_Right);			//===�ٶȻ�PID����
		Turn_Pwm = turn(Encoder_Left, Encoder_Right, gyroz);			//===ת��PID����
		if(1 == Flag_Left || 1 == Flag_Right)
		{
			
			MotorA = Balance_Pwm - Velocity_Pwm + Turn_Pwm;
			MotorB = Balance_Pwm - Velocity_Pwm - Turn_Pwm;
		}
		else
		{
			MotorA = Balance_Pwm - Velocity_Pwm;
			MotorB = Balance_Pwm - Velocity_Pwm;
		}
		Limit_PWM();  													//===PWM�޷�
		Set_PWM_Val(MotorA, MotorB);                  					//===��ֵ��PWM�Ĵ��� 
		MotorStop(pitch);												//===���Ƕ��Ƿ�����
	}	
}

int balance_UP(float Angle,float Mechanical_balance,float Gyro)
{  
	float Bias;
	int balance;
	Bias = Angle - Mechanical_balance;    								//===���ƽ��ĽǶ���ֵ�ͻ�е���
	balance = balance_UP_KP*Bias + balance_UP_KD*Gyro;  				//===����ƽ����Ƶĵ��PWM  PD����   kp��Pϵ�� kd��Dϵ�� 
	return balance;
}

int velocity(int encoder_left, int encoder_right)
{
	static float Velocity, Encoder_Least, Encoder;
	Encoder_Least = (encoder_left + encoder_right);					//===��ȡ�����ٶ�ƫ��==�����ٶȣ����ұ�����֮�ͣ�-Ŀ���ٶȣ��˴�Ϊ�㣩 
	if(1 == Flag_Front)
	{
		Encoder_Least -= Velocity_Val;
	}
	else if(1 == Flag_Back)
	{
		Encoder_Least += Velocity_Val;
	}
	else
	{
		Encoder_Least = (encoder_left + encoder_right);
	}
	Encoder *= 0.7;		                                                //===һ�׵�ͨ�˲���       
	Encoder += Encoder_Least*0.3;	                                    //===һ�׵�ͨ�˲���    	
	Velocity = Encoder*velocity_KP;
	return Velocity;
}

int turn(int EncoderLeft, int EncoderRight, float gyro)
{
	float Turn, Bias, Encoder_Temp, Turn_Val;
	Encoder_Temp = Myabs(EncoderLeft+EncoderRight);
	if(Flag_Left==1 || Flag_Right==1)
	{
		Turn_Val = 50/Encoder_Temp;
		if(Turn_Val<0.6)
			Turn_Val = 0.6;
		if(Turn_Val>3)
			Turn_Val = 3;
	}
	else
	{
		Turn_Val = 0.9;
		Encoder_Temp = 0;
	}
	if(1 == Flag_Left)
	{
		Turn_Way -= Turn_Val;//��ת
	}
	else if(1 == Flag_Right)
	{
		Turn_Way += Turn_Val;//��ת
	}
	else
	{
		Turn_Way = 0;
	}
	Bias = gyro - 0;
	Turn = -Bias*Turn_KP;
	Turn += Turn_Way;
	return Turn;
}

void Remote_Control(void)
{
	u8 key;
	key = Remote_Scan();									//����ң��
		if(key)
		{
			switch(key)
			{	    
				case 98:									//ǰ��
						Velocity_Val = 15;				
						Flag_Front = 1;
						Flag_Back=Flag_Left=Flag_Right=0;
						break;
				case 168:									//����
						Velocity_Val = 15;				
						Flag_Back=1;
						Flag_Front=Flag_Left=Flag_Right=0;
						break;
				case 34:
						Flag_Left=1;						//��ת
						Flag_Right=0;
						break;
				case 194:									//��ת
						Flag_Right=1;
						Flag_Left=0;
						break;	   
				case 144:
						if(Velocity_Val>90 || Velocity_Val==90)
							Velocity_Val = 90;
						else
							Velocity_Val++;					//����
						break;
				case 224:
						if(Velocity_Val<0 || Velocity_Val==0)
							Velocity_Val = 0;				//����
						else
							Velocity_Val--;
						break;		  
				case 162:
						Flag_Stop=1;						//ֹͣ
						break;
				case 2:
						Flag_Stop=0;						//����
						break;
				case 226:printf("\n����\n");break;			//����
				default:break;
			}
		}
}

void Buletooth_Control(void)								//����ң��
{
	u8 reclen=0;
	if(USART3_RX_STA&0X8000)								//���յ�һ��������
		{
			reclen=USART3_RX_STA&0X7FFF;					//�õ����ݳ���
		  	USART3_RX_BUF[reclen]=0;	 					//���������
			//printf("data = %d\n", *USART3_RX_BUF);
			switch(*USART3_RX_BUF)
			{
				case 49:
						if(Velocity_Val>90 || Velocity_Val==90)
							Velocity_Val = 90;
						else
							Velocity_Val++;					//����
						break;
				case 50:
						Velocity_Val = 15;					//ǰ��
						Flag_Front = 1;
						Flag_Back=Flag_Left=Flag_Right=0;
						break;
				case 65://50
						Velocity_Val = 15;					//ǰ��
						Flag_Front = 1;
						Flag_Back=Flag_Left=Flag_Right=0;
						break;
				case 51:
						if(Velocity_Val<0 || Velocity_Val==0)
							Velocity_Val = 0;				//����
						else
							Velocity_Val--;
						break;
				case 52:
						Flag_Left=1;						//��ת
						Flag_Right=0;
						break;
				case 71://52
						Flag_Left=1;						//��ת
						Flag_Right=0;
						break;
				case 53:
						Flag_Stop=0;						//����
						break;
				case 54:
						Flag_Right=1;						//��ת
						Flag_Left=0;
						break;
				case 67://54
						Flag_Right=1;						//��ת
						Flag_Left=0;
						break;
				case 55:
						Flag_Stop=1;						//ֹͣ
						break;
				case 56:
						Velocity_Val = 15;					//����
						Flag_Back=1;
						Flag_Front=Flag_Left=Flag_Right=0;
						break;
				case 69://56
						Velocity_Val = 15;					//����
						Flag_Back=1;
						Flag_Front=Flag_Left=Flag_Right=0;
						break;
				case 57:printf("\n����\n");break;			//����
				default:break;
			}
			USART3_RX_STA=0;
		}
}









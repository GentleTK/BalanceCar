#include "control.h"

extern int MotorA, MotorB;

int Balance_Pwm, Velocity_Pwm, Turn_Pwm;

float Mechanical_angle = 0; 
float balance_UP_KP = -420; 	 											// 小车直立环KP
float balance_UP_KD = -1.2;

float velocity_KP = -80;

float Turn_KP = -1;

void PID_Control(void)
{
	while(mpu_dmp_get_data(&pitch,&roll,&yaw))
	{
		MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);						//得到陀螺仪数据
		Balance_Pwm = balance_UP(pitch, Mechanical_angle, gyroy);		//===平衡环PID控制
		Velocity_Pwm = velocity(Encoder_Left, Encoder_Right);			//===速度环PID控制
		Turn_Pwm = turn(Encoder_Left, Encoder_Right, gyroz);			//===转向环PID控制
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
		Limit_PWM();  													//===PWM限幅
		Set_PWM_Val(MotorA, MotorB);                  					//===赋值给PWM寄存器 
		MotorStop(pitch);												//===检查角度是否正常
	}	
}

int balance_UP(float Angle,float Mechanical_balance,float Gyro)
{  
	float Bias;
	int balance;
	Bias = Angle - Mechanical_balance;    								//===求出平衡的角度中值和机械相关
	balance = balance_UP_KP*Bias + balance_UP_KD*Gyro;  				//===计算平衡控制的电机PWM  PD控制   kp是P系数 kd是D系数 
	return balance;
}

int velocity(int encoder_left, int encoder_right)
{
	static float Velocity, Encoder_Least, Encoder;
	Encoder_Least = (encoder_left + encoder_right);					//===获取最新速度偏差==测量速度（左右编码器之和）-目标速度（此处为零） 
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
	Encoder *= 0.7;		                                                //===一阶低通滤波器       
	Encoder += Encoder_Least*0.3;	                                    //===一阶低通滤波器    	
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
		Turn_Way -= Turn_Val;//左转
	}
	else if(1 == Flag_Right)
	{
		Turn_Way += Turn_Val;//右转
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
	key = Remote_Scan();									//红外遥控
		if(key)
		{
			switch(key)
			{	    
				case 98:									//前进
						Velocity_Val = 15;				
						Flag_Front = 1;
						Flag_Back=Flag_Left=Flag_Right=0;
						break;
				case 168:									//后退
						Velocity_Val = 15;				
						Flag_Back=1;
						Flag_Front=Flag_Left=Flag_Right=0;
						break;
				case 34:
						Flag_Left=1;						//左转
						Flag_Right=0;
						break;
				case 194:									//右转
						Flag_Right=1;
						Flag_Left=0;
						break;	   
				case 144:
						if(Velocity_Val>90 || Velocity_Val==90)
							Velocity_Val = 90;
						else
							Velocity_Val++;					//加速
						break;
				case 224:
						if(Velocity_Val<0 || Velocity_Val==0)
							Velocity_Val = 0;				//减速
						else
							Velocity_Val--;
						break;		  
				case 162:
						Flag_Stop=1;						//停止
						break;
				case 2:
						Flag_Stop=0;						//启动
						break;
				case 226:printf("\n避障\n");break;			//避障
				default:break;
			}
		}
}

void Buletooth_Control(void)								//蓝牙遥控
{
	u8 reclen=0;
	if(USART3_RX_STA&0X8000)								//接收到一次数据了
		{
			reclen=USART3_RX_STA&0X7FFF;					//得到数据长度
		  	USART3_RX_BUF[reclen]=0;	 					//加入结束符
			//printf("data = %d\n", *USART3_RX_BUF);
			switch(*USART3_RX_BUF)
			{
				case 49:
						if(Velocity_Val>90 || Velocity_Val==90)
							Velocity_Val = 90;
						else
							Velocity_Val++;					//加速
						break;
				case 50:
						Velocity_Val = 15;					//前进
						Flag_Front = 1;
						Flag_Back=Flag_Left=Flag_Right=0;
						break;
				case 65://50
						Velocity_Val = 15;					//前进
						Flag_Front = 1;
						Flag_Back=Flag_Left=Flag_Right=0;
						break;
				case 51:
						if(Velocity_Val<0 || Velocity_Val==0)
							Velocity_Val = 0;				//减速
						else
							Velocity_Val--;
						break;
				case 52:
						Flag_Left=1;						//左转
						Flag_Right=0;
						break;
				case 71://52
						Flag_Left=1;						//左转
						Flag_Right=0;
						break;
				case 53:
						Flag_Stop=0;						//启动
						break;
				case 54:
						Flag_Right=1;						//右转
						Flag_Left=0;
						break;
				case 67://54
						Flag_Right=1;						//右转
						Flag_Left=0;
						break;
				case 55:
						Flag_Stop=1;						//停止
						break;
				case 56:
						Velocity_Val = 15;					//后退
						Flag_Back=1;
						Flag_Front=Flag_Left=Flag_Right=0;
						break;
				case 69://56
						Velocity_Val = 15;					//后退
						Flag_Back=1;
						Flag_Front=Flag_Left=Flag_Right=0;
						break;
				case 57:printf("\n避障\n");break;			//避障
				default:break;
			}
			USART3_RX_STA=0;
		}
}









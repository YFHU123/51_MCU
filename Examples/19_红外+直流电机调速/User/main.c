/* 头文件定义区 */
#include <reg52.h>
#include "LCD1602.h"
#include "IR.h"

/*引脚定义 */
sbit Motor=P1^0;

/* 变量声明区 */
unsigned char Num;
unsigned char Address;
unsigned char Command;

//直流电机
unsigned char Counter,Compare;	//计数值和比较值，用于输出PWM
unsigned char Speed = 0;//速度

/* 红外处理函数 */
void IR_Proc()
{
	if(IR_GetDataFlag() || IR_GetRepeatFlag())	//如果收到数据帧或者收到连发帧
	{
		Address=IR_GetAddress();		//获取遥控器地址码
		Command=IR_GetCommand();		//获取遥控器命令码
		
		if(Command==IR_VOL_MINUS)		//如果遥控器VOL-按键按下
		{
			Speed--;						//Num自减
		}
		if(Command==IR_VOL_ADD)			//如果遥控器VOL+按键按下
		{
			Speed++;
		}

		Speed%=4;
        if(Speed==0) Compare=0;	//设置比较值，改变PWM占空比
        if(Speed==1) Compare=50;
        if(Speed==2) Compare=75;
        if(Speed==3) Compare=100;
		
		LCD_ShowNum(1,7,Speed,1);		//显示Speed
	}
}

/* 定时器1初始化函数 */
void Timer1_Init(void)		//100微秒@12.000MHz
{
	TMOD &= 0x0F;			//设置定时器模式
	TL1 = 0x9C;				//设置定时初始值
	TH1 = 0xFF;				//设置定时初始值
	TF1 = 0;				//清除TF1标志
	TR1 = 1;				//定时器1开始计时
    ET1 = 1;
    EA = 1;
}

/* 定时器1中断服务函数 */
void Timer1Sever() interrupt 3
{
    //重装载（89C52）
	TL1 = 0x9C;				//设置定时初始值
	TH1 = 0xFF;				//设置定时初始值

    Counter++;
	Counter%=100;	//计数值变化范围限制在0~99
	if(Counter<Compare)	//计数值小于比较值
	{
		Motor=1;		//输出1
	}
	else				//计数值大于比较值
	{
		Motor=0;		//输出0
	}
}   

void main()
{
	LCD_Init();
	LCD_Clear();
	Timer1_Init();
	IR_Init();
	
	LCD_ShowString(1,1,"SPEED:");

	while(1)
	{
		IR_Proc();
	}
}

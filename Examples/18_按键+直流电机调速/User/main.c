#include <intrins.h>
#include <reg52.h>
#include "Key.h"
#include "Seg.h"

/*引脚定义 */
sbit Motor=P1^0;

/* 变量声明区 */
unsigned char Seg_Slow_Down;                           // 数码管减速
unsigned char Seg_Pos;                                 // 数码管扫描专用变量
unsigned char Seg_Buf[8] = {16, 16, 16, 16, 16, 16, 16, 16};   // 数码管显示数组 默认熄灭
unsigned char Seg_Point[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // 数码管小数点显示数组 默认熄灭

unsigned char Key_Slow_Down;//按键减速
unsigned char Key_Val,Key_Old,Key_Down,Key_Up;

unsigned char Counter,Compare;	//计数值和比较值，用于输出PWM
unsigned char Speed = 0;//速度


/* 信息处理函数 */
void Seg_Proc()
{
}

/* 按键处理函数 */
void Key_Proc()
{
    if(Key_Slow_Down) return;//阀门
    Key_Slow_Down = 1;
 
    Key_Val = Key_Read();//实时读取键码值
    Key_Down = Key_Val & (Key_Old ^ Key_Val);//读取下降沿
    Key_Up = ~Key_Val & (Key_Old ^ Key_Val);//读取上升沿
    Key_Old = Key_Val;//传递旧值

    //按键逻辑
    if(Key_Down == 1)
    {
        Speed++;
        Speed%=4;
        if(Speed==0) Compare=0;	//设置比较值，改变PWM占空比
        if(Speed==1) Compare=50;
        if(Speed==2) Compare=75;
        if(Speed==3) Compare=100;
    }
        Seg_Buf[0] = Speed;
    
}
 
/* 定时器0初始化函数 */
void Timer0_Init(void)		//1毫秒@12.000MHz
{
    TMOD &= 0xF0;			//设置定时器模式
    TL0 = 0x20;				//设置定时初始值
    TH0 = 0xD1;				//设置定时初始值
    TF0 = 0;				//清除TF0标志
    TR0 = 1;				//定时器0开始计时
    ET0 = 1;
    EA = 1;
}
 
/* 定时器0中断服务函数 */
void Timer0Sever() interrupt 1
{
    //重装载（89C52）
	TL0 = 0x18;				//设置定时初始值
	TH0 = 0xFC;				//设置定时初始值
    
    if (++Seg_Slow_Down == 200)
        Seg_Slow_Down = 0; // 数码管扫描 200ms
    if (++Seg_Pos == 8)
        Seg_Pos = 0; // 0-7之间循环扫描

    Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos], Seg_Point[Seg_Pos]); // 动态显示

    if(++Key_Slow_Down == 10) Key_Slow_Down = 0;//按键轮询 10ms
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


/* 主函数 */
void main(void)
{
    Timer0_Init();
    Timer1_Init();            
    while(1)
    {
        Key_Proc();           
        Seg_Proc();
    }
}


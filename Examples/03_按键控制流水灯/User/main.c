/* 头文件声明区 */
#include <reg52.h>
#include <intrins.h>
#include "Key.h"
#include "Seg.h"

/*引脚定义 */
sbit LED1 = P1^0;

/* 变量声明区 */
unsigned char Key_Slow_Down;//按键减速
unsigned char Key_Val,Key_Old,Key_Down,Key_Up;

/* 信息处理函数 */
void LED_Proc()
{ 
}

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
	if(Key_Down == 1) P1 = _crol_(P1, 1);
    if(Key_Down == 2) P1 = _cror_(P1, 1);
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

    if(++Key_Slow_Down == 10) Key_Slow_Down = 0;//按键轮询 10ms
}

/* 主函数 */
void main(void)
{
    Timer0_Init();
    P1=~(0x01);            
    while(1)
    {
        Key_Proc();           
    }
}


/* 头文件声明区 */
#include <reg52.h>
#include <intrins.h>
#include <stdio.h>//标准库
#include <string.h>
#include <Key.h>
#include <Uart.h>

/*引脚定义 */
sbit LED1 = P1^0;

/* 变量声明区 */
unsigned char Key_Slow_Down;//按键减速
unsigned char Key_Val,Key_Old,Key_Down,Key_Up;
unsigned char Uart_Slow_Down;//串口减速专用变量
unsigned char Uart_Recv[10];//串口接收数据储存数组
unsigned char Uart_Recv_Index;//串口接收数组指针
unsigned char Uart_Send[10];//串口接收数据储存数组 
unsigned int cnt = 0;
unsigned int Timer = 0;
bit Flag = 0;

/* 按键处理函数 */
void Key_Proc()
{
    if(Key_Slow_Down) return;//阀门
    Key_Slow_Down = 1;

	Key_Val = Key_Read();//实时读取键码值
    Key_Down = Key_Val & (Key_Old ^ Key_Val);//读取下降沿
    Key_Up = ~Key_Val & (Key_Old ^ Key_Val);//读取上升沿
    Key_Old = Key_Val;//传递旧值

	/* 按键操作逻辑 */
	if(Key_Down == 1) LED1 = ~LED1;

}

/* 串口处理函数 */
void Uart_Proc()
{
	if(Uart_Slow_Down) return;
	Uart_Slow_Down = 1;//串口减速程序	

	
}

/* 定时器0初始化函数 */
void Timer0_Init(void)		//1毫秒@12.000MHz
{
	TMOD &= 0xF0;			//设置定时器模式
	TMOD |= 0x01;			//设置定时器模式
	TL0 = 0x18;				//设置定时初始值
	TH0 = 0xFC;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//开定时器0中断
	EA = 1;					//开总中断
}

 
/* 定时器0中断服务函数 */
void Timer0Sever() interrupt 1  //1ms
{
	//重装载（89C52）
	TL0 = 0x18;				//设置定时初始值
	TH0 = 0xFC;				//设置定时初始值

    if(++Key_Slow_Down == 10) Key_Slow_Down = 0;//按键轮询 10ms
	if(++Uart_Slow_Down == 200) Uart_Slow_Down = 0;//串口轮询 200ms


}

/* 串口1中断服务函数 */
void Uart1Server() interrupt 4
{
	if(RI == 1) //串口接收数据
	{
		Uart_Recv[Uart_Recv_Index] = SBUF;
		Uart_Recv_Index++;
		RI = 0;
	}
}

void main() 
{
	Timer0_Init();
	UartInit();

	Uart_Send_String("Hello World!\r\n");
	printf("你好，世界！\r\n");

    while (1)
	{
		Uart_Proc();
		
    }
}


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
unsigned int Uart_Timer = 0;
bit Uart_Flag = 0;

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
	if(Key_Down == 1)
	{
		cnt++;
		printf("cnt=%d\r\n",cnt);
	}

}

/* 串口处理函数 */
void Uart_Proc()
{
    if(Uart_Slow_Down) return;
    Uart_Slow_Down = 1;

    // 当满足：接收到数据 且 接收已经停止
    if((Uart_Recv_Index > 0) && (Uart_Flag == 0))
    {
        // --- 回传 ---
        Uart_Send_String(Uart_Recv);
        Uart_Send_String("\r\n");

        // --- 逻辑判断部分 ---
        if(strcmp(Uart_Recv, "ON") == 0) 
        {
            LED1 = 0;
        }
        else if(strcmp(Uart_Recv, "OFF") == 0)
        {
            LED1 = 1;
        }

        Uart_Recv_Index = 0; 
        memset(Uart_Recv, 0, sizeof(Uart_Recv)); 
    }
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

/* 串口1中断服务函数 */
void Uart1Server() interrupt 4
{
    if(RI == 1) 
    {
        RI = 0;
        if(Uart_Recv_Index < 10) 
        {
            Uart_Recv[Uart_Recv_Index++] = SBUF;
        }
        Uart_Flag = 1; 
        Uart_Timer = 0; // 只要有数据进来，就重置计时器（用于超时检测）
    }
}

/* 定时器0中断服务函数 */
void Timer0Sever() interrupt 1  //1ms
{
	//重装载（89C52）
	TL0 = 0x18;				//设置定时初始值
	TH0 = 0xFC;				//设置定时初始值

    if(++Key_Slow_Down == 10) Key_Slow_Down = 0;//按键轮询 10ms
	if(++Uart_Slow_Down == 200) Uart_Slow_Down = 0;//串口轮询 200ms

	// --- 必须补上这部分逻辑 ---
    if(Uart_Flag == 1) // 如果串口正在接收
    {
        if(++Uart_Timer >= 10) // 超过10ms没有新数据进来
        {
            Uart_Flag = 0;     // 判定为一帧接收结束
            Uart_Timer = 0;
        }
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
		Key_Proc();
    }
}


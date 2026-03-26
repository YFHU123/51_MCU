/* 头文件声明区 */
#include <reg52.h>
#include <intrins.h>
#include "Seg.h"
#include "Key.h"
#include "ds1307.h"

/* 变量声明区 */
unsigned char Seg_Slow_Down;//数码管减速
unsigned char Seg_Pos;//数码管扫描专用变量
unsigned char Seg_Buf[8] = {16,16,16,16,16,16,16,16};//数码管显示数组 默认熄灭
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};//数码管小数点显示数组 默认熄灭

/* 时间 */
unsigned char ucRtc[3] = {0x13, 0x59, 0x50}; // 初始化时间13:59:50

/* 信息处理函数 */
void Seg_Proc()
{
    if (Seg_Slow_Down) return;
    Seg_Slow_Down = 1;

    Read_Rtc(ucRtc);//读取时间到 ucRtc 数组

    //BCD转换为十进制显示
    Seg_Buf[0] = ucRtc[0] / 16;
    Seg_Buf[1] = ucRtc[0] % 16;
    Seg_Buf[2] = 17; //-
    Seg_Buf[3] = ucRtc[1] / 16;
    Seg_Buf[4] = ucRtc[1] % 16;
    Seg_Buf[5] = 17; //-
    Seg_Buf[6] = ucRtc[2] / 16;
    Seg_Buf[7] = ucRtc[2] % 16;
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
    ET0 = 1;				//打开定时器0中断
    EA = 1;					//打开总中断
}

/* 定时器0中断服务函数 */
void Timer0Sever() interrupt 1
{
    TL0 = 0x18;				//设置定时初始值
	TH0 = 0xFC;				//设置定时初始值

    if(++Seg_Slow_Down == 200) Seg_Slow_Down = 0;//按键轮询 10ms
    if(++Seg_Pos == 8) Seg_Pos = 0;//0-7之间循环扫描

    Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);//动态显示
}



/* 主函数 */
void main()
{
    Timer0_Init();//定时器0初始化
    Set_Rtc(ucRtc); // 设置初始时间
    while(1)
    {
        Seg_Proc();
    }
}
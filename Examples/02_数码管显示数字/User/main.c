/* 头文件声明区 */
#include <reg52.h>
#include <intrins.h>
#include "Seg.h"


/* 变量声明区 */
unsigned char Seg_Slow_Down;                           // 数码管减速
unsigned char Seg_Pos;                                 // 数码管扫描专用变量
unsigned char Seg_Buf[8] = {1, 2, 3, 4, 5, 6, 7, 8};   // 数码管显示数组 默认熄灭
unsigned char Seg_Point[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // 数码管小数点显示数组 默认熄灭

/* 信息处理函数 */
void Seg_Proc()
{
}

/* 定时器0初始化函数 */
void Timer0_Init(void) // 1毫秒@12.000MHz
{
    TMOD &= 0xF0; // 设置定时器模式
    TMOD |= 0x01; // 设置定时器模式
    TL0 = 0x18;   // 设置定时初始值
    TH0 = 0xFC;   // 设置定时初始值
    TF0 = 0;      // 清除TF0标志
    TR0 = 1;      // 定时器0开始计时
    ET0 = 1;      // 打开定时器0中断
    EA = 1;       // 打开总中断
}

/* 定时器0中断服务函数 */
void Timer0Sever() interrupt 1
{
    TL0 = 0x18; // 设置定时初始值
    TH0 = 0xFC; // 设置定时初始值

    if (++Seg_Slow_Down == 200)
        Seg_Slow_Down = 0; // 数码管扫描 200ms
    if (++Seg_Pos == 8)
        Seg_Pos = 0; // 0-7之间循环扫描

    Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos], Seg_Point[Seg_Pos]); // 动态显示
}

/* 主函数 */
void main()
{
    Timer0_Init(); // 定时器0初始化
    while (1)
    {
        Seg_Proc();
    }
}
#include <reg52.h>
#include <intrins.h>
#include "Seg.h"
#include "onewire.h"

/* 变量声明区 */
unsigned char Seg_Slow_Down; // 数码管减速
unsigned char Seg_Pos;       // 数码管扫描专用变量
unsigned char Seg_Buf[8] = {16,16,16,16,16,16,16,16}; // 默认熄灭
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};      // 默认熄灭

/* 温度 */
float t;

/* 信息处理函数 */
void Seg_Proc()
{
    if(Seg_Slow_Down) return; 
    Seg_Slow_Down = 0;

    t = Read_Temperature();
    
    Seg_Buf[0] = (unsigned char)t / 10 % 10;
    Seg_Buf[1] = (unsigned char)t % 10;
    Seg_Point[1] = 1; // 小数点
    Seg_Buf[2] = (unsigned char)(t * 10) % 10;
}

/* 定时器0初始化函数 */
void Timer0_Init(void)      // 1毫秒@12.000MHz
{
    // AUXR &= 0x7F;           // 12T模式
    TMOD &= 0xF0;           
    TMOD |= 0x01;           
    TL0 = 0x18;             
    TH0 = 0xFC;             
    TF0 = 0;                
    TR0 = 1;                
    ET0 = 1;                
    EA = 1;                 
}

/* 定时器0中断服务函数 */
void Timer0Sever() interrupt 1
{
    TL0 = 0x18;             
    TH0 = 0xFC;             

    if(++Seg_Slow_Down == 200) Seg_Slow_Down = 0;//数码管扫描 200ms
    if(++Seg_Pos == 8) Seg_Pos = 0;
    Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos], Seg_Point[Seg_Pos]);
}

/* 主函数 */
void main()
{
    Timer0_Init();
    while(1)
    {
        Seg_Proc();
    }
}
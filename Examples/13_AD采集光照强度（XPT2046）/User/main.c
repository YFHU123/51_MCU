#include <reg52.h>
#include <intrins.h>
#include "Seg.h"
#include "XPT2046.h"

/* 变量声明区 */
unsigned char Seg_Slow_Down; // 数码管减速
unsigned char Seg_Pos;       // 数码管扫描专用变量
unsigned char Seg_Buf[8] = {16,16,16,16,16,16,16,16}; // 默认熄灭
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};      // 默认熄灭

/* AD 相关变量 */
unsigned int adc_value; // AD转换后的数字量

/* 数据处理函数 */
void Seg_Proc()
{
    if(Seg_Slow_Down) return; 
    Seg_Slow_Down = 0;

    adc_value = XPT2046_ReadAD(XPT_GR);//0-255

    Seg_Buf[5] = adc_value / 100;      // 百位
    Seg_Buf[6] = adc_value / 10 % 10;  // 十位
    Seg_Buf[7] = adc_value % 10;       // 个位
    
    // 消隐高位 0
    if(Seg_Buf[5] == 0) 
    {
        Seg_Buf[5] = 16; 
        
        // 处理十位：只有在百位已经熄灭的前提下，十位是 0 才能熄灭
        if(Seg_Buf[6] == 0) 
        {
            Seg_Buf[6] = 16;
        }
    }
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
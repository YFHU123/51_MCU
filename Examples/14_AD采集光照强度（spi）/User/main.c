#include <reg52.h>
#include <intrins.h>
#include "Seg.h"
#include "spi.h"

/* 变量声明区 */
unsigned char Seg_Slow_Down; // 数码管减速
unsigned char Seg_Pos;       // 数码管扫描专用变量
unsigned char Seg_Buf[8] = {16,16,16,16,16,16,16,16}; // 默认熄灭
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};      // 默认熄灭

/* AD 相关变量 */
unsigned int adc_value;     // 存储原始的 12 位 AD 值 (0-4095)
unsigned char light_percent; // 存储转换后的百分比 (0-100)

/* 信息处理函数 */
void Seg_Proc()
{
    if(Seg_Slow_Down) return; 
    Seg_Slow_Down = 0;

    adc_value = SPI_Read_AD(CH_PHOTO); 
    
    //  将 12 位采样值转换为光照百分比
    // 公式逻辑：(1 - 采样值/4095) * 100
    // 注意：光敏电阻通常是光线越强，分压越低，所以用 1 减去它，实现光越强百分比越高
    light_percent = (unsigned int)((1.0f - (float)adc_value / 4095.0f) * 100);


    Seg_Buf[5] = light_percent / 100;      // 百位 (最高 100)
    Seg_Buf[6] = light_percent / 10 % 10;  // 十位
    Seg_Buf[7] = light_percent % 10;       // 个位

    if(Seg_Buf[5] == 0) Seg_Buf[5] = 16;
    if(Seg_Buf[6] == 0) Seg_Buf[6] = 16;

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
#include <reg52.h>
#include <intrins.h>
#include "LCD1602.h"
#include "onewire.h"
#include <stdio.h>

/* 变量声明区 */
float t;
unsigned char display_buf[16]; 

/* 信息处理函数 */
void LCD_Proc()
{
    t = Read_Temperature();

    sprintf(display_buf, "Temp: %.1f C", t);
    LCD_ShowString(1, 1, display_buf);
}

/* 定时器0初始化函数 */
void Timer0_Init(void) // 1毫秒@12.000MHz
{
    TMOD &= 0xF0;           
    TMOD |= 0x01;           
    TL0 = 0x18;             
    TH0 = 0xFC;             
    TR0 = 1;                
    ET0 = 1;                
    EA = 1;                 
}

/* 定时器0中断服务函数 */
void Timer0Sever() interrupt 1
{
    TL0 = 0x18;             
    TH0 = 0xFC;             

}

/* 主函数 */
void main()
{
    Timer0_Init();
    LCD_Init();      // 初始化 LCD
    LCD_Clear();     // 清屏
    
    while(1)
    {
        LCD_Proc();
    }
}
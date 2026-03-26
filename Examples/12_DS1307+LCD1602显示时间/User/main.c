/* 头文件声明区 */
#include <reg52.h>
#include <intrins.h>
#include "Seg.h"
#include "Key.h"
#include "ds1307.h"
#include "LCD1602.h"

/* 变量声明区 */


/* 时间 */
unsigned char ucRtc[3] = {0x13, 0x59, 0x50}; // 初始化时间13:59:50

/* 信息处理函数 */
void LCD_Proc()
{
    Read_Rtc(ucRtc);//读取时间到 ucRtc 数组

    LCD_ShowString(1, 1, "Real Time Clock");
    LCD_ShowNum(2, 5, ucRtc[0] / 16, 1);
    LCD_ShowNum(2, 6, ucRtc[0] % 16, 1);
    LCD_ShowChar(2, 7, ':');
    LCD_ShowNum(2, 8, ucRtc[1] / 16, 1);
    LCD_ShowNum(2, 9, ucRtc[1] % 16, 1);
    LCD_ShowChar(2, 10, ':');
    LCD_ShowNum(2, 11, ucRtc[2] / 16, 1);
    LCD_ShowNum(2, 12, ucRtc[2] % 16, 1);
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
}



/* 主函数 */
void main()
{
    LCD_Init();//初始化LCD
    Timer0_Init();//定时器0初始化
    Set_Rtc(ucRtc); // 设置初始时间
    while(1)
    {
        LCD_Proc();
    }
}
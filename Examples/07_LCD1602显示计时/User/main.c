/* 头文件声明区 */
#include <reg52.h>
#include "LCD1602.h"

/* 变量声明区 */
int timer = 0;
int cnt = 0; 


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
    
    if(++timer == 1000)// 1s
    {
        cnt++;
        timer = 0; 
    } 
    

}

void main()
{

    LCD_Init();
    Timer0_Init();
    LCD_ShowString(1, 1, "TIME COUNT:");
 
    while(1)
    {
        LCD_ShowNum(2, 1, cnt, 5);
    }    
}
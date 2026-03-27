/* 头文件声明区 */
#include <reg52.h>
#include <intrins.h>
#include "Key.h"
#include "AT24C02.h"
#include "LCD1602.h"

/*引脚定义 */
sbit LED1 = P1^0;

/* 变量声明区 */
unsigned char Key_Slow_Down;//按键减速
unsigned char Key_Val,Key_Old,Key_Down,Key_Up;

unsigned char KeyNum;
unsigned int Data;

unsigned int Lcd_Msg_Timer = 0; // 用于控制第二行提示信息的显示时间

bit Lcd_Update_Flag = 0;

/* 信息处理函数 */
void LED_Proc()
{ 
}

void Delay(unsigned int xms)
{
	unsigned char i, j;
	while(xms--)
	{
		i = 2;
		j = 239;
		do
		{
			while (--j);
		} while (--i);
	}
}


/* 信息处理函数  */
void Lcd_Proc()
{
    // 逻辑1：刷新第一行的 Data（只有按键按下时执行）
    if(Lcd_Update_Flag) 
    {
        Lcd_Update_Flag = 0; 
        LCD_ShowNum(1, 6, Data, 5); 
    }

    // 逻辑2：刷新第二行的提示信息（独立判断，不被 Flag 阻塞）
    if(Lcd_Msg_Timer == 1) 
    {
        // 这里不需要 Lcd_Update_Flag，因为它是由时间触发的
        LCD_ShowString(2, 1, "        "); 
        Lcd_Msg_Timer = 0;
    }
}

/* 按键处理函数 */
void Key_Proc()
{
    if(Key_Slow_Down) return;
    Key_Slow_Down = 1;
 
    Key_Val = Key_Read();
    Key_Down = Key_Val & (Key_Old ^ Key_Val);
    Key_Old = Key_Val;

    if(Key_Down > 0)
    {
        Lcd_Update_Flag = 1; // 只要有键按下，就允许刷新一次
        switch(Key_Down)
        {
            case 1: Data--; break;
            case 2: Data++; break;
            case 3: // 数据保存
            AT24C02_WriteByte(0, Data % 256);
            Delay(5); 
            AT24C02_WriteByte(1, Data / 256);
            LCD_ShowString(2, 1, "Write OK");
            Lcd_Msg_Timer = 6000; // 设置显示时间为 2000ms (2秒)
            break;

        case 4: // 数据读取
            Data = AT24C02_ReadByte(0);
            Data |= (unsigned int)AT24C02_ReadByte(1) << 8;
            LCD_ShowString(2, 1, "Read OK ");
            Lcd_Msg_Timer = 6000;
            Lcd_Update_Flag = 1; // 确保读取后立即刷新第一行数字
            break;
        }
    }
}
 
/* 定时器0初始化函数 */
void Timer0_Init(void)		//1毫秒@12.000MHz
{
    TMOD &= 0xF0;			//设置定时器模式
    TL0 = 0x20;				//设置定时初始值
    TH0 = 0xD1;				//设置定时初始值
    TF0 = 0;				//清除TF0标志
    TR0 = 1;				//定时器0开始计时
    ET0 = 1;
    EA = 1;
}
 
/* 定时器0中断服务函数 */
void Timer0Sever() interrupt 1
{
    //重装载（89C52）
	TL0 = 0x18;				//设置定时初始值
	TH0 = 0xFC;				//设置定时初始值

    if(++Key_Slow_Down == 10) Key_Slow_Down = 0;//按键轮询 10ms

    if(Lcd_Msg_Timer > 0) Lcd_Msg_Timer--; // 倒计时，显示时间到后清空提示信息
}

/* 主函数 */
void main(void)
{
    LCD_Init();
    Timer0_Init();
    
    // // 开机自动读取一次 EEPROM 中的历史数据
    // Data = AT24C02_ReadByte(0);
    // Data |= (unsigned int)AT24C02_ReadByte(1) << 8;
    
    // 初始显示
    LCD_ShowString(1, 1, "Data:");
    LCD_ShowNum(1, 6, Data, 5); 
    
    while(1)
    {
        Key_Proc();           
        Lcd_Proc(); // 必须放在这里轮询
    }
}


#include <reg52.h>
#include <intrins.h>
#include "Key.h"
#include "Seg.h"
#include "LCD1602.h"

/*引脚定义 */
sbit LED1 = P1^0;

/* LED 模式定义 */
#define MODE_OFF    0
#define MODE_FLOW   1  
#define MODE_BLINK  2  

/* 变量声明区 */
unsigned char LED_Mode = MODE_OFF;
unsigned int  LED_Timer = 0;   
unsigned char LED_Data = 0xFE; 

unsigned char Password[] = {1, 2, 3, 4, 5, 6}; 
unsigned char InputBuf[6];               // 缓冲区也改为6
unsigned char InputCount = 0;            
unsigned char Lock_State = 0;            // 0:输入中, 1:成功, 2:失败
unsigned char Setting_Mode = 0;          // 0:普通模式, 1:修改密码模式
unsigned char Lcd_Update_Flag = 1;

unsigned char Key_Slow_Down;
unsigned char Key_Val, Key_Old, Key_Down;

/* 密码比对函数 */
void Check_Password()
{
    unsigned char i;
    for(i=0; i<6; i++)
    {
        if(InputBuf[i] != Password[i])
        {
            Lock_State = 2; 
            LED_Mode = MODE_BLINK;
            return;
        }
    }
    Lock_State = 1; 
    LED_Mode = MODE_FLOW;
}

/* 信息处理函数  */
void Lcd_Proc()
{
    unsigned char i;
    if(!Lcd_Update_Flag) return; //只有按键按下，LCD才会刷新
    Lcd_Update_Flag = 0; 
    
    if(Setting_Mode == 1) 
    {
        LCD_ShowString(1, 1, "New Password(6):"); 
        for(i=0; i<6; i++)
        {
            LCD_ShowChar(2, i+1, (InputCount >= i+1) ? '*' : '-');
        }
        LCD_ShowString(2, 8, "Save:S16");
    }
    else
    {
        LCD_ShowString(1, 1, "New password(6):");
        if(Lock_State == 0)
        {
            for(i=0; i<6; i++)
            {
                LCD_ShowChar(2, i+1, (InputCount >= i+1) ? '*' : ' ');
            }
            LCD_ShowString(2, 7, "          ");
        }
        else if(Lock_State == 1) // 成功
        {
            LCD_ShowString(2, 1, "OK! Unlocked    ");
        }
        else if(Lock_State == 2) // 失败
        {
            LCD_ShowString(2, 1, "Error! Retry    ");
        }
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
        Lcd_Update_Flag = 1;

        if(Key_Down <= 10 && InputCount < 6)
        {
            if(Setting_Mode == 0 && Lock_State != 0) {
                InputCount = 0;
                Lock_State = 0;
            }
            InputBuf[InputCount] = (Key_Down == 10) ? 0 : Key_Down; 
            InputCount++;
        }
        // S11: 确认键
        else if(Key_Down == 11)
        {
            // --- 修改点5：必须输满6位才能确认 ---
            if(Setting_Mode == 0 && InputCount == 6) Check_Password();
        }
        // S12: 清除键
        else if(Key_Down == 12)
        {
            InputCount = 0;
            Lock_State = 0;
            Setting_Mode = 0;
            LED1 = 1;
            LED_Mode = MODE_OFF;
        }
        // S16: 设置键
        else if (Key_Down == 16) 
        {
            if(Lock_State == 1) 
            {
                if(Setting_Mode == 0) 
                {
                    Setting_Mode = 1; 
                    InputCount = 0;   
                }
                else 
                {
                    // --- 新密码也必须是6位才能保存 ---
                    if(InputCount == 6)
                    {
                        unsigned char i;
                        for(i=0; i<6; i++) Password[i] = InputBuf[i];
                        Setting_Mode = 0;
                        Lock_State = 0;
                        InputCount = 0;
                        LED_Mode = MODE_OFF;
                        LCD_ShowString(1, 1, "Save Success!   "); 
                    }
                }
            }
            else 
            {
                LCD_ShowString(1, 1, "Please Unlock!  ");
            }
        }
    }
}

/* 指示灯处理函数 */
void LED_Proc()
{
    switch(LED_Mode)
    {
        case MODE_FLOW:
            if(LED_Timer >= 200) 
            {
                LED_Timer = 0;
                LED_Data = _cror_(LED_Data, 1); 
                P1 = LED_Data;
            }
            break;
        case MODE_BLINK:
            if(LED_Timer >= 300)
            {
                LED_Timer = 0;
                P1 = ~P1; 
            }
            break;
        default:
            P1 = 0xFF; 
            LED_Timer = 0;
            break;
    }
}

/* 定时器0初始化 */
void Timer0_Init(void)
{
    TMOD &= 0xF0;
    TL0 = 0x20;
    TH0 = 0xD1; 
    TF0 = 0;
    TR0 = 1;
    ET0 = 1;
    EA = 1;
}

/* 定时器0中断 */
void Timer0Sever() interrupt 1
{
    TL0 = 0x18;
    TH0 = 0xFC; 

    if(++Key_Slow_Down >= 10) Key_Slow_Down = 0; 
    LED_Timer++;
}

void main(void)
{
    Timer0_Init();    
    LCD_Init();        
    while(1)
    {
        Key_Proc();           
        Lcd_Proc();
        LED_Proc();
    }
}
#include <reg52.h>
#include "LCD1602.h"


void main()
{

    LCD_Init();
    LCD_ShowString(1, 1, "Hello World!"); 
    LCD_ShowNum(2, 1, 12345,5);

    while(1)
    {
    }
}
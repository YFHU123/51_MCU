#include <intrins.h>
#include <reg52.h>

sbit LED = P1^0;

void Delay(unsigned int t)
{
	while(t--);
}


void LED_Proc()
{
    unsigned char Time,i;
    
    for(Time=0;Time<100;Time++)		//改变亮灭时间，由暗到亮
    {
        for(i=0;i<20;i++)			//计次延时
        {
            LED=0;					//LED亮
            Delay(Time);			//延时Time
            LED=1;					//LED灭
            Delay(100-Time);		//延时100-Time
        }
    }
    for(Time=100;Time>0;Time--)		//改变亮灭时间，由亮到暗
    {
        for(i=0;i<20;i++)			//计次延时
        {
            LED=0;					//LED亮
            Delay(Time);			//延时Time
            LED=1;					//LED灭
            Delay(100-Time);		//延时100-Time
        }
    }
}


void main()
{
    while (1)
    {
        LED_Proc();
    }
}



#include <Uart.h>
#include <stdio.h>//标准库

/* 串口初始化函数 */
void UartInit(void)     // 9600bps @ 12.000MHz
{
    SCON = 0x50;        // 8位数据, 可变波特率
    TMOD &= 0x0F;       // 清除定时器1设置
    TMOD |= 0x20;       // 设置定时器1为8位自动重装模式
    
    // 波特率计算公式: TH1 = 256 - (晶振频率 / 12 / 32 / 波特率)
    // 12MHz 下 9600 波特率误差较大，建议使用 0xFD
    TH1 = 0xFD;         
    TL1 = 0xFD;         
    
    TR1 = 1;            // 启动定时器1
    ES = 1;             // 开启串口中断
    EA = 1;             // 开启总中断
}
/* 字节发送函数 */
void SendByte(unsigned char dat)
{
	SBUF=dat;//将dat数据赋给SBUF，将数据发送出去
	while(TI == 0);//等待数据发送
	TI = 0;//将发送标志位清零
}

/* 字符串发送函数 */
void Uart_Send_String(unsigned char *dat)
{
	while(*dat != '\0')//当字符不为空时，继续发送
		SendByte(*dat++);//发送后指针dat加1，指向下一个字节
}

/* 重写 putchar 函数 */
char putchar(char c)
{
    SendByte(c); // 调用你写好的字节发送函数
    return c;
}
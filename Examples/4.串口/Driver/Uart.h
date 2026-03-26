#include <reg52.h>

void UartInit(void);		//9600bps@12.000MHz
void SendByte(unsigned char dat);
void Uart_Send_String(unsigned char *dat);

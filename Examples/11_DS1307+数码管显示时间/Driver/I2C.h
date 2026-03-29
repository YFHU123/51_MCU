#include <reg52.h>

void I2C_Start();
void I2C_Stop();
void I2C_SendByte(unsigned char dat);
unsigned char I2C_RecvByte();
void I2C_SendAck(unsigned char AckBit);
unsigned char I2C_ReceiveAck(void);
#include "spi.h"
#include <intrins.h>

// 引脚定义
sbit CS   = P2^2;
sbit DCLK = P2^1;
sbit DIN  = P2^0;
sbit DOUT = P2^4;

// 内部字节写入函数 (模拟 SPI 总线)
void SPI_WriteByte(unsigned char dat)
{
    unsigned char i;
    for(i=0; i<8; i++)
    {
        DCLK = 0;
        DIN = dat & 0x80; // 发送最高位
        dat <<= 1;
        DCLK = 1;         // 上升沿采样
    }
}

// 内部数据读取函数 (12位采样：0-4096)
unsigned int SPI_ReadData()
{
    unsigned char i;
    unsigned int dat = 0;
    for(i=0; i<12; i++)
    {
        dat <<= 1;
        DCLK = 1;
        DCLK = 0;         // 下降沿输出
        if(DOUT) dat |= 0x01;
    }
    return dat;
}

// 对外统一接口
unsigned int SPI_Read_AD(unsigned char cmd)
{
    unsigned int temp;
    CS = 0;               // 选中 XPT2046
    SPI_WriteByte(cmd);   // 发送控制字
    DCLK = 0;             // 转换等待
    _nop_(); _nop_();
    temp = SPI_ReadData(); // 读取12位结果
    CS = 1;               // 释放总线
    return temp;
}
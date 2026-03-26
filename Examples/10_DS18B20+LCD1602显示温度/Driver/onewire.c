#include "onewire.h"
#include <intrins.h>

// 由于 reg52.h 没有 P4，必须手动声明
sfr P4 = 0xe8;
sbit DS = P4^6; 

void Delay_us(unsigned char us)
{
    while(us--);    
}

bit ds_init()
{
    bit i;
    EA = 0;        // 通信期间关闭中断，防止闪烁和读数错误
    DS = 1;
    _nop_();
    DS = 0;
    Delay_us(75);  // 复位脉冲
    DS = 1; 
    Delay_us(4);   // 等待响应
    i = DS;        
    Delay_us(20); 
    DS = 1;
    EA = 1;        // 恢复中断
    return i;
}

void write_byte(unsigned char dat)
{
    unsigned char i;
    for(i=0; i<8; i++)
    {
        EA = 0;
        DS = 0;
        _nop_();
        DS = dat & 0x01;
        Delay_us(10);
        DS = 1; 
        EA = 1;
        dat >>= 1;
    }
}

unsigned char read_byte()
{
    unsigned char i, j, dat = 0;
    for(i=0; i<8; i++)
    {
        EA = 0;
        DS = 0;
        _nop_();
        DS = 1;
        _nop_();
        j = DS;
        Delay_us(10);
        DS = 1;
        EA = 1;
        dat = (j<<7)|(dat>>1); // 厂家低位在前逻辑
    }
    return dat;
}

float Read_Temperature()
{
    unsigned char L, M;
    unsigned int temp;
    
    if(ds_init()) return 0.0;
    
    write_byte(0xcc);
    write_byte(0x44); // 开启转换
    
    ds_init();
    write_byte(0xcc);
    write_byte(0xbe); // 读暂存器
    L = read_byte();
    M = read_byte();
    
    temp = (M << 8) | L;
    return temp * 0.0625;
}
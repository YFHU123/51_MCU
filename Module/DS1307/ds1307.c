#include <intrins.h>
#include "ds1307.h"

//引脚定义
sbit SCL = P2^1; 
sbit SDA = P2^0;

/* IIC 总线基础信号 */
void IIC_Start() {
    SDA = 1; SCL = 1; _nop_();
    SDA = 0; _nop_();
    SCL = 0;
}

void IIC_Stop() {
    SDA = 0; SCL = 1; _nop_();
    SDA = 1; _nop_();
}

void IIC_SendByte(unsigned char dat) {
    unsigned char i;
    for(i=0; i<8; i++) {
        SDA = dat >> 7; // IIC 高位在前
        dat <<= 1;
        SCL = 1; _nop_(); SCL = 0;
    }
    SDA = 1; SCL = 1; _nop_(); SCL = 0; // 释放 SDA 等待应答
}

unsigned char IIC_RecvByte() {
    unsigned char i, dat = 0;
    SDA = 1; 
    for(i=0; i<8; i++) {
        SCL = 1; _nop_();
        dat <<= 1;
        if(SDA) dat |= 0x01;
        SCL = 0;
    }
    return dat;
}

/* DS1307 读字节 */
void Write_Ds1307_Byte(unsigned char addr, unsigned char dat) {
    IIC_Start();
    IIC_SendByte(0xD0); // DS1307 写地址
    IIC_SendByte(addr); 
    IIC_SendByte(dat);  
    IIC_Stop();
}

/* DS1307 写字节 */
unsigned char Read_Ds1307_Byte(unsigned char addr) {
    unsigned char dat;
    IIC_Start();
    IIC_SendByte(0xD0); 
    IIC_SendByte(addr);
    IIC_Start();        // 重启总线进行读取
    IIC_SendByte(0xD1); // DS1307 读地址
    dat = IIC_RecvByte();
    IIC_Stop();
    return dat;
}

/* 设置时间：数组格式为 [时, 分, 秒]，必须是 BCD 码 */
void Set_Rtc(unsigned char *ucRtc) {
    Write_Ds1307_Byte(0x02, ucRtc[0]); // 时
    Write_Ds1307_Byte(0x01, ucRtc[1]); // 分
    Write_Ds1307_Byte(0x00, ucRtc[2]); // 秒 
}

/* 读取时间：数组格式为 [时, 分, 秒] */
void Read_Rtc(unsigned char *ucRtc) {
    ucRtc[0] = Read_Ds1307_Byte(0x02); // 时
    ucRtc[1] = Read_Ds1307_Byte(0x01); // 分
    ucRtc[2] = Read_Ds1307_Byte(0x00); // 秒
}
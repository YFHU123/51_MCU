#include <intrins.h>
#include "ds1307.h"
#include "I2C.h"


/**
 * @brief 写入 DS1307 寄存器
 * @param addr 寄存器地址
 * @param dat 数据
 */
void Write_Ds1307_Byte(unsigned char addr, unsigned char dat) {
    I2C_Start();
    I2C_SendByte(0xD0);     // 设备地址
    I2C_ReceiveAck();       // 必须接收应答
    
    I2C_SendByte(addr);     // 寄存器地址
    I2C_ReceiveAck();       // 必须接收应答
    
    I2C_SendByte(dat);      // 写入数据
    I2C_ReceiveAck();       // 必须接收应答
    
    I2C_Stop();
}

/**
 * @brief 读取 DS1307 寄存器
 * @param addr 寄存器地址
 * @return 数据
 */
unsigned char Read_Ds1307_Byte(unsigned char addr) {
    unsigned char dat;
    I2C_Start();
    I2C_SendByte(0xD0); 
    if(I2C_ReceiveAck()) return 0; // 如果没应答，直接返回
    
    I2C_SendByte(addr);
    I2C_ReceiveAck();
    
    I2C_Start();        // 重启总线 (Restart)
    I2C_SendByte(0xD1); 
    I2C_ReceiveAck();
    
    dat = I2C_RecvByte();
    I2C_SendAck(1);     // 【关键】读取最后一个字节后发送 NACK (1)
    I2C_Stop();
    return dat;
}

/**
 * @brief 设置 DS1307 时间
 * @param ucRtc 时间数组，格式为 [时, 分, 秒]，必须是 BCD 码
 */
void Set_Rtc(unsigned char *ucRtc) {
    Write_Ds1307_Byte(0x02, ucRtc[0]); // 时
    Write_Ds1307_Byte(0x01, ucRtc[1]); // 分
    Write_Ds1307_Byte(0x00, ucRtc[2]); // 秒 
}

/**
 * @brief 读取 DS1307 时间
 * @param ucRtc 时间数组，格式为 [时, 分, 秒]，必须是 BCD 码
 */
void Read_Rtc(unsigned char *ucRtc) {
    ucRtc[0] = Read_Ds1307_Byte(0x02); // 时
    ucRtc[1] = Read_Ds1307_Byte(0x01); // 分
    ucRtc[2] = Read_Ds1307_Byte(0x00); // 秒
}
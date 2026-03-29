#include "I2C.h"
#include <intrins.h>

#define I2C_Delay() {_nop_(); _nop_(); _nop_(); _nop_();}

//引脚定义
sbit I2C_SCL = P2^1; 
sbit I2C_SDA = P2^0;


/**
 * @brief 发送 I2C 开始信号
 */
void I2C_Start() {
	I2C_SDA=1;
	I2C_SCL=1;
    I2C_Delay();
	I2C_SDA=0;
    I2C_Delay();
	I2C_SCL=0;
}

/**
 * @brief 发送 I2C 停止信号
 */
void I2C_Stop() {
	I2C_SDA=0;
	I2C_SCL=1;
    I2C_Delay();
	I2C_SDA=1;
    I2C_Delay();
}

/**
 * @brief 发送 I2C 字节
 * @param dat 要发送的字节
 */
void I2C_SendByte(unsigned char dat) {
    unsigned char i;
    for(i=0; i<8; i++) {
        I2C_SDA = dat >> 7; // I2C 高位在前
        dat <<= 1;
        I2C_Delay();
        I2C_SCL = 1;
        I2C_Delay();
        I2C_SCL = 0;
    }
}

/**
 * @brief 读取 I2C 字节
 * @return 读取到的字节
 */
unsigned char I2C_RecvByte() {
    unsigned char i, dat = 0;
    I2C_SDA = 1; 
    for(i=0; i<8; i++) {
        I2C_Delay();
        I2C_SCL = 1;
        I2C_Delay();
        dat <<= 1;
        if(I2C_SDA) dat |= 0x01;
        I2C_SCL = 0;
    }
    return dat;
}

/**
 * @brief 发送 I2C 应答
 * @param AckBit 0为应答，1为非应答
 */
void I2C_SendAck(unsigned char AckBit) {
    I2C_SDA = AckBit;  // 0为应答，1为非应答
    I2C_Delay();
    I2C_SCL = 1;
    I2C_Delay();
    I2C_SCL = 0;
}

/**
 * @brief 读取 I2C 应答
 * @return 读取到的应答位
 */
unsigned char I2C_ReceiveAck(void) {
    unsigned char AckBit;
    I2C_SDA = 1;          // 主机释放 SDA 控制权
    I2C_Delay();
    I2C_SCL = 1;          // 时钟拉高
    I2C_Delay();          //给从机一点时间把 SDA 拉低
    AckBit = I2C_SDA;     // 读取从机状态
    I2C_SCL = 0;          // 结束应答周期
    return AckBit;
}
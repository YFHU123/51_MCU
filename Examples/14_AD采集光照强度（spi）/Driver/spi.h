#include <reg52.h>

// 常用通道指令封装
#define CH_PHOTO  0x94  // 光敏电阻 (AIN0)
#define CH_NTC    0xd4  // 热敏电阻 (AIN1)
#define CH_EXT    0xa4  // 外部输入 (AIN2)
#define CH_POT    0xe4  // 电位器   (AIN3)

// 函数声明
unsigned int SPI_Read_AD(unsigned char cmd);

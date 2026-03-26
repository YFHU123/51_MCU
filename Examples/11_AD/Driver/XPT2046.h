#include <reg52.h>

#define XPT_GR   0x94  // AIN0: 光敏电阻 
#define XPT_NTC  0xD4 // AIN1: NTC 热敏电阻  
#define XPT_VBAT 0xAC  // AIN2:  VBAT外部输入电压
#define XPT_POT  0xE4 // AIN3: 可调电阻电位器

unsigned int XPT2046_ReadAD(unsigned char Command);


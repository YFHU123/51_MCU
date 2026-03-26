#include "Seg.h"

sbit DU = P2 ^ 6;  // 数码管段选控制
sbit WEI = P2 ^ 7; // 数码管位选控制

// 数码管段码表
unsigned char seg_duan[] = {
	0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, // 0-9
	0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71,							// 10-15 (A-F)
	0x00,														// 16 (全灭)
	0x40														// 17 (-)	
};

// 数码管位选表 (低电平有效)
unsigned char seg_wei[] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};//

// 数码管显示函数
void Seg_Disp(unsigned char wei, unsigned char duan, unsigned char point)
{
	P0 = 0x00; // 消隐
	DU = 1;
	DU = 0;

	P0 = seg_wei[wei];
	WEI = 1;
	WEI = 0;

	P0 = seg_duan[duan];
	if (point)
		P0 |= 0x80; // 将最高位置1
	DU = 1;
	DU = 0;
}
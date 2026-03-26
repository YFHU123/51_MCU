#include <Key.h>


//矩阵键盘
unsigned char Key_Read()
{
    unsigned char temp = 0;

    // --- 扫描第 1 列：P3.0 置低---
    P3 = 0xFE; //1111 1110
    if((P3 & 0x10) == 0) temp = 1;  // 检测 P3.4 (S1)
    if((P3 & 0x20) == 0) temp = 2;  // 检测 P3.5 (S2)
    if((P3 & 0x40) == 0) temp = 3;  // 检测 P3.6 (S3)
    if((P3 & 0x80) == 0) temp = 4;  // 检测 P3.7 (S4)

    // --- 扫描第 2 列：P3.1 置低---
    P3 = 0xFD; // 1111 1101
    if((P3 & 0x10) == 0) temp = 5;  // 检测 P3.4 (S5)
    if((P3 & 0x20) == 0) temp = 6;  // 检测 P3.5 (S6)
    if((P3 & 0x40) == 0) temp = 7;  // 检测 P3.6 (S7)
    if((P3 & 0x80) == 0) temp = 8;  // 检测 P3.7 (S8)

    // --- 扫描第 3 列：P3.2 置低---
    P3 = 0xFB; //1111 1011
    if((P3 & 0x10) == 0) temp = 9;  // 检测 P3.4 (S9)
    if((P3 & 0x20) == 0) temp = 10; // 检测 P3.5 (S10)
    if((P3 & 0x40) == 0) temp = 11; // 检测 P3.6 (S11)
    if((P3 & 0x80) == 0) temp = 12; // 检测 P3.7 (S12)

    // --- 扫描第 4 列：P3.3 置低---
    P3 = 0xF7; //1111 0111
    if((P3 & 0x10) == 0) temp = 13; // 检测 P3.4 (S13)
    if((P3 & 0x20) == 0) temp = 14; // 检测 P3.5 (S14)
    if((P3 & 0x40) == 0) temp = 15; // 检测 P3.6 (S15)
    if((P3 & 0x80) == 0) temp = 16; // 检测 P3.7 (S16)

    return temp;
}

// // 独立按键
// unsigned char Key_Read()
// {
//     unsigned char temp = 0;

//     if((P3 & 0x01) == 0) temp = 1; 
//     if((P3 & 0x02) == 0) temp = 2;
//     if((P3 & 0x04) == 0) temp = 3; 
//     if((P3 & 0x08) == 0) temp = 4;

//     return temp;
// }
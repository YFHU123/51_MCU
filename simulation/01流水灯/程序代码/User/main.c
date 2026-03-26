/* 头文件声明区 */
#include <intrins.h>
#include <reg52.h>

unsigned char tab[] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF,
                       0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD};


/**
 * @brief  粗略延时函数（教学/调试用）
 * @param  i 延时单位（约 1ms @12MHz，受编译器优化影响）
 * @warning 仅用于仿真观察或非精确时序场合。
 */
void Delay(unsigned int i) {
    unsigned char t;
    while (i--) {
        for (t = 0; t < 120; t++);
    }
}



void main() {
    unsigned char i;
    while (1) {
        for (i = 0; i < 14; i++) {
            P1 = tab[i];
            Delay(500);
        }
    }
}


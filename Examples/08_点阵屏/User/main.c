#include <reg52.h>
#include "Matrix.h"

extern unsigned char heart[];
extern unsigned char smile[];
extern unsigned char code Animation[];
extern unsigned char code string1[];

void main() {
    Init_74HC595(); 
    while(1) {
        MatrixLED_ShowFrame(smile);
        //MatrixLED_Scrolling(Animation, 48, 15);
    }
}
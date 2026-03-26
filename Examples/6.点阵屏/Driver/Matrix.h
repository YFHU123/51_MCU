#include <reg52.h>

void Init_74HC595();
void ShiftByte(unsigned char Byte);
void MatrixLED_ShowColumn(unsigned char Column, unsigned char Data);
void MatrixLED_ShowFrame(unsigned char *frame);
void MatrixLED_Scrolling(unsigned char *pArray, unsigned char ArrayLen, unsigned char Speed);
void Matrix16x16_ShowColumn(unsigned char Column, unsigned char DataH, unsigned char DataL);
void Matrix16x16_Scrolling(unsigned char *pArray, unsigned int TotalLen, unsigned char Speed);
void Matrix16x16_ScrollLeft(unsigned char *pArray, unsigned char NumOfChars, unsigned char Speed);
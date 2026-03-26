#include "Matrix.h"
#include "font.h"

sbit SER = P2^0;//数据输入
sbit SCK = P2^1;//时钟信号
sbit RCK = P2^2;//锁存信号
sbit EN = P1^7;//使能

// 定义底层静态变量，用于记录滚动状态
static unsigned char Offset = 0;// 滚动的偏移位置
static unsigned char Count = 0;// 速度计数器


void Init_74HC595() {
    EN = 0; 
    SCK = 0;
    RCK = 0;
}

/**
  * @brief 写入 74HC595 核心函数（不锁存，仅移位）
  */
void ShiftByte(unsigned char Byte) 
{
    unsigned char i;
    for(i=0; i<8; i++) {
        SER = (Byte & (0x80 >> i)) ? 1 : 0;
        SCK = 1; 
        SCK = 0;
    }
}

/**
  * @brief 双 74HC595 级联驱动函数
  * @param ColumnSelect 列选数据（阳极）
  * @param RowData 行数据（阴极，所以需要取反）
  */
void MatrixLED_ShowColumn(unsigned char Column, unsigned char Data) 
{
    unsigned char ColumnSelect = (0x80 >> Column);
    unsigned char RowData = ~Data;

    ShiftByte(RowData);      
    ShiftByte(ColumnSelect); 
    RCK = 1;RCK = 0;//锁存信号

   // Delay1ms(1); // 扫描暂留

    //消隐
    ShiftByte(0x00);
    ShiftByte(0x00);
    RCK = 1; RCK = 0;
}


/**
  * @brief  显示一帧数据的封装函数
  * @param  frame 指向8字节点阵数据的指针
  */
void MatrixLED_ShowFrame(unsigned char *frame)
{
    unsigned char i;
    for(i = 0; i < 8; i++)
    {
        MatrixLED_ShowColumn(i, frame[i]);
    }
}

/**
 * @brief  滚动显示指定数组内容
 * 
 * @param pArray 指向要滚动显示的数据数组
 * @param ArrayLen 数组长度
 * @param Speed 滚动速度（越大越慢）
 */
void MatrixLED_Scrolling(unsigned char *pArray, unsigned char ArrayLen, unsigned char Speed)
{
    unsigned char i, j;
    unsigned char Data, temp;
    
    for(i = 0; i < 8; i++)
    {
        Data = pArray[(7 - i) + Offset]; 
        temp = 0;
        for(j = 0; j < 8; j++)
        {
            if(Data & (0x01 << j)) temp |= (0x80 >> j);
        }
        MatrixLED_ShowColumn(i, temp);
    }

    Count++;
    if(Count > Speed)
    {
        Count = 0;
        Offset++; 
        
        if(Offset > (ArrayLen - 8))
        {
            Offset = 0; 
        }
    }
}

/**
  * @brief 16x16 点阵显示核心函数（自动修正左右反置与镜像）
  * @param Column 扫描行号 (0-15)
  * @param DataL  左半部分数据字节
  * @param DataH  右半部分数据字节
  */
void Matrix16x16_ShowColumn(unsigned char Column, unsigned char DataL, unsigned char DataH) 
{
    unsigned int RowSelect = ~(0x0001 << Column); 
    unsigned char i, tempL = 0, tempH = 0;

    // 将取模软件的高位在前转为低位在前，匹配 595 输出顺序
    for(i = 0; i < 8; i++) {
        if(DataL & (0x01 << i)) tempL |= (0x80 >> i);
        if(DataH & (0x01 << i)) tempH |= (0x80 >> i);
    }

    // 1. 发送行选高字节 (挤到 U5)
    ShiftByte((unsigned char)(RowSelect >> 8));   
    
    // 2. 发送行选低字节 (挤到 U4)
    ShiftByte((unsigned char)(RowSelect & 0xFF)); 

    // 3. 先发右半边数据 (挤到 U3)
    ShiftByte(tempH); 

    // 4. 后发左半边数据 (留在 U2)
    ShiftByte(tempL); 

    RCK = 1; RCK = 0; // 锁存显示

    // // --- 消隐处理 (解决重影) ---
    // ShiftByte(0xFF); ShiftByte(0xFF); // 行熄灭
    // ShiftByte(0x00); ShiftByte(0x00); // 列断电
    // RCK = 1; RCK = 0;
}

void Matrix16x16_Scrolling(unsigned char *pArray, unsigned int TotalLen, unsigned char Speed)
{
    static unsigned int ScrollOffset = 0; // 改为 int，防止多字滚动时超过 255
    static unsigned char SpeedCount = 0;   
    unsigned char i;
    unsigned int rowindex;

    for(i = 0; i < 16; i++)
    {
        // 关键：TotalLen / 2 是总行数（两个字就是 32 行）
        // 这里计算的是：当前显示的第 i 行，对应整个长数组中的哪一行
        rowindex = (i + ScrollOffset) % (TotalLen / 2);
        
        // 每一行取两个连续字节：rowindex*2 是左字节，rowindex*2+1 是右字节
        Matrix16x16_ShowColumn(i, pArray[rowindex * 2], pArray[rowindex * 2 + 1]);
    }

    SpeedCount++;
    if(SpeedCount > Speed)
    {
        SpeedCount = 0;
        ScrollOffset++; 
        
        if(ScrollOffset >= (TotalLen / 2)) 
        {
            ScrollOffset = 0; 
        }
    }
}

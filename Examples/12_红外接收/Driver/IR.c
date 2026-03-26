#include <reg52.h>
#include "Timer0.h"
#include "Int0.h"

unsigned int IR_Time;
unsigned char IR_State;

unsigned char IR_Data[4];
unsigned char IR_pData;

unsigned char IR_DataFlag;
unsigned char IR_RepeatFlag;
unsigned char IR_Address;
unsigned char IR_Command;

/**
  * @brief  红外遥控初始化
  * @param  无
  * @retval 无
  */
void IR_Init(void)
{
	Timer0_Init();
	Int0_Init();
}

/**
  * @brief  红外遥控获取收到数据帧标志位
  * @param  无
  * @retval 是否收到数据帧，1为收到，0为未收到
  */
unsigned char IR_GetDataFlag(void)
{
	if(IR_DataFlag)
	{
		IR_DataFlag=0;
		return 1;
	}
	return 0;
}

/**
  * @brief  红外遥控获取收到连发帧标志位
  * @param  无
  * @retval 是否收到连发帧，1为收到，0为未收到
  */
unsigned char IR_GetRepeatFlag(void)
{
	if(IR_RepeatFlag)
	{
		IR_RepeatFlag=0;
		return 1;
	}
	return 0;
}

/**
  * @brief  红外遥控获取收到的地址数据
  * @param  无
  * @retval 收到的地址数据
  */
unsigned char IR_GetAddress(void)
{
	return IR_Address;
}

/**
  * @brief  红外遥控获取收到的命令数据
  * @param  无
  * @retval 收到的命令数据
  */
unsigned char IR_GetCommand(void)
{
	return IR_Command;
}

//外部中断0中断函数，下降沿触发执行
void Int0_Routine(void) interrupt 0
{
    if(IR_State==0)             // 状态0：空闲
    {
        Timer0_SetCounter(0);   
        Timer0_Run(1);          
        IR_State=1;             
    }
    else if(IR_State==1)        // 状态1：等待Start或Repeat
    {
        IR_Time=Timer0_GetCounter();    
        Timer0_SetCounter(0);   
        
        // 判定 Start 信号 (标准值12442)
        if(IR_Time > 12442 - 500 && IR_Time < 12442 + 500)
        {
            IR_State=2;         
        }
        // 判定 Repeat 信号 (标准值10368)
        else if(IR_Time > 10368 - 500 && IR_Time < 10368 + 500)
        {
            IR_RepeatFlag=1;    
            Timer0_Run(0);      
            IR_State=0;         
        }
        else                    
        {
            IR_State=1;         
        }
    }
    else if(IR_State==2)        // 状态2：接收数据
    {
        IR_Time=Timer0_GetCounter();    
        Timer0_SetCounter(0);   
        
        // 判定数据 "0" (标准值1032)
        if(IR_Time > 1032 - 500 && IR_Time < 1032 + 500)
        {
            IR_Data[IR_pData/8] &= ~(0x01 << (IR_pData%8)); 
            IR_pData++;         
        }
        // 判定数据 "1" (标准值2074)
        else if(IR_Time > 2074 - 500 && IR_Time < 2074 + 500)
        {
            IR_Data[IR_pData/8] |= (0x01 << (IR_pData%8));  
            IR_pData++;         
        }
        else                    
        {
            IR_pData=0;         
            IR_State=1;         
        }

        if(IR_pData >= 32)      
        {
            IR_pData=0;         
            // 数据校验：地址码反码与数据码反码校验
            if((IR_Data[0] == ~IR_Data[1]) && (IR_Data[2] == ~IR_Data[3]))  
            {
                IR_Address = IR_Data[0];  
                IR_Command = IR_Data[2];
                IR_DataFlag = 1;  
            }
            Timer0_Run(0);      
            IR_State=0;         
        }
    }
}
/**
 * @file BSP_Init_Stubs.c
 * @brief 各外设模块所有函数的弱符号（weak）空定义
 *
 * 当某外设模块的源文件未参与编译时，链接器自动选择此处的空函数（或返回 0），
 * 避免 "undefined symbol" 链接错误。
 * 当外设源文件参与编译时，真实函数覆盖弱定义。
 *
 * 配合 BSP_Init() 使用，可彻底消除 #ifdef 条件编译。
 */

#include "BSP_Init.h"
#include <stdint.h>
/* ======================== OLED ======================== */
__weak void OLED_Init(void)
{
}
__weak void OLED_Clear(void)
{
}
__weak void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
    (void)Line;
    (void)Column;
    (void)Char;
}
__weak void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
    (void)Line;
    (void)Column;
    (void)String;
}
__weak void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    (void)Line;
    (void)Column;
    (void)Number;
    (void)Length;
}
__weak void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
    (void)Line;
    (void)Column;
    (void)Number;
    (void)Length;
}
__weak void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    (void)Line;
    (void)Column;
    (void)Number;
    (void)Length;
}
__weak void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    (void)Line;
    (void)Column;
    (void)Number;
    (void)Length;
}

/* ==================== ADC / GPIOBSP ==================== */
__weak void GPIOBSP_Init(void)
{
}
__weak uint16_t AD_GetValue(uint8_t ADC_Channel)
{
    (void)ADC_Channel;
    return 0;
}
__weak void AD_Init(void)
{
}

/* ======================== 按键 ======================== */
__weak void Key_Init(void)
{
}
__weak void Timer_Init(void)
{
}
__weak uint8_t Key_Check(uint8_t n, uint8_t Flag)
{
    (void)n;
    (void)Flag;
    return 0;
}
__weak void Key_Tick(void)
{
}

/* ===================== 电机（PWM） ===================== */
__weak void PWM_Init(void)
{
}
__weak void PWM_Start_Pulse(int i, uint32_t len, int mode, int f)
{
    (void)i;
    (void)len;
    (void)mode;
    (void)f;
}

/* ======================= 编码器 ======================= */
__weak void Encoder_Init(void)
{
}
__weak int16_t Encoder_Get(void)
{
    return 0;
}
__weak int16_t Encoder_Getpost(void)
{
    return 0;
}

/* ====================== 编码电机 ====================== */
__weak void Emoter_Init(void)
{
}
__weak void Emoter_SetDuty(uint8_t duty)
{
    (void)duty;
}

/* ====================== NRF24L01 ====================== */
__weak uint8_t NRF24L01_ReadReg(uint8_t RegAddress)
{
    (void)RegAddress;
    return 0;
}
__weak void NRF24L01_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count)
{
    (void)RegAddress;
    (void)DataArray;
    (void)Count;
}
__weak void NRF24L01_WriteReg(uint8_t RegAddress, uint8_t Data)
{
    (void)RegAddress;
    (void)Data;
}
__weak void NRF24L01_WriteRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count)
{
    (void)RegAddress;
    (void)DataArray;
    (void)Count;
}
__weak void NRF24L01_ReadRxPayload(uint8_t *DataArray, uint8_t Count)
{
    (void)DataArray;
    (void)Count;
}
__weak void NRF24L01_WriteTxPayload(uint8_t *DataArray, uint8_t Count)
{
    (void)DataArray;
    (void)Count;
}
__weak void NRF24L01_FlushTx(void)
{
}
__weak void NRF24L01_FlushRx(void)
{
}
__weak uint8_t NRF24L01_ReadStatus(void)
{
    return 0;
}
__weak void NRF24L01_PowerDown(void)
{
}
__weak void NRF24L01_StandbyI(void)
{
}
__weak void NRF24L01_Rx(void)
{
}
__weak void NRF24L01_Tx(void)
{
}
__weak void NRF24L01_Init(void)
{
}
__weak uint8_t NRF24L01_Send(void)
{
    return 0;
}
__weak uint8_t NRF24L01_Receive(void)
{
    return 0;
}
__weak void NRF24L01_UpdateRxAddress(void)
{
}

/* ======================== 串口 ======================== */
__weak void Serial_Init(void)
{
}
__weak void Serial_SendByte(uint8_t Byte)
{
    (void)Byte;
}
__weak void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
    (void)Array;
    (void)Length;
}
__weak void Serial_SendString(char *String)
{
    (void)String;
}
__weak void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
    (void)Number;
    (void)Length;
}
__weak void Serial_Printf(char *format, ...)
{
    (void)format;
}

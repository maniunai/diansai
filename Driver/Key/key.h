#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h" // Device header

#define KEY_COUNT 4 // 按键总数

#define KEY_1 0
#define KEY_2 1
#define KEY_3 2
#define KEY_4 3

#define KEY_HOLD 0x01   // 按住
#define KEY_DOWN 0x02   // 按下
#define KEY_UP 0x04     // 松开
#define KEY_SINGLE 0x08 // 单击
#define KEY_DOUBLE 0x10 // 双击
#define KEY_LONG 0x20   // 长按
#define KEY_REPEAT 0x40 // 重复
void Timer_Init(void);
void Key_Init(void);
uint8_t Key_Check(uint8_t n, uint8_t Flag);
void Key_Tick(void);

#endif

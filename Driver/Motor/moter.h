#include "stm32f10x.h"
#ifndef _MOTER_H
#define _MOTER_H

void PWM_Init(void);
void PWM_Start_Pulse(int i, uint32_t len, int mode, int f);
extern int PWM_Current_Cnt, PWM_Pulse_Count;
#endif
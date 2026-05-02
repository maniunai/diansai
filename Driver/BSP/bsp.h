#include "stm32f10x.h"
#ifndef _BSP_H
#define _BSP_H
void GPIOBSP_Init(void);
uint16_t AD_GetValue(uint8_t ADC_Channel);
void AD_Init(void);
void PWM_Init(void);
#endif
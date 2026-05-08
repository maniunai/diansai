#ifndef _EMOTER_H
#define _EMOTER_H

#include "stm32f10x.h"

void Emoter_Init(void);
void Emoter_SetDuty(uint8_t duty); // 0~100

#endif
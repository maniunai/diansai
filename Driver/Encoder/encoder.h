#include "stm32f10x.h"
#ifndef _ENCODER_H
#define _ENCODER_H

void Encoder_Init(void);
int16_t Encoder_Get(void);
int16_t Encoder_Getpost(void);
#endif
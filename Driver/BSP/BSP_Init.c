#include "BSP_Init.h"
#include "driver.h"
/* 前向声明 —— 对应各外设模块的 Init 函数，
 * 配合 BSP_Init_Stubs.c 的弱符号使用，不需要包含各模块头文件。 */
// void OLED_Init(void);
// void GPIOBSP_Init(void);
// void AD_Init(void);
// void Key_Init(void);
// void Timer_Init(void);
// void PWM_Init(void);
// void Encoder_Init(void);
// void Emoter_Init(void);
// void NRF24L01_Init(void);
// void Serial_Init(void);

void BSP_Init(void)
{
    OLED_Init();     // I2C1, PB8, PB9, AFIO
    GPIOBSP_Init();  // PB14, EXTI, NVIC 分组
    AD_Init();       // ADC1, PA0~PA3
    Key_Init();      // PB1, 11, 13, 14
    Timer_Init();    // TIM2, NVIC
    PWM_Init();      // TIM4, PB3~PB7
    Encoder_Init();  // TIM3, PA6, PA7
    Emoter_Init();   // TIM2, PA2, PA4, PA5
    NRF24L01_Init(); // PA0~PA4
    Serial_Init();   // PA2, PA3
}

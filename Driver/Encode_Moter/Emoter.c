#include "Emoter.h"

/* TIM2 已在 Key/Timer_Init 中初始化 (PSC=71, ARR=999 → 1ms)
 * 本文件只配置 PWM 通道，不动时基/中断/NVIC               */

void Emoter_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 开启GPIOA的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  // 开启TIM2的时钟

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // PA2 → TIM2_CH3

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // PA2 → TIM2_CH3
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // PA2 → TIM2_CH3
    GPIO_SetBits(GPIOA, GPIO_Pin_5);
    
    /*配置时钟源*/
    TIM_InternalClockConfig(TIM2); // 选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟

    /*时基单元初始化*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; // 定义结构体变量
    TIM_TimeBaseInitStructure.TIM_ClockDivision =
        TIM_CKD_DIV1; // 时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 计数器模式，选择向上计数
    TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;                // 计数周期，即ARR的值
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;               // 预分频器，即PSC的值
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            // 重复计数器，高级定时器才会用到
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure); // 将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元

    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 500;     // 初始 CCR = 500 → 50% 占空比 (ARR=999)
    TIM_OC3Init(TIM2, &TIM_OCInitStructure); // TIM2_CH3
    TIM_Cmd(TIM2, ENABLE);                   // 使能TIM2，定时器开始运行
}

/* 设置占空比 duty: 0~100 */
void Emoter_SetDuty(uint8_t duty)
{
    if (duty > 100)
        duty = 100;
    uint32_t ccr = (uint32_t)duty * 10 - 1; // ARR=999, duty% → CCR
    TIM_SetCompare3(TIM2, ccr);
}
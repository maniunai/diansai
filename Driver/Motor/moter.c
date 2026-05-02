#include "moter.h"
#define MS1 GPIO_Pin_3
#define MS2 GPIO_Pin_4
#define MS3 GPIO_Pin_5
#define DIR GPIO_Pin_7
#define STEP GPIO_Pin_6
int PWM_Current_Cnt = 0, PWM_Pulse_Count = 0;
int Fre = 100;
void PWM_Init(void)
{
    /*开启时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  // 开启TIM2的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 开启GPIOA的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // 开启 AFIO 时钟，用于重映射
    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = STEP; // GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 将PA0引脚初始化为复用推挽输出
                                           // 受外设控制的引脚，均需要配置为复用模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = DIR | MS2 | MS1 | MS3; // GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 将PA0引脚初始化为复用推挽输出
                                           // 受外设控制的引脚，均需要配置为复用模式
    GPIO_SetBits(GPIOB, DIR);
    /* 释放 PB3、PB4 为普通 GPIO（禁用 JTAG，保留 SWD） */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    /*配置时钟源*/
    TIM_InternalClockConfig(TIM4); // 选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟

    /*时基单元初始化*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; // 定义结构体变量
    TIM_TimeBaseInitStructure.TIM_ClockDivision =
        TIM_CKD_DIV1; // 时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 计数器模式，选择向上计数
    TIM_TimeBaseInitStructure.TIM_Period = (1000000 / Fre) - 1;     // 计数周期，即ARR的值
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;               // 预分频器，即PSC的值
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            // 重复计数器，高级定时器才会用到
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure); // 将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元

    /*输出比较初始化*/
    TIM_OCInitTypeDef TIM_OCInitStructure;                    // 定义结构体变量
    TIM_OCStructInit(&TIM_OCInitStructure);                   // 结构体初始化，若结构体没有完整赋值
                                                              // 则最好执行此函数，给结构体所有成员都赋一个默认值
                                                              // 避免结构体初值不确定的问题
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         // 输出比较模式，选择PWM模式1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 输出极性，选择为高，若选择极性为低，则输出高低电平取反
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;             // 输出使能
    TIM_OCInitStructure.TIM_Pulse = TIM_TimeBaseInitStructure.TIM_Period / 2; // 初始的CCR值
    TIM_OC1Init(TIM4, &TIM_OCInitStructure); // 将结构体变量交给TIM_OC1Init，配置TIM2的输出比较通道1
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    /*TIM使能*/
    TIM_Cmd(TIM4, ENABLE); // 使能TIM2，定时器开始运行
}
/**
 * 函    数：设置pwm输出的脉冲个数，频率以及工作模式，占空比恒为50%
 * 参    数：i控制正反转，len是脉冲数，mode是工作模式代表转一圈是几个脉冲，f是脉冲频率
 * 返 回 值：无
 */
void PWM_Start_Pulse(int i, uint32_t len, int mode, int f)
{
    if (i == 1)
    {
        GPIO_SetBits(GPIOB, DIR);
    }
    else
    {
        GPIO_ResetBits(GPIOB, DIR);
    }
    uint32_t arr = (1000000 / f) - 1;
    // TIM_Cmd(TIM4, DISABLE);
    if (mode == 200)
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_3);
        GPIO_ResetBits(GPIOB, GPIO_Pin_4);
        GPIO_ResetBits(GPIOB, GPIO_Pin_5);
    }
    if (mode == 400)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_3);
        GPIO_ResetBits(GPIOB, GPIO_Pin_4);
        GPIO_ResetBits(GPIOB, GPIO_Pin_5);
    }
    if (mode == 800)
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_3);
        GPIO_SetBits(GPIOB, GPIO_Pin_4);
        GPIO_ResetBits(GPIOB, GPIO_Pin_5);
    }
    if (mode == 1600)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_3);
        GPIO_SetBits(GPIOB, GPIO_Pin_4);
        GPIO_ResetBits(GPIOB, GPIO_Pin_5);
    }
    if (mode == 3200)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_3);
        GPIO_SetBits(GPIOB, GPIO_Pin_4);
        GPIO_SetBits(GPIOB, GPIO_Pin_5);
    }
    TIM_SetAutoreload(TIM4, arr);
    TIM_SetCompare1(TIM4, arr / 2);
    PWM_Current_Cnt = 0;   // 清零计数
    PWM_Pulse_Count = len; // 设置要发多少个
    TIM_SetCounter(TIM4, 0);
    TIM_Cmd(TIM4, ENABLE); // 启动 PWM
}

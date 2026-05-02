#include "Key.h"

#define KEY_PRESSED 1
#define KEY_UNPRESSED 0

#define KEY_TIME_DOUBLE 200 // 双击阈值
#define KEY_TIME_LONG 2000  // 长按阈值
#define KEY_TIME_REPEAT 100 // 重复速度（间隔）

uint8_t Key_Flag[KEY_COUNT];
void Timer_Init(void)
{
    /*开启时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // 开启TIM2的时钟

    /*配置时钟源*/
    TIM_InternalClockConfig(TIM2); // 选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟

    /*时基单元初始化*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; // 定义结构体变量
    TIM_TimeBaseInitStructure.TIM_ClockDivision =
        TIM_CKD_DIV1; // 时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 计数器模式，选择向上计数
    TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;               // 计数周期，即ARR的值
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;             // 预分频器，即PSC的值
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            // 重复计数器，高级定时器才会用到
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure); // 将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元

    /*中断输出配置*/
    TIM_ClearFlag(TIM2, TIM_FLAG_Update); // 清除定时器更新标志位
                                          // TIM_TimeBaseInit函数末尾，手动产生了更新事件
                                          // 若不清除此标志位，则开启中断后，会立刻进入一次中断
                                          // 如果不介意此问题，则不清除此标志位也可

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // 开启TIM2的更新中断


    /*NVIC配置*/
    NVIC_InitTypeDef NVIC_InitStructure;                      // 定义结构体变量
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;           // 选择配置NVIC的TIM2线
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 指定NVIC线路使能
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5; // 指定NVIC线路的抢占优先级为2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 指定NVIC线路的响应优先级为1
    NVIC_Init(&NVIC_InitStructure);                           // 将结构体变量交给NVIC_Init，配置NVIC外设

    /*TIM使能*/
    TIM_Cmd(TIM2, ENABLE); // 使能TIM2，定时器开始运行
}

void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t Key_GetState(uint8_t n)
{
    if (n == KEY_1)
    {
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
        {
            return KEY_PRESSED;
        }
    }
    else if (n == KEY_2)
    {
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
        {
            return KEY_PRESSED;
        }
    }
    else if (n == KEY_3)
    {
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 1)
        {
            return KEY_PRESSED;
        }
    }
    else if (n == KEY_4)
    {
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 1)
        {
            return KEY_PRESSED;
        }
    }
    return KEY_UNPRESSED;
}

uint8_t Key_Check(uint8_t n, uint8_t Flag)
{
    if (Key_Flag[n] & Flag)
    {
        if (Flag != KEY_HOLD)
        {
            Key_Flag[n] &= ~Flag;
        }
        return 1;
    }
    return 0;
}

void Key_Tick(void)
{
    static uint8_t Count, i;
    static uint8_t CurrState[KEY_COUNT], PrevState[KEY_COUNT];
    static uint8_t S[KEY_COUNT];
    static uint16_t Time[KEY_COUNT];

    for (i = 0; i < KEY_COUNT; i++)
    {
        if (Time[i] > 0)
        {
            Time[i]--;
        }
    }

    Count++;
    if (Count >= 20)
    {
        Count = 0;

        for (i = 0; i < KEY_COUNT; i++)
        {
            PrevState[i] = CurrState[i];
            CurrState[i] = Key_GetState(i);

            if (CurrState[i] == KEY_PRESSED)
            {
                Key_Flag[i] |= KEY_HOLD;
            }
            else
            {
                Key_Flag[i] &= ~KEY_HOLD;
            }

            if (CurrState[i] == KEY_PRESSED && PrevState[i] == KEY_UNPRESSED)
            {
                Key_Flag[i] |= KEY_DOWN;
            }

            if (CurrState[i] == KEY_UNPRESSED && PrevState[i] == KEY_PRESSED)
            {
                Key_Flag[i] |= KEY_UP;
            }

            if (S[i] == 0)
            {
                if (CurrState[i] == KEY_PRESSED)
                {
                    Time[i] = KEY_TIME_LONG;
                    S[i] = 1;
                }
            }
            else if (S[i] == 1)
            {
                if (CurrState[i] == KEY_UNPRESSED)
                {
                    Time[i] = KEY_TIME_DOUBLE;
                    S[i] = 2;
                }
                else if (Time[i] == 0)
                {
                    Time[i] = KEY_TIME_REPEAT;
                    Key_Flag[i] |= KEY_LONG;
                    S[i] = 4;
                }
            }
            else if (S[i] == 2)
            {
                if (CurrState[i] == KEY_PRESSED)
                {
                    Key_Flag[i] |= KEY_DOUBLE;
                    S[i] = 3;
                }
                else if (Time[i] == 0)
                {
                    Key_Flag[i] |= KEY_SINGLE;
                    S[i] = 0;
                }
            }
            else if (S[i] == 3)
            {
                if (CurrState[i] == KEY_UNPRESSED)
                {
                    S[i] = 0;
                }
            }
            else if (S[i] == 4)
            {
                if (CurrState[i] == KEY_UNPRESSED)
                {
                    S[i] = 0;
                }
                else if (Time[i] == 0)
                {
                    Time[i] = KEY_TIME_REPEAT;
                    Key_Flag[i] |= KEY_REPEAT;
                    S[i] = 4;
                }
            }
        }
    }
}

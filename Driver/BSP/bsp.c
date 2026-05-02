#include "bsp.h"
uint16_t AD_Value[4]; // 定义用于存放AD转换结果的全局数组
void GPIOBSP_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruc;
    GPIO_InitStruc.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruc.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStruc.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruc);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);

    EXTI_InitTypeDef EXTI_StructInit;
    EXTI_StructInit.EXTI_Line = EXTI_Line14;
    EXTI_StructInit.EXTI_LineCmd = ENABLE;
    EXTI_StructInit.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_StructInit.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_StructInit);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);
}
void AD_Init(void)
{
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  // 开启ADC1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 开启GPIOA的时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);    // 开启DMA1的时钟

    /*设置ADC时钟*/
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 选择时钟6分频，ADCCLK = 72MHz / 6 = 12MHz

    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 将PA0、PA1、PA2和PA3引脚初始化为模拟输入

    /*规则组通道配置*/
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5); // 规则组序列1的位置，配置为通道0
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5); // 规则组序列2的位置，配置为通道1
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5); // 规则组序列3的位置，配置为通道2
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5); // 规则组序列4的位置，配置为通道3

    /*ADC初始化*/
    ADC_InitTypeDef ADC_InitStructure;                     // 定义结构体变量
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;     // 模式，选择独立模式，即单独使用ADC1
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 数据对齐，选择右对齐
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 外部触发，使用软件触发，不需要外部触发
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // 连续转换，使能，每转换一次规则组序列后立刻开始下一次转换
    ADC_InitStructure.ADC_ScanConvMode = ENABLE; // 扫描模式，使能，扫描规则组的序列，扫描数量由ADC_NbrOfChannel确定
    ADC_InitStructure.ADC_NbrOfChannel = 4; // 通道数，为4，扫描规则组的前4个通道
    ADC_Init(ADC1, &ADC_InitStructure);     // 将结构体变量交给ADC_Init，配置ADC1

    /*DMA初始化*/
    DMA_InitTypeDef DMA_InitStructure;                              // 定义结构体变量
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; // 外设基地址，给定形参AddrA
    DMA_InitStructure.DMA_PeripheralDataSize =
        DMA_PeripheralDataSize_HalfWord; // 外设数据宽度，选择半字，对应16为的ADC数据寄存器
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址自增，选择失能，始终以ADC数据寄存器为源
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value; // 存储器基地址，给定存放AD转换结果的全局数组AD_Value
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // 存储器数据宽度，选择半字，与源数据宽度对应
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 存储器地址自增，选择使能，每次转运后，数组移到下一个位置
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // 数据传输方向，选择由外设到存储器，ADC数据寄存器转到数组
    DMA_InitStructure.DMA_BufferSize = 4;           // 转运的数据大小（转运次数），与ADC通道数一致
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // 模式，选择循环模式，与ADC的连续转换一致
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // 存储器到存储器，选择失能，数据由ADC外设触发转运到存储器
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; // 优先级，选择中等
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);          // 将结构体变量交给DMA_Init，配置DMA1的通道1

    /*DMA和ADC使能*/
    DMA_Cmd(DMA1_Channel1, ENABLE); // DMA1的通道1使能
    ADC_DMACmd(ADC1, ENABLE);       // ADC1触发DMA1的信号使能
    ADC_Cmd(ADC1, ENABLE);          // ADC1使能

    /*ADC校准*/
    ADC_ResetCalibration(ADC1); // 固定流程，内部有电路会自动执行校准
    while (ADC_GetResetCalibrationStatus(ADC1) == SET)
        ;
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) == SET)
        ;

    /*ADC触发*/
    ADC_SoftwareStartConvCmd(
        ADC1, ENABLE); // 软件触发ADC开始工作，由于ADC处于连续转换模式，故触发一次后ADC就可以一直连续不断地工作
}


/**
 * 函    数：获取AD转换的值
 * 参    数：ADC_Channel 指定AD转换的通道，范围：ADC_Channel_x，其中x可以是0/1/2/3
 * 返 回 值：AD转换的值，范围：0~4095
 */
uint16_t AD_GetValue(uint8_t ADC_Channel)
{
    int i = 25;
    while(i)
    {
    ADC_RegularChannelConfig(ADC1, ADC_Channel, 1,
                             ADC_SampleTime_239Cycles5); // 在每次转换前，根据函数形参灵活更改规则组的通道1
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);              // 软件触发AD转换一次
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
        ; // 等待EOC标志位，即等待AD转换结束
    i--;
    }
    return ADC_GetConversionValue(ADC1); // 读数据寄存器，得到AD转换的结果
}

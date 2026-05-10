// #include "NRF24L01.h"
// #include "OLED.h"
// #include "Serial.h"
// #include "encoder.h"
// #include "key.h"
// #include "moter.h"
// #include "Emoter.h"
// #include "bsp.h"
#include "BSP_Init.h"
#include "stm32f10x.h"
#include "string.h"
#include "stdio.h"
#include <math.h>
#include <stdlib.h>
// ========== 外设使能配置 ==========
#define EN_OLED    // OLED 使能(AFIO,I2C1,PB8,PB9)
// #define EN_KEY     // 按键  使能(TIM2,PB1,PB11,PB13,PB15)
// #define EN_MOTOR   // 电机  使能(TIM2,AFIO,PB3,PB4,PB5,PB6,PB7)
//#define EN_ENCODER // 编码器 使能(TIM3,PA6,PA7)
//#define EN_SERIAL  // 串口  使能(USART2,PA2,PA3)
#define EN_NRF24L01//NRF24L01   使能(PA0,PA1,PA2,PA3)
// #define EN_EMOTER  //编码电机   使能(PA2,PA5,TIM2)
//#define EN_ADC     //ADC    使能(PB14,ADC1,DMA1,PA0,PA1,PA2,PA3)

#ifdef EN_OLED
#include "OLED.h"
#endif
#ifndef EN_OLED
#define OLED_Init()                                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define OLED_Clear()                                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define OLED_ShowChar(l, c, ch)                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(l);                                                                                                     \
        (void)(c);                                                                                                     \
        (void)(ch);                                                                                                    \
    } while (0)
#define OLED_ShowString(l, c, s)                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(l);                                                                                                     \
        (void)(c);                                                                                                     \
        (void)(s);                                                                                                     \
    } while (0)
#define OLED_ShowNum(l, c, n, len)                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(l);                                                                                                     \
        (void)(c);                                                                                                     \
        (void)(n);                                                                                                     \
        (void)(len);                                                                                                   \
    } while (0)
#define OLED_ShowSignedNum(l, c, n, len)                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(l);                                                                                                     \
        (void)(c);                                                                                                     \
        (void)(n);                                                                                                     \
        (void)(len);                                                                                                   \
    } while (0)
#define OLED_ShowHexNum(l, c, n, len)                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(l);                                                                                                     \
        (void)(c);                                                                                                     \
        (void)(n);                                                                                                     \
        (void)(len);                                                                                                   \
    } while (0)
#define OLED_ShowBinNum(l, c, n, len)                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(l);                                                                                                     \
        (void)(c);                                                                                                     \
        (void)(n);                                                                                                     \
        (void)(len);                                                                                                   \
    } while (0)
#endif

#ifdef EN_KEY
#include "key.h"
#endif
#ifndef EN_KEY
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

#define Timer_Init()                                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define Key_Init()                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define Key_Check(n, f) (0)
#define Key_Tick()                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#endif

#ifdef EN_MOTOR
#include "moter.h"
#endif
#ifndef EN_MOTOR
extern int PWM_Current_Cnt, PWM_Pulse_Count;
extern int Fre;

#define PWM_Init()                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define PWM_Start_Pulse(i, len, m, f)                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(i);                                                                                                     \
        (void)(len);                                                                                                   \
        (void)(m);                                                                                                     \
        (void)(f);                                                                                                     \
    } while (0)
#endif

#ifdef EN_ENCODER
#include "encoder.h"
#endif
#ifndef EN_ENCODER
#define Encoder_Init()                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define Encoder_Get() (0)
#define Encoder_Getpost() (0)
#endif

#ifdef EN_SERIAL
#include "Serial.h"
#endif
#ifndef EN_SERIAL
extern char Serial_RxPacket[100]; // 定义接收数据包数组，数据包格式"@MSG\r\n"
extern uint8_t Serial_RxFlag;     // 定义接收数据包标志位

#define Serial_Init()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define Serial_SendByte(b)                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(b);                                                                                                     \
    } while (0)
#define Serial_SendArray(a, len)                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(a);                                                                                                     \
        (void)(len);                                                                                                   \
    } while (0)
#define Serial_SendString(s)                                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(s);                                                                                                     \
    } while (0)
#define Serial_SendNumber(n, len)                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(n);                                                                                                     \
        (void)(len);                                                                                                   \
    } while (0)
#define Serial_Printf(f, ...)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(f);                                                                                                     \
    } while (0)
#endif

#ifdef EN_EMOTER
#include "Emoter.h"
#endif
#ifndef EN_EMOTER
#define Emoter_Init()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define Emoter_SetDuty(d)                                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(d);                                                                                                     \
    } while (0)
#endif

#ifdef EN_NRF24L01
#include "NRF24L01.h"
#endif
#ifndef EN_NRF24L01
/*发送部分*/
extern uint8_t NRF24L01_TxAddress[5];                       // 发送地址，固定5字节
#define NRF24L01_TX_PACKET_WIDTH 4                          // 发送数据包宽度，范围：1~32字节
extern uint8_t NRF24L01_TxPacket[NRF24L01_TX_PACKET_WIDTH]; // 发送数据包

/*接收部分*/
extern uint8_t NRF24L01_RxAddress[5];                       // 接收通道0地址，固定5字节
#define NRF24L01_RX_PACKET_WIDTH 4                          // 接收通道0数据包宽度，范围：1~32字节
extern uint8_t NRF24L01_RxPacket[NRF24L01_RX_PACKET_WIDTH]; // 接收数据包

/*函数宏 - 空操作*/
#define NRF24L01_ReadReg(addr) (0)
#define NRF24L01_ReadRegs(addr, arr, cnt)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(addr);                                                                                                  \
        (void)(arr);                                                                                                   \
        (void)(cnt);                                                                                                   \
    } while (0)
#define NRF24L01_WriteReg(addr, dat)                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(addr);                                                                                                  \
        (void)(dat);                                                                                                   \
    } while (0)
#define NRF24L01_WriteRegs(addr, arr, cnt)                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(addr);                                                                                                  \
        (void)(arr);                                                                                                   \
        (void)(cnt);                                                                                                   \
    } while (0)
#define NRF24L01_ReadRxPayload(arr, cnt)                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(arr);                                                                                                   \
        (void)(cnt);                                                                                                   \
    } while (0)
#define NRF24L01_WriteTxPayload(arr, cnt)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(arr);                                                                                                   \
        (void)(cnt);                                                                                                   \
    } while (0)
#define NRF24L01_FlushTx()                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define NRF24L01_FlushRx()                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define NRF24L01_ReadStatus() (0)
#define NRF24L01_PowerDown()                                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define NRF24L01_StandbyI()                                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define NRF24L01_Rx()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define NRF24L01_Tx()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define NRF24L01_Init()                                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define NRF24L01_Send() (0)
#define NRF24L01_Receive() (0)
#define NRF24L01_UpdateRxAddress()                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#endif

#ifdef EN_ADC
#include "bsp.h"
#endif
#ifndef EN_ADC
#define GPIOBSP_Init()                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define AD_GetValue(ch) (0)
#define AD_Init()                                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#endif
#include "FreeRTOS.h"
#include "driver.h"
#include "queue.h"
#include "semphr.h" // 互斥锁头文件
#include "stdlib.h"
#include "task.h"
extern uint16_t AD_Value[4];
unsigned char ucKey_Val, unKey_Down, ucKey_Up, ucKey_Old;
__IO uint32_t uwTick_Key_Set_Point = 0;
uint8_t ucKey_LongPress = 2;
uint32_t uwTick_Key_LongStart = 0;
int32_t CountSenser_count;
uint16_t speed = 0;
float x, y1, y2;
TaskHandle_t Task1Handle;
TaskHandle_t Task2Handle;
TaskHandle_t Task3Handle;
TaskHandle_t Task4Handle;
TaskHandle_t Task5Handle;
// 队列句柄
QueueHandle_t g_AdcQueue;
xSemaphoreHandle oled;
void Task1(void *pvParameters);
void Task2(void *pvParameters);
void Task3(void *pvParameters);
void Task4(void *pvParameters);
void Task5(void *pvParameters);
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    OLED_Init();    // I2C1,PB8,PB9,AFIO
    GPIOBSP_Init(); // PB14,EXTI,NVIV分组
    // AD_Init();           //ADC1,PA0,PA1,PA2,PA3
    Key_Init();     // PB1,11,13,14
    PWM_Init();     // TIM4,PB3,4,5,6,7
    Encoder_Init(); // TIM3,PA6,7
    Timer_Init();   // TIM2,NVIC
    // NRF24L01_Init();        //PA0,1,2,3,4
    Serial_Init(); // PA2,3
    oled = xSemaphoreCreateMutex();
    g_AdcQueue = xQueueCreate(4, sizeof(uint16_t));
    xTaskCreate(Task1, "Task1", 128, NULL, 2, &Task1Handle);
    xTaskCreate(Task2, "Task2", 128, NULL, 2, &Task2Handle);
    xTaskCreate(Task3, "Task3", 128, NULL, 2, &Task3Handle);
    xTaskCreate(Task4, "Task4", 128, NULL, 2, &Task4Handle);
    xTaskCreate(Task5, "Task5", 128, NULL, 2, &Task5Handle);
    // 挂起【指定任务】
    // vTaskSuspend(Task5Handle);
    // 恢复【指定任务】
    // BaseType_t vTaskResume(TaskHandle_t xTaskToResume);
    vTaskStartScheduler();
    while (1)
    {
    }
}

void Task1(void *pvParameters)
{
    uint16_t recv_val;
    int i = 0;
    while (1)
    {
        if (uxQueueMessagesWaiting(g_AdcQueue) >= 4)
        {
            xSemaphoreTake(oled, portMAX_DELAY);
            for (i = 0; i < 4; i++)
            {
                xQueueReceive(g_AdcQueue, &recv_val, 0);
                if (i <= 2)
                    OLED_ShowNum(i + 1, 1, recv_val, 4); // 显示通道0的转换结果AD0
            }
            // OLED_ShowNum(1, 6, 50 + (AD_Value[0] * (50000 - 50)) / 4095, 5);
            xSemaphoreGive(oled);
        }
        // vTaskDelay(1);
    }
}
void Task2(void *pvParameters)
{
    int i = 0;
    while (1)
    {
        for (i = 0; i <= 3; i++)
        {
            xQueueSend(g_AdcQueue, &AD_Value[i], portMAX_DELAY);
        }
        // vTaskDelay(1);
    }
}
void Task3(void *pvParameters)
{
    int32_t encoder_pos = 0;
    int32_t target_pulse = 0;
    int32_t diff_pulse = 0;
    int32_t motor_current_pulse = 0; // 电机当前累计脉冲
    while (1)
    {
        // 1. 读编码器位置
        encoder_pos = Encoder_Getpost();
        // 2. 计算电机目标脉冲
        target_pulse = encoder_pos * 200;

        // 3. 计算位置差
        diff_pulse = target_pulse - motor_current_pulse;
        if (diff_pulse > 0)
        {
            PWM_Start_Pulse(1, diff_pulse, 1600, 1000);
        }
        else if (diff_pulse < 0)
        {
            PWM_Start_Pulse(0, -diff_pulse, 1600, 1000);
        }

        // ✅【关键修复】发完脉冲再更新当前位置
        motor_current_pulse = target_pulse;
        // OLED 显示
        xSemaphoreTake(oled, portMAX_DELAY);
        OLED_ShowSignedNum(1, 6, encoder_pos, 5);
        OLED_ShowSignedNum(2, 6, motor_current_pulse, 5);
        OLED_ShowSignedNum(3, 6, diff_pulse, 5);
        xSemaphoreGive(oled);

        vTaskDelay(1);
    }
}
void Task4(void *pvParameters)
{
    int check = 0;
    while (1)
    {
        if (Key_Check(KEY_1, KEY_SINGLE))
            check = 1;
        if (Key_Check(KEY_2, KEY_SINGLE))
            check = 2;
        if (check == 1)
        {
            NRF24L01_TxPacket[0]++;
            NRF24L01_TxPacket[1]++;
            NRF24L01_TxPacket[2]++;
            NRF24L01_TxPacket[3]++;
            NRF24L01_Receive();
        }
        if (check == 2)
        {
            if (NRF24L01_Receive())
            {
                xSemaphoreTake(oled, portMAX_DELAY);
                OLED_ShowHexNum(4, 1, NRF24L01_RxPacket[0], 2);
                xSemaphoreGive(oled);
            }
        }
    }
}
void Task5(void *pvParameters)
{
    while (1)
    {

        // OLED_ShowString(1, 1, "TxPacket");
        // OLED_ShowString(3, 1, "RxPacket");
        // OLED_ShowString(4, 1, Serial_RxPacket);
        if (Serial_RxFlag == 1)
        {
            xSemaphoreTake(oled, portMAX_DELAY);
            // OLED_ShowString(4, 1, "                ");
            OLED_ShowString(4, 1, Serial_RxPacket);

            char *Tag = strtok(Serial_RxPacket, ",");
            if (strcmp(Tag, "key") == 0)
            {
                char *Name = strtok(NULL, ",");
                char *Action = strtok(NULL, ",");

                if (strcmp(Name, "1") == 0 && strcmp(Action, "up") == 0)
                {
                    printf("key,1,up\r\n");
                }
                else if (strcmp(Name, "2") == 0 && strcmp(Action, "down") == 0)
                {
                    printf("key,2,down\r\n");
                }
            }
            else if (strcmp(Tag, "slider") == 0)
            {
                char *Name = strtok(NULL, ",");
                char *Value = strtok(NULL, ",");

                if (strcmp(Name, "1") == 0)
                {
                    uint8_t IntValue = atoi(Value);

                    printf("slider,1,%d\r\n", IntValue);
                }
                else if (strcmp(Name, "2") == 0)
                {
                    float FloatValue = atof(Value);

                    printf("slider,2,%f\r\n", FloatValue);
                }
            }
            else if (strcmp(Tag, "joystick") == 0)
            {
                int8_t LH = atoi(strtok(NULL, ","));
                int8_t LV = atoi(strtok(NULL, ","));
                int8_t RH = atoi(strtok(NULL, ","));
                int8_t RV = atoi(strtok(NULL, ","));

                printf("joystick,%d,%d,%d,%d\r\n", LH, LV, RH, RV);
            }
            Serial_RxFlag = 0;
            xSemaphoreGive(oled);
        }

        // vTaskDelay(200);
    }
}
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) // 判断是否是TIM2的更新事件触发的中断
    {
        // speed = Encoder_Get();                      // 每隔固定时间段读取一次编码器计数增量值，即为速度值
        Key_Tick();
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // 清除TIM2更新事件的中断标志位
    }
}
void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
    {
        PWM_Current_Cnt++;
        if (PWM_Current_Cnt >= PWM_Pulse_Count)
        {
            TIM_Cmd(TIM4, DISABLE);
            PWM_Current_Cnt = 0;
        }

        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}
void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line14) == SET)
    {
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
        {
            CountSenser_count++;
        }
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
}

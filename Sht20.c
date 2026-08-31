#include<stdio.h>
#include<unistd.h>
#include"ohos_init.h"
#include"cmsis_os2.h"
#include "hal_bsp_sht20.h"

void thread1(void)
{
    while(1)
    {
        osSemaphoreRelease(sem1);
        osSemaphoreRelease(sem1);
        printf("\n");
        printf("Thread1释放信号量!\n");
        osDelay(300);
    }
}

void thread2(void)
{
    float temperature = 0,humidity = 0;
    printf("i2c_sht20_demo()!");
    SHT20_Init();
    while(1)
    {
        osSemaphoreAcquire(SEM1,osWaitForever);
        SHT20_ReadData(&temperature,&humidity);
        printf("temperature = %.2f    humidity = &.2f\r\n",temperature,humidity);
        printf("Thread2得到信号量!\n");
        osDelay(1);
    }
}

void thread3(void)
{
    while(1)
    {
        osSemaphoreAcquire(sem1,osWaitForever);
        printf("Thread3得到信号量!\n");
        osDelay(1);
    }
}

osSemaphoreId_t  sem1;

static void i2c_sht20_demo(void);
{
    osThreadAttr_t attr;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;

    attr.name = "thread1";
    attr.priority = 25;
    if (osThreadNew((osThreadFunc_t)thread1, NULL &attr) == NULL)
    {
        printf("Failed to create thread1!\n");
    }
    attr.name = "thread2";
    attr.priority = 25;
    if (osThreadNew((osThreadFunc_t)thread2, NULL &attr) == NULL)
    {
        printf("Failed to create thread2!\n");
    }
    attr.name = "thread3";
    attr.priority = 25;
    if (osThreadNew((osThreadFunc_t)thread3, NULL &attr) == NULL)
    {
        printf("Failed to create thread3!\n");
    }
    sem1 = osSemaphoreNew(4,0,NULL);
    if(sem1 == NULL)
    {
        printf("Failed to create Semaphore!\n");
    }
}

APP_FEATURE_INIT(i2c_sht20_demo);
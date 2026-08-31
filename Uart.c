#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifiiot_errno.h"
#include "wifiiot_gpio.h"
#include "wifiiot_gpio_ex.h"
#include "wifiiot_adc.h"
#include "wifiiot_uart.h"

typedef struct
{
  char *Buf;  
  uint8_t Idx;
} MSGQUEUE_OBJ_t;
MSGQUEUE_OBJ_t msg;
typedef struct
{
  char *Buf;
  uint8_t Idx;
} MSGQUEUE_OBJ_t_rx;
MSGQUEUE_OBJ_t_rx msg_rx;

osMessageQueueId_t mid_MsgQueue;
osStatus_t  status;

#define MSGQUEUE_OBJECTS 16
#define UART_TASK_STACK_SIZE 1024 * 16
#define UART_TASK_PRIO 25
#define UART_BUFF_SIZE 1000

static const char *data = "Hello, QST!\r\n";

static void UART_ExampleEntry(void)
{
    mid_MsgQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, 100, NULL);
    if (mid_MsgQueue == NULL)
    {
        printf("Falied to create Message Queue!\n");
    }

    osThreadAttr_t attr;
    attr.name = "UART_Task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = UART_TASK_STACK_SIZE;
    attr.priority = UART_TASK_PRIO;
    if (osThreadNew((osThreadFunc_t)UART_Task, NULL, &attr) == NULL)
    {
        printf(" Falied to create UART_Task!\n");
    }
    attr.name = "thread2";
    attr.stack_size = UART_TASK_STACK_SIZE;
    attr.priority = 25;
    if (osThreadNew((osThreadFunc_t)thread2, NULL, &attr) == NULL)
    {
        printf("Falied to create thread2!\n");
    }
    attr.name = "thread3";
    attr.stack_size = 1024;
    attr.priority = 25;
    if (osThreadNew((osThreadFunc_t)thread3, NULL, &attr) == NULL)
    {
        printf("Falied to create thread3!\n");
    }
}

static void UART_Task(void)
{
    uint32_t ret;
GpioInit();
IoSetFunc(WIFI_IOT_IO_NAME_GPIO_0, WIFI_IOT_IO_FUNC_GPIO_0_UART1_TXD);
IoSetFunc(WIFI_IOT_IO_NAME_GPIO_1, WIFI_IOT_IO_FUNC_GPIO_1_UART1_RXD);
WifiIotUartAttribute uart_attr = {
.baudRate = 9600,
.dataBits = 8,
.stopBits = 1,
.parity = 0,
};
ret = UartInit(WIFI_IOT_UART_IDX_1, &uart_attr, NULL);
if (ret != WIFI_IOT_SUCCESS)
{
    printf("Failed to init uart! Err code = %d\n", ret);
    return;
}

    printf("UART Test Start\n");
while (1)
{
    printf("************UART_example*************\r\n");
    UartWrite(WIFI_IOT_UART_IDX_1, (unsigned char *)data, strlen(data));
    status = osMessageQueueGet(mid_MsgQueue, &msg_rx, NULL, osWaitForever);
    if (status == osOK)  printf("Message Queue id:%d, Get msg_rx:%s\n", msg_rx.Idx, msg_rx.Buf);
}
}

void thread2(void)
{
    uint8_t rt;
    uint8_t uart_buff[UART_BUFF_SIZE] = {0};
    uint8_t *uart_buff_ptr = uart_buff;
    sleep(1);
    msg.Idx = 12U;
    while (1)
    {
        printf("任务2正在运行!\n");
        rt=UartRead(WIFI_IOT_UART_IDX_1, uart_buff_ptr, UART_BUFF_SIZE);
        printf("Uart1 read data:%s\n", uart_buff_ptr);
        uart_buff_ptr[rt]='\0'; 
        msg.Buf=(char*)uart_buff_ptr ; 
        rt=osMessageQueuePut(mid_MsgQueue, &msg, 0U, 0U);
        if(rt==0)
        printf("Message Queue Send msg:%s\n", msg.Buf); 
        else
        printf("Message Queue Send msg failed");    
        sleep(1);
}

void thread3(void)
{
    while (1)
    {
        printf("任务3正在运行!\n");
        sleep(3);
    }
}


APP_FEATURE_INIT(UART_ExampleEntry);
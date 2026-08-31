#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifiiot_gpio.h"
#include "wifiiot_gpio_ex.h"
#include "wifiiot_watchdog.h" 
#include "hi_io.h"
#include "hi_time.h"

/*任务入口*/
static void Hcsr04(void)
{
    WatchDogDisable();
    osThreadAttr_t attr;

    attr.name = "Hcsr04";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = osPriorityNormal;

    if (osThreadNew(Hcsrtext, NULL, &attr) == NULL){
        printf("Failed to create Task!\n");
    }
}

#define GPIO_8 8
#define GPIO_7 7
#define GPIO_FUNC 0
#define IoTGpioSetDir GpioSetDir

float GetDistance (void)
{
    static unsigned long start_time = 0, time = 0;
    float distance = 0.0;
    WifiIotGpioValue = WIFI_IOT_GPIO_VALUE0;
    unsigned int  flag = 0;

    hi_io_set_func(GPIO_8, GPIO_FUNC);

    GpioSetDir(GPIO_8,WIFI_IOT_GPIO_DIR_IN);
    GpioSetDir(GPIO_7,WIFI_IOT_GPIO_DIR_OUT);
    GpioSetOutputVal(GPIO_7,WIFI_IOT_GPIO_VALUE1);
    hi_udelay(20);
    GpioSetOutputVal(GPIO_7,WIFI_IOT_GPIO_VALUE0);

    while (1){
        GpioGetInputVal(GPIO_8, &value);
        if(value == WIFI_IOT_GPIO_VALUE1 && flag == 0){
            start_time = hi_get_us();
            flag = 1;
        }
        if(value == WIFI_IOT_GPIO_VALUE0 && flag == 1){
            time = hi_get_us() - start_time;
            start_time = 0;
            break;
        }
    }
    distance = time * 0.034 / 2;
    return distance;
}

void Hcsrtext(void* parame){
    (void)parame;
    printf("start test hcsr04\r\n");

    while(1){
        float distance = GetDistance();
        printf("distance is %.1f (cm)\r\n", distance);
        osDelay(200);
    }
}

APP_FEATURE_INIT(Hcsr04);
#include "bsp_timer.h"
#include "bsp_delay.h"

#include "Ticker.h"

#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "driver/rtc_io.h"

Ticker t1;

//Define a timer structure that runs repeatedly in a cycle
esp_timer_create_args_t Mtimer0 = {
    .callback = &timer0_cb,             //Set the callback function
    .arg = NULL,                        //Do not carry parameters
    .name = "Timer_1ms"                 //Timer name
};

esp_timer_handle_t timer0_handle = 0;


//TIMER0    1ms
void bsp_Timer0_Init(void) {
    esp_err_t err = esp_timer_create(&Mtimer0, &timer0_handle);
    err = esp_timer_start_periodic(timer0_handle, 1000);

    D_printf("重复周期运行的定时器创建状态码: %s", err == ESP_OK ? "ok!\r\n" : "failed!\r\n"); //Create a status code for a timer that runs in repeated cycles
}

extern void disposeAllTimeData(void);
extern volatile int spin_cal;
void timer0_cb(void * arg) {
    spin_cal++;
    disposeAllTimeData();
}
///////////////////////////////////////////////////////////////////
//TIMER1    100ms
void bsp_Timer1_Init(void) {
    t1.attach(0.1, timer1_cb);
}
extern void disposePer100ms(void);
extern void refreshADCVal(void);
void timer1_cb(void) {
    disposePer100ms();
    refreshADCVal();
}
///////////////////////////////////////////////////////////////////
static void IRAM_ATTR timer2_cb(void * arg);
//TIMER2    TIMER_INTR_US=7us
void bsp_Timer2_Init(void) {
    timer_idx_t timer_idx = TIMER_0;
    timer_autoreload_t auto_reload = TIMER_AUTORELOAD_EN;

    esp_err_t ret;
    timer_config_t config = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = auto_reload,
        .divider = TIMER_DIVIDER
    };

    ret = timer_init(TIMER_GROUP_0, timer_idx, &config);
    ESP_ERROR_CHECK(ret);
    ret = timer_set_counter_value(TIMER_GROUP_0, timer_idx, 0x00000000ULL);
    ESP_ERROR_CHECK(ret);
    ret = timer_set_alarm_value(TIMER_GROUP_0, timer_idx, ALARM_VAL_US);
    ESP_ERROR_CHECK(ret);
    ret = timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    ESP_ERROR_CHECK(ret);
    /* Register an ISR handler */
    timer_isr_register(TIMER_GROUP_0, timer_idx, timer2_cb, NULL, ESP_INTR_FLAG_IRAM, NULL);
    // timer_start(TIMER_GROUP_0, TIMER_0);
    // timer_pause(TIMER_GROUP_0, TIMER_0);
}

extern void RingArray_Intr(void);
static void IRAM_ATTR timer2_cb(void * arg) {
    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
    RingArray_Intr();
}
//////////////////////////////////////////////////////////////////

//int64_t tick = esp_timer_get_time();
// if (tick > 100000000)
// {
//  //Stop the timer from working and get whether it stopped successfully
//  esp_err_t err = esp_timer_stop(timer0_handle);
//  printf("要停止的定时器名字：%s , 是否停止成功：%s", Mtimer0.name,    //Name of the timer to be stopped      //Whether to stop successfully
//                err == ESP_OK ? "ok!\r\n" : "failed!\r\n");
//  err = esp_timer_delete(timer0_handle);
//  printf("要删除的定时器名字：%s , 是否停止成功：%s", Mtimer0.name,    //The name of the timer to be deleted      //Whether to stop successfully
//                err == ESP_OK ? "ok!\r\n" : "failed!\r\n");
// }
//printf("方法回调名字: %s , 距离定时器开启时间间隔 = %lld \r\n", __func__, tick);   //Method callback name     //Distance timer turn on time interval





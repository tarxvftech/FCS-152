#include "bsp_delay.h"


void delay_us(int nus) {
    esp_rom_delay_us(nus);
}

void delay_ms(int nms) {
    vTaskDelay(nms / portTICK_PERIOD_MS);
}

void delay_s(int ns) {
    delay_ms(ns*1000);
}
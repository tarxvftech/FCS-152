#ifndef __BSP_DAC_H__
#define __BSP_DAC_H__
#include "FCS152_KDU.h"
#include "hal/dac_types.h"
#include "driver/dac_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "driver/dac.h"
#include "driver/timer.h"
#include "esp_log.h"
#include "soc/dac_periph.h"

#include "bsp_timer.h"


void bsp_DAC_Init(void);
void bsp_Timer2_Init(void);
void RingTone(ToneClass tone, int state);

#endif

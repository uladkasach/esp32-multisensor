#ifndef MEASURE_DISTANCE_H_   /* guard */
    #define MEASURE_DISTANCE_H_

    #include <stdio.h>
    #include <stdint.h>
    #include <stddef.h>
    #include <string.h>

    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/semphr.h"
    #include "freertos/queue.h"

    #include "soc/gpio_reg.h"
    #include "driver/gpio.h"
    #include "esp_intr_alloc.h"

    #include "utils.h"

    uint32_t measure_distance_for_sensor(int sensor_id);
    void register_monitor(int sensor_id, int gpio_echo_pin_number, int gpio_trig_pin_number);

#endif

#ifndef UTILS_H_   /* guard */
    #define UTILS_H_

    /*
        methods
    */
    void milli_delay(int milli);
    void micro_delay(int microns);
    char* concat(const char *s1, const char *s2);
    void initialize_non_volatile_storage(void);
    void start_wifi(void);

    /*
        dependencies
    */
    // O.S. libraries
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/event_groups.h"

    // standard c libraries
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>

    // esp utils
    #include "esp_system.h"
    #include "nvs_flash.h"
    // custom
    #include "wifi_connect.h"

#endif // UTILS_H_

#ifndef MAIN_H   /* guard */
    #define MAIN_H

    /*
        dependencies
    */
    // standard c libraries
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <time.h>
    #include <sys/time.h>

    // O.S. libraries
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/event_groups.h"

    // esp utils
    #include "driver/gpio.h"
    #include "esp_system.h"
    #include "esp_wifi.h"
    #include "esp_event_loop.h"
    #include "esp_attr.h"
    #include "esp_sleep.h"
    #include "nvs_flash.h"
    #include "lwip/err.h"
    #include "apps/sntp/sntp.h"

    #include <lwip/err.h>
    #include <lwip/sockets.h>
    #include <lwip/sys.h>
    #include <lwip/netdb.h>
    #include <lwip/dns.h>




#endif

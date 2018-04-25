#ifndef WIFI_H_   /* guard */
    #define WIFI_H_

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
    #include "esp_wifi.h"
    #include "esp_event_loop.h"
    #include "esp_attr.h"

    #include <lwip/sockets.h>
    #include <lwip/err.h>
    #include <lwip/sys.h>
    #include <lwip/netdb.h>
    #include <lwip/dns.h>

    //esp logging
    #include "esp_log.h"
    #include "global_config.h"


    /*
        implementations
    */
    // methods
    void start_wifi_connection(void);
    void stop_wifi_connection(void);
    void wait_until_wifi_connected(void);
    void initialise_wifi();
    esp_err_t event_handler(void *ctx, system_event_t *event);




#endif

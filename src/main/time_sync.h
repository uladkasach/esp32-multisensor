#ifndef TIME_SYNC_H_   /* guard */
    #define TIME_SYNC_H_

    /*
        methods
    */
    void wait_until_time_updated(void);
    void task_update_internal_time_with_sntp( void *pvParameters );

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

    //esp logging
    #include "esp_log.h"

    // esp utils
    #include "apps/sntp/sntp.h"



#endif

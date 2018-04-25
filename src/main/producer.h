#ifndef PRODUCER_H   /* guard */
    #define PRODUCER_H

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

    // custom
    #include "global_config.h"
    #include "utils.h"
    #include "measure_distance.h"

    /*
        implementations
    */
    void measure_parse_queue(int ids[]);
#endif

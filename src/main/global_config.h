#ifndef GLOBAL_CONFIG   /* guard */
    #define GLOBAL_CONFIG

    /*
        dependencies
    */
    // O.S. libraries
    #include "freertos/queue.h"

    /*
        implementationss
    */
    // sensors count
    #define SENSORS_COUNT (4) // define how many sensors we're using; used to initialize the semmaphors and start time values arrays

    // settings
    #define PRODUCER_DELAY_MILLISECOND 300 // produce every X milliseconds
    #define CONSUMER_DELAY_MILLISECOND 750 // consume every X milliseconds
    #define DATA_PER_OUTPUT 10 // max k data per output
    #define QUEUE_SIZE 10

    // data string sizes
    #define DISTANCE_DATA_LENGTH (8 + 1)* sizeof(char) // length of a distance measurement + delimeter
    #define TIMESTAMP_SIZE 8*sizeof(char)
    #define DATA_STRING_SIZE ((DISTANCE_DATA_LENGTH*SENSORS_COUNT + TIMESTAMP_SIZE + 2 )* sizeof(char)) //  2 for delimterers

    // wifi
    #define EXAMPLE_WIFI_SSID "Space Farms"
    #define EXAMPLE_WIFI_PASS "Waterfall"

    // queue
    QueueHandle_t data_queue;

#endif

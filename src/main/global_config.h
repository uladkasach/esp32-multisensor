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

    // data string sizes
    #define DISTANCE_DATA_LENGTH (8 + 1)* sizeof(char) // length of a distance measurement + delimeter
    #define TIMESTAMP_SIZE 8*sizeof(char)
    #define DATA_STRING_SIZE ((DISTANCE_DATA_LENGTH*SENSORS_COUNT + TIMESTAMP_SIZE + 3)* sizeof(char)) //  2 for delimterers

    // queue
    QueueHandle_t data_queue;

#endif

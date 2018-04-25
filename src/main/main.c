/*
    default esp & standard dependencies
*/
#include "main.h"

/*
    custom dependencies
*/
#include "global_config.h"
#include "utils.h"
#include "time_sync.h"
#include "producer.h"
#include "consumer.h"
#include "console.h"

//esp logging
#include "esp_log.h"

// task handles
xTaskHandle time_updater_task_handle;
xTaskHandle producer_task_handle;
xTaskHandle consumer_task_handle;

// global settings - producer and consumer are overwritten when initializing environment
int producer_delay_millisecond = 300;
int consumer_delay_millisecond = 1000;
int ids[4] = {0, 1, 2, 3};
int ids_len = 4;

/*
    tasks
*/
static void task_producer(void)
{
    for( ;; )
    {
        measure_parse_queue(ids, ids_len);
        milli_delay(producer_delay_millisecond);
    }
}
static void task_consumer(void)
{
    //const int sleep_count = 10;
    for( ;; )
    {
        output_data_from_queue();
        milli_delay(consumer_delay_millisecond);
    }
}



void initialize_environment(struct Settings settings){
    // init utilities (wifi, nvs)
    initialize_non_volatile_storage();
    initialise_wifi();
    join_wifi(settings.ssid, settings.pass, 10000);
    initialize_udp_socket(settings.port, settings.ip_addr);

    // init queue
    data_queue = xQueueCreate(settings.buffer_size, DATA_STRING_SIZE); // create queue capable of holding 10 data strings at a time
    if(data_queue != 0){
        printf("%s\n", "data_queue initialized successfully");
    }

    // install global ISR handler - requires usage of gpio_isr_handler_add
    gpio_install_isr_service(0);

    // register the distance monitors
    register_sensor(0, 5, 2); // create lister for sensor 0: echo on 5, trig on 2
    register_sensor(1, 4, 0); // create lister for sensor 1: echo on 4, trig on 0
    register_sensor(2, 17, 16);
    register_sensor(3, 19, 18);

    // update global variables based on settings
    producer_delay_millisecond = settings.sensing_period_milliseconds;
    consumer_delay_millisecond = 3*producer_delay_millisecond;
}
void start_tasks(){
    // start time syncer
    BaseType_t xReturned_updateTask;
    xReturned_updateTask = xTaskCreate(task_update_internal_time_with_sntp,  // pointer to function
                "time_update_task",        // Task name string for debug purposes
                4000,            // Stack depth as word
                NULL,           // function parameter (like a generic object)
                1,              // Task Priority (Greater value has higher priority)
                &time_updater_task_handle);  // Task handle
    if( xReturned_updateTask == pdPASS )
    {
        printf("time updater task started successfully\n");
    }

    // wait untill the time is updated
    wait_until_time_updated();

    // start producer task
    BaseType_t xReturned_producer_task;
    xReturned_producer_task = xTaskCreate(task_producer,  // pointer to function
                "pulse_task",        // Task name string for debug purposes
                4000,            // Stack depth as word
                NULL,           // function parameter (like a generic object)
                2,              // Task Priority (Greater value has higher priority)
                &producer_task_handle);  // Task handle
    if( xReturned_producer_task == pdPASS )
    {
        printf("producer task started successfully\n");
    }


    // start consumer task
    BaseType_t xReturned_consumer_task;
    xReturned_consumer_task = xTaskCreate(task_consumer,  // pointer to function
                "time_output_task",        // Task name string for debug purposes
                10000,            // Stack depth as word
                NULL,           // function parameter (like a generic object)
                2,              // Task Priority (Greater value has higher priority)
                &consumer_task_handle);  // Task handle
    if( xReturned_consumer_task == pdPASS )
    {
        printf("consumer task started successfully\n");
    }
}



//Main application
void app_main()
{

    // extract users requested configurations
    initialize_console();
    struct Settings settings = retreive_settings_with_cli();

    // start environment
    initialize_environment(settings);

    // start the tasks
    start_tasks();

    /*
    while(1){
        measure_parse_queue(ids, ids_len);
        milli_delay(1000);
    }
    */


}

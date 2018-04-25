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

//esp logging
#include "esp_log.h"
static const char *TAG = "app";
static const char *TAG_RETREIVE = "retreive";


// for udp server init socket
int port_number = 3000;
char* ip_addr = "192.168.43.48"; // ifconfig -a


//Main application
void app_main()
{

    /*
        start utilities
    */
    // init utilities (wifi, nvs)
    initialize_non_volatile_storage();
    initialise_wifi();
    start_wifi();
    initialize_udp_socket(port_number, ip_addr);

    // init queue
    data_queue = xQueueCreate(QUEUE_SIZE, DATA_STRING_SIZE ); // create queue capable of holding 10 data strings at a time
    if(data_queue != 0){
        printf("%s\n", "data_queue initialized successfully");
    }

    // install global ISR handler - requires usage of gpio_isr_handler_add
    gpio_install_isr_service(0);

    // register the distance monitors
    register_sensor(0, 5, 2); // create lister for sensor 0: echo on 5, trig on 2
    register_sensor(1, 4, 0); // create lister for sensor 1: echo on 4, trig on 0

    int index = 0;
    while(1) {
        index++;
        int ids[2] = {0, 1};
        measure_parse_queue(ids);
        milli_delay(500);
        if(index == 5){
            output_data_from_queue();
        }
    }

}

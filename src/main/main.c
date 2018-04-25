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
#include "measure_distance.h"

//esp logging
#include "esp_log.h"
static const char *TAG = "app";
static const char *TAG_RETREIVE = "retreive";

// generate measurement frame
time_t time_now;
struct tm time_now_timeinfo;
char strftime_buf[64];
char* generate_data_string(uint32_t distances[]){
    //printf("begin generation of string\n");

    // generate distances string, for all distances passed in distances array
    char *all_distances_string = "";
    char *distance_string = (char*)malloc(DISTANCE_DATA_LENGTH);
    int number_of_distances = sizeof(*distances)/sizeof(distances[0]) + 1;
    int index;
    for(index = 0; index < number_of_distances; index++){
        uint32_t this_distance = distances[index];
        //printf("index: %d -> distance :%08u\n", index, this_distance);
        sprintf(distance_string, "%08u;", this_distance); // int + delimeter
        all_distances_string = concat(all_distances_string, distance_string);
    }
    free(distance_string); // free buffer now that we're done with it
    //printf("%s\n", all_distances_string);

    // get the time
    time(&time_now);
    setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1); // set timezone to eastern
    tzset();
    localtime_r(&time_now, &time_now_timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%X", &time_now_timeinfo);

    // build the data string
    char *data_string = (char*)malloc(DATA_STRING_SIZE);
    sprintf(data_string, "%s;%s#", strftime_buf, all_distances_string); // add terminator to end

    // return the data
    return data_string;
}

// queuing
void queue_data_string(char *data_string){
    // add data to queue
    portBASE_TYPE queue_response = xQueueSendToBack(data_queue, data_string, 10); // allow up to 10 ticks of blocking
    if(queue_response != pdPASS){
        printf("%s\n", "(!) queue_send blocked task and waited for 10 ticks and was still not able to add to queue");
    }

}


void measure_parse_queue(int ids[]){

    // get distance for each id
    int number_of_distances = sizeof(*ids)/sizeof(ids[0]) + 1;
    uint32_t distances[number_of_distances];
    int index;
    for(index = 0; index < number_of_distances; index++){
        distances[index] = measure_distance_for_sensor(ids[index]);
    }

    // convert distances to data string
    char* data_string = generate_data_string(distances);
    printf("%s\n", data_string);

    // queue the data string
    queue_data_string(data_string);

    // free the data, since queue copies the data instead of using reference
    free(data_string);
}


//Main application
void app_main()
{

    printf("begin \n");


    // init queue
    data_queue = xQueueCreate(QUEUE_SIZE, DATA_STRING_SIZE ); // create queue capable of holding 10 data strings at a time
    if(data_queue != 0){
        printf("%s\n", "data_queue initialized successfully");
    }


    // install global ISR handler - requires usage of gpio_isr_handler_add
    gpio_install_isr_service(0);


    register_monitor(0, 5, 2); // create lister for sensor 0: echo on 5, trig on 2
    register_monitor(1, 4, 0); // create lister for sensor 1: echo on 4, trig on 0


    while(1) {
        int ids[2] = {0, 1};
        measure_parse_queue(ids);
        milli_delay(500);
    }

}

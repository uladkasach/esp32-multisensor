#include "producer.h"


// generate measurement frame
time_t time_now;
struct tm time_now_timeinfo;
char strftime_buf[64];
char* generate_data_string(uint32_t distances[], int distances_len){
    //printf("begin generation of string\n");

    // generate distances string, for all distances passed in distances array
    char *all_distances_string = "";
    char *distance_string = (char*)malloc(DISTANCE_DATA_LENGTH);
    int number_of_distances = distances_len;// sizeof(*distances)/sizeof(distances[0]) + 1;
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


void measure_parse_queue(int ids[], int ids_len){

    // get distance for each id
    int number_of_distances = ids_len; // sizeof(ids)/sizeof(ids[0]);
    uint32_t distances[number_of_distances];
    int index;
    for(index = 0; index < number_of_distances; index++){
        distances[index] = measure_distance_for_sensor(ids[index]);
    }

    // convert distances to data string
    char* data_string = generate_data_string(distances, ids_len);
    //printf("%s\n", data_string);

    // queue the data string
    queue_data_string(data_string);

    // free the data, since queue copies the data instead of using reference
    free(data_string);
}

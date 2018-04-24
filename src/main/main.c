
// This implementation uses queues
// https://www.freertos.org/Embedded-RTOS-Queues.html
// queues support blocking and synchronization (since values are placed by value and not by reference)

// udp example: https://github.com/Ebiroll/qemu_esp32/blob/master/examples/19_udp/main/main.c

#include "main.h"



/*
    global constants
*/
#define TRIG_PIN (2)
#define ECHO_PIN (5)
#define DATA_STRING_SIZE ((5+2+8)* sizeof(char))// 5 for distance, 2 for delimterers, 8 for timestamp
#define PRODUCER_DELAY_MILLISECOND 300 // produce every X milliseconds
#define CONSUMER_DELAY_MILLISECOND 750 // consume every X milliseconds
#define DATA_PER_OUTPUT 10 // max k data per output
#define QUEUE_SIZE 10 // how big the queue is

// for communicating with UDP server
#define RECEIVER_PORT_NUM 3000
#define SENDER_PORT_NUM 3000
#define RECEIVER_IP_ADDR "192.168.1.105"
#define SENDER_IP_ADDR "127.0.0.1"

/*
    define global variables
*/
QueueHandle_t data_queue;

xTaskHandle time_updater_task_handle;
xTaskHandle producer_task_handle;
xTaskHandle consumer_task_handle;

time_t time_now;
struct tm time_now_timeinfo;
char strftime_buf[64];



/*
    inner-headers
*/
void setup();
void loop();
void app_main();
void milli_delay();


// for udp
int socket_fd;


/*
    producer
*/
int measure_distance(){
    // clear trigger
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    long duration = pulseIn(ECHO_PIN, HIGH, 1000000);

    // Calculating the distance
    int distance = duration*0.0343/2; // microns * meter / micron / 2; /2 because distance pulse travels is there AND back

    // print the distance
    //printf("%s: %d\n", "distance", distance);

    // return the distance
    return distance;
}
char* generate_data_string(){
    // get the distance
    int distance = measure_distance();

    // get the time
    time(&time_now);
    setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1); // set timezone to eastern
    tzset();
    localtime_r(&time_now, &time_now_timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%X", &time_now_timeinfo);

    // build the data string
    char *data_string = (char*)malloc(DATA_STRING_SIZE);
    sprintf(data_string, "%s;%05d#", strftime_buf, distance); // add terminator to end

    // return the data
    return data_string;
}
void queue_data_string(){
    // retreive new datastring
    char *data_string = generate_data_string();
    //printf("%s\n", data_string);

    // add data to queue
    portBASE_TYPE queue_response = xQueueSendToBack(data_queue, data_string, 10); // allow up to 10 ticks of blocking
    if(queue_response != pdPASS){
        printf("%s\n", "(!) queue_send blocked task and waited for 10 ticks and was still not able to add to queue");
    }

    // free the data, since queue coppies the data and does not store referenc
    free(data_string);
}


/*
    consumer
*/
struct sockaddr_in sa,ra;
void initialize_socket(){
    socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if ( socket_fd < 0 )
    {

        printf("socket call failed");
        exit(0);

    }

    memset(&ra, 0, sizeof(struct sockaddr_in));
    ra.sin_family = AF_INET;
    ra.sin_addr.s_addr = inet_addr(RECEIVER_IP_ADDR);
    ra.sin_port = htons(RECEIVER_PORT_NUM);


}
void send_udp_packet(char* total_output_string, int total_data_points){

    int sent_data = lwip_sendto_r(socket_fd, total_output_string, total_data_points*DATA_STRING_SIZE,0,(struct sockaddr*)&ra,sizeof(ra));
    if(sent_data < 0)
    {
        printf("send failed\n");
        close(socket_fd);
        exit(2);
    }
    printf("data packet sent successfully!\n");
}
void output_data_from_queue(){
    // total data_string and queue_string
    char *total_output_string = "";
    char *queue_string = (char*)malloc(DATA_STRING_SIZE);

    // read data from queue
    //      read DATA_PER_OUTPUT times, untill queue is empty - then just finish
    int index;
    int total_data_points = 0;
    for(index=0; index < DATA_PER_OUTPUT; index++){
        portBASE_TYPE queue_data = xQueueReceive(data_queue, queue_string, 10); // wait up to 10 ticks of blocking
        if(queue_data){
            // data was found - append it to the total output string
            //printf("from queue: %s\n", queue_string);
            total_output_string = concat(total_output_string, queue_string);
            total_data_points++;
            //printf("total output string now: %s\n", total_output_string);
        } else {
            // no data was found - break the loop by seting index = DATA_PER_OUTPUT
            //printf("%s\n", "(!) queue_receive was not able to receive data from queue even after 10 ticks of blocking");
            index=DATA_PER_OUTPUT;
        }
    }

    // output the total data from queue buffer
    printf("final combined output string: %s\n", total_output_string);
    printf("%s\n", "-------");

    // send the data with udp
    send_udp_packet(total_output_string, total_data_points);

    // free the data
    free(queue_string);
    free(total_output_string);
}



/*
    tasks
*/

static void task_producer( void *pvParameters )
{
    for( ;; )
    {
        ESP_LOGI(TAG_RETREIVE, "(!) measuring and recording distance...")
        queue_data_string();
        milli_delay(PRODUCER_DELAY_MILLISECOND);
    }
}
static void task_consumer( void *pvParameters )
{
    //const int sleep_count = 10;
    for( ;; )
    {
        output_data_from_queue();
        milli_delay(CONSUMER_DELAY_MILLISECOND);
    }
}

/*
    begin main methods
*/
void setup(){
    initArduino();

    // define pins
    pinMode(TRIG_PIN, OUTPUT); // Sets the trigPin as an Output
    pinMode(ECHO_PIN, INPUT); // Sets the echoPin as an Input

    // init queue
    data_queue = xQueueCreate(QUEUE_SIZE, DATA_STRING_SIZE ); // create queue capable of holding 10 data strings at a time
    if(data_queue != 0){
        printf("%s\n", "data_queue initialized successfully");
    }

    // start WiFi and Flash and wait for WiFi connection
    initialize_utilities();

    // start socket
    initialize_socket();
}

void app_main()
{
    setup();

    // start time syncer
    BaseType_t xReturned_updateTask;
    xReturned_updateTask = xTaskCreate(task_update_internal_time_with_sntp,  // pointer to function
                "time_update_task",        // Task name string for debug purposes
                8000,            // Stack depth as word
                NULL,           // function parameter (like a generic object)
                1,              // Task Priority (Greater value has higher priority)
                &time_updater_task_handle);  // Task handle
    if( xReturned_updateTask == pdPASS )
    {
        ESP_LOGI(TAG, "time updater task started successfully");
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
        ESP_LOGI(TAG, "producer task started successfully");
    }


    // start consumer task
    BaseType_t xReturned_consumer_task;
    xReturned_consumer_task = xTaskCreate(task_consumer,  // pointer to function
                "time_output_task",        // Task name string for debug purposes
                8000,            // Stack depth as word
                NULL,           // function parameter (like a generic object)
                2,              // Task Priority (Greater value has higher priority)
                &consumer_task_handle);  // Task handle
    if( xReturned_consumer_task == pdPASS )
    {
        ESP_LOGI(TAG, "consumer task started successfully");
    }


}

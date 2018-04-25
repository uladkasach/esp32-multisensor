/* SPI Slave example, sender (uses SPI master driver)
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#define SENSORS_COUNT (4) // define how many sensors we're using; used to initialize the semmaphors and start time values arrays

/*
Pins in use. The SPI Master can use the GPIO mux, so feel free to change these if needed.
*/
#define TRIG_PIN (2)
#define ECHO_PIN (5)
#define GPIO_HANDSHAKE 5

//esp logging
#include "esp_log.h"
static const char *TAG = "app";

#include "measure_distance.h";

//Main application
void app_main()
{

    printf("begin \n");

    // install global ISR handler - requires usage of gpio_isr_handler_add
    gpio_install_isr_service(0);

    // create lister for sensor 0
    register_monitor(0, 5, 2);


    while(1) {
        //Wait for slave to be ready for next byte before sending

        /*
        if( xSemaphoreTake( xSemaphore, ( TickType_t ) 1000 ) == pdTRUE )
        {
            printf("got the semaphore!\n");
            // xSemaphoreGive( rdySem );
        }
        else
        {
            printf("semaphore was not able to be taken within tick time\n");
        }
        */
        measure_distance_for_sensor(0);
        milli_delay(500);
    }

}

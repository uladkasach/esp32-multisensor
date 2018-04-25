#include "measure_distance.h"

//esp logging
#include "esp_log.h"
static const char *TAG = "measure";


// define F_CPU - copied from https://github.com/espressif/arduino-esp32/blob/f648ad7ceaa5c3e0af54de5777b3ed30b3dd1119/cores/esp32/esp32-hal.h
#ifndef F_CPU
#define F_CPU (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000000U)
#endif

// define conversion function - copied from https://github.com/espressif/arduino-esp32/blob/3dc30dce811c0de1ca15d825ba0b898239070311/cores/esp32/Arduino.h
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )


// define values for the monitor arrays
static SemaphoreHandle_t measure_semaphors [ SENSORS_COUNT ];
static SemaphoreHandle_t waiting_semaphors [ SENSORS_COUNT ];
static uint32_t sensor_trigger_times [ SENSORS_COUNT ];
static uint32_t sensor_echo_times [ SENSORS_COUNT ];
static int sensor_trigger_pins [ SENSORS_COUNT ];
static int sensor_echo_pins [ SENSORS_COUNT ];


static void pulse_pin(int pin){
    gpio_set_level(pin, 0);
    micro_delay(2);
    gpio_set_level(pin, 1);
    micro_delay(10);
    gpio_set_level(pin, 0);
}

static void IRAM_ATTR echo_handler(int sensor_id){
    sensor_echo_times[sensor_id]=xthal_get_ccount();
    //ets_printf(" (ECHO) echo was received for sensor %d at  %u \n", sensor_id, sensor_echo_times[sensor_id]);

    // release the semaphore.
    BaseType_t mustYield=false;
    xSemaphoreGiveFromISR(waiting_semaphors[sensor_id], &mustYield);
    if (mustYield) portYIELD_FROM_ISR();

    // time difference will be calculated back at distance measurement function
}
uint32_t measure_distance_for_sensor(int sensor_id){

    // grab mutex lock for measuring, dont start another trigger untill first one releases
    if(xSemaphoreTake( measure_semaphors[sensor_id], 3000 ) != pdTRUE){
        ESP_LOGI(TAG, "could not take semaphore for measuring (!)(!)(!)(!)(!)");
        return 0;
    }
    // ESP_LOGI(TAG, "took semaphore for measuring");

    // start lock to wait asynchronously using a semaphore - do it before trigger to remove tasks between trigger and interupt
    if(xSemaphoreTake( waiting_semaphors[sensor_id], 1000 ) != pdTRUE){
        ESP_LOGI(TAG, "could not take semaphore for waiting (!)(!)(!)(!)(!)");
        xSemaphoreGive( measure_semaphors[sensor_id] );
        return 0;
    }
    // ESP_LOGI(TAG, "took semaphore for waiting");

    // trigger echo
    sensor_trigger_times[sensor_id]=xthal_get_ccount();
    int trigger_pin = sensor_trigger_pins[sensor_id]; // record time triger began
    pulse_pin(trigger_pin);
    // ESP_LOGI(TAG, "trig was pulsed for sensor %d at  %u", sensor_id, sensor_trigger_times[sensor_id]);


    // wait for echo to be completed (time will be placed into sensor_echo_times)
    if(xSemaphoreTake( waiting_semaphors[sensor_id], 1000 ) != pdTRUE){
        ESP_LOGI(TAG, "could not finish waiting for echo (!)(!)(!)(!)(!) \n");
        xSemaphoreGive( measure_semaphors[sensor_id] );
        return 0;
    }
    // ESP_LOGI(TAG, "finished waiting for echo, time echo recorded: %u", sensor_echo_times[sensor_id]);

    // calculate time difference
    uint32_t diff_time=sensor_echo_times[sensor_id]-sensor_trigger_times[sensor_id];

    // release waiting semaphore
    xSemaphoreGive( waiting_semaphors[sensor_id] );

    // release measure semaphore
    xSemaphoreGive( measure_semaphors[sensor_id] );

    // convert clocksycles to microseconds
    diff_time = clockCyclesToMicroseconds(diff_time);

    // return distance
    //ESP_LOGI(TAG, "difference for sensor %d was %u", sensor_id, diff_time);
    return diff_time;
}


void register_monitor(int sensor_id, int gpio_echo_pin_number, int gpio_trig_pin_number){
    // register echo pin number
    sensor_echo_pins[sensor_id] = gpio_echo_pin_number;

    // register echo pin number
    sensor_trigger_pins[sensor_id] = gpio_trig_pin_number;

    // create semaphore
    measure_semaphors[sensor_id] = xSemaphoreCreateMutex(); // used to only trigger one at a time
    waiting_semaphors[sensor_id] = xSemaphoreCreateMutex(); // used to wait asynchronously for echo

    // GPIO trigger config
    gpio_config_t trig_gpio;
    trig_gpio.pin_bit_mask = (1<<gpio_trig_pin_number);
    trig_gpio.mode         = GPIO_MODE_OUTPUT;
    trig_gpio.pull_up_en   = GPIO_PULLUP_DISABLE;
    trig_gpio.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&trig_gpio);

    //GPIO echo config
    gpio_config_t echo_gpio;
    echo_gpio.pin_bit_mask = (1<<gpio_echo_pin_number);
    echo_gpio.mode         = GPIO_MODE_INPUT;
    echo_gpio.pull_up_en   = GPIO_PULLUP_DISABLE;
    echo_gpio.pull_down_en = GPIO_PULLDOWN_ENABLE;
    echo_gpio.intr_type    = GPIO_INTR_NEGEDGE;
    gpio_config(&echo_gpio);

    //set up echo interrupt.
    gpio_isr_handler_add(gpio_echo_pin_number, echo_handler, sensor_id); // pass sensor_id as an argument

    // confirm creation
    printf("distance monitor was created for sensor %d\n", sensor_id);
}

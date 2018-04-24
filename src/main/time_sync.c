#include "time_sync.h"

static void request_time_update(void);
static void update_time_with_sntp(void);

// constants
static const char *TIME_SYNC_LOG_TAG = "TIME_SYNC";


/*
    time syncer
*/
static void request_time_update(void){
    ESP_LOGI(TIME_SYNC_LOG_TAG, "retreiving time with SNTP protocol");

    //start_wifi_connection();
    //wait_until_wifi_connected();

    update_time_with_sntp();
    wait_until_time_updated();

    //stop_wifi_connection();

}
static void update_time_with_sntp(void){
    ESP_LOGI(TIME_SYNC_LOG_TAG, "requesting update of time with SNTP");

    ESP_LOGI(TIME_SYNC_LOG_TAG, "starting SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL); // set mode to listen only, we will manually poll every hour in a seperate thread
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
    ESP_LOGI(TIME_SYNC_LOG_TAG, "   SNTP polling opened. Waiting untill time updated...");
    wait_until_time_updated();
    ESP_LOGI(TIME_SYNC_LOG_TAG, "   time has been updated. closed SNTP polling.");
    sntp_stop();
    // sntp_stop

}
void wait_until_time_updated(void){
    // init vars
    time_t now;
    struct tm timeinfo;

    // get the time
    time(&now);
    localtime_r(&now, &timeinfo);

    // test and block untill ready w/ max block of 10s
    int retry = 0;
    const int retry_count = 10;
    while(timeinfo.tm_year < (1990 - 1900) && ++retry < retry_count) { // consider all years above 1990 to be valid
        ESP_LOGI(TIME_SYNC_LOG_TAG, "       Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }
}

void task_update_internal_time_with_sntp( void *pvParameters ){
    const int sleep_sec = 60; // 10 seconds
    for( ;; )
    {
        ESP_LOGI(TIME_SYNC_LOG_TAG, "(!) Starting Update of Internal Wall-Clock Time Again...")

        request_time_update();

        ESP_LOGI(TIME_SYNC_LOG_TAG, "update of wall clock time has completed. entering task wait for %d seconds", sleep_sec);
        vTaskDelay( sleep_sec * 1000 / portTICK_PERIOD_MS ); // wait / yield time to other tasks
    }
}

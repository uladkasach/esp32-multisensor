#include "wifi_connect.h"


// constants
static EventGroupHandle_t wifi_event_group;
static const int CONNECTED_BIT = BIT0;
const char *WIFI_LOG_TAG = "WIFI";


void start_wifi_connection(void){
    ESP_LOGI(WIFI_LOG_TAG, "starting WiFi radio and connection");
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );
}
void stop_wifi_connection(void){
    ESP_LOGI(WIFI_LOG_TAG, "stopping WiFi radio and connection");
    ESP_ERROR_CHECK( esp_wifi_disconnect() );
    ESP_ERROR_CHECK( esp_wifi_stop() );
}
void wait_until_wifi_connected(void){
    ESP_LOGI(WIFI_LOG_TAG, "waiting untill wifi has connected");
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY); // waits untill wifi is connected
    ESP_LOGI(WIFI_LOG_TAG, "wifi is now connected");
}
void initialise_wifi(void){
    ESP_LOGI(WIFI_LOG_TAG, "initializing WIFI");
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(WIFI_LOG_TAG, "   Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_LOGI(WIFI_LOG_TAG, "    WIFI initialized successfully");
}
esp_err_t event_handler(void *ctx, system_event_t *event){
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

#include "utils.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
    utilities
*/
void milli_delay(int milli){
    vTaskDelay(milli / portTICK_PERIOD_MS); // wait `milli` milliseconds
}
void micro_delay(int microns){
    ets_delay_us(microns);
}
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the null-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
void initialize_non_volatile_storage(void){
    ESP_ERROR_CHECK( nvs_flash_init() ); // initialize non-volatile storage
}
void initialize_utilities(void){
    initialize_non_volatile_storage();
    initialise_wifi();
    start_wifi_connection();
    wait_until_wifi_connected();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

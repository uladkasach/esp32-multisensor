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
    if (result == NULL) {
        printf(stderr, "Fatal: failed to allocate bytes.\n");
        abort();
    }
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
void initialize_non_volatile_storage(void){
    ESP_ERROR_CHECK( nvs_flash_init() ); // initialize non-volatile storage
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

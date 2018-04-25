#ifndef CONSOLE_H   /* guard */
    #define CONSOLE_H

    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>

    #include "esp_log.h"
    #include "esp_err.h"
    #include "esp_console.h"
    #include "esp_vfs_dev.h"
    #include "driver/uart.h"
    #include "linenoise/linenoise.h"
    #include "argtable3/argtable3.h"
    #include "esp_vfs_fat.h"
    #include "nvs.h"
    #include "nvs_flash.h"



    struct Settings {
        char ssid[32];
        char pass[64];
        char ip_addr[20];
        int port;
        int sensing_period_milliseconds; // sensing frequency
        int buffer_size; // queue size
    };
    void initialize_console();
    struct Settings retreive_settings_with_cli();
    void show_settings(struct Settings settings);

#endif

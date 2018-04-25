#ifndef CONSUMER_H   /* guard */
    #define CONSUMER_H

    // esp utils
    #include "esp_system.h"
    #include "esp_wifi.h"
    #include "lwip/err.h"
    #include "apps/sntp/sntp.h"

    #include <lwip/sockets.h>
    #include <lwip/err.h>
    #include <lwip/sys.h>
    #include <lwip/netdb.h>
    #include <lwip/dns.h>

    // impelmentaion
    #include "global_config.h"
    #include "utils.h"

    void initialize_udp_socket(int port_number, char* ip_addr);
    void output_data_from_queue();

#endif

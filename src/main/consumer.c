#include "consumer.h"

/*
    consumer
*/
int socket_fd;
struct sockaddr_in sa,ra;
void initialize_udp_socket(int port_number, char* ip_addr){
    socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if ( socket_fd < 0 )
    {
        printf("socket call failed");
        exit(0);
    }
    memset(&ra, 0, sizeof(struct sockaddr_in));
    ra.sin_family = AF_INET;
    ra.sin_addr.s_addr = inet_addr(ip_addr);
    ra.sin_port = htons(port_number);
}
void send_udp_packet(char* total_output_string, int total_data_points){

    int sent_data = lwip_sendto_r(socket_fd, total_output_string, (total_data_points)*DATA_STRING_SIZE,0,(struct sockaddr*)&ra,sizeof(ra));
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

#include "console.h"



void initialize_console()
{
    /* Disable buffering on stdin and stdout */
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    esp_vfs_dev_uart_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_uart_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK( uart_driver_install(CONFIG_CONSOLE_UART_NUM,
            256, 0, 0, NULL, 0) );

    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(CONFIG_CONSOLE_UART_NUM);

    /* Initialize the console */
    esp_console_config_t console_config = {
            .max_cmdline_args = 8,
            .max_cmdline_length = 256,
            .hint_color = atoi(LOG_COLOR_CYAN)
    };
    ESP_ERROR_CHECK( esp_console_init(&console_config) );

    /* Configure linenoise line completion library */
    /* Enable multiline editing. If not set, long commands will scroll within
     * single line.
     */
    linenoiseSetMultiLine(1);

    /* Tell linenoise where to get command completions and hints */
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback*) &esp_console_get_hint);

    /* Set command history size */
    linenoiseHistorySetMaxLen(100);

}



void show_settings(struct Settings settings){
    printf("SSID         : %s\n", settings.ssid);
    printf("PASS         : %s\n", settings.pass);
    printf("IP_ADDR      : %s\n", settings.ip_addr);
    printf("PORT         : %d\n", settings.port);
    printf("SENSE_PERIOD : %d\n", settings.sensing_period_milliseconds);
    printf("BUFFER_SIZE  : %d\n", settings.buffer_size);
}
void show_help(){
    printf("\n"
        "Type 'SHOW' to display the current system configurations\n"
        "Type 'SET' to modify system configurations\n"
        "    - 'SET SSID <VALUE>' to set the SSID of your WIFI\n"
        "    - 'SET PASS <VALUE>' to set the Password of your WIFI\n"
        "    - 'SET IP_ADDR <VALUE>' to set the IP address of your UDP Server\n"
        "    - 'SET PORT <VALUE>' to set the Port Number of your UDP Server [default = 3000]\n"
        "    - 'SET SENSE_PERIOD <VALUE>' to set the Sensing Period in Milliseconds [default = 100]\n"
        "    - 'SET BUFFER_SIZE <VALUE>' to set the size of the shared data buffer [default = 10]\n"
        "Type 'START' to start the sensors, record the values, and output to the udp server\n");
}
char ** parse_set_command(char* command){
    char ** res  = NULL;
    int n_spaces = 0, i;

    /* split string and append tokens to 'res' */
    char *  p    = strtok (command, " "); // get first token
    while (p) {
        res = realloc (res, sizeof (char*) * ++n_spaces); // reallocate space
        if (res == NULL) exit (-1); /* memory allocation failed */
        res[n_spaces-1] = p; // store that token
        p = strtok (NULL, " "); // continue getting tokens
    }

    /* realloc one extra element for the last NULL */
    res = realloc (res, sizeof (char*) * (n_spaces+1));
    res[n_spaces] = 0;

    /* print the result */
    for (i = 0; i < (n_spaces); ++i)
        printf ("res[%d] = %s\n", i, res[i]);

    // return the result
    return res;
}
struct Settings retreive_settings_with_cli(){
    // initialize settings object
    struct Settings settings = {
        .ssid="undefined",
        .pass="undefined",
        .ip_addr="undefined",
        .port=3000,
        .sensing_period_milliseconds=100,
        .buffer_size=10,
    };
    if(true){
        strcpy(settings.ssid, "SpaceFarms");
        strcpy(settings.pass, "Waterfall");
        strcpy(settings.ip_addr, "192.168.43.48");
    }

    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    const char* prompt = LOG_COLOR_I "esp32> " LOG_RESET_COLOR;

    printf("\n"
       "Type 'HELP' to get a list of commands.\n"
       "Use UP/DOWN arrows to navigate through command history.\n"
       "Press TAB when typing command name to auto-complete.\n");

    /* Figure out if the terminal supports escape sequences */
    int probe_status = linenoiseProbe();
    if (probe_status) { /* zero indicates success */
        printf("\n"
               "Your terminal application does not support escape sequences.\n"
               "Line editing and history features are disabled.\n"
               "On Windows, try using Putty instead.\n");
        linenoiseSetDumbMode(1);
        /* Since the terminal doesn't support escape sequences,
         * don't use color codes in the prompt.
         */
        prompt = "esp32> ";
    }

    /* Main loop */
    while(true) {
        /* Get a line using linenoise.
         * The line is returned when ENTER is pressed.
         */
        char* line = linenoise(prompt);
        if (line == NULL) { /* Ignore empty lines */
            continue;
        }
        /* Add the command to the history */
        linenoiseHistoryAdd(line);

        if(strcmp("HELP", line)==0){
            show_help();
        } else if(strcmp("SHOW", line)==0){
            show_settings(settings);
        } else if(strcmp("START", line) == 0){
            printf("Starting Functionality...\n");
            printf(" `-> final settings:\n");
            show_settings(settings);
            printf("-------------------------\n");
            break;
        } else {
            char short_string[4];     // Destination string
            strncpy(short_string, line, 3);
            short_string[3] = 0; // null terminate destination
            if(strcmp("SET", short_string)==0){
                char** parts = parse_set_command(line);
                if(strcmp("SSID", parts[1])==0){
                    strcpy(settings.ssid, parts[2]);
                } else if(strcmp("PASS", parts[1])==0){
                    strcpy(settings.pass, parts[2]);
                } else if(strcmp("IP_ADDR", parts[1])==0){
                    strcpy(settings.ip_addr, parts[2]);
                } else if(strcmp("PORT", parts[1])==0){
                    settings.port=atoi(parts[2]);
                } else if(strcmp("SENSE_PERIOD", parts[1])==0){
                    settings.sensing_period_milliseconds=atoi(parts[2]);
                } else if(strcmp("BUFFER_SIZE", parts[1])==0){
                    settings.buffer_size=atoi(parts[2]);
                } else {
                    printf("no match found - set option '%s' is not valid", parts[1]);
                }
                free (parts); // free the parts memory allocated
            } else {
                printf("Sorry, that command was not understood\n");
            }
        }

        /* linenoise allocates line buffer on the heap, so need to free it */
        linenoiseFree(line);
    }

    // return final settings
    return settings;
}

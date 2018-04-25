### OVERVIEW
1. <s> update internal clock
2. read distance with ultrasonic sensor using interupts & semaphore
    - 0. record time of trigger, send trigger, wait for interupt
    - 2. wait for low: low time
    - 3. return time difference
3. abstract producer task creation to support different sensors
    - sensor defined by "sensor id", "sensor pin trig", "sensor pin echo"
3. update data to add add data to data queue according to pattern : [sensor_id, sensed_time, sensed_distance]
4. <s> send data queue strings to a url
5. create command line interface
    - use "config"
6. create server to display information as requested



## GPIOS FOR SENSORS
register_sensor(0, 5, 2); // create lister for sensor 0: echo on 5, trig on 2
register_sensor(1, 4, 0); // create lister for sensor 1: echo on 4, trig on 0
register_sensor(2, 17, 16);
register_sensor(3, 19, 18);



## References:
https://github.com/espressif/esp-idf/blob/07b61d54f7e502bb12527e4b582de121ffa6fa02/examples/peripherals/spi_slave/sender/main/app_main.c

^^ semaphore demo

https://docs.aws.amazon.com/freertos-kernel/latest/dg/interrupt-management.html - describes interupt related methods

http://esp32.info/docs/esp_idf/html/d6/d79/group__GPIO__Driver__APIs.html -- interupt handler

http://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/gpio.html



### To load the program onto your board and run it you must:
1. setup the environment
    - part 1: setup espressif framework
        - see the instructions in the file `_dev/setup_and_hello_world.sh`
    - part 2: setup the arduino component for espressif
        - run `./init.sh`
            - run `chmod +x ./init.sh` if you need to enable permission to execute the file
3. configure the settings
    - 1. find the LAN-IP of your computer and set it as the `RECEIVER_IP_ADDR` in `main.c`
    - 2. play with the `CONSUMER_DELAY_MILLISECOND`, `PRODUCER_DELAY_MILLISECOND`, `DATA_PER_OUTPUT`, and `QUEUE_SIZE` to tune them to your liking
2. run `./build --build --flash --monitor`
    - `--build` compiles the code
    - `--flash` ports it to the board
    - `--monitor` restarts the board and displays the output of the board
        - e.g., logs; it shows you the serial port

TODO:
- make udp server log to csv file so that it can be used for statistical analysis
- calibrate the "distance" calculated by the sensor
    - it should be pretty close but needs verified.

NOTE:
sometimes the packtets that are sent are corrupt; make sure you get rid of those

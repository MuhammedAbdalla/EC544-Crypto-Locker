// source material
// https://www.geeksforgeeks.org/socket-programming-cc/

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

#define BLINK_GPIO CONFIG_BLINK_GPIO

// open()
#include <fcntl.h>

// socket
#include <sys/socket.h>

// waitpid
#include <sys/wait.h> 

// for the signal handling
#include <signal.h>
#include "lockerfunctions.h"

static const char *TAG = "example";
static uint8_t s_led_state = 0;
static led_strip_handle_t led_strip;
// static pid_t pid, childpid;     
// static int status;

// instantiate a bit array of locker
// static int bit_reservations[HOURS_IN_DAY * (MINS_IN_HOURS/LOCKER_MIN_TIME_RESERVE)] = {0};

#ifdef CONFIG_BLINK_LED_RMT

static void blink_led(void)
{
    /* If the addressable LED is enabled */
    if (s_led_state) {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        led_strip_set_pixel(led_strip, 0, 20, 20, 20);
        /* Refresh the strip to send data */
        led_strip_refresh(led_strip);
    } else {
        /* Set all LED off to clear all pixels */
        led_strip_clear(led_strip);
    }
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1, // at least one LED on board
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

#elif CONFIG_BLINK_LED_GPIO

static void blink_led(void)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(BLINK_GPIO, s_led_state);
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}
#endif


void fast_blink(void) {
	s_led_state = 0;
	for(int c = 0; c<100; c++){
		blink_led();
		s_led_state = !s_led_state;
		vTaskDelay(25 / portTICK_PERIOD_MS);
	}
}

void slow_blink(void) {
	s_led_state = 1;
	for(int c = 0; c<20; c++){
		blink_led();
		s_led_state = !s_led_state;
		vTaskDelay(125 / portTICK_PERIOD_MS);
	}
}

void app_main(int argc, char const* argv[]) {
	struct sockaddr_in address;
	reservations = NULL;
	
	int server_fd, client_fd, valread, opt, addrlen;
	char *ACK = "ACK";
	char cmd[256];
	char buffer[1024] = {0};
	
	/* Configure the peripheral according to the LED type */
    configure_led();

	opt = 1;
	addrlen = sizeof(address);
	ACK = "ACK";

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		perror("socket failed");
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
	}

	// prepare the the socket structure
	address.sin_family = AF_INET; // inet_addr("IP ADDRESS")
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// bind
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
    }

	// printf("starting server on:\n");

	// pid = fork();

	// if (pid == 0) {
		// int status = 0b0;
		//ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
		printf("connecting to AWS cloud...\n");
		strcpy(cmd, "./AWS/activate_aws.sh");
		if (system(cmd) == -1) {
			perror("activate AWS ssh");
			exit(FAILURE);
		}
		
		strcpy(cmd, "./AWS/connect_aws.sh");
		if (system(cmd) == -1){
			perror("connect AWS ssh");
			exit(FAILURE);
		}
		
	// 	exit(SUCCESS);
		
	// } else if (pid == -1) {
	// 	perror("fork");
	// 	exit(FAILURE);
	// } else {
	// 	childpid = waitpid(-1, &status, 0);
	// 	printf("status %d\n",status);
	// 	if (!WIFEXITED(status)){
	// 		exit(FAILURE);
	// 	} else {
			printf("ssh completed\n");
	// 	}
	// }
	while(1){
		ESP_LOGI("TEST", "SOCKET SET UP");
		vTaskDelay(1000 / portTICK_PERIOD_MS);;
	}

	while (1) {
		vTaskDelay(500 / portTICK_PERIOD_MS);
		if (listen(server_fd, 3) == -1) {
			perror("listen");
			exit(FAILURE);
		}

		client_fd = accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen);
		if (client_fd == -1) {
			perror("accept");
			exit(FAILURE);
		}

		// read the incoming message
		valread = read(client_fd, buffer, 1024);
		if (valread == -1) {
			// close the socket and continue reading in new clients
			perror("read");
			close(client_fd);
			continue;
		} else {
			char *data[4] = {"192.168.0.1", "12:30am", "3:30am", "3"};
			if (strcmp(buffer, "CREATE") == 0) {
				create_reservation(data);
				fast_blink(void);
			} else if (strcmp(buffer, "DELETE") == 0) {
				delete_reservation(buffer);
				slow_blink(void);
			} else if (strcmp(buffer, "MODIFY") == 0) {

			} else {
				perror("locker command");
			}
			print_reservations(reservations);

			exit(SUCCESS);
		}

		// send back a server response
		printf("[SERVER] \"client: %s key...\"\n", buffer);
		if (send(client_fd, ACK, strlen(ACK), 0) == -1) {
			printf("\tmessage failed\n");
			close(client_fd);
		} else {
			printf("\tmessage sent\n");
		}

		// closing the connected client socket
    	close(client_fd);
	}

    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
}

// docker run --mount source=shared-stuff,target=/stuff
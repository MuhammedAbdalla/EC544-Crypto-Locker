#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "locker.h"

int connect_aws(char* cmd) {
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char msg[1024] = "null";
    char buffer[1024] = {0};

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        printf("\n Socket creation error \n");
        return FAILURE;
    }
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
  
    // Convert IPv4 and IPv6 addresses from text to binary
    if (inet_pton(AF_INET, "192.168.99.190", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported\n");
        return FAILURE;
    }
  
    status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (status == -1) {
        printf("\nConnection Failed\n");
        return FAILURE;
    }

    // //Send some data - HTTP example
	// message = "GET / HTTP/1.1\r\n\r\n";

	strcpy(msg,cmd);
    printf("[client]: %s\n", msg);
    if (send(client_fd, msg, strlen(msg), 0) == -1) {
		printf("\tmessage failed\n");
    } else {
        printf("\tmessage sent\n");
    }

    valread = read(client_fd, buffer, 1024);
    if (valread == -1) {
        // close the socket and continue reading in new clients
        perror("read");
        close(client_fd);
    }
	printf("[server]: %s\n", buffer);
    // closing the connected socket
    close(client_fd);

	return SUCCESS;
}

int ssh_setup(void) {
	char cmd[256];

	printf("\n...connecting to AWS cloud...\n");
	strcpy(cmd, "./server/AWS/activate_aws.sh");
	if (system(cmd) == -1) {
		perror("activate AWS ssh");
		return FAILURE;
	}

	strcpy(cmd, "./server/AWS/connect_aws.sh");
	if (system(cmd) == -1){
		perror("connect AWS ssh");
		return FAILURE;
	}

	printf("\n[[[ connected to AWS cloud ]]]\n");
	return SUCCESS;
}
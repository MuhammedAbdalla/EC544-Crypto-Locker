// source material
// https://www.geeksforgeeks.org/socket-programming-cc/

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
// socket
#include <sys/socket.h>

// waitpid
#include <sys/wait.h> 

// for the signal handling
#include <signal.h>

#define FAILURE 0
#define SUCCESS 1

int main(int argc, char const* argv[]) {
    int PORT;

    printf("Server IP: %s\n", argv[1]);
    printf("Client IP: %s\n", argv[2]);
    printf("msg %s\n\n\n", argv[3]);

    if (getenv("PORT")) {
        PORT = atoi(getenv("PORT"));
    } else {
        PORT = 8080;
    } 

    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char msg[1024] = "(NULL)";
    char buffer[1024] = {0};

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        printf("\n Socket creation error \n");
        return FAILURE;
    }
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
  
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
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

    if (argv[3] != NULL) 
        strcpy(msg, argv[3]);

    printf("[client] %s: %s\n",argv[2], msg);
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
  
    // closing the connected socket
    close(client_fd);

    return SUCCESS;
}
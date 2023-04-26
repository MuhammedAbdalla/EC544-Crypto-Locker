// source material
// https://www.geeksforgeeks.org/socket-programming-cc/

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


int main(int argc, char const* argv[]) {
    if (!getenv("PORT")) {
        perror("port");
        exit(EXIT_FAILURE);
    }

    const int PORT = atoi(getenv("PORT"));

    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char* msg = "requesting resource";
    char buffer[1024] = {0};

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
  
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "172.20.185.58", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (status == -1) {
        printf("\nConnection Failed \n");
        return -1;
    }

    send(client_fd, msg, strlen(msg), 0);
    printf("message sent\n");
    valread = read(client_fd, buffer, 1024);
    printf("%s\n", buffer);
  
    // closing the connected socket
    close(client_fd);

    return 0;
}
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

// open()
#include <fcntl.h>

// socket
#include <sys/socket.h>

// waitpid
#include <sys/wait.h> 

// for the signal handling
#include <signal.h>
#include "lockerserver.h"

#define HOURS_IN_DAY 24
#define LOCK_INC_MINS 30

pid_t pid, childpid;     
int status;


// instantiate a bit array of locker
char half_hour_segments[HOURS_IN_DAY * (60/LOCK_INC_MINS)] = {0};

int execute_sys_cmd(char* text, char* error) {
	char *cmd;
	strcpy(cmd, text);
	if (system(cmd) == -1) {
		perror(error);
		return FAILURE;
	}
	return SUCCESS;
}



int main(int argc, char const* argv[]) {
	struct sockaddr_in address;
	struct hostent *host_entry;

	int server_fd, client_fd, valread, opt, addrlen;
	char *ACK, *cmd, *server_ip, host[256];
	char buffer[1024] = {0};
	
	opt = 1;
	addrlen = sizeof(address);

	ACK = "ACK";

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		perror("socket failed");
		return FAILURE;
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		return FAILURE;
	}

	// prepare the the socket structure
	address.sin_family = AF_INET; // inet_addr("IP ADDRESS")
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// // get IP and Host Name of Server (self)
	// if (gethostname(host, sizeof(host)) == -1) {
	// 	perror("gethostname");
	// 	return FAILURE;
	// }

	// if ((host_entry = gethostbyname(host)) == NULL) {
	// 	perror("gethostentry");
	// 	return FAILURE;
	// }

	// int ip_list_size = (sizeof(host_entry->h_addr_list)/sizeof(host_entry->h_addr_list[0]));
	// for (int i = 0; i < ip_list_size; i++) {
	// 	if ( (server_ip = inet_ntoa( *((struct in_addr*) host_entry->h_addr_list[0]))) != NULL) {
	// 		printf("IP %d: %s\n", i, server_ip);
	// 	}
	// }

	// if ( (server_ip = inet_ntoa( *((struct in_addr*) host_entry->h_addr_list[0]))) == NULL) {
	// 	perror("inet_ntoa");
	// 	return FAILURE;
	// }
	
	// printf("Current Host Name: %s\n", host);
   	// printf("Host IP: %s\n", server_ip);

	// bind
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return FAILURE;
    }

	// printf("starting server on:\n");

	
	pid = fork();

	if (pid == 0) {
		int status = 0b0;
		char IPv4[16];

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
		
		exit(SUCCESS);
		
	} else if (pid == -1) {
		perror("fork");
		exit(FAILURE);
	} else {
		childpid = waitpid(-1, &status, 0);
		printf("status %d\n",status);
		if (WIFEXITED(status) == -1){
			return FAILURE;
		} else {
			printf("ssh completed\n");
		}
	}

	while (1) {
		if (listen(server_fd, 3) == -1) {
			perror("listen");
			return FAILURE;
		}

		client_fd = accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen);
		if (client_fd == -1) {
			perror("accept");
			return FAILURE;
		}

		// read the incoming message
		valread = read(client_fd, buffer, 1024);
		if (valread == -1) {
			// close the socket and continue reading in new clients
			perror("read");
			close(client_fd);
			continue;
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
    return SUCCESS;
}

// docker run --mount source=shared-stuff,target=/stuff
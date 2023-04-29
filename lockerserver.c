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
#include "lockerfunctions.h"

pid_t pid, childpid;     
int status;

// instantiate a bit array of locker
static int bit_reservations[HOURS_IN_DAY * (MINS_IN_HOURS/LOCKER_MIN_TIME_RESERVE)] = {0};


void test_reservations() {
	char* data2[4] = {"test.client.ip2", "12:30am", "3:30am", "3"};

	for (int i = 0; i < 10; i++) {
		char buffer[32];
		sprintf(buffer,"test.client.%d",i);

		char* data[4] = {buffer, "ts", "te", "duration"};
		create_reservation(data);
	}

	print_reservations();

	delete_reservation("test.client.0");

	delete_reservation("test.client.3");

	delete_reservation("test.client.10");


	for (int i = 6; i < 15; i++) {
		char buffer[32];
		sprintf(buffer,"test.client.%d",i);

		char* data[4] = {buffer, "ts", "te", "duration"};
		create_reservation(data);
	}

	print_reservations();

	exit(SUCCESS);
}


int main(int argc, char const* argv[]) {
	test_reservations();
	struct sockaddr_in address;

	int server_fd, client_fd, valread, opt, addrlen;
	char *ACK, *cmd, host[256];
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

	// bind
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return FAILURE;
    }

	// printf("starting server on:\n");

	pid = fork();

	if (pid == 0) {
		// int status = 0b0;

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
		} else {
			char *data[4] = {"192.168.0.1", "12:30am", "3:30am", "3"};
			if (strcmp(buffer, "CREATE") == 0) {
				create_reservation(data);
			} else if (strcmp(buffer, "DELETE") == 0) {
				delete_reservation(buffer);
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
    return SUCCESS;
}

// docker run --mount source=shared-stuff,target=/stuff
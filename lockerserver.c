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

static struct locker_reservations *reservations;

// instantiate a bit array of locker
static int bit_reservations[HOURS_IN_DAY * (MINS_IN_HOURS/LOCKER_MIN_TIME_RESERVE)] = {0};


void print_reservations() {
	if (reservations != NULL) {
		printf(" LOCKER: print_reservations\n");
		struct user_reservation *element = reservations -> user_reservations;
		while (element != NULL) {
			printf("Locker User: %s\n", element->name);
			element = element -> next;
		}
	}
}


struct user_reservation* search_reservation(char* name) {
	if (reservations != NULL) {
		printf(" LOCKER: search_reservation\n");
		struct user_reservation *element = reservations -> user_reservations;
		// go to the end
		while (element != NULL) {
			// printf("%s\n",elgit ement -> name);
			if (strcmp(element -> name, name) != 0)
				return element;
			element = element -> next;
		}
	}
	return NULL;
}


struct user_reservation* create_reservation(char** entry_data) {
	printf(" LOCKER: create_reservation: ");

	struct user_reservation *element = NULL;

	if (reservations == NULL) {
		printf("first entry ");
		reservations = malloc(sizeof(struct locker_reservations));
	}

	printf("append entry\n");
	if (search_reservation(entry_data[0]) == NULL) {
		

		// malloc the actual reservation
		reservations -> user_reservations = malloc(sizeof(struct user_reservation));

		// create a pointer to tha malloc region
		element = reservations -> user_reservations; 

		// go to the end
		while (element != NULL) {
			element = element -> next;
		}

		/*
		char* name;
        char* start_time;
        char* end_time;

        int KEY;
        int start_time_idx;
        int end_time_idx;

        int duration_hours;
        int bit_duration;
		*/

		element = malloc(sizeof(struct user_reservation));

		element -> name = strdup(entry_data[0]);
		element -> start_time = strdup(entry_data[1]);
		element -> end_time = strdup(entry_data[2]);
		element -> duration_hours = atoi(entry_data[3]);

		printf("%s\n",element->name);

		element -> next = NULL;	
	}

	return element;
	
}


struct user_reservation* delete_reservation(char* name) {
	if (reservations != NULL) {
		if (search_reservation(name) != NULL) {
			printf(" LOCKER: delete_reservation\n");

			struct user_reservation* prev = NULL;
			struct user_reservation* curr = reservations -> user_reservations;
			struct user_reservation* next = reservations -> user_reservations -> next;

			//chain it
			while (curr -> name != name && curr != NULL) {
				prev = curr;
				curr = next;
				next = curr->next;
			}

			if (prev)
				prev -> next = next;

			return curr;
		}
	}
	return NULL;
}


struct user_reservation* modify_reservation(char* name) {
	return NULL;
}


int main(int argc, char const* argv[]) {
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
		int status = 0b0;

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

			char* data[4] = {"test.client.ip", "12:30am", "3:30am", "3"};
			struct user_reservation* new;
			if (strcmp(buffer, "CREATE") == 0) {
				new = create_reservation(data);
			} else if (strcmp(buffer, "DELETE") == 0) {
				new = delete_reservation("test.client.ip");
			} else if (strcmp(buffer, "MODIFY") == 0) {

			} else {
				perror("locker command");
			}
			print_reservations();
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
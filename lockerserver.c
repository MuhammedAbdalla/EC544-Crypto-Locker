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

static struct locker_reservations *reservations = NULL;

// instantiate a bit array of locker
static int bit_reservations[HOURS_IN_DAY * (MINS_IN_HOURS/LOCKER_MIN_TIME_RESERVE)] = {0};


void print_reservations(struct locker_reservations *reservations) {
	printf(" LOCKER: print_reservations\n");
	struct user_reservation *element = reservations->head;
	while (element != NULL) {
		printf("  User: %s\n", element->name);
		element = element -> next;
	}
}


struct user_reservation* search_reservation(char* name) {
	if (reservations != NULL) {
		printf(" LOCKER: search_reservation");
		struct user_reservation *element = reservations->head;
		// go to the end
		while (element != NULL) {
			// printf("%s\n",elgit ement -> name);
			if (strcmp(element->name, name) == 0) {
				printf("  found %s\n",element->name);
				return element;
			}
			element = element -> next;
		}
	}
	printf(" NULL\n");
	return NULL;
}


struct user_reservation* create_reservation(char** entry_data) {
	printf(" LOCKER: create_reservation: ");

	if (reservations == NULL) {
		printf("first entry ");

		// malloc the linked list
		reservations = malloc(sizeof(struct locker_reservations));
		reservations -> head = malloc(sizeof(struct user_reservation));

		reservations -> head->name = strdup(entry_data[0]);
		reservations -> head->start_time = strdup(entry_data[1]);
		reservations -> head->end_time = strdup(entry_data[2]);
		reservations -> head->duration_hours = atoi(entry_data[3]);

		reservations -> head -> next = NULL;	
	} else {
		printf("append entry\n");
		if (search_reservation(entry_data[0]) == NULL) {
			struct user_reservation* cursor = reservations->head;

			while (cursor->next!=NULL)
				cursor=cursor->next;
			

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
		
			struct user_reservation* newReservation = malloc(sizeof(struct user_reservation));
			
			newReservation->name = strdup(entry_data[0]);
			newReservation->start_time = strdup(entry_data[1]);
			newReservation->end_time = strdup(entry_data[2]);
			newReservation->duration_hours = atoi(entry_data[3]);
			newReservation -> next = NULL;	

			cursor->next= newReservation;

			return newReservation;
		}	
	}
	return NULL;
}


struct user_reservation* delete_reservation(char* name) {
	if (reservations != NULL) {
		printf(" LOCKER: delete_reservation\n");
		if (search_reservation(name) != NULL) {
			struct user_reservation* prev = NULL;
			struct user_reservation* curr = reservations->head;
			struct user_reservation* next = reservations->head->next;

			// if first element first element of the LL
			if (strcmp(curr->name, name) == 0) {
				reservations->head = reservations->head->next;
				return curr;
			}

			while (strcmp(curr->name, name) != 0 && curr != NULL) {
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

void test_reservations(){
	char* data1[4] = {"test.client.ip1", "12:30am", "3:30am", "3"};
	char* data2[4] = {"test.client.ip2", "12:30am", "3:30am", "3"};

	struct user_reservation* new;

	create_reservation(data1);
	print_reservations(reservations);
	create_reservation(data2);
	print_reservations(reservations);
	delete_reservation("test.client.ip1");
	print_reservations(reservations);

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
			struct user_reservation *new;
			char **data;
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
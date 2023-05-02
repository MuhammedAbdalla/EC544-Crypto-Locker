#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "locker.h"

pid_t pid, childpid;     
int status;

static struct locker_reservations* reservations;
static struct user_reservation set_bit_duration(int start_idx, int end_idx);

int connect_aws(const char* SERVER_IP, const char* cmd) {
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        printf("\n Socket creation error \n");
        return FAILURE;
    }
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(AWS_PORT);
  
    // Convert IPv4 and IPv6 addresses from text to binary
    if (inet_pton(AF_INET, AWS_IP, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported\n");
        return FAILURE;
    }
	
	printf("connecting to %s:%d\n",AWS_IP, AWS_PORT);
    status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (status == -1) {
        printf("\nConnection Failed\n");
        return FAILURE;
    }

    // //Send some data - HTTP example
	// message = "GET / HTTP/1.1\r\n\r\n";
    printf("[client]: %s\n", cmd);
    if (send(client_fd, cmd, strlen(cmd), 0) == -1) {
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
	printf("[server|%s]: %s\n", AWS_IP, buffer);
    // closing the connected socket
    close(client_fd);

	return SUCCESS;
}

int ssh_setup(void) {
	char cmd[256];

    if ((pid = fork()) == 0) {
        char *args[3] = {"ls","-a", NULL};
        if (execvp(args[0], args) == -1)
            perror("system commands");
    } else {
        childpid = waitpid(-1, &status, 0);
        if (WIFEXITED(status)) 
            printf("Child %d terminated with status: %d\n", childpid, WEXITSTATUS(status));
        
    }
    
	printf("\n...connecting to AWS cloud...\n");
	strcpy(cmd, "./AWS/activate_aws.sh");
	if (system(cmd) == -1) {
		perror("activate AWS ssh");
		return FAILURE;
	}

	strcpy(cmd, "./AWS/connect_aws.sh");
	if (system(cmd) == -1){
		perror("connect AWS ssh");
		return FAILURE;
	}

	printf("\n[[[ connected to AWS cloud ]]]\n");
	return SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// print all reservation details
static void print_reservations(struct locker_reservations *R) {
	printf(" LOCKER: print_reservations\n");
	struct user_reservation *element = reservations->head;
	while (element != NULL) {
		printf("  User: %s\n", element->user);
		element = element -> next;
	}
}

// searches for reservations
// returns null if none found
static struct user_reservation* search_reservation(char* user) {
	if (reservations != NULL) {
		struct user_reservation *element = reservations->head;
		// go to the end
		while (element != NULL) {
			// printf("%s\n",elgit ement -> name);
			if (strcmp(element->user, user) == 0) {
				printf(" LOCKER: search_reservation found %s\n",element->user);
				return element;
			}
			element = element -> next;
		}
	}
	return NULL;
}

// create new user reservation if name doesn't exist
// use name as client IP for uniqueness
static struct user_reservation* create_reservation(char** entry_data) {
	printf(" LOCKER: create_reservation: ");

	if (reservations == NULL) {
		reservations = (struct locker_reservations*) malloc(sizeof(struct locker_reservations));
		printf("first entry\n");

		// malloc the linked list

		reservations->head = (struct user_reservation*) malloc(sizeof(struct user_reservation));

		reservations->head->user = strdup(entry_data[0]);
		reservations->head->start_time = strdup(entry_data[1]);
		reservations->head->end_time = strdup(entry_data[2]);
		reservations->head->duration_hours = atoi(entry_data[3]);

		reservations->head->next = NULL;	
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
		
			struct user_reservation* newReservation = (struct user_reservation*) malloc(sizeof(struct user_reservation));
			
			newReservation->user = strdup(entry_data[0]);
			newReservation->start_time = strdup(entry_data[1]);
			newReservation->end_time = strdup(entry_data[2]);
			newReservation->duration_hours = atoi(entry_data[3]);
			newReservation -> next = NULL;	

			cursor->next = newReservation;

			return newReservation;
		}	
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int test_reservations(void) {
  reservations = NULL;
	// reservations->head = NULL;

	for (int i = 0; i < 10; i++) {
		char buffer[32];
		sprintf(buffer,"test.client.%d",i);

		char* data[4] = {buffer, "ts", "te", "duration"};
		create_reservation(data);
	}

	printf("reservations exists");

	print_reservations(reservations);

	delete_reservation("test.client.0");

	delete_reservation("test.client.3");

	delete_reservation("test.client.10");


	for (int i = 6; i < 15; i++) {
		char buffer[32];
		sprintf(buffer,"test.client.%d",i);

		char* data[4] = {buffer, "ts", "te", "duration"};
		create_reservation(data);
	}

	print_reservations(reservations);

	printf("passed reservations test\n");

	return SUCCESS;
}

// delete from LL if exists
static struct user_reservation* delete_reservation(char* user) {
	if (reservations != NULL) {
		printf(" LOCKER: delete_reservation\n");
		if (search_reservation(user) != NULL) {
			struct user_reservation* prev = NULL;
			struct user_reservation* curr = reservations->head;
			struct user_reservation* next = reservations->head->next;

			// if first element first element of the LL
			if (strcmp(curr->user, user) == 0) {
				reservations->head = reservations->head->next;
				return curr;
			}

			while (strcmp(curr->user, user) != 0 && curr != NULL) {
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

void update_reservation(char* user) {
  if (reservations != NULL) {
    reservations -> USER = reservations -> head -> user;
    if (reservations -> head == NULL) {
      reservations -> LOCKED = 0;
    }
  }
}


// delete from LL if exists
// set the bits FIRST then do bitwise XOR, then if bits overlap, return overlap error
static struct user_reservation* modify_reservation(char* user) {
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


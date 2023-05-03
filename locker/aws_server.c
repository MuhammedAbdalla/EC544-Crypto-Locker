#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "aws_server.h"



static struct locker_reservations* reservations;
static struct user_reservation set_bit_duration(int start_idx, int end_idx);
static struct locker_keys keys;
static int server_fd, esp32_fd;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// print all reservation details
static void print_reservations(struct locker_reservations* R) {
	if (R != NULL) {
		printf(" LOCKER: print_reservations\n");
		struct user_reservation *element = R->head;
		while (element != NULL) {
			printf("  User: %s\n", element->user);
			element = element -> next;
		}
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
static struct user_reservation* create_reservation(char entry_data[][16]) {
	printf(" LOCKER: create_reservation: ");

	if (reservations == NULL) {
		reservations = (struct locker_reservations*) malloc(sizeof(struct locker_reservations));
		printf("first entry\n");

		// malloc the linked list

		reservations->head = (struct user_reservation*) malloc(sizeof(struct user_reservation));

		reservations->head->user = strdup(entry_data[1]);
		reservations->head->start_time = strdup(entry_data[3]);
		reservations->head->end_time = strdup(entry_data[3]);
		reservations->head->duration_hours = atoi(entry_data[7]);
		
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
			
			newReservation->user = strdup(entry_data[1]);
			newReservation->start_time = strdup(entry_data[2]);
			newReservation->end_time = strdup(entry_data[3]);
			newReservation->duration_hours = atoi(entry_data[7]);
			newReservation -> next = NULL;	

			cursor->next = newReservation;

			return newReservation;
		}	
	}
	return NULL;
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

static void update_reservation() {
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


// returns the file descriptor of the AWS
struct sockaddr_in setup_socket() {
	struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
	
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		perror("socket failed");
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
	}

	// prepare the the socket structure
	address.sin_family = AF_INET; // inet_addr("IP ADDRESS")
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(SERVER_PORT);

	// bind
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
    }

	printf("hosting server ec2-user@%s:%d\n",AWS_IP, SERVER_PORT);
	return address;
}

void listen_aws(struct sockaddr_in server) {
	struct sockaddr_in client;
	char *ACK = "ACK";
	char buffer[1024][16] = {0};
	int addrlen = sizeof(server);

	if (listen(server_fd, 1) < 0) {
        perror("listen");
        exit(FAILURE);
    }


	int client_fd;
	for (int i = 0; (client_fd = accept(server_fd, (struct sockaddr*)&client,(socklen_t*)&addrlen)) != -1 ; i++) {
		if (read(client_fd, buffer[i], 1024) == -1)
			perror("read");

		if (strcmp(buffer[i], "\0") == 0)
			break;

		printf("[SERVER] \"client msg %d: %s\"\n", i, buffer[i]);

		if (strcmp(buffer[i], "ESP32-LOCKER") == 0)
			break;
		
	}
	
	if (strcmp(buffer[0], "CREATE") == 0) {
		printf("create");
		create_reservation(buffer);
		// print_reservations(reservations);
	} else if (strcmp(buffer[0], "DELETE") == 0) {
		printf("delete");
		delete_reservation(buffer[1]);
	} else if (strcmp(buffer[0], "DISPLAY") == 0) {
		print_reservations(reservations);
	} else if (strcmp(buffer[0], "ESP32-LOCKER") == 0) {
		printf("LOCKER CONNECT");
		esp32_fd = client_fd;
	}


	update_reservation();


	if (send(client_fd, ACK, strlen(ACK), 0) == FAILURE)
		perror("ACK message failed");


	// if (esp32_fd != -1) {
	// 	printf("writing to ESP32");
	// 	char* str;
	// 	sprintf(str, "%d", reservations->LOCKED);
	// 	if (send(esp32_fd, str, strlen(str), 0) == FAILURE)
	// 		perror("esp32 message failed");
	// }
	
	// if (strcmp(buffer[0], "ESP32-LOCKER") != 0) {
		close(client_fd);
	// }
}


int test_reservations(void) {
  reservations = NULL;
	// reservations->head = NULL;

	for (int i = 0; i < 10; i++) {
		char buffer[32];
		sprintf(buffer,"test.client.%d",i);

		// char data[1024][4] = {buffer, "ts", "te", "duration"};
		// create_reservation(data);
	}

	printf("reservations exists\n");

	print_reservations(reservations);

	delete_reservation("test.client.0");

	delete_reservation("test.client.3");

	delete_reservation("test.client.10");


	for (int i = 6; i < 15; i++) {
		char buffer[32];
		sprintf(buffer,"test.client.%d",i);

		// char data[1024][4] = {buffer, "ts", "te", "duration"};
		// create_reservation(data);
	}

	print_reservations(reservations);

	printf("passed reservations test\n");

	return SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[]) {
	server_fd = esp32_fd = -1;

	struct sockaddr_in server = setup_socket();

	if (server_fd == FAILURE) {
		perror("connection to AWS");
		exit(FAILURE);
	}

	while (1) {
		 listen_aws(server);
		// SET UP CONNECTION OF SOCKET
	}
}
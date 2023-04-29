#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "lockerfunctions.h"


void print_reservations(struct locker_reservations *R) {
	printf(" LOCKER: print_reservations\n");
    printf("%s\n",R->head->name);
	struct user_reservation *element = R->head;
	while (element != NULL) {
		printf("  User: %s\n", element->name);
		element = element -> next;
	}
}


struct user_reservation* search_reservation(char* name) {
	if (reservations != NULL) {
		struct user_reservation *element = reservations->head;
		// go to the end
		while (element != NULL) {
			// printf("%s\n",elgit ement -> name);
			if (strcmp(element->name, name) == 0) {
				printf(" LOCKER: search_reservation found %s\n",element->name);
				return element;
			}
			element = element -> next;
		}
	}
	return NULL;
}

struct user_reservation* create_reservation(char** entry_data) {
	printf(" LOCKER: create_reservation: ");

	if (reservations == NULL) {
		printf("first entry\n");

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
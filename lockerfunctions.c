// #include <stdlib.h>
// #include <string.h>
// #include <stdio.h>
// #include <unistd.h>
// #include "lockerfunctions.h"

// void print_reservations(struct locker_reservations* reservations) {
// 	printf("print_reservations\n");
// 	struct user_reservation *element = reservations -> user_reservations;
// 	printf("first entry\n");
// 	while (element != NULL) {
// 		printf("Locker User: %s", element->name);
// 		element = element -> next;
// 	}
// 	free(element);
// }

// struct user_reservation* search_reservation(struct locker_reservations* reservations, char* name) {
// 	printf("search_reservation\n");
// 	struct user_reservation *element = reservations -> user_reservations;
// 	// go to the end
// 	while (element != NULL) {
// 		printf("%s\n",element -> name);
// 		if (strcmp(element -> name, name) != 0)
// 			return element;
// 		element = element -> next;
// 	}

// 	free(element);

// 	return NULL;
// }

// struct user_reservation* create_reservation(struct locker_reservations* reservations, char** entry_data) {
// 	printf("create_reservation\n");

// 	struct user_reservation *element;
// 	if (reservations == NULL) {
// 		reservations = malloc(sizeof(struct locker_reservations));
// 	}

// 	if (search_reservation(reservations, entry_data[0]) == NULL) {
// 		element = reservations -> user_reservations;
// 		printf("the rest\n");
// 		// go to the end
// 		while (element != NULL) {
// 			element = element -> next;
// 		}

// 		/*
// 		char* name;
//         char* start_time;
//         char* end_time;

//         int KEY;
//         int start_time_idx;
//         int end_time_idx;

//         int duration_hours;
//         int bit_duration;
// 		*/

// 		element = malloc(sizeof(struct user_reservation));

// 		element -> name = entry_data[0];
// 		element -> start_time = entry_data[1];
// 		element -> end_time = entry_data[2];
// 		element -> duration_hours = atoi(entry_data[3]);

// 		printf("%s\n",element->name);

// 		element -> next = NULL;
// 		return element;
// 	}

	
// }

// struct user_reservation* delete_reservation(struct locker_reservations* reservations, char* name) {
// 	printf("delete_reservation\n");
//     if (search_reservation(reservations, name) != NULL) {

//         struct user_reservation* prev = NULL;
//         struct user_reservation* curr = reservations -> user_reservations;
//         struct user_reservation* next = reservations -> user_reservations -> next;

//         //chain it
// 		while (curr -> name != name && curr != NULL) {
// 			prev = curr;
// 			curr = next;
// 			next = curr->next;
// 		}

// 		if (prev)
// 			prev -> next = next;

// 		return curr;
//     }

// 	return NULL;
// }

// struct user_reservation* modify_reservation(struct locker_reservations* reservations, char* name) {
// 	return NULL;
// }
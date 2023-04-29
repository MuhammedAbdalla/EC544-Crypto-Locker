#include <string.h>
#include <stdio.h>
#include "lockerfunctions.h"

struct locker_reservations *reservations;

struct user_reservation* search_reservation(char* name) {
	struct user_reservation *element = reservations -> user_reservations;
	// go to the end
	while (element != NULL) {
		if (strcmp(element -> name, name) != 0)
			return element;
		element = element -> next;
	}

	free(element);

	return NULL;
}

struct user_reservation* create_reservation(char** entry_data) {
	struct user_reservation *element = reservations -> user_reservations;
	if (search_reservation(entry_data[0]) == NULL) {
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

		element -> name = entry_data[0];
		element -> start_time = entry_data[1];
		element -> end_time = entry_data[2];
		element -> duration_hours = atoi(entry_data[3]);

		element -> next = NULL;

	}

	return element;
}

struct user_reservation* delete_reservation(char* name) {
    if (search_reservation(name) != NULL) {
        struct user_reservation* curr = reservations

        return NULL;
    }

	return NULL;
}

struct user_reservation* modify_reservation(char* name) {
	return NULL;
}
#include "lockerfunctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_ASSERT(x) do { \
	if (!(x)) { \
		fprintf(stderr, "%s:%d: Assertion (%s) failed!\n", \
				__FILE__, __LINE__, #x); \
	       	abort(); \
	} \
} while(0)


int main(void)
{ 
	reservations = NULL;

	for (int i = 0; i < 10; i++) {
		char buffer[32];
		sprintf(buffer,"test.client.%d",i);

		char* data[4] = {buffer, "ts", "te", "duration"};
		create_reservation(data);
	}

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

	return 0;
}
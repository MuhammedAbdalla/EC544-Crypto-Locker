
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
	
	char* data2[4] = {"test.client.ip2", "12:30am", "3:30am", "3"};

	for (int i = 0; i < 10; i++) {
		char* data[4] = {sprintf("test.client.ip%s", i+""), "12:30am", "3:30am", "3"};
		create_reservation(data);
	}

	print_reservations(reservations);

	delete_reservation("test.client.ip1");

	print_reservations(reservations);

	delete_reservation("test.client.ip3");

	print_reservations(reservations);

	delete_reservation("test.client.ip10");

	print_reservations(reservations);

	for (int i = 6; i < 15; i++) {
		char* data[4] = {sprintf("test.client.ip%d",i), "12:30am", "3:30am", "3"};
		create_reservation(data);
	}

	return 0;
}
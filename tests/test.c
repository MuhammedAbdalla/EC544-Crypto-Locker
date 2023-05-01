#include "../server/locker.h"
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


int main(int argc, char* argv[])
{ 
	reservations = NULL;

	if (strcmp(argv[1], "aws")) {
    	TEST_ASSERT(connect_aws(argv[1]) == SUCCESS);
		// TEST_ASSERT(ssh_setup(argv[1]) == SUCCESS);
		printf("passed ssh test\n");
	} else if (strcmp(argv[1], "locker")) {
		TEST_ASSERT(test_reservations() == SUCCESS); 
		printf("passed reservations test\n");
	}
	
}
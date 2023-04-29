#include "lockerserver.h"
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
	struct pipeline* my_pipeline = pipeline_build("ls\n   &");

	// Test that a pipeline was returned
	TEST_ASSERT(my_pipeline != NULL);
}
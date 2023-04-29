override CFLAGS := -Wall -Werror -std=gnu99 -O1 -g $(CFLAGS) -I.

# Build the locker .o file
lockerserver.o: lockerserver.c lockerfunctions.h

# Run the test programs
clean:
	rm -f *.o 

create:
	gcc lockerserver.c lockerfunctions.c -g -o lockerserver
	./lockerserver

test:
	gcc tests/test_1.c lockerfunctions.h -g -o test_1
	./test_1
override CFLAGS := -Wall -Werror -std=gnu99 -O1 -g $(CFLAGS) -I.

# Build the parser .o file
lockerserver.o: lockerserver.c lockerserver.h
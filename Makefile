override CFLAGS := -Wall -Werror -std=gnu99 -O1 -g $(CFLAGS) -I.

# Build the locker .o file
lockerfunctions.o: lockerfunctions.c lockerfunctions.h

# Build the test file
test_c_files=$(shell find tests -type f -name '*.c')
tests_o_files=$(test_c_files:.c=.o)
test_files=$(test_c_files:.c=)

.SECONDARY: $(tests_o_files)
.PHONY: clean checkprogs

tests/%: tests/%.o lockerfunctions.o
	$(CC) $(LDFLAGS) $+ $(LOADLIBES) $(LDLIBS) -o $@ -g

checkprogs: $(test_files)

# Run the test programs
test: checkprogs
	tests/run_tests.sh $(test_files)

create: 
	gcc lockerserver.c lockerfunctions.c -g -o lockerserver
	./lockerserver

clean:
	rm -f *.o $(test_files) $(test_o_files)

override CFLAGS := -Wall -Werror -std=gnu99 -O1 -g $(CFLAGS) -I.

# Build the locker .o file
lockerfunctions.o: server/lockerfunctions.c server/lockerfunctions.h

# Build the test file
test_c_files=$(shell find tests -type f -name '*.c')
tests_o_files=$(test_c_files:.c=.o)
test_files=$(test_c_files:.c=)

.SECONDARY: $(tests_o_files)
.PHONY: clean checkprogs

test/%: tests/%.o lockerfunctions.o
	$(CC) $(LDFLAGS) $+ $(LOADLIBES) $(LDLIBS) -o $@ -g

checkprogs: $(test_files)

# Run the test programs
test_aws:
	gcc tests/tests_aws.c server/lockerfunctions.c -g
	cp a.out server/a.out
	./server/a.out $(ARGS)

test:
	gcc tests/test.c server/lockerfunctions.c -g
	cp a.out server/a.out
	./a.out $(ARGS)


clean:
	rm -f *.o $(test_files) $(test_o_files)

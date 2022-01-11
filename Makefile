CC=gcc
CFLAGS=-g -Werror -Wall
LDLIBS=-lpthread


all:test test-format test_posix test_ours

test_posix: test_posix.o
	$(CC) -o $@ $(CFLAGS) $^ $(LDLIBS)
test_ours:test_ours.o stdes.o
	$(CC) -o $@ $(CFLAGS) $^ $(LDLIBS)
test-format:test-format.o stdes.o
	$(CC) -o $@ $(CFLAGS) $^ $(LDLIBS)
test:test.o stdes.o
	$(CC) -o $@ $(CFLAGS) $^ $(LDLIBS)
stdes.o: stdes.c stdes.h
	$(CC) -c $(CFLAGS) $<
test.o: test.c stdes.h
	$(CC) -c $(CFLAGS) $< 
test-format.o: test-format.c stdes.h
	$(CC) -c $(CFLAGS) $< 
test_posix.o : test_posix.c
	$(CC) -c $(CFLAGS) $<
test_ours.o : test_ours.c stdes.h
	$(CC) -c $(CFLAGS) $<
clean:
	rm -f *.o test test-format test_posix test_ours
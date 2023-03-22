all: main

assembly: CFLAGS += -S -fno-asynchronous-unwind-tables
assembly: main
debug: CFLAGS += -DDEBUG -g -pedantic-errors
debug: main

CC = gcc $(CFLAGS)

main: main.o alloc_ff_mmap.o free_ff_mmap.o
	$(CC) -o main main.o alloc_ff_mmap.o free_ff_mmap.o

main.o: main.c free_list.h
	$(CC) -c main.c

alloc_ff_mmap.o: alloc_ff_mmap.c free_list.h
	$(CC) -c alloc_ff_mmap.c

free_ff_mmap.o: free_ff_mmap.c free_list.h
	$(CC) -c free_ff_mmap.c

clean: 
	rm -rf main main.s a.out *.o *.dSYM
all: main

assembly: CFLAGS += -S
assembly: main
debug: CFLAGS += -o main -DDEBUG -g
debug: main

CC = gcc $(CFLAGS)

main: 
	$(CC) main.c

clean: 
	rm -f main main.s a.out
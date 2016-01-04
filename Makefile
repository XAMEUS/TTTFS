CC = gcc
CFLAGS = -Wall
EXEC = all
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))

all: tfs_create test

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< '-lm'

tfs_create: ll.o error.o tfs_create.c
	gcc ll.o tfs_create.c -o tfs_create '-lm'

test: ll.o error.o test.c
	gcc ll.o test.c -o test '-lm'

clean:
	rm -f $(OBJECTS)

mrproper: clean
	rm -f $(EXEC)

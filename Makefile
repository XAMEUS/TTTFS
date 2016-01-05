CC = gcc
CFLAGS = -Wall
EXEC = all
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))

all: tfs_create tfs_partition tfs_analyse test

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< '-lm'

tfs_create: ll.o error.o tfs_create.c
	gcc ll.o tfs_create.c -o tfs_create '-lm' -Wall

tfs_partition: ll.o error.o tfs_partition.c
	gcc ll.o tfs_partition.c -o tfs_partition '-lm' -Wall

tfs_analyse: ll.o error.o tfs_analyse.c
	gcc ll.o tfs_analyse.c -o tfs_analyse '-lm' -Wall

test: ll.o error.o test.c
	gcc ll.o test.c -o test '-lm' -Wall

clean:
	rm -f $(OBJECTS)

mrproper: clean
	rm -f $(EXEC)

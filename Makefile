CC = gcc
CFLAGS = -Wall
EXEC = all
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))

all: tfs_create tfs_partition tfs_analyse tfs_format

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< '-lm'

tfs_create: ll.o error.o tfs_create.c
	gcc ll.o tfs_create.c -o tfs_create '-lm' -Wall

tfs_partition: ll.o error.o tfs_partition.c
	gcc ll.o tfs_partition.c -o tfs_partition '-lm' -Wall

tfs_analyse: ll.o error.o tfs_analyse.c
	gcc ll.o tfs_analyse.c -o tfs_analyse '-lm' -Wall

tfs_format: ll.o error.o tfs_format.c
	gcc ll.o tfs_format.c -o tfs_format '-lm' -Wall

disk:
	./tfs_create; ./tfs_partition -p 32; ./tfs_format
	./tfs_analyse

od:
	od -D disk.tfs

clean:
	rm -f $(OBJECTS)
	rm tfs_create tfs_partition tfs_analyse tfs_format
	rm disk.tfs

mrproper: clean
	rm -f $(EXEC)

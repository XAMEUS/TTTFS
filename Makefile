CC = gcc
CFLAGS = -Wall
EXEC = all
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))

all: tfs_create tfs_partition tfs_analyse tfs_format tfs_cp test

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< '-lm'

tfs_create: ll.o error.o tfs_create.c
	gcc ll.o tfs_create.c -o tfs_create '-lm' -Wall

tfs_partition: ll.o error.o tfs_partition.c
	gcc ll.o tfs_partition.c -o tfs_partition '-lm' -Wall

tfs_analyse: ll.o error.o tfs_analyse.c
	gcc ll.o tfs_analyse.c -o tfs_analyse '-lm' -Wall

tfs_format: ll.o error.o tfs_format.c stdltfs.o
	gcc ll.o tfs_format.c -o tfs_format '-lm' -Wall

tfs_cp: ll.o error.o stdltfs.o stdiotfs.o tfs_cp.c stdltfs.o
	gcc ll.o stdltfs.o stdiotfs.o error.o tfs_cp.c -o tfs_cp '-lm' -Wall

test: ll.o error.o stdltfs.o stdiotfs.o
	gcc ll.o stdltfs.o stdiotfs.o error.o test.c -o out '-lm' -Wall

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

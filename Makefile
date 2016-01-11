CC = gcc
CFLAGS = -Wall
EXEC = all
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))

all: tfs_create tfs_partition tfs_analyse tfs_format tfs_cp tfs_mkdir tfs_rm tfs_cat

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< '-lm'

tfs_create: utils.o ll.o error.o tfs_create.c
	gcc utils.o ll.o tfs_create.c -o tfs_create '-lm' -Wall

tfs_partition: utils.o ll.o error.o tfs_partition.c
	gcc utils.o ll.o tfs_partition.c -o tfs_partition '-lm' -Wall

tfs_analyse: utils.o ll.o error.o tfs_analyse.c
	gcc utils.o ll.o tfs_analyse.c -o tfs_analyse '-lm' -Wall

tfs_format: utils.o ll.o error.o tfs_format.c stdltfs.o
	gcc utils.o ll.o tfs_format.c -o tfs_format '-lm' -Wall

tfs_cp: utils.o ll.o error.o stdltfs.o stdiotfs.o tfs_cp.c stdltfs.o
	gcc utils.o ll.o stdltfs.o stdiotfs.o error.o tfs_cp.c -o tfs_cp '-lm' -Wall

tfs_mkdir: utils.o ll.o error.o stdltfs.o stdiotfs.o tfs_mkdir.c stdltfs.o
	gcc utils.o ll.o stdltfs.o stdiotfs.o error.o tfs_mkdir.c -o tfs_mkdir '-lm' -Wall

tfs_rm: utils.o ll.o error.o stdltfs.o stdiotfs.o tfs_rm.c stdltfs.o
	gcc utils.o ll.o stdltfs.o stdiotfs.o error.o tfs_rm.c -o tfs_rm '-lm' -Wall

tfs_cat: utils.o ll.o error.o stdltfs.o stdiotfs.o tfs_cat.c stdltfs.o
	gcc utils.o ll.o stdltfs.o stdiotfs.o error.o tfs_cat.c -o tfs_cat '-lm' -Wall

test: utils.o ll.o error.o stdltfs.o stdiotfs.o
	gcc utils.o ll.o stdltfs.o stdiotfs.o error.o test.c -o out '-lm' -Wall

disk:
	./tfs_create; ./tfs_partition -p 32; ./tfs_format
	./tfs_analyse

od:
	od -D disk.tfs

clean:
	rm -f $(OBJECTS)
	rm tfs_create tfs_partition tfs_analyse tfs_format tfs_cp tfs_mkdir tfs_rm tfs_cat
	rm disk.tfs

mrproper: clean
	rm -f $(EXEC)

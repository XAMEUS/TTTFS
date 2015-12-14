CC = gcc
CFLAGS = -Wall
EXEC = tfs_create
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) -o $@ $^ '-lm'

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< '-lm'

clean:
	rm -f $(OBJECTS)

mrproper: clean
	rm -f $(EXEC)

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "error.h"

typedef struct disk_id {
	int fd;
} disk_id;

struct block {
	unsigned char data[1024];
};

typedef struct block* block;

error read_block(disk_id id, block b, uint32_t num);

error write_block(disk_id id, block b, uint32_t num);


unsigned char* to_little_endian(uint32_t num);
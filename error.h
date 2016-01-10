#include <stdio.h>
#include <errno.h>

#define BLOCK_OUT_OF_BOUNDS 135
#define B0_FULL 136
#define DISK_COMPLETE_PARTITION 137
#define PARTITION_NOT_FOUND 138
#define FORMAT_NOT_ENOUGH_SPACE 139
#define BLOCK_ALREADY_FREE 140
#define ALL_BLOCKS_ALLOCATED 141
#define FULL_FILE_TABLE 142
#define FILE_OUT_OF_BOUNDS 143
#define FILE_ALREADY_FREE 144
#define NOT_ENOUGH_FREE_BLOCKS 145

typedef int error;

char* error_description(error e);
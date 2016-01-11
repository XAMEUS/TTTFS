#include "stdltfs.h"

#define READ 1
#define WRITE 2
#define APPEND 4
#define TRUNC 8
#define CREATE 16

#define TFS_FOPEN_MAX 512

typedef struct {
    int id;
    int flags;
	uint32_t pos;
	disk_id disk;
	uint32_t par;
	uint32_t size;
} TFS_FILE;

int tfs_mkdir(disk_id id, uint32_t par, char* path, char* name);

int tfs_rmdir(disk_id id, uint32_t par, char *path, char* name);

int tfs_open(disk_id id, uint32_t par, char* path, char* name, int flags);

int tfs_close(int fd);

int tfs_read(int fd, void *buf, int size);

int tfs_write(int fd, void *buf, int size);

int tfs_get_size(int fd);
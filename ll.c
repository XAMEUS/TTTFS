#include "ll.h"
#include "error.h"

error read_physical_block(disk_id id, block b, uint32_t num);

error write_physical_block(disk_id id, block b, uint32_t num);

unsigned char* to_little_endian(uint32_t num)
{
	unsigned char* t = malloc (sizeof (char) *  4);
	int i;
	for (i = 0; i < 4; i = i +1)
	{
		t[i] = num % 256;
		num = num / 256;
	}
	return t;
}

uint32_t to_uint32_t(unsigned char* c)
{
	uint32_t num = 0;
	int i;
	for (i = 0; i < 4; i = i + 1)
	{
		num += c[i] * pow(256, i);
	}
	return num;
}

uint32_t read_uint32_t(block b, uint32_t pos)
{
	if (pos > 255) return BLOCK_OUT_OF_BOUNDS;
	int i;
	unsigned char* t = malloc (4 * sizeof (char));
	for (i = 0; i < 4; i = i + 1)
	{
		t[i]= b->data[i+pos*4];
	}
	return to_uint32_t(t);
}

error write_uint32_t(block b, uint32_t pos, uint32_t n)
{
	if (pos > 255) return BLOCK_OUT_OF_BOUNDS;
	int i;
	unsigned char* t = malloc (4 * sizeof (char));
	t = to_little_endian(n);
	for (i = 0; i < 4; i = i + 1)
	{
		b->data[i+pos*4] = t[i];
	}
	return 0;
}

error read_physical_block(disk_id id, block b, uint32_t num)
{
	errno = 0;
	int e = lseek(id.fd,(num*1024),SEEK_SET);
	if (e == -1) return errno;
	read(id.fd,b->data,1024);
	return errno;
}

error write_physical_block(disk_id id, block b, uint32_t num)
{
	errno = 0;
	int e = lseek(id.fd,(num*1024),SEEK_SET);
	if (e == -1) return errno;
	write(id.fd,b->data,1024);
	return errno;
}


error read_block(disk_id id, block b, uint32_t num)
{
	return read_physical_block(id, b, num);
}

error write_block(disk_id id, block b, uint32_t num)
{
	return write_physical_block(id, b, num);
}

error start_disk(char* name, disk_id* id)
{
	errno = 0;
	id->fd = open(name, O_RDWR);
	if (id->fd < 0) return errno;
	block b = malloc (sizeof (struct block));
	if (b == NULL) return 134;
	error e = read_block(*id, b, 0);
	if (e) return e;
	id->b0 = b;
	return 0;
}

error stop_disk(disk_id id)
{
	errno = 0;
	close(id.fd);
	return errno;
}


error sync_disk(disk_id id)
{
	return write_block(id, id.b0, 0);
}

void myfree(block b)
{	
	int i;
	for(i = 0; i < 1024; i++) b->data[i] = 0;
}
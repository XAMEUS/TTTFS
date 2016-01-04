#include <math.h>
#include <errno.h>
#include "ll.h"

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

error read_physical_block(disk_id id, block b, uint32_t num)
{
	lseek(id.fd,(num*1024),SEEK_SET);
	read(id.fd,b->data,1024);
	error e;
	return e;
}

error write_physical_block(disk_id id, block b, uint32_t num)
{
	lseek(id.fd,(num*1024),SEEK_SET);
	write(id.fd,b->data,1024);
	error e;
	return e;
}


error read_block(disk_id id, block b, uint32_t num)
{
	return read_physical_block(id, b, num);
}

error write_block(disk_id id, block b, uint32_t num)
{
	return write_physical_block(id, b, num);
}
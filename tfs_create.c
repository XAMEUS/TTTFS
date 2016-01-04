#include "ll.h"

int tfs_create(uint32_t size, char* name);

int tfs_create(uint32_t size, char* name)
{
	int i;
	disk_id dk;
	dk.fd = open(name, O_RDWR|O_TRUNC|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	block b = malloc (sizeof (struct block));
	unsigned char* t = malloc (4 * sizeof (char));
	t = to_little_endian(size);
	for (i = 0; i < 4; i = i + 1)
	{
		b->data[i] = t[i];
	}
	for (i = 4; i < 1024; i = i + 1)
	{
		b->data[i] = 0;
	}
	write_block(dk, b, 0);
	block bb = malloc (sizeof (struct block));

	for (i = 0; i < 1024; i = i + 1)
	{
		bb->data[i] = 0;
	}
	for (i = 1; i < size; i++)
	{
		write_block(dk, bb, i);
	}
	return 0;
}

int main(int argc, char* argv[])
{
	int xarg = 1;
	uint32_t size = 1024;
	char* filename = "disk.tfs";
	while (xarg < argc)
	{
		if (strcmp(argv[xarg], "-s") == 0)
		{
			if (xarg < argc - 1)
			{
				size = atoi(argv[xarg+1]);
				xarg += 2;
				continue;
			}
			else
			{
				fprintf(stderr, "-s doit être suivi d'un entier");
			}
		}
		else
		{
			filename = argv[xarg];
		}
		xarg += 1;
	}
	return tfs_create(size, filename);
}
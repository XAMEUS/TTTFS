#include "ll.h"

int tfs_partition(uint32_t size, disk_id* id);

int tfs_partition(uint32_t size, disk_id* id)
{
	uint32_t volume = read_uint32_t(id->b0, 0);
	uint32_t partitions = read_uint32_t(id->b0, 1);
	uint32_t partitions_available = 254 - partitions;
	if (partitions_available == 0)
		return B0_FULL;
	uint32_t free_blocks = volume;
	uint32_t i;
	for (i = 0; i < partitions; i = i + 1)
	{
		free_blocks -= read_uint32_t(id->b0, i+2);
	}
	if (free_blocks < size)
		return DISK_COMPLETE_PARTITION;
	write_uint32_t(id->b0, 1, partitions + 1);
	write_uint32_t(id->b0, partitions + 2, size);
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "Missing args\n");
		return 1;
	}
	int xarg = 1;
	uint32_t size = 0;
	char* filename;
	if (strcmp(argv[xarg], "-p") == 0)
		filename = "disk.tfs";
	else
	{
		filename = argv[xarg];
		xarg++;
	}
	disk_id dk;
	int e = 0;
	e = start_disk(filename, &dk);
	if (e != 0) return e;
	while (xarg < argc)
	{
		if (strcmp(argv[xarg], "-p") == 0)
		{
			if (xarg < argc - 1)
			{
				size = atoi(argv[xarg+1]);
				xarg += 2;
				e = tfs_partition(size, &dk);
				if (e != 0) return e;
				continue;
			}
			else
			{
				fprintf(stderr, "-p doit être suivi d'un entier");
				return 1;
			}
		}
		else
		{
			fprintf(stderr, "argument non reconnu :) (mais je t'aime)");
			return 1;
		}
	}
	e = sync_disk(dk);
	if (e != 0) return e;
	e = stop_disk(dk);
	return e;
}
#include "ll.h"

int tfs_partition(uint32_t size, disk_id id);

int tfs_partition(uint32_t size, disk_id id)
{
	int volume = read_uint32_t(id.b0, 0);
	int partitions = read_uint32_t(id.b0, 1);
	return 0;
}

int main(int argc, char* argv[])
{
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
				e = tfs_partition(size, dk);
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
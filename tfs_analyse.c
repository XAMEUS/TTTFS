#include "ll.h"

int main(int argc, char* argv[])
{
	char* filename;
	if (argc < 2)
		filename = "disk.tfs";
	else
		filename = argv[1];
	
	disk_id dk;
	int e = 0;
	e = start_disk(filename, &dk);
	if (e != 0) return e;
	uint32_t volume = read_uint32_t(dk.b0, 0);
	uint32_t partitions = read_uint32_t(dk.b0, 1);
	printf("DISK:: %s\n", filename);
	printf("VOLUME:: %u\n", volume);
	printf("PARTITIONS:: %u\n", partitions);
	int i;
	for (i = 0; i < partitions; i = i + 1)
	{
		printf("P%d:: %u\n", i, read_uint32_t(dk.b0, i+2));
	}
	e = stop_disk(dk);
	return e;
}
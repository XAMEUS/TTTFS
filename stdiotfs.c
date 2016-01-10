#include "stdiotfs.h"

int tfs_mkdir(disk_id id, uint32_t par, char* path, char* name)
{
	if (par >= read_uint32_t(id.b0, 1))
		return -PARTITION_NOT_FOUND;
	int e = id_from_path(id, par, path);
	if (e < 0) return e;
	block file_table = malloc (sizeof (struct block));
	read_block(id, file_table, id.pos_partition[par] + 1 + e / 16);
	uint32_t type = read_uint32_t(file_table, (e%16) * 16 + 1);
	if (type != 1) return -NOT_A_DIRECTORY;

    block p0 = malloc (sizeof (struct block));
	error er;
	if ((er = read_block(id, p0, id.pos_partition[par]))) return -er;
	uint32_t TTTFS_VOLUME_FIRST_FREE_FILE = read_uint32_t(p0, 7);
	
	uint32_t size = read_uint32_t(file_table, (e%16) * 16);
	update_blocks_file(id, par, e, size + 32);
	
	uint32_t pos_last_data_block = last_data_block_of_file(id, par, e);
    block last_data_block = malloc (sizeof (struct block));
	read_block(id, last_data_block, id.pos_partition[par] + pos_last_data_block);
	write_uint32_t(last_data_block, (size%1024)/4, TTTFS_VOLUME_FIRST_FREE_FILE);

	int i;
	for (i = 0; i < 27 && name[i] != '\0'; i++)
	{
		last_data_block->data[4 + size%1024 + i] = name[i];
	}
	last_data_block->data[4 + size%1024 + i] = '\0';
	write_block(id, last_data_block, id.pos_partition[par] + pos_last_data_block);

	create_file(id, par, e, 1, 0);
	return TTTFS_VOLUME_FIRST_FREE_FILE;
}
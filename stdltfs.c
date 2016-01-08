#include "stdltfs.h"
#include "ll.h"
#include "error.h"

int is_free(disk_id id, uint32_t pos_par, uint32_t free_block, uint32_t pos)
{
	uint32_t next_free_block;
	block b = malloc (sizeof (struct block));
	read_block(id, b, pos_par + free_block);
	next_free_block = read_uint32_t(b, 255);
	while (free_block != next_free_block)
	{
		if (free_block == pos)
			return 1;
		free_block = next_free_block;
		read_block(id, b, pos_par + free_block);
		next_free_block = read_uint32_t(b, 255);
	}
	if (free_block == pos) return 1;
	return 0;
}

error free_block(disk_id id, uint32_t par, uint32_t pos)
{
	if (par >= read_uint32_t(id.b0, 1))
		return PARTITION_NOT_FOUND;
	block p0 = malloc (sizeof (struct block));
	error e;
	if ((e = read_block(id, p0, id.pos_partition[par]))) return e;
	uint32_t TTTFS_VOLUME_FREE_BLOCK_COUNT = read_uint32_t(p0, 3);
	uint32_t TTTFS_VOLUME_BLOCK_COUNT = read_uint32_t(p0, 2);
	uint32_t TTTFS_VOLUME_FIRST_FREE_BLOCK = read_uint32_t(p0, 4);
	if (TTTFS_VOLUME_BLOCK_COUNT < pos)
		return BLOCK_OUT_OF_BOUNDS;
	if (is_free(id, id.pos_partition[par], TTTFS_VOLUME_FIRST_FREE_BLOCK, pos))
		return BLOCK_ALREADY_FREE;
	
	block b = malloc (sizeof (struct block));
	if (TTTFS_VOLUME_FREE_BLOCK_COUNT == 0)
		write_uint32_t(b, 255, pos);
	else
		write_uint32_t(b, 255, TTTFS_VOLUME_FIRST_FREE_BLOCK);
	
	write_uint32_t(p0, 4, pos);
	write_block(id, p0, id.pos_partition[par]);
	write_block(id, b, id.pos_partition[par] + pos);
	return 0;
}
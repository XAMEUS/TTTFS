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
	
	write_uint32_t(p0, 3, TTTFS_VOLUME_FREE_BLOCK_COUNT + 1);
	write_uint32_t(p0, 4, pos);
	write_block(id, p0, id.pos_partition[par]);
	write_block(id, b, id.pos_partition[par] + pos);
	return 0;
}

error alocate_block(disk_id id, uint32_t par)
{
	if (par >= read_uint32_t(id.b0, 1))
		return PARTITION_NOT_FOUND;
	block p0 = malloc (sizeof (struct block));
	error e;
	if ((e = read_block(id, p0, id.pos_partition[par]))) return e;
	uint32_t TTTFS_VOLUME_FREE_BLOCK_COUNT = read_uint32_t(p0, 3);
	uint32_t TTTFS_VOLUME_FIRST_FREE_BLOCK = read_uint32_t(p0, 4);
	if (TTTFS_VOLUME_FREE_BLOCK_COUNT == 0)
		return ALL_BLOCKS_ALLOCATED;
	
	block b = malloc (sizeof (struct block));
    if ((e = read_block(id, b, id.pos_partition[par] + TTTFS_VOLUME_FIRST_FREE_BLOCK))) return e;

	write_uint32_t(p0, 3, TTTFS_VOLUME_FREE_BLOCK_COUNT - 1);
	write_uint32_t(p0, 4, read_uint32_t(b, 255));
	write_block(id, p0, id.pos_partition[par]);
    myfree(b);
	write_block(id, b, id.pos_partition[par] + TTTFS_VOLUME_FIRST_FREE_BLOCK);
	return 0;
}

error create_file(disk_id id, uint32_t par, uint32_t type, uint32_t subtype)
{
    if (par >= read_uint32_t(id.b0, 1))
		return PARTITION_NOT_FOUND;
    block p0 = malloc (sizeof (struct block));
	error e;
	if ((e = read_block(id, p0, id.pos_partition[par]))) return e;
    
	uint32_t TTTFS_VOLUME_FIRST_FREE_BLOCK = read_uint32_t(p0, 4);
	uint32_t TTTFS_VOLUME_FREE_FILE_COUNT = read_uint32_t(p0, 6);
	uint32_t TTTFS_VOLUME_FIRST_FREE_FILE = read_uint32_t(p0, 7);
    if (TTTFS_VOLUME_FREE_FILE_COUNT == 0)
        return FULL_FILE_TABLE;
    
	block file_table = malloc (sizeof (struct block));
    if ((e = read_block(id, file_table, id.pos_partition[par] + TTTFS_VOLUME_FIRST_FREE_FILE / 16 + 1))) return e;
    uint32_t size = 0;
    if (type == 1) size = 64;
    uint32_t next_free = read_uint32_t(file_table, (TTTFS_VOLUME_FIRST_FREE_FILE%16) * 16 + 15);
    write_uint32_t(file_table, (TTTFS_VOLUME_FIRST_FREE_FILE%16) * 16, size);
    write_uint32_t(file_table, (TTTFS_VOLUME_FIRST_FREE_FILE%16) * 16 + 1, type);
    write_uint32_t(file_table, (TTTFS_VOLUME_FIRST_FREE_FILE%16) * 16 + 2, subtype);
    write_uint32_t(file_table, (TTTFS_VOLUME_FIRST_FREE_FILE%16) * 16 + 3, TTTFS_VOLUME_FIRST_FREE_BLOCK);
    alocate_block(id, par);
    
    if (type == 1)
    {
        block root = malloc (sizeof (struct block));
	    write_uint32_t(root, 0, 0);
	    root->data[4] = '.';
	    root->data[5] = '\0';
	    write_uint32_t(root, 8, 0);
	    root->data[36] = '.';
	    root->data[37] = '.';
	    root->data[38] = '\0';
	    write_block(id, root, id.pos_partition[par] + TTTFS_VOLUME_FIRST_FREE_BLOCK);
    }

    write_uint32_t(p0, 7, next_free);
    write_block(id, p0, id.pos_partition[par]);
    write_block(id, file_table, id.pos_partition[par] + TTTFS_VOLUME_FIRST_FREE_FILE / 16 + 1);
    return 0;
}

error remove_file(disk_id id, uint32_t par, uint32_t pos)
{
    if (par >= read_uint32_t(id.b0, 1))
		return PARTITION_NOT_FOUND;
    block p0 = malloc (sizeof (struct block));
	error e;
	if ((e = read_block(id, p0, id.pos_partition[par]))) return e;
    
	uint32_t TTTFS_VOLUME_FIRST_FREE_BLOCK = read_uint32_t(p0, 4);
	uint32_t TTTFS_VOLUME_MAX_FILE_COUNT = read_uint32_t(p0, 5);
	uint32_t TTTFS_VOLUME_FREE_FILE_COUNT = read_uint32_t(p0, 6);
	uint32_t TTTFS_VOLUME_FIRST_FREE_FILE = read_uint32_t(p0, 7);
    if (TTTFS_VOLUME_MAX_FILE_COUNT < pos)
        return FILE_OUT_OF_BOUNDS;
    
    #TODO
	block file_table = malloc (sizeof (struct block));
    if ((e = read_block(id, file_table, id.pos_partition[par] + TTTFS_VOLUME_FIRST_FREE_FILE / 16 + 1))) return e;
    uint32_t size = 0;
    if (type == 1) size = 64;
    uint32_t next_free = read_uint32_t(file_table, (TTTFS_VOLUME_FIRST_FREE_FILE%16) * 16 + 15);
    write_uint32_t(file_table, (TTTFS_VOLUME_FIRST_FREE_FILE%16) * 16, size);
    write_uint32_t(file_table, (TTTFS_VOLUME_FIRST_FREE_FILE%16) * 16 + 1, type);
    write_uint32_t(file_table, (TTTFS_VOLUME_FIRST_FREE_FILE%16) * 16 + 2, subtype);
    write_uint32_t(file_table, (TTTFS_VOLUME_FIRST_FREE_FILE%16) * 16 + 3, TTTFS_VOLUME_FIRST_FREE_BLOCK);
    alocate_block(id, par);
    
    if (type == 1)
    {
        block root = malloc (sizeof (struct block));
	    write_uint32_t(root, 0, 0);
	    root->data[4] = '.';
	    root->data[5] = '\0';
	    write_uint32_t(root, 8, 0);
	    root->data[36] = '.';
	    root->data[37] = '.';
	    root->data[38] = '\0';
	    write_block(id, root, id.pos_partition[par] + TTTFS_VOLUME_FIRST_FREE_BLOCK);
    }

    write_uint32_t(p0, 7, next_free);
    write_block(id, p0, id.pos_partition[par]);
    write_block(id, file_table, id.pos_partition[par] + TTTFS_VOLUME_FIRST_FREE_FILE / 16 + 1);
    return 0;
}








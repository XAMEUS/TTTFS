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

int tfs_rmdir(disk_id id, uint32_t par, char *path, char* name)
{
	if (par >= read_uint32_t(id.b0, 1))
		return -PARTITION_NOT_FOUND;
	int pos_dir_parent = id_from_path(id, par, path);
	if (pos_dir_parent < 0) return pos_dir_parent;
	
	block file_table = malloc (sizeof (struct block));
	read_block(id, file_table, id.pos_partition[par] + 1 + pos_dir_parent / 16);
	uint32_t size = read_uint32_t(file_table, (pos_dir_parent%16) * 16);
	
	int pos_dir_son = 0;
	int i;
	block dblock = malloc (sizeof (struct block));
	block dblock2 = malloc (sizeof (struct block));
	block indirect1 = malloc (sizeof (struct block));
	block indirect2 = malloc (sizeof (struct block));
	for (i = 0; i <= size / 1024 && i < 10; i++)
    { //    	int e = search_file(id, par, read_uint32_t(file_table, (file%16) * 16 + 3 + i), size - i * 1024, path);
		uint32_t pos_dir_dblock = read_uint32_t(file_table, (pos_dir_parent%16) * 16 + 3 + i);
		read_block(id, dblock, id.pos_partition[par] + pos_dir_dblock);
		int k;
		for (k = 0; k < 32 && k < (size - i * 1024) / 32; k++)
		{			
			if (pos_dir_son)
			{
				memcpy(dblock->data + (k-1) * 32, dblock->data + k*32, 32);
			}
			if (pos_dir_son == 0 && strcmp((char*) (dblock->data + k * 32 + 4), name) == 0)
				pos_dir_son = read_uint32_t(dblock, k * 8);
		}
		if (pos_dir_son) {
			if (size > 1024 * (i + 1) && i < 9)
			{
				uint32_t pos_dir_dblock2 = read_uint32_t(file_table, (pos_dir_parent%16) * 16 + 3 + i + 1);
				read_block(id, dblock2, id.pos_partition[par] + pos_dir_dblock2);
				memcpy(dblock->data + 1024 - 32, dblock2->data, 32);
			}
			else if (size > 1024 * 10 && i == 9)
			{
				uint32_t pos_indirect1 = read_uint32_t(file_table, (pos_dir_parent%16) * 16 + 13);
				read_block(id, indirect1, id.pos_partition[par] + pos_indirect1);
				uint32_t pos_dir_dblock2 = read_uint32_t(indirect1, 0);
				read_block(id, dblock2, id.pos_partition[par] + pos_dir_dblock2);
				memcpy(dblock->data + 1024 - 32, dblock2->data, 32);
			}
			write_block(id, dblock, id.pos_partition[par] + pos_dir_dblock);
		}
    }
	if (size / 1024 > 10)
	{
		uint32_t pos_indirect1 = read_uint32_t(file_table, (pos_dir_parent%16) * 16 + 13);
	    if (read_block(id, indirect1, id.pos_partition[par] + pos_indirect1)) return -1;
		for (i = 0; i <= size / 1024 - 10 && i < 256; i++)
		{
			uint32_t pos_dir_dblock = read_uint32_t(indirect1, i);
			read_block(id, dblock, id.pos_partition[par] + pos_dir_dblock);
			int k;
			for (k = 0; k < 32 && k < (size - (i + 10) * 1024) / 32; k++)
			{			
				if (pos_dir_son)
				{
					memcpy(dblock->data + (k-1) * 32, dblock->data + k*32, 32);
				}
				if (pos_dir_son == 0 && strcmp((char*) (dblock->data + k * 32 + 4), name) == 0)
					pos_dir_son = read_uint32_t(dblock, k * 8);
			}
			if (pos_dir_son) {
				if (size > 1024 * (i + 1 + 10) && i < 255)
				{
					uint32_t pos_dir_dblock2 = read_uint32_t(indirect1, i + 1);
					read_block(id, dblock2, id.pos_partition[par] + pos_dir_dblock2);
					memcpy(dblock->data + 1024 - 32, dblock2->data, 32);
				}
				else if (size > 1024 * 265 && i == 255)
				{
					uint32_t pos_indirect2 = read_uint32_t(file_table, (pos_dir_parent%16) * 16 + 14);
					read_block(id, indirect2, id.pos_partition[par] + pos_indirect2);
					read_block(id, indirect1, id.pos_partition[par] + read_uint32_t(indirect2, 0));
					uint32_t pos_dir_dblock2 = read_uint32_t(indirect1, 0);
					read_block(id, dblock2, id.pos_partition[par] + pos_dir_dblock2);
					memcpy(dblock->data + 1024 - 32, dblock2->data, 32);
				}
				write_block(id, dblock, id.pos_partition[par] + pos_dir_dblock);
			}
		}
	}
	if (size / 1024 > 266)
	{
		uint32_t pos_indirect2 = read_uint32_t(file_table, (pos_dir_parent%16) * 16 + 14);
	    if (read_block(id, indirect2, id.pos_partition[par] + pos_indirect2)) return -1;
		for (i = 0; i <= (size / 1024 - 266) / 256 && i < 256; i++)
		{
		    block indirect1 = malloc (sizeof (struct block));
			uint32_t pos_indirect1 = read_uint32_t(indirect2, i);
			if (read_block(id, indirect1, pos_indirect1)) return -1;
			int j;
			for (j = 0; j <= (size / 1024 - 266 - 256 * i) && j < 256; j++)
			{
				uint32_t pos_dir_dblock = read_uint32_t(indirect1, j);
				read_block(id, dblock, id.pos_partition[par] + pos_dir_dblock);
				int k;
				for (k = 0; k < 32 && k < (size - (j + 266 + i * 256) * 1024) / 32; k++)
				{			
					if (pos_dir_son)
					{
						memcpy(dblock->data + (k-1) * 32, dblock->data + k*32, 32);
					}
					if (pos_dir_son == 0 && strcmp((char*) (dblock->data + k * 32 + 4), name) == 0)
						pos_dir_son = read_uint32_t(dblock, k * 8);
				}
				if (pos_dir_son) {
					if (size > 1024 * (j + 1 + 266 + i * 256) && j < 255)
					{
						uint32_t pos_dir_dblock2 = read_uint32_t(indirect1, i + 1);
						read_block(id, dblock2, id.pos_partition[par] + pos_dir_dblock2);
						memcpy(dblock->data + 1024 - 32, dblock2->data, 32);
					}
					else if (size > 1024 * (j + 1 + 266 + i * 256) && j == 255 && i < 255)
					{
						read_block(id, indirect1, id.pos_partition[par] + read_uint32_t(indirect2, i+1));
						uint32_t pos_dir_dblock2 = read_uint32_t(indirect1, 0);
						read_block(id, dblock2, id.pos_partition[par] + pos_dir_dblock2);
						memcpy(dblock->data + 1024 - 32, dblock2->data, 32);
					}
					write_block(id, dblock, id.pos_partition[par] + pos_dir_dblock);
				}
			}
		}
	}

	read_block(id, file_table, id.pos_partition[par] + 1 + pos_dir_son / 16);
	uint32_t son_type = read_uint32_t(file_table, (pos_dir_son%16) * 16 + 1);
	uint32_t son_size = read_uint32_t(file_table, (pos_dir_son%16) * 16);

	if(son_type == 1 && son_size > 64){
		return -DIRECTORY_NOT_EMPTY; // recursion ?
	}
	error er;
	if((er = remove_file(id, par, pos_dir_son) != 0)) return er;	
	update_blocks_file(id, par, pos_dir_parent, size - 32);
	return pos_dir_son;
}
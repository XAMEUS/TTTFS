#include "stdltfs.h"

int is_free(disk_id id, uint32_t par, uint32_t free_block, uint32_t pos)
{
	uint32_t pos_par = id.pos_partition[par];
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
	if (is_free(id, par, TTTFS_VOLUME_FIRST_FREE_BLOCK, pos))
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

error create_file(disk_id id, uint32_t par, uint32_t dir, uint32_t type, uint32_t subtype)
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
    write_uint32_t(file_table, (TTTFS_VOLUME_FIRST_FREE_FILE%16) * 16 + 15, 0);
	
	alocate_block(id, par);
	if ((e = read_block(id, p0, id.pos_partition[par]))) return e;
	
    
    if (type == 1)
    {
        block root = malloc (sizeof (struct block));
	    write_uint32_t(root, 0, TTTFS_VOLUME_FIRST_FREE_FILE);
	    root->data[4] = '.';
	    root->data[5] = '\0';
	    write_uint32_t(root, 8, dir);
	    root->data[36] = '.';
	    root->data[37] = '.';
	    root->data[38] = '\0';
	    write_block(id, root, id.pos_partition[par] + TTTFS_VOLUME_FIRST_FREE_BLOCK);
    }

	write_uint32_t(p0, 6, TTTFS_VOLUME_FREE_FILE_COUNT - 1);
    write_uint32_t(p0, 7, next_free);
    write_block(id, p0, id.pos_partition[par]);
    write_block(id, file_table, id.pos_partition[par] + TTTFS_VOLUME_FIRST_FREE_FILE / 16 + 1);
    return 0;
}

int is_free_file(disk_id id, uint32_t par, uint32_t free_file, uint32_t pos)
{
	uint32_t pos_par = id.pos_partition[par];
    uint32_t next_free_file;
	block file_table = malloc (sizeof (struct block));
	read_block(id, file_table, pos_par + free_file / 16 + 1);
	next_free_file = read_uint32_t(file_table, free_file % 16 * 16 + 15);
	while (free_file != next_free_file)
	{
		if (free_file == pos)
			return 1;
		free_file = next_free_file;
		read_block(id, file_table, pos_par + free_file / 16 + 1);
		next_free_file = read_uint32_t(file_table, free_file % 16 * 16 + 15);
	}
	if (free_file == pos) return 1;
	return 0;
}

error remove_file(disk_id id, uint32_t par, uint32_t pos)
{
    if (par >= read_uint32_t(id.b0, 1))
		return PARTITION_NOT_FOUND;
    block p0 = malloc (sizeof (struct block));
	error e;
	if ((e = read_block(id, p0, id.pos_partition[par]))) return e;
    
	uint32_t TTTFS_VOLUME_MAX_FILE_COUNT = read_uint32_t(p0, 5);
	uint32_t TTTFS_VOLUME_FREE_FILE_COUNT = read_uint32_t(p0, 6);
	uint32_t TTTFS_VOLUME_FIRST_FREE_FILE = read_uint32_t(p0, 7);
    if (TTTFS_VOLUME_MAX_FILE_COUNT < pos)
        return FILE_OUT_OF_BOUNDS;
    if (is_free_file(id, id.pos_partition[par], TTTFS_VOLUME_FIRST_FREE_FILE, pos))
        return FILE_ALREADY_FREE;
    
    block file_table = malloc (sizeof (struct block));
    if ((e = read_block(id, file_table, id.pos_partition[par] + pos / 16 + 1))) return e;
    uint32_t size = read_uint32_t(file_table, (pos%16) * 16);
    int i;
    for (i = 0; i <= size / 1024 && i < 10; i++)
    {
        free_block(id, par, (pos%16) * 16 + 3 + i);
    }
	if (size / 1024 > 10)
	{
		block indirect1 = malloc (sizeof (struct block));
		uint32_t pos_indirect1 = read_uint32_t(file_table, (pos%16) * 16 + 13);
	    if ((e = read_block(id, indirect1, id.pos_partition[par] + pos_indirect1))) return e;
		for (i = 0; i <= (size - 10) / 1024 && i < 256; i++)
		{
		    free_block(id, par, read_uint32_t(indirect1, i));
		}
	    free_block(id, par, id.pos_partition[par] + pos_indirect1);
	}
	if (size / 1024 > 266)
	{
		block indirect2 = malloc (sizeof (struct block));
		uint32_t pos_indirect2 = read_uint32_t(file_table, (pos%16) * 16 + 14);
	    if ((e = read_block(id, indirect2, id.pos_partition[par] + pos_indirect2))) return e;
		for (i = 0; i <= (size - 266) / 1024 / 1024 && i < 256; i++)
		{
		    block indirect1 = malloc (sizeof (struct block));
			uint32_t pos_indirect1 = read_uint32_t(indirect2, i);
			if ((e = read_block(id, indirect1, pos_indirect1))) return e;
			int j;
			for (j = 0; j <= (size - 266 - 256 * i) / 1024 && j < 256; j++)
			{
				free_block(id, par, read_uint32_t(indirect1, j));
			}
			free_block(id, par, pos_indirect1);
		}
	    free_block(id, par, id.pos_partition[par] + pos_indirect2);
	}

	if (TTTFS_VOLUME_FREE_FILE_COUNT == 0)
		write_uint32_t(file_table, (pos%16) * 16 + 15, pos);
	else
		write_uint32_t(file_table, (pos%16) * 16 + 15, TTTFS_VOLUME_FIRST_FREE_FILE);
	
	write_uint32_t(p0, 6, TTTFS_VOLUME_FREE_FILE_COUNT + 1);
	write_uint32_t(p0, 7, pos);
	write_block(id, p0, id.pos_partition[par]);
	write_block(id, file_table, id.pos_partition[par] + pos / 16 + 1);
	
    return 0;
}

error add_blocks_file(disk_id id, uint32_t par, uint32_t file, uint32_t nblocks)
{
	if (par >= read_uint32_t(id.b0, 1))
		return PARTITION_NOT_FOUND;
    block p0 = malloc (sizeof (struct block));
	error e;
	if ((e = read_block(id, p0, id.pos_partition[par]))) return e;
    
	uint32_t TTTFS_VOLUME_FREE_BLOCK_COUNT = read_uint32_t(p0, 3);
	uint32_t TTTFS_VOLUME_FIRST_FREE_BLOCK = read_uint32_t(p0, 4);
	
	if (TTTFS_VOLUME_FREE_BLOCK_COUNT < nblocks)
		return NOT_ENOUGH_FREE_BLOCKS;
	
    block file_table = malloc (sizeof (struct block));
	if ((e = read_block(id, file_table, id.pos_partition[par] + file / 16 + 1))) return e;
	uint32_t tfs_size = read_uint32_t(file_table, file % 16 * 16);
	
	int i;
    for (i = tfs_size / 1024 + 1; i <= tfs_size / 1024 + nblocks && i < 10; i++)
    {
		write_uint32_t(file_table, (file % 16) * 16 + 3 + i, TTTFS_VOLUME_FIRST_FREE_BLOCK);
        alocate_block(id, par);
		if ((e = read_block(id, p0, id.pos_partition[par]))) return e;
		TTTFS_VOLUME_FIRST_FREE_BLOCK = read_uint32_t(p0, 4);
    }
	if (tfs_size / 1024 + nblocks > 10)
	{
		block indirect1 = malloc (sizeof (struct block));
		if (tfs_size / 1024 <= 10)
		{
			write_uint32_t(file_table, (file % 16) * 16 + 13, TTTFS_VOLUME_FIRST_FREE_BLOCK);
		    alocate_block(id, par);
			if ((e = read_block(id, p0, id.pos_partition[par]))) return e;
			TTTFS_VOLUME_FIRST_FREE_BLOCK = read_uint32_t(p0, 4);
		}
	    if ((e = read_block(id, indirect1, id.pos_partition[par] + read_uint32_t(file_table, (file % 16) * 16 + 13)))) return e;

		for (i = tfs_size / 1024 + 1; i <= (tfs_size - 10) / 1024 + nblocks && i < 256; i++)
		{
		    write_uint32_t(indirect1, i, TTTFS_VOLUME_FIRST_FREE_BLOCK);
		    alocate_block(id, par);
			if ((e = read_block(id, p0, id.pos_partition[par]))) return e;
			TTTFS_VOLUME_FIRST_FREE_BLOCK = read_uint32_t(p0, 4);
		}
	}
	if (tfs_size / 1024 + nblocks > 266)
	{
		block indirect2 = malloc (sizeof (struct block));
		uint32_t pos_indirect2 = read_uint32_t(file_table, (file%16) * 16 + 14);
		if (tfs_size / 1024 <= 266)
		{
			write_uint32_t(file_table, pos_indirect2, TTTFS_VOLUME_FIRST_FREE_BLOCK);
		    alocate_block(id, par);
			if ((e = read_block(id, p0, id.pos_partition[par]))) return e;
			TTTFS_VOLUME_FIRST_FREE_BLOCK = read_uint32_t(p0, 4);
		}
	    if ((e = read_block(id, indirect2, id.pos_partition[par] + pos_indirect2))) return e;
		for (i = tfs_size / 1024 / 1024; i <= (tfs_size - 266) / 1024 / 1024 + nblocks / 1024 && i < 256; i++)
		{
		    block indirect1 = malloc (sizeof (struct block));
			if (tfs_size / 1024 <= 266 + 256 * i)
			{
				write_uint32_t(indirect2, i, TTTFS_VOLUME_FIRST_FREE_BLOCK);
				alocate_block(id, par);
				if ((e = read_block(id, p0, id.pos_partition[par]))) return e;
				TTTFS_VOLUME_FIRST_FREE_BLOCK = read_uint32_t(p0, 4);
			}
			if ((e = read_block(id, indirect1, id.pos_partition[par] + read_uint32_t(indirect2, i)))) return e;

			int j;
			for (j = tfs_size / 1024 + 1; j <= (tfs_size - 266 - 256 * i) / 1024 + nblocks && j < 256; j++)
			{
				write_uint32_t(indirect1, j, TTTFS_VOLUME_FIRST_FREE_BLOCK);
				alocate_block(id, par);
				if ((e = read_block(id, p0, id.pos_partition[par]))) return e;
				TTTFS_VOLUME_FIRST_FREE_BLOCK = read_uint32_t(p0, 4);
			}
		}
	}
	return 0;
}

error remove_blocks_file(disk_id id, uint32_t par, uint32_t file, uint32_t nblocks)
{
    if (par >= read_uint32_t(id.b0, 1))
		return PARTITION_NOT_FOUND;
	error e;
    
    block file_table = malloc (sizeof (struct block));
    if ((e = read_block(id, file_table, id.pos_partition[par] + file / 16 + 1))) return e;
    uint32_t size = read_uint32_t(file_table, (file%16) * 16);
    int i;
    for (i = size / 1024 - nblocks + 1; i <= size / 1024 && i < 10; i++)
    {
        free_block(id, par, (file%16) * 16 + 3 + i);
    }
	if (size / 1024 > 10)
	{
		block indirect1 = malloc (sizeof (struct block));
		uint32_t pos_indirect1 = read_uint32_t(file_table, (file%16) * 16 + 13);
	    if ((e = read_block(id, indirect1, id.pos_partition[par] + pos_indirect1))) return e;
		for (i = size / 1024 + 1 - nblocks; i <= (size - 10) / 1024 && i < 256; i++)
		{
		    free_block(id, par, read_uint32_t(indirect1, i));
		}
		if (size / 1024 + 1 - nblocks <= 10)
		    free_block(id, par, id.pos_partition[par] + pos_indirect1);
	}
	if (size / 1024 > 266)
	{
		block indirect2 = malloc (sizeof (struct block));
		uint32_t pos_indirect2 = read_uint32_t(file_table, (file%16) * 16 + 14);
	    if ((e = read_block(id, indirect2, id.pos_partition[par] + pos_indirect2))) return e;
		for (i = size / 1024 / 1024 - nblocks / 1024; i <= (size - 266) / 1024 / 1024 && i < 256; i++)
		{
		    block indirect1 = malloc (sizeof (struct block));
			uint32_t pos_indirect1 = read_uint32_t(indirect2, i);
			if ((e = read_block(id, indirect1, pos_indirect1))) return e;
			int j;
			for (j = size / 1024 + 1 - nblocks; j <= (size - 266 - 256 * i) / 1024 && j < 256; j++)
			{
				free_block(id, par, read_uint32_t(indirect1, j));
			}
			if (size / 1024 + 1 - nblocks <= (266 + 256 * i))
				free_block(id, par, pos_indirect1);
		}
		if (size / 1024 <= 266)
		    free_block(id, par, id.pos_partition[par] + pos_indirect2);
	}

	write_block(id, file_table, id.pos_partition[par] + file / 16 + 1);
	
    return 0;
}


error update_blocks_file(disk_id id, uint32_t par, uint32_t file, uint32_t next_size)
{
	if (par >= read_uint32_t(id.b0, 1))
		return PARTITION_NOT_FOUND;
	error e;

	block file_table = malloc (sizeof (struct block));
	if ((e = read_block(id, file_table, id.pos_partition[par] + file / 16 + 1))) return e;
	uint32_t tfs_size = read_uint32_t(file_table, file % 16 * 16);

	if (next_size < tfs_size)
	{
		uint32_t nblocks = tfs_size / 1024 - next_size / 1024;
		e = remove_blocks_file(id, par, file, nblocks);
	}
	else
	{
		uint32_t nblocks = next_size / 1024 - tfs_size / 1024;
		e = add_blocks_file(id, par, file, nblocks);
	}
	if (e) return e;
	if ((e = read_block(id, file_table, id.pos_partition[par] + file / 16 + 1))) return e;
	write_uint32_t(file_table, (file % 16) * 16, next_size);
	write_block(id, file_table, id.pos_partition[par] + file / 16 + 1);
	return 0;
}

error search_dir(disk_id id, uint32_t par, uint32_t file, char* path){ //cherche dans le repertoire si un lien de nom path existe
	f (par >= read_uint32_t(id.b0, 1))
		return PARTITION_NOT_FOUND;
    block p0 = malloc (sizeof (struct block));
	error e;
	if ((e = read_block(id, p0, id.pos_partition[par]))) return e;
    
	uint32_t TTTFS_VOLUME_MAX_FILE_COUNT = read_uint32_t(p0, 5);
	uint32_t TTTFS_VOLUME_FREE_FILE_COUNT = read_uint32_t(p0, 6);
	uint32_t TTTFS_VOLUME_FIRST_FREE_FILE = read_uint32_t(p0, 7);
    if (TTTFS_VOLUME_MAX_FILE_COUNT < file)
        return FILE_OUT_OF_BOUNDS;

    block file_table = malloc (sizeof (struct block));
    if ((e = read_block(id, file_table, id.pos_partition[par] + file / 16 + 1))) return e;
    //TODO if file != repertoire : erreur
    uint32_t size = read_uint32_t(file_table, (file%16) * 16);
    int i;
    //TODO (return -1 if true ?)
    for (i = 0; i <= size / 1024 && i < 10; i++)
    {
    	//HERE
    }
	if (size / 1024 > 10)
	{
		block indirect1 = malloc (sizeof (struct block));
		uint32_t pos_indirect1 = read_uint32_t(file_table, (file%16) * 16 + 13);
	    if ((e = read_block(id, indirect1, id.pos_partition[par] + pos_indirect1))) return e;
		for (i = 0; i <= (size - 10) / 1024 && i < 256; i++)
		{
		    //HERE
		}
	}
	if (size / 1024 > 266)
	{
		block indirect2 = malloc (sizeof (struct block));
		uint32_t pos_indirect2 = read_uint32_t(file_table, (file%16) * 16 + 14);
	    if ((e = read_block(id, indirect2, id.pos_partition[par] + pos_indirect2))) return e;
		for (i = 0; i <= (size - 266) / 1024 / 1024 && i < 256; i++)
		{
		    block indirect1 = malloc (sizeof (struct block));
			uint32_t pos_indirect1 = read_uint32_t(indirect2, i);
			if ((e = read_block(id, indirect1, pos_indirect1))) return e;
			int j;
			for (j = 0; j <= (size - 266 - 256 * i) / 1024 && j < 256; j++)
			{
				//HERE
			}
		}
	}
    return 0;
}

uint32_t path_to_id(char* path){
	char* token = strtok(path,"//");
	if(strcmp(token, "FILE:")!=0){
		return BAD_PATHNAME;
	}

	token = strtok(path,"/");
	if(strcmp(token,"HOST")==0){
		return PATH_TO_HOST;
	}

	disk_id id;
	start_disk(token,&id);
	token = strtok(path,"/");
	uint32_t par = atoi(token);
	if(par + 1 > read_uint32_t(id.b0,1)){
		return PARTITION_NOT_FOUND;
	}

	uint32_t curdir = 0;
	block file_table = malloc (sizeof (struct block));
	read_block(id,file_table,id.pos_partition[par] + curdir / 16 + 1); //racine
	token = strtok(path,"/");
	while(token!=NULL){

	}


}
#include "ll.h"

int tfs_format(uint32_t partition, uint32_t max_file, char* name);

int tfs_format(uint32_t partition, uint32_t TTTFS_VOLUME_MAX_FILE_COUNT, char* name)
{
	int i;
	disk_id dk;
	start_disk(name, &dk);
	uint32_t partitions = read_uint32_t(dk.b0, 1);
	if (partitions < partition + 1)
		return 138;
	int pos = 1;
	for(i = 2; i < 2 + partition; i++)
	{
		pos += read_uint32_t(dk.b0, i);
	}
	uint32_t TTTFS_VOLUME_BLOCK_COUNT = read_uint32_t(dk.b0, i);
	if (TTTFS_VOLUME_BLOCK_COUNT <= TTTFS_VOLUME_MAX_FILE_COUNT/16 + TTTFS_VOLUME_MAX_FILE_COUNT + 2)
		return 139;
	uint32_t TTTFS_VOLUME_FILE_TABLE_COUNT = (TTTFS_VOLUME_MAX_FILE_COUNT + 15) / 16;
	uint32_t TTTFS_VOLUME_FREE_BLOCK_COUNT = TTTFS_VOLUME_BLOCK_COUNT - 2 - TTTFS_VOLUME_FILE_TABLE_COUNT;
	uint32_t TTTFS_VOLUME_FIRST_FREE_BLOCK = 2 + TTTFS_VOLUME_FILE_TABLE_COUNT;
	uint32_t TTTFS_VOLUME_FREE_FILE_COUNT = TTTFS_VOLUME_MAX_FILE_COUNT - 1;
	uint32_t TTTFS_VOLUME_FIRST_FREE_FILE = 1;
	block p0 = malloc (sizeof (struct block));
	if (p0 == NULL) return 134;
	error e;
	e = write_uint32_t(p0, 0, TTTFS_MAGIC_NUMBER);
	if (e) return e;
	e = write_uint32_t(p0, 1, TTTFS_VOLUME_BLOCK_SIZE);
	if (e) return e;
	e = write_uint32_t(p0, 2, TTTFS_VOLUME_BLOCK_COUNT);
	if (e) return e;
	e = write_uint32_t(p0, 3, TTTFS_VOLUME_FREE_BLOCK_COUNT);
	if (e) return e;
	e = write_uint32_t(p0, 4, TTTFS_VOLUME_FIRST_FREE_BLOCK);
	if (e) return e;
	e = write_uint32_t(p0, 5, TTTFS_VOLUME_MAX_FILE_COUNT);
	if (e) return e;
	e = write_uint32_t(p0, 6, TTTFS_VOLUME_FREE_FILE_COUNT);
	if (e) return e;
	e = write_uint32_t(p0, 7, TTTFS_VOLUME_FIRST_FREE_FILE);
	if (e) return e;
	
	write_block(dk, p0, pos);
	myfree(p0);
	
	block file_table = malloc (sizeof (struct block));
	// tfs_size, taille des fichiers en octet
	if ((e = write_uint32_t(file_table, 0, 64))) return e;
	// tfs_type, type du fichier
	if ((e = write_uint32_t(file_table, 1, TFS_DIRECTORY))) return e;
	// tfs_subtype
	if ((e = write_uint32_t(file_table, 2, 0))) return e;
	// numero de block
	if ((e = write_uint32_t(file_table, 3, TTTFS_VOLUME_FIRST_FREE_BLOCK - 1))) return e;
	
	int j;
	for (i = 0; i < TTTFS_VOLUME_FILE_TABLE_COUNT; i++)
	{
		if (i != 0)
		{
			myfree(file_table);
		}
		int c = 15; // nombre de fichier dans chaque file table
		if (i == TTTFS_VOLUME_FILE_TABLE_COUNT - 1) c = (TTTFS_VOLUME_MAX_FILE_COUNT - 1) % 16;
		for (j = 0; j <= c; j++)
		{
			write_uint32_t(file_table, j * 16 + 15, i * 16 + j + 1);
		}
		if (i == TTTFS_VOLUME_FILE_TABLE_COUNT - 1)
			write_uint32_t(file_table, (j-1) * 16 + 15, i * 16 + j - 1);
		write_block(dk, file_table, pos + i + 1);
	}
	myfree(file_table);
	
	block root = malloc (sizeof (struct block));
	write_uint32_t(root, 0, 0);
	root->data[4] = '.';
	root->data[5] = '\0';
	write_uint32_t(root, 8, 0);
	root->data[36] = '.';
	root->data[37] = '.';
	root->data[38] = '\0';
	write_block(dk, root, pos + TTTFS_VOLUME_FIRST_FREE_BLOCK - 1);
	myfree(root);
	
	for (i = TTTFS_VOLUME_FIRST_FREE_BLOCK; i < TTTFS_VOLUME_BLOCK_COUNT - 1; i++)
	{
		block free_block = malloc (sizeof (struct block));
		write_uint32_t(free_block, 255, i + 1);
		write_block(dk, free_block, pos + i);
		myfree(free_block);
	}
	block free_block = malloc (sizeof (struct block));
	write_uint32_t(free_block, 255, i);
	write_block(dk, free_block, pos + i);
	myfree(free_block);
	
	return 0;
}

int main(int argc, char* argv[])
{
	int xarg = 1;
	uint32_t partition = 0;
	uint32_t max_file = 16;
	char* filename = "disk.tfs";
	if (argc >= 2 && strcmp(argv[xarg], "-p") == 0)
	{
		partition = atoi(argv[xarg+1]);
		xarg += 2;
	}
	if (argc - xarg >= 2 && strcmp(argv[xarg], "-mf") == 0)
	{
		partition = atoi(argv[xarg+1]);
		xarg += 2;
	}
	if (argc - xarg > 0 && xarg < argc)
	{
		filename = argv[argc - 1];
	}
	return tfs_format(partition, max_file, filename);
}
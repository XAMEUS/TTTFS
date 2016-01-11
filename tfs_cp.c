#include "stdiotfs.h"

int tfs_cp(uint32_t size, char* name);

int tfs_cp(uint32_t size, char* name)
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
	char path1[strlen(argv[1]];
	strcpy(path1, argv[1]);
	char path2[strlen(argv[2]];
	strcpy(path2, argv[2]);
	
	int flag1 = 0;
	int flag2 = 0;
	int fd1;
	int fd2;

	char* token = strtok(path1, "//");

	if(strcmp(token, "FILE:")!=0)
	{
		fprintf(stderr, "bad argument: %s", token);
		return 1;
	}

	token = strtok(NULL,"/");
	if(strcmp(token,"HOST")==0)
	{
		fd1 = open(argv[1] + 11, O_RDONLY);
		flag1 = 1;
	}
	else
	{
		printf("%s\n", token);
		disk_id id;
		start_disk(token, &id);
		token = strtok(NULL, "/");
		uint32_t par = atoi(token);
		if(par + 1 > read_uint32_t(id.b0,1))
		{
			printf("error");
			return PARTITION_NOT_FOUND;
		}
		
		uint32_t curdir = 0;
		block file_table = malloc (sizeof (struct block));
		read_block(id,file_table,id.pos_partition[par] + curdir / 16 + 1); //racine
		token = strtok(path,"/");
		while(token!=NULL){

		}
	}
	return 0;
}








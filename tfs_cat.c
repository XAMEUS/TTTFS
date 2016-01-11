#include "stdiotfs.h"

int main(int argc, char* argv[])
{
	char path1[strlen(argv[1])];
	strcpy(path1, argv[1]);
	
	char** path1_splited = str_split(path1, "/");
	
	disk_id dk1;
	char* name;
	
	if (strcmp(path1_splited[0], "FILE:") != 0)
	{
		fprintf(stderr, "error, bad %s", argv[1]);
		return 1; 
	}
	
	int path1_len = strlen(argv[1]);
	char path1_location[path1_len];
	int k;
	for (k = 0; k < path1_len; k++)
		path1_location[k] = 0;
	
	if (strcmp(path1_splited[1], "HOST") == 0)
	{
		fprintf(stderr, "use mkdir");
	}
	else
	{
		start_disk(path1_splited[1], &dk1);
		uint32_t par = atoi(path1_splited[2]);
		int j = 3;
		
		strcat(path1_location, "/");
		while (path1_splited[j + 1] != NULL)
		{
			strcat(path1_location, path1_splited[j]);
			strcat(path1_location, "/");
			j++;
		}
		name = path1_splited[j];
		int fd = tfs_open(dk1, par, path1_location, name, READ);
		tfs_cat(fd);
		tfs_close(fd);
	}
	
	stop_disk(dk1);
	
	return 0;
	
}








#include "stdiotfs.h"

int tfs_cp_disk_disk(int fd1, int fd2);

int tfs_cp_disk_disk(int fd1, int fd2)
{
	char buf[1024];
	int i;
	for (i = 0; i * 1024 < tfs_get_size(fd1); i++)
	{
		int s = tfs_get_size(fd1) - i*1024;
		if (s > 1024) s = 1024;
		tfs_read(fd1, buf, s);
		tfs_write(fd2, buf, s);
	}
	return 0;
}

int tfs_cp_host_disk(int fd1, int fd2)
{
	printf("cp_host_disk\n");
	char buf[1024];
	int i;
	struct stat st;
	fstat(fd1, &st);
	for (i = 0; i * 1024 < st.st_size; i++)
	{
		int s = st.st_size - i*1024;
		if (s > 1024) s = 1024;
		read(fd1, buf, s);
		tfs_write(fd2, buf, s);
	}
	return 0;
}

int tfs_cp_disk_host(int fd1, int fd2)
{
	char buf[1024];
	int i;
	for (i = 0; i * 1024 < tfs_get_size(fd1); i++)
	{
		int s = tfs_get_size(fd1) - i*1024;
		if (s > 1024) s = 1024;
		tfs_read(fd1, buf, s);
		write(fd2, buf, s);
	}
	return 0;
}

char **str_split(char *s, const char *ct)
{
   char **tab = NULL;
 
   if (s && ct)
   {
      int i;
      char *cs = NULL;
      size_t size = 1;
 
/* (1) */
      for (i = 0; (cs = strtok (s, ct)); i++)
      {
         if (size <= i + 1)
         {
            void *tmp = NULL;
 
/* (2) */
            size <<= 1;
            tmp = realloc (tab, sizeof (*tab) * size);
            if (tmp)
            {
               tab = tmp;
            }
            else
            {
               fprintf (stderr, "Memoire insuffisante\n");
               free (tab);
               tab = NULL;
               exit (EXIT_FAILURE);
            }
         }
/* (3) */
         tab[i] = cs;
         s = NULL;
      }
      tab[i] = NULL;
   }
   return tab;
}


int main(int argc, char* argv[])
{
	char path1[strlen(argv[1])];
	strcpy(path1, argv[1]);
	char path2[strlen(argv[2])];
	strcpy(path2, argv[2]);
	
	char** path1_splited = str_split(path1, "/");
	char** path2_splited = str_split(path2, "/");
	
	int flag1 = 0;
	int flag2 = 0;
	disk_id dk1;
	disk_id dk2;
	int fd1;
	int fd2;
	char* name;
	
	if (strcmp(path1_splited[0], "FILE:") != 0)
	{
		fprintf(stderr, "error, bad %s", argv[1]);
		return 1; 
	}
	if (strcmp(path2_splited[0], "FILE:") != 0)
	{
		fprintf(stderr, "error, bad %s", argv[2]);
		return 1;
	}
	
	int path1_len = strlen(argv[1]);
	char path1_location[path1_len];
	int k;
	for (k = 0; k < path1_len; k++)
		path1_location[k] = 0;
	
	if (strcmp(path1_splited[1], "HOST") == 0)
	{
		flag1 = 1;
		int j = 2;
		while (path1_splited[j + 1] != NULL)
		{
			strcat(path1_location, "/");
			strcat(path1_location, path1_splited[j]);
			j++;
		}
		name = path1_splited[j];
		strcat(path1_location, "/");
		strcat(path1_location, name);
		fd1 = open(path1_location, O_RDONLY);
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
		fd1 = tfs_open(dk1, par, path1_location, name, READ);
	}
	
	int path2_len = strlen(argv[2]);
	char path2_location[path2_len];
	for (k = 0; k < path2_len; k++)
		path2_location[k] = 0;
	if (strcmp(path2_splited[1], "HOST") == 0)
	{
		flag2 = 1;
		int j = 2;
		while (path2_splited[j] != NULL)
		{
			strcat(path2_location, "/");
			strcat(path2_location, path2_splited[j]);
			j++;
		}
		strcat(path2_location, "/");
		strcat(path2_location, name);
		fd2 = open(path2_location, O_CREAT|O_TRUNC|O_WRONLY, S_IRWXU);
	}
	else
	{
		start_disk(path2_splited[1], &dk2);
		uint32_t par = atoi(path2_splited[2]);
		int j = 3;
		
		strcat(path2_location, "/");
		while (path2_splited[j] != NULL)
		{
			strcat(path2_location, path2_splited[j]);
			strcat(path2_location, "/");
			j++;
		}
		fd2 = tfs_open(dk2, par, path2_location, name, CREATE|WRITE);
	}
	
	printf("%s\n%s\n", path1_location, path2_location);
	if (flag1 && flag2)
	{
		return 1;
	}
	if (flag1)
	{
		tfs_cp_host_disk(fd1, fd2);
		close(fd1);
		tfs_close(fd2);
		stop_disk(dk2);
	}
	else if (flag2)
	{
		tfs_cp_disk_host(fd1, fd2);
		close(fd2);
		tfs_close(fd1);
		stop_disk(dk1);
	}
	else
	{
		tfs_cp_disk_disk(fd1, fd2);
		tfs_close(fd2);
		tfs_close(fd1);
		stop_disk(dk1);
		stop_disk(dk2);
	}
	
	return 0;
	
}








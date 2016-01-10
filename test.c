#include "stdiotfs.h"

int main(int argc, char* argv[])
{
	disk_id dk; int i;

	if ((start_disk("disk.tfs", &dk)))
	{
		printf("cannot start disk!\n");
		return 1;
	}
    /*printf("is_free(dk, 0, 3, 2) = %d\n", is_free(dk, 0, 3, 2));
    printf("is_free(dk, 0, 3, 3) = %d\n", is_free(dk, 0, 3, 3));
    printf("is_free(dk, 0, 3, 4) = %d\n", is_free(dk, 0, 3, 4));
    printf("is_free(dk, 0, 3, 5) = %d\n", is_free(dk, 0, 3, 5));
	*/
	//error e;
	//e = free_block(dk, 0, 2);	
	//printf("free_block(dk, 0, 2) = %d:%s\n", e, error_description(e));
	
	//e = alocate_block(dk, 0);
	//printf("alocate_block(dk, 0) = %d:%s\n", e, error_description(e));

	/*e = create_file(dk, 0, 0, 1, 0);
	printf("create_file(dk, 0, 0, 1, 0) = %d:%s\n", e, error_description(e));
	
	for (i = 0; i < 4; i++)
		printf("is_free_file(dk, 0, 2, %d) = %d\n", i, is_free_file(dk, 0, 2, i));
	*/
	//printf("search_file : %d\n", search_file(dk, 0, 2, 64, "aze"));	
	//printf("search_dir : %d\n", search_dir(dk, 0, 0, ".."));
	//char text[] = "/";
	//char* token = strtok(text, "/");
	//printf("%s\n", token);
	
	tfs_mkdir(dk, 0, "/", "a");
	char path[] = "/a/";
	tfs_mkdir(dk, 0, path, "b");
	char path2[] = "/a/b";
	tfs_mkdir(dk, 0, path2, "c");
	
	stop_disk(dk);
	return 0;
}
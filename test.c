#include "stdltfs.h"

int main(int argc, char* argv[])
{
	disk_id dk; int i;

	if ((start_disk("disk.tfs", &dk)))
	{
		printf("cannot start disk!\n");
		return 1;
	}
    printf("is_free(dk, 0, 3, 2) = %d\n", is_free(dk, 0, 3, 2));
    printf("is_free(dk, 0, 3, 3) = %d\n", is_free(dk, 0, 3, 3));
    printf("is_free(dk, 0, 3, 4) = %d\n", is_free(dk, 0, 3, 4));
    printf("is_free(dk, 0, 3, 5) = %d\n", is_free(dk, 0, 3, 5));
	
	error e;
	//e = free_block(dk, 0, 2);	
	//printf("free_block(dk, 0, 2) = %d:%s\n", e, error_description(e));
	
	//e = alocate_block(dk, 0);
	//printf("alocate_block(dk, 0) = %d:%s\n", e, error_description(e));

	e = create_file(dk, 0, 0, 1, 0);
	printf("create_file(dk, 0, 0, 1, 0) = %d:%s\n", e, error_description(e));
	
	for (i = 0; i < 4; i++)
		printf("is_free_file(dk, 0, 2, %d) = %d\n", i, is_free_file(dk, 0, 2, i));
	
	stop_disk(dk);
	return 0;
}
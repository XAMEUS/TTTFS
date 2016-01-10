#include "ll.h"

int is_free(disk_id id, uint32_t par, uint32_t free_block, uint32_t pos);

error free_block(disk_id id, uint32_t par, uint32_t pos);

error alocate_block(disk_id id, uint32_t par);

error create_file(disk_id id, uint32_t par, uint32_t dir, uint32_t type, uint32_t subtype);

int is_free_file(disk_id id, uint32_t par, uint32_t free_file, uint32_t pos);

error remove_file(disk_id id, uint32_t par, uint32_t pos);

error add_blocks_file(disk_id id, uint32_t par, uint32_t file, uint32_t nblocks);

error remove_blocks_file(disk_id id, uint32_t par, uint32_t file, uint32_t nblocks);

error update_blocks_file(disk_id id, uint32_t par, uint32_t file, uint32_t next_size);

int search_file(disk_id id, uint32_t par, uint32_t dir, uint32_t size, char* name);

int search_dir(disk_id id, uint32_t par, uint32_t file, char* path);

int id_from_path(disk_id id, uint32_t par, char* path);

uint32_t last_data_block_of_file(disk_id id, uint32_t par, uint32_t file);
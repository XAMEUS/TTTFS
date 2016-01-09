#include "ll.h"

int is_free(disk_id id, uint32_t par, uint32_t free_block, uint32_t pos);

error free_block(disk_id id, uint32_t par, uint32_t pos);

error alocate_block(disk_id id, uint32_t par);

error create_file(disk_id id, uint32_t par, uint32_t dir, uint32_t type, uint32_t subtype);

int is_free_file(disk_id id, uint32_t par, uint32_t free_file, uint32_t pos);

error remove_file(disk_id id, uint32_t par, uint32_t pos);
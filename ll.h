/**
* \file ll.h
* \brief Accès bas niveau.
* \author Elbez Samuel, Gourgoulhon Maxime, Jacquette Pierrick, Perrachon Quentin
* \version 0.1
* \date 11 janvier 2016
*
* Programme simulant la couche d’accès de bas niveau à un disque dur
*
*/
#include "utils.h"

#define TTTFS_MAGIC_NUMBER 0x31534654
#define TTTFS_VOLUME_BLOCK_SIZE 1024

#define TFS_REGULAR 0
#define TFS_DIRECTORY 1 
#define TFS_PSEUDO 2

/* PSEUDO FILE */
#define TFS_DATE 0
#define TFS_DISK 1


/**
* \struct block
* \brief Objet block contenant un tableau de 1024 octets.
*/
struct block {
	unsigned char data[1024];
};
typedef struct block* block;

typedef struct disk_id {
	uint32_t fd;
	block b0;
	uint32_t pos_partition[254];
} disk_id;


//================================================================================
// Basic API
//================================================================================

/**
* \fn error start_disk(char* name,disk_id* id)
* \brief permet de manipuler un disque en lui associant une identité dynamique.
* \param name le nom du fichier .tfs.
* \param id le disk_id dans lequel ouvrir le disque.
* \return error
*/
error start_disk(char* name, disk_id* id);

/**
* \fn error read_block(disk_id id,block b,uint32_t num)
* \brief permet de lire un bloc sur le disque.
* \param id le disque où lire le block.
* \param b le block dans lequel écrire le block lu.
* \param num le numéro du block à lire.
* \return error
*/
error read_block(disk_id id, block b, uint32_t num);

/**
* \fn error write_block(disk_id id,block b,uint32_t num)
* \brief permet d’écrire un bloc sur le disque
* \param id le disque où écrire le block.
* \param b le block à écrire.
* \param num le numéro du block à écrire.
* \return error
*/
error write_block(disk_id id, block b, uint32_t num);

/**
* \fn error sync_disk(disk_id id)
* \brief cette fonction peut ne rien faire du tout...
* \param id le disque à sync...
* \return error
*/
error sync_disk(disk_id id);

/**
* \fn error stop_disk(disk_id id)
* \brief permet de terminer une session de travail sur un disque.
* \param id le disque à démonter.
* \return error
*/
error stop_disk(disk_id id); 


//================================================================================
// Basic API
//================================================================================

/**
* \fn unsigned char* to_little_endian(uint32_t num)
* \brief converti un int (4 octets) en little endian sur un unsigned char[4].
* \param num l'entier à convertir.
* \return unsigned char* un tableau de 4 char.
*/
unsigned char* to_little_endian(uint32_t num);

/**
* \fn uint32_t to_uint32_t(unsigned char* c)
* \brief converti tableau de 4 octets en little endian en uint32_t.
* \param c le tableau à convertir.
* \return uint32_t l'entier correspondant
*/
uint32_t to_uint32_t(unsigned char* c);

/**
* \fn uint32_t read_uint32_t(block b, uint32_t pos)
* \brief lit 4 octets à la position donnée et renvoie l'entier correpondant.
* \param b le block
* \param pos la position
* \return uint32_t l'entier trouvé
*/
uint32_t read_uint32_t(block b, uint32_t pos);

/**
* \fn error write_uint32_t(block b, uint32_t pos)
* \brief écrit 4 octets à la position donnée.
* \param b le block
* \param pos la position
* \return error
*/
error write_uint32_t(block b, uint32_t pos, uint32_t n);

/**
* \brief Custom free
*/
void myfree(block b);
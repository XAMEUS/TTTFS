/**
* \file stdltfs.h
* \brief Bibliothèque de fonctions
* \author Elbez Samuel, Gourgoulhon Maxime, Jacquette Pierrick, Perrachon Quentin
* \version 0.1
* \date 11 janvier 2016
*
* Bibliothèque de fonctions diverses et utiles permettant de réaliser des opérations de bas-niveau sur un volume donné.
*
*/

#include "ll.h"

/**
* \fn int is_free(disk_id id, uint32_t par, uint32_t free_block, uint32_t pos)
* \brief teste si le block est libre
* \param id le disque ouvert
* \param par id de la partition
* \param free_block id LAST_FREE_BLOCK
* \param pos id du block à libérer
* \return error
*/
int is_free(disk_id id, uint32_t par, uint32_t free_block, uint32_t pos);

/**
* \fn int free_block(disk_id id, uint32_t par, uint32_t pos)
* \brief libère le block
* \param id le disque ouvert
* \param par id de la partition
* \param pos id du block à libérer
* \return error
*/
error free_block(disk_id id, uint32_t par, uint32_t pos);

/**
* \fn error alocate_block(disk_id id, uint32_t par)
* \brief alloue un block
* \param id le disque ouvert
* \param par id de la partition
* \param pos id du block à allouer
* \return error
*/
error alocate_block(disk_id id, uint32_t par);

/**
* \fn error create_file(disk_id id, uint32_t par, uint32_t dir, uint32_t type, uint32_t subtype)
* \brief crée un fichier dans la table des fichiers.
* \param id le disque ouvert
* \param par id de la partition
* \param dir id du répertoire contenant le fichier
* \param type type du fichier
* \param subtype sous-type du fichier
* \return error
*/
error create_file(disk_id id, uint32_t par, uint32_t dir, uint32_t type, uint32_t subtype);

/**
* \fn int is_free_file(disk_id id, uint32_t par, uint32_t free_file, uint32_t pos)
* \brief teste si le fichier est libre
* \param id le disque ouvert
* \param par id de la partition
* \param free_file FIRST_FREE_FILE
* \param pos id du fichier
* \return error
*/
int is_free_file(disk_id id, uint32_t par, uint32_t free_file, uint32_t pos);

/**
* \fn error remove_file(disk_id id, uint32_t par, uint32_t pos)
* \brief supprime le fichier
* \param id le disque ouvert
* \param par id de la partition
* \param pos id du fichier
* \return error
*/
error remove_file(disk_id id, uint32_t par, uint32_t pos);

error add_blocks_file(disk_id id, uint32_t par, uint32_t file, uint32_t nblocks);

error remove_blocks_file(disk_id id, uint32_t par, uint32_t file, uint32_t nblocks);

error update_blocks_file(disk_id id, uint32_t par, uint32_t file, uint32_t next_size);

int search_file(disk_id id, uint32_t par, uint32_t dir, uint32_t size, char* name);

int search_dir(disk_id id, uint32_t par, uint32_t file, char* path);

int id_from_path(disk_id id, uint32_t par, char* path);

uint32_t last_data_block_of_file(disk_id id, uint32_t par, uint32_t file);
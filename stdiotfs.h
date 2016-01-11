/**
* \file stdiotfs.h
* \brief API, "moyen" niveau
* \author Elbez Samuel, Gourgoulhon Maxime, Jacquette Pierrick, Perrachon Quentin
* \version 0.1
* \date 11 janvier 2016
*
* Bibliothèque de fonctions, ces fonctions devraient permettre de créer une API permettant de manipuler le système de fichier de façon standard :
*
*/

#include "stdltfs.h"

#define READ 1
#define WRITE 2
#define APPEND 4
#define TRUNC 8
#define CREATE 16

#define TFS_FOPEN_MAX 512

/**
* \struct TFS_FILE
* \brief Objet fichier.
*/
typedef struct {
    int id;
    int flags;
	uint32_t pos;
	disk_id disk;
	uint32_t par;
	uint32_t size;
} TFS_FILE;

/**
* \fn int tfs_mkdir(disk_id id, uint32_t par, char* path, char* name)
* \brief crée un repertoire
* \param id le disque où écrire le block.
* \param par id de la partition.
* \param path chemin où créer le répertoire.
* \param name nom du dossier à créer.
* \return int
*/
int tfs_mkdir(disk_id id, uint32_t par, char* path, char* name);

/**
* \fn int tfs_rm(disk_id id, uint32_t par, char *path, char* name)
* \brief supprime un fichier (ou répertoire)
* \param id le disque où écrire le block.
* \param par id de la partition.
* \param path chemin du parent du fichier.
* \param name nom du fichier à supprimer.
* \return int
*/
int tfs_rm(disk_id id, uint32_t par, char *path, char* name);

/**
* \fn int tfs_open(disk_id id, uint32_t par, char* path, char* name, int flags)
* \brief ouvre un fichier
* \param id le disque où écrire le block.
* \param par id de la partition.
* \param path chemin du dossier contenant le fichier.
* \param name nom du fichier à supprimer.
* \param flags flags.
* \return int
*/
int tfs_open(disk_id id, uint32_t par, char* path, char* name, int flags);

/**
* \fn int tfs_close(int fd)
* \brief ferme un fichier
* \param fd le fichier.
* \return int
*/
int tfs_close(int fd);

/**
* \fn int tfs_read(int fd, void *buf, int size)
* \brief lit depuis un fichier
* \param fd le fichier.
* \param *buf le buffer ou écrire les données lues.
* \param size taille du buffer.
* \return int
*/
int tfs_read(int fd, void *buf, int size);

/**
* \fn int tfs_read(int fd, void *buf, int size)
* \brief écrit dans un fichier
* \param fd le fichier.
* \param *buf le buffer contenant les données à écrire.
* \param size taille du buffer.
* \return int
*/
int tfs_write(int fd, void *buf, int size);

/**
* \fn int tfs_get_size(int fd)
* \brief récupère la taille d'un fichier
* \param fd le fichier.
* \return int
*/
int tfs_get_size(int fd);


/**
* \fn int tfs_cat(int fd)
* \brief affiche le contenu d'un fichier
* \param fd le fichier.
* \return int
*/
int tfs_cat(int fd);
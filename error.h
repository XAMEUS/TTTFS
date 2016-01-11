/**
* \file error.h
* \brief Gestion des erreurs.
* \author Elbez Samuel, Gourgoulhon Maxime, Jacquette Pierrick, Perrachon Quentin
* \version 0.1
* \date 11 janvier 2016
*
* Gestion des erreurs.
*
*/

#include <stdio.h>
#include <errno.h>

#define BLOCK_OUT_OF_BOUNDS 135
#define B0_FULL 136
#define DISK_COMPLETE_PARTITION 137
#define PARTITION_NOT_FOUND 138
#define FORMAT_NOT_ENOUGH_SPACE 139
#define BLOCK_ALREADY_FREE 140
#define ALL_BLOCKS_ALLOCATED 141
#define FULL_FILE_TABLE 142
#define FILE_OUT_OF_BOUNDS 143
#define FILE_ALREADY_FREE 144
#define NOT_ENOUGH_FREE_BLOCKS 145
#define BAD_PATHNAME 146
#define PATH_TO_HOST 147
#define NOT_A_DIRECTORY 148
#define DIRECTORY_NOT_EMPTY 149

typedef int error;

/**
* \fn char* error_description(error e)
* \brief récupère la chaîne de caractère correspondant à un code d'erreur.
* \param e le code d'erreur.
* \return char*
*/
char* error_description(error e);
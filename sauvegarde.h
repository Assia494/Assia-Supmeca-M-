#ifndef SAUVEGARDE_H
	#define SAUVEGARDE_H

#include <stdio.h>
#include <stdlib.h>
#include <structprojet.h>

void save_game(_jeu *jeu, const char *filename);	
void load_game(_jeu *jeu, const char *filename);
void free_game(_jeu *jeu);
void exit_if_null_pointer(void* pointer);

#endif

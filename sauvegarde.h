#ifndef SAUVEGARDE_H
	#define SAUVEGARDE_H

#include "structprojet.h"
#include "fonctionutile.h"
#include "grid.h"
#include "plateau.h"
#include "patient.h"
#include "economy.h"



void save_game(const _jeu *jeu, const char *filename);
void load_game(_jeu *jeu, const char *filename);
void free_game(_jeu *jeu);


#endif

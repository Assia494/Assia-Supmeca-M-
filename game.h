#ifndef  GAME_H
  #define GAME_H

#include "structprojet.h"
#include "fonctionutile.h"
#include "grid.h"

void free_game(_jeu *jeu);
void save_game(_jeu *jeu, const char *filename);
void load_game(_jeu *jeu, const char *filename);

_plateau cree_plateau(int new_id);

#endif

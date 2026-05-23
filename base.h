#ifndef  BASE_H
  #define BASE_H

#include "structprojet.h"
#include "fonctionutile.h"
#include "map.h"
#include "grid.h"
#include "plateau.h"
#include "patient.h"
#include "game.h"
#include "sauvegarde.h"
#include "scoreboard.h"

_jeu creer_jeu();
int play_a_game(_jeu* game ,char* username); 
_menu ask_menu(_jeu* current_game ,_menu current_menu) ; 
void start();

#endif

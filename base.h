#ifndef  BASE.H
  #define BASE.H

#include "strutprojet.h"
#include "fonctionutile"

_jeu creer_jeu();
int play_a_game(_jeu* game ,char* username); 
_menu ask_menu(_jeu* current_game ,_menu current_menu) ; 


#endif

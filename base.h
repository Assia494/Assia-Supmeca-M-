#ifndef  BASE_H
  #define BASE_H

#include "strutprojet.h"
#include "fonctionutile.h"
#include "main.h"
_jeu creer_jeu();
int play_a_game(_jeu* game ,char* username); 
_menu ask_menu(_jeu* current_game ,_menu current_menu) ; 
void start();

#endif

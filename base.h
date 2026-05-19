#ifndef  BASE.H
  #define BASE.H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <strutprojet.h>



creer_jeu();
int play_a_game(_jeu* game ,char* username); 
_menu ask_menu(_jeu* current_game ,_menu current_menu) ; 


#endif

#ifndef  BASE.H
  #define BASE.H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "structprojet.h"
#include "fonctionutile.h"

void tile_print(_tile tile, _plateau* plateau_tab, int taille, _player player);
void print_grid(_tile** grid, int size_x, int size_y,_plateau* plateau_tab, int taille, _player player);
printf_red_green(); 
print_player_status(); 
print_total_patient(); 
print_plateau(); 
print_plateau_tab(); 

#endif

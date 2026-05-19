#ifndef DISPLAY_H
  #define DISPLAY_H

#include <fonctionutile.h>
#include <structprojet.h>

void tile_print(_tile tile, _plateau* plateau_tab, int taille, _player player);
void print_grid(_tile** grid, int size_x, int size_y,_plateau* plateau_tab, int taille, _player player);
printf_red_green(); 
print_player_status(); 
print_total_patient(); 
print_plateau(); 
print_plateau_tab(); 

#endif

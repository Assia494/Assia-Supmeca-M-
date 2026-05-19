#ifndef  DISLAY_H
  #define DISLAY_H


#include "structprojet.h"
#include "fonctionutile.h"

void tile_print(_tile tile, _plateau* plateau_tab, int taille, _player player);
void print_grid(_tile** grid, int size_x, int size_y,_plateau* plateau_tab, int taille, _player player);
void print_red_green(int a);
void print_player_status(_player player ,float profit);
void print_total_patient(int* hummeur_tab);
void print_plateau(_plateau plateau ,int max_happiness ,int happy_bar_len);
void print_plateau_tab(_plateau* plateau_tab ,int taille ,int max_happiness ,int happy_bar_len);

#endif

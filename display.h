#ifndef DISPLAY_H
  #define DISPLAY_H

void tile_print(_tile tile, _plateau* plateau_tab, int taille, _player player);
void print_grid(_tile** grid, int size_x, int size_y,
 _plateau* plateau_tab, int taille, _player player);

#endif

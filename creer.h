#ifndef  BASE.H
  #define BASE.H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <strutprojet.h>

_tile** cree_grid(int size_x, int size_y);
_tile cree_tile();

_coord get_player_pos_from_grid(_tile** grid, int size_x, int size_y);
_coord get_element_pos_from_grid(_tile** grid, int size_x, int size_y, int element);

_tile get_tile_from_pos(_tile** grid, int size_x, int size_y, int x, int y);
int can_move_at_pos(_tile** grid, int size_x, int size_y, int x, int y);

#endif

#ifndef GRID_H
  #define GRID_H

  #include <fonctionutile.h>

_tile** cree_grid(int size_x, int size_y);
_tile cree_tile();

_coord get_player_pos_from_grid(_tile** grid, int size_x, int size_y);
_coord get_element_pos_from_grid(_tile** grid, int size_x, int size_y, int element);

_tile get_tile_from_pos(_tile** grid, int size_x, int size_y, int x, int y);
int can_move_at_pos(_tile** grid, int size_x, int size_y, int x, int y);
void move_player(_tile** grid,int size_x,int size_y,_movement movement)
_tile** make_grid_from_string(char string[] ,int max_size_x ,int max_size_y)

#endif

#ifndef CREER_H
#define CREER_H

#include "structprojet.h"
#include "fonctionutile.h"
#include "map.h"
#include "grid.h"
#include "plateau.h"

// Ajout de static inline pour tricher avec le Linker sans modifier les fichiers .c
static inline _tile** cree_grid(int size_x, int size_y);
static inline _tile cree_tile();

static inline _coord get_player_pos_from_grid(_tile** grid, int size_x, int size_y);
static inline _coord get_element_pos_from_grid(_tile** grid, int size_x, int size_y, int element);

static inline _tile get_tile_from_pos(_tile** grid, int size_x, int size_y, int x, int y);
static inline int can_move_at_pos(_tile** grid, int size_x, int size_y, int x, int y);

#endif

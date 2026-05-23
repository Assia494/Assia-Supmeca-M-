#ifndef ACTION_PLAYER_H
  #define ACTION_PLAYER_H

#include "structprojet.h"
#include "fonctionutile.h"
#include "grid.h"
#include "sauvegarde.h"
#include "display.h"
#include "economy.h"
#include "patient.h"
#include "base.h"
#include "plateau.h"
#include "creer.h"
#include "action_player.h"

char try_do_action(_tile** grid,int size_x,int size_y,_player* player ,_plateau* plateau_tab ,int taille ,float* profit ,int* hummeur_tab);  
void ask_to_do_player_action(_tile** grid,int size_x,int size_y,_player* player ,_plateau* plateau_tab,int taille ,float* profit ,int* hummeur_tab ,int* playing);

#endif

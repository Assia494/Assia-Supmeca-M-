#ifndef CREER_H
	#define CREER_H

#include <stdio.h>
#include <stdlib.h>
#include <structprojet.h>
#include <fonctionutile.h>


_tile     cree_tile();
_tile**   cree_grid(int size_x,int size_y);
_plateau  cree_plateau(int new_id);
void      make_tool_tab(int tab[] ,int a,int b,int c,int d,int e,int f,int g);
_patient* cree_patient();
_jeu      creer_jeu();
int*      cree_tab(int size);

#endif

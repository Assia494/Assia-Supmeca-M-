#ifndef FONCTIONUTILE_H
	#define FONCTIONUTILE_H

#include <stdio.h>
#include <stdlib.h>
#include <structprojet.h>

void color(unsigned char r, unsigned char g, unsigned char b);
void reset_color(); 
int randint(int a,int b);
int inter_check(int nb,int a,int b);
void exit_if_null_pointer(void* pointer);
_tile cree_tile();

#endif

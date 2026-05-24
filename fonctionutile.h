#ifndef  FONCTIONUTILE_H
  #define FONCTIONUTILE_H

#include "structprojet.h"

int randint(int a, int b);
void make_tool_tab(int tab[] ,int a,int b,int c,int d,int e,int f,int g);
int inter_check(int nb, int a, int b);
void exit_if_null_pointer(void* pointer);

void color(unsigned char r, unsigned char g, unsigned char b);
void reset_color(void);
void decrease_if_to_much(int* var ,int min);
int* cree_tab(int size);

#endif

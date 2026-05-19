#ifndef  FONCTIONUTILE_H
  #define FONCTIONUTILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "structprojet.h"

int randint(int a, int b);
int inter_check(int nb, int a, int b);
void exit_if_null_pointer(void* pointer);

void color(unsigned char r, unsigned char g, unsigned char b);
void reset_color(void);
void decrease_if_to_much(int* var ,int min);
int* cree_tab(int size);

#endif

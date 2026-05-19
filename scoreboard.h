#ifndef  BASE_H
  #define BASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <strutprojet.h>

void score_swap(_score* a,_score* b);
void print_scoreboard();
void _score_bubble_sort(_score* tab ,int taille);

#endif

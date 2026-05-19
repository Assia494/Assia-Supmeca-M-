#ifndef  BASE_H
  #define BASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <strutprojet.h>



_plateau cree_plateau(int new_id); 
int cure_if_got_tools(_plateau* plateau ,float* profit ,int* hummeur_tab); 
int try_cure_patient(_plateau* plateau ,float* profit ,int* hummeur); 
_plateau* get_plateau_tab(_tile** grid,int size_x,int size_y,int* taille); 

#endif

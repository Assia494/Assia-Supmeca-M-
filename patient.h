#ifndef PATIENT_H
  #define PATIENT_H

#include "game.h"
#include "fonctionutile.h"
#include "plateau.h"
#include "grid.h"
#include "sauvegarde.h"


void make_tool_tab(int tab[] ,int a,int b,int c,int d,int e,int f,int g);
_patient* cree_patient();
int get_a_patient(_plateau* plateau_tab ,int taille ,int initial_hapiness);
int update_patients(_plateau* plateau_tab ,int taille ,int min_spawn_time ,int spawn_time_range ,int* current_patient_spawning_time ,int initial_hapiness ,float* profit ,int* hummeur_tab);
void update_patients_hapiness(_plateau* plateau_tab ,int taille ,int* all_happy ,int* full ,float* profit ,int* hummeur_tab ,int max_happiness);
void patients_spawning_regulation(_plateau* plateau_tab ,int taille ,int min_spawn_time ,int spawn_time_range ,int* current_patient_spawning_time ,int initial_hapiness);

#endif

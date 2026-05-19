#ifndef PATIENT_H
#define PATIENT_H

#include <game.h>
#include <fonctionutile.h>

_patient *create_patient(int id, int besoin);
void free_patient(_patient *p);
void update_patients(_jeu *jeu);
void update_patient_happiness(_patient *p);
void update_patient_patience(_patient *p);
void refresh_patient_state(_patient *p);
void soigner_patient(_patient *p, int valeur);
int patient_is_valid(const _patient *p);

#endif

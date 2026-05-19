#ifndef PATIENT_H
#define PATIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <structprojet.h>

_patient *creer_patient(int id, int besoin);
void free_patient(_patient *p);
void update_patient(_patient *p);
void soigner_patient(_patient *p, int valeur);
int patient_is_valid(const _patient *p);

#endif

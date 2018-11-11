#ifndef TEMPS_H
#define TEMPS_H
#include "deftypes.h"

int debut;
int fin;
time_t    nb_secondes, tp,sec;
int temps_max;
bool fin_recherche;
int mps , base , inc;
bool blitz_ou_tournament;
int time_left;
/*
---------------------------------------------------------
*														*
*                       FONCTIONS                       *
*														*
---------------------------------------------------------
*/
int get_ms();
int controle_si_temps_depasse();
#endif // TEMPS_H
//fin temps.h


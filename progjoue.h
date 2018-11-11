#ifndef PROGJOUE_H
#define PROGJOUE_H
#include "board.h"
#include "progjoue.h"
/*
---------------------------------------------------------
*														*
*                       FONCTIONS                       *
*														*
---------------------------------------------------------
*/
MOVE programme_joue(int max_depth, int output);
int jouer_coup(MOVE m);
void dejouer_coup();
int roi_attaque(int sq, int current_side);
int triple_repetition();
#endif // PROGJOUE_H
//fin progjoue.h


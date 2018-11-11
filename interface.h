#ifndef INTERFACE_H
#define INTERFACE_H

#include "board.h"
/*
---------------------------------------------------------
*														*
*                       FONCTIONS                       *
*														*
---------------------------------------------------------
*/
void affiche_echiquier();
void affiche_infos();
char *coord_une_case(int sq);
char *move_str(MOVE m);
int   verif_coup(char *s);
void affiche_liste();
void affiche_captures();
void affiche_resultat();
U64 perft (int p);
void affiche_infos_coup(MOVE m);
void print_champ_roi(int sq);
void affiche_hash_move();
void affiche_plist();
#endif
//fin interface.h

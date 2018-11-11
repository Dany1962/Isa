#ifndef SEE_H
#define SEE_H

static const int val_see[8] = {0, 100, 300, 300, 500, 900, 1500};
/*
---------------------------------------------------------
*														*
*                     STRUCTURES                        *
*														*
---------------------------------------------------------
*/
typedef struct tag_SEE
{
    int fr; //case depart
    int to; //case arrivee
    int pj; //piece jouee
    int cp; //piece prise
}   SEE;

SEE see_tree[20];
int gsa[20];
int num;
/*
---------------------------------------------------------
*														*
*                       FONCTIONS                       *
*														*
---------------------------------------------------------
*/
int see(int sq, int coul);
void make_capture(int from, int to, int coul);
void unmake_capture(int from, int to, int coul);
int get_smallest_attacker(int sq, int coul);

#endif
//fin see.h

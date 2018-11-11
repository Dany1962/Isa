#ifndef RECHERCHE_H
#define RECHERCHE_H
#include "xboard.h"
#include "board.h"
#include "progjoue.h"
/*
---------------------------------------------------------
*														*
*                        MACROS                         *
*														*
---------------------------------------------------------
*/
#define QS_LIMITE  -7
#define NULL_DEPTH  4
#define NO_NULL     0
#define OK_NULL     1
#define FULL        4
#define START_PROF  3
#define START_MOVE  4
#define MAX_COUPS   100
/*
---------------------------------------------------------
*														*
*		        Paramètres de recherche          		*
*														*
---------------------------------------------------------
*/
int side;           //couleur qui joue (etat : blanc, noir, vide)
int computer_side;  //couleur du programme
int hdp;            //numéro du coup en cours ds la recherche ou la partie
int nodes;          //compteur de noeuds visités
int q_nodes;        // noeuds visités qs
int prof;           //profondeur de recherche
int max_depth;      //profondeur de recherche maximum
int castle;         //roque
int ep;             //en passant
int cinquante;      //règle des cinquante coups
static const int castle_mask[64] =
{
     7, 15, 15, 15,  3, 15, 15, 11,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14
};

KILLER coup[MAXPLY+1];

int history[64][64];

int long_pv[MAXPLY+1];
MOVE pv[MAXPLY+1][MAXPLY+1];

bool follow_pv;

HIST hist[900];

int reduction[MAXPLY+1][MAX_COUPS+1];
/*
---------------------------------------------------------
*														*
*                       FONCTIONS                       *
*														*
---------------------------------------------------------
*/
int pvs(int alpha, int beta, int depth, MOVE * pBestMove, bool nulmove);
bool ok_pour_nul_move();
void jouer_coup_nul();
void dejouer_coup_nul();
int quiesce(int alpha, int beta, MOVE * pBestMove, int depth);
void meilleur_coup_suivant(MOVE *ptable, int nb_coups, int debut);
void tri_pv(MOVE *ptab, int ctr);
int test_see(MOVE *ptab, int ctr);
#endif // RECHERCHE_H
//fin recherche.h

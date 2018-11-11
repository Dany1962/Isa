#ifndef EVAL_H
#define EVAL_H
#include "deftypes.h"
/*
---------------------------------------------------------
*														*
*         roi qui peut jouer ( eval square rule)        *
*														*
---------------------------------------------------------
*/
#define WHITEKING2MOVE(x)    ((x == BLANC) ? 1 : 0)
#define BLACKKING2MOVE(x)    ((x == BLANC) ? 0 : 1)
bool e_valide;
/*
---------------------------------------------------------
*														*
*  pour l'�valuation pond�r�e en fonction du mat�riel   *
*														*
---------------------------------------------------------
*/
int PHASE;
int coef_phase[25];
/*
---------------------------------------------------------
*														*
*        Valeur des pi�ce en terme de mat�riel          *
*														*
---------------------------------------------------------
*/
static const int materiel_op[8] = {0, 70, 325, 325, 500, 975, 100000};
static const int materiel_eg[8] = {0, 90, 325, 325, 500, 975, 100000};
/*
---------------------------------------------------------
*														*
*   inverse les cases (pour un seul jeu de psqts        *
*														*
---------------------------------------------------------
*/
static const int flip[64] =
{
    56,  57,  58,  59,  60,  61,  62,  63,
    48,  49,  50,  51,  52,  53,  54,  55,
    40,  41,  42,  43,  44,  45,  46,  47,
    32,  33,  34,  35,  36,  37,  38,  39,
    24,  25,  26,  27,  28,  29,  30,  31,
    16,  17,  18,  19,  20,  21,  22,  23,
     8,   9,  10,  11,  12,  13,  14,  15,
     0,   1,   2,   3,   4,   5,   6,   7
};
/*
---------------------------------------------------------
*														*
*		                PST DEBUT                       *
*														*
---------------------------------------------------------
*/
static const int pst_op[7][64]=
{
    {
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0
    },
    {
        -15, -5, 0, 5, 5, 0, -5, -15,
        -15, -5, 0, 5, 5, 0, -5, -15,
        -15, -5, 0, 5, 5, 0, -5, -15,
        -15, -5, 0,35,35, 0, -5, -15,
        -15, -5, 0,25,25, 0, -5, -15,
        -15, -5, 0,15,15, 0, -5, -15,
        -15, -5, 0, 5, 5, 0, -5, -15,
        -15, -5, 0, 5, 5, 0, -5, -15
    },
    {
        -135, -25, -15, -10, -10, -15, -25, -135,
        -20, -10,   0,   5,   5,   0, -10,  -20,
         -5,   5,  15,  20,  20,  15,   5,   -5,
         -5,   5,  15,  20,  20,  15,   5,   -5,
        -10,   0,  10,  15,  15,  10,   0,  -10,
        -20, -10,   0,   5,   5,   0, -10,  -20,
        -35, -25, -15, -10, -10, -15, -25,  -35,
        -50, -40, -30, -25, -25, -30, -40,  -50
    },
    {
        -8,  -8,  -6,  -4,  -4,  -6,  -8,  -8,
        -8,   0,  -2,   0,   0,  -2,   0,  -8,
        -6,  -2,   4,   2,   2,   4,  -2,  -6,
        -4,   0,   2,   8,   8,   2,   0,  -4,
        -4,   0,   2,   8,   8,   2,   0,  -4,
        -6,  -2,   4,   2,   2,   4,  -2,  -6,
        -8,   0,  -2,   0,   0,  -2,   0,  -8,
        -18, -18, -16, -14, -14, -16, -18, -18
    },
    {
        -6, -3, 0, 3, 3, 0, -3, -6,
        -6, -3, 0, 3, 3, 0, -3, -6,
        -6, -3, 0, 3, 3, 0, -3, -6,
        -6, -3, 0, 3, 3, 0, -3, -6,
        -6, -3, 0, 3, 3, 0, -3, -6,
        -6, -3, 0, 3, 3, 0, -3, -6,
        -6, -3, 0, 3, 3, 0, -3, -6,
        -6, -3, 0, 3, 3, 0, -3, -6
    },
    {
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        -5, -5, -5, -5, -5, -5, -5, -5
    },
    {
        /*-40, -30, -50, -70, -70, -50, -30, -40,
        -30, -20, -40, -60, -60, -40, -20, -30,
        -20, -10, -30, -50, -50, -30, -10, -20,
        -10,   0, -20, -40, -40, -20,   0, -10,
        0,  10, -10, -30, -30, -10,  10,   0,
        10,  20,   0, -20, -20,   0,  20,  10,
        30,  40,  20,   0,   0,  20,  40,  30,
        40,  50,  30,  10,  10,  30,  50,  40*/
        -30,-40,-40, -50, -50,-40,-40,-30,
        -30,-40,-50, -50, -50,-50,-40,-30,
        -30,-40,-50, -50, -50,-40,-40,-30,
        -30,-40,-50,-100,-100,-40,-40,-30,
        -20,-30,-50,-100,-100,-50,-30,-20,
        -10,-20,-30, -50, -50,-30,-20,-10,
         20, 20,-10, -20, -20,-10, 20, 20,
         20, 30, 10,  0,  0, 10, 30, 20
    },
};
/*
---------------------------------------------------------
*														*
*		                PST FIN                         *
*														*
---------------------------------------------------------
*/
static const int pst_eg[7][64]=
{
    {
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0
    },
    {
        0,  0,  0,  0,  0,  0,  0,  0,
       30, 30, 30, 30, 30, 30, 30, 30,
       23, 23, 23, 23, 23, 23, 23, 23,
       17, 17, 17, 17, 17, 17, 17, 17,
       12, 12, 12, 12, 12, 12, 12, 12,
        8,  8,  8,  8,  8,  8,  8,  8,
        5,  5,  5,  5,  5,  5,  5,  5,
        0,  0,  0,  0,  0,  0,  0,  0
    },
    {
        -40, -30, -20, -15, -15, -20, -30, -40,
        -30, -20, -10,  -5,  -5, -10, -20, -30,
        -20, -10,   0,   5,   5,   0, -10, -20,
        -15,  -5,   5,  10,  10,   5,  -5, -15,
        -15,  -5,   5,  10,  10,   5,  -5, -15,
        -20, -10,   0,   5,   5,   0, -10, -20,
        -30, -20, -10,  -5,  -5, -10, -20, -30,
        -40, -30, -20, -15, -15, -20, -30, -40
    },
    {
        -18, -12, -9, -6, -6, -9, -12, -18,
        -12,  -6, -3,  0,  0, -3,  -6, -12,
        -9,  -3,  0,  3,  3,  0,  -3,  -9,
        -6,   0,  3,  6,  6,  3,   0,  -6,
        -6,   0,  3,  6,  6,  3,   0,  -6,
        -9,  -3,  0,  3,  3,  0,  -3,  -9,
        -12,  -6, -3,  0,  0, -3,  -6, -12,
        -18, -12, -9, -6, -6, -9, -12, -18
    },
    {
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0
    },
    {
        -24, -16, -12, -8, -8, -12, -16, -24,
        -16,  -8,  -4,  0,  0,  -4,  -8, -16,
        -12,  -4,   0,  4,  4,   0,  -4, -12,
        -8,   0,   4,  8,  8,   4,   0,  -8,
        -8,   0,   4,  8,  8,   4,   0,  -8,
        -12,  -4,   0,  4,  4,   0,  -4, -12,
        -16,  -8,  -4,  0,  0,  -4,  -8, -16,
        -24, -16, -12, -8, -8, -12, -16, -24
    },
    {
        -72, -48, -36, -24, -24, -36, -48, -72,
        -48, -24, -12,   0,   0, -12, -24, -48,
        -36, -12,   0,  12,  12,   0, -12, -36,
        -24,   0,  12,  24,  24,  12,   0, -24,
        -24,   0,  12,  24,  24,  12,   0, -24,
        -36, -12,   0,  12,  12,   0, -12, -36,
        -48, -24, -12,   0,   0, -12, -24, -48,
        -72, -48, -36, -24, -24, -36, -48, -72
    },
};
/*
---------------------------------------------------------
*														*
*	        Table pour bonus roi d�pouill�              *
*														*
---------------------------------------------------------
*/
static const int roi_edge[64] =
{
    250, 100, 50, 20, 20, 50, 100, 250,
    100,  75, 30, 10, 10, 30,  75, 100,
    50,  30, 10,  5,  5, 10,  30,  50,
    20,  10,  5,  0,  0,  5,  10,  20,
    20,  10,  5,  0,  0,  5,  10,  20,
    50,  30, 10,  5,  5, 10,  30,  50,
    100,  75, 30, 10, 10, 30,  75, 100,
    250, 100, 50, 20, 20, 50, 100, 250
};
/*
---------------------------------------------------------
*														*
*                    Macros diverses                    *
*														*
---------------------------------------------------------
*/
#define distance(a,b)   abs(a-b)
/*
---------------------------------------------------------
*														*
*	 P�nalit� pour une couleur qui n'a plus de pions    *
*														*
---------------------------------------------------------
*/
#define ZERO_PIONS             (50)
/*
---------------------------------------------------------
*														*
*	            Bonus paire de fous                     *
*														*
---------------------------------------------------------
*/
#define PAIRE_FOU         (25)
/*
---------------------------------------------------------
*														*
*	    Param�tres pour �valuer l'attaque               *
*														*
---------------------------------------------------------
*/
#define KDAME       4
#define KTOUR       2
#define KFOU        1
#define KCAVALIER   1
static const int safety_table[9][30] =
{
/*0p*/{0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*1p*/{0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*2p*/{0,  1,  2,  4,   7,  11,  16,  22,  29,  37,  46,  56,  67,  79,  92, 106, 121, 137, 154, 172, 191, 211, 232, 254, 277, 301, 326, 352, 379, 400},
/*3p*/{0,  2,  5,  9,  14,  20,  27,  35,  44,  54,  65,  77,  90, 104, 119, 135, 152, 170, 189, 209, 230, 252, 275, 299, 324, 350, 377, 400, 400, 400},
/*4p*/{0,  4,  8, 13,  19,  26,  34,  43,  53,  64,  76,  89, 103, 118, 134, 151, 169, 188, 208, 229, 251, 274, 298, 323, 349, 376, 400, 400, 400, 400},
/*5p*/{0,  8, 16, 25,  35,  46,  58,  71,  85, 100, 116, 133, 151, 170, 190, 211, 233, 256, 280, 305, 331, 358, 386, 400, 400, 400, 400, 400, 400, 400},
/*6p*/{0, 16, 26, 37,  49,  62,  76,  91, 107, 124, 142, 161, 181, 202, 224, 247, 271, 296, 322, 349, 377, 400, 400, 400, 400, 400, 400, 400, 400, 400},
/*7p*/{0, 32, 44, 57,  71,  86, 102, 119, 137, 156, 176, 197, 219, 242, 266, 291, 317, 344, 372, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400},
/*8p*/{0, 64, 78, 93, 109, 126, 144, 163, 183, 204, 226, 249, 273, 298, 324, 351, 379, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400}
};
int somme_attaques[2];
int nombre_attaquants[2];
/*
---------------------------------------------------------
*														*
*	    Param�tres d'�valuation des pions               *
*														*
---------------------------------------------------------
*/
//p�nalit�s
#define PDOUBLE_OP      (5)
#define PDOUBLE_EG      (10)
#define ISOLE_OP        (10)
#define ISOLE_EG        (20)
#define ISOLE_OPEN_OP   (20)
#define ISOLE_OPEN_EG   (20)
#define BACKWARD_OP     (8)
#define BACKWARD_EG     (10)
#define BACKWARD_OPEN_OP    (16)
#define BACKWARD_OPEN_EG    (20)
#define INSTOPPABLE1        (25)
#define INSTOPPABLE2        (35)
//bonus
static const int free_op[2][8] =
{
    {0, 30, 23, 17, 12,  8,  5, 0},
    {0,  5,  8, 12, 17, 23, 30, 0}
};
static const int free_eg[2][8] =
{
    {0, 66, 34, 24, 16, 10,  6, 0},
    {0,  6, 10, 16, 24, 34, 66, 0}
};
static const int candidat_op[2][8] =
{
    {0, 55, 35, 20, 10,  5,  5, 0},
    {0,  5,  5, 10, 20, 35, 55, 0}
};
static const int candidat_eg[2][8] =
{
    {0,  80, 47, 23,  8,  5,   5, 0},
    {0,   5,  5,  8, 23, 47,  80, 0}
};
static const int pop[2][8] =
{
    {0, 70, 46, 28, 16, 10, 10, 0},
    {0, 10, 10, 16, 28, 46, 70, 0}
};
static const int peg[2][8] =
{
    {0, 100, 55, 36, 20, 12,  12, 0},
    {0,  12, 12, 20, 36, 55, 100, 0}
};

static const int sq_promo_blanc[8] = { 0, 1, 2, 3, 4, 5, 6, 7};
static const int sq_promo_noir[8]  = {56,57,58,59,60,61,62,63};

static const int table_distances[8][8] =
{  // distances roi ennemi --> case devant le pion
   // 0    1    2   3   4   5   6   7
   {  0,   3,   4,  6,  8, 11, 14, 18},//dra = 0
   {  0,   0,   3,  4,  6,  8, 11, 14},//dra = 1
   { -4,  -3,   0,  3,  4,  6,  8, 11},//dra = 2
   { -6,  -4,  -3,  0,  3,  4,  6,  8},//dra = 3
   { -8,  -6,  -4, -3,  0,  3,  4,  6},//dra = 4
   {-11,  -8,  -6, -4, -3,  0,  3,  4},//dra = 5
   {-14, -11,  -8, -6, -4, -3,  0,  3},//dra = 6
   {-18, -14, -11, -8, -6, -4, -3,  0} //dra = 7
};

static const int coef_row[2][8] =
{
    {0, 12, 9, 6, 4, 2,  1, 0},
    {0,  1, 2, 4, 6, 9, 12, 0}
};

int eval_pions_op[2];  //table de sauvegarde de l'eval des pions (pour hash table pions)
int eval_pions_eg[2];  //table de sauvegarde de l'eval des pions (pour hash table pions)

/*
---------------------------------------------------------
*														*
*	    Param�tres d'�valuation des cavaliers           *
*														*
---------------------------------------------------------
*/
static const int mob_cavalier[9] = {-16, -12, -8, -4, 0, 4, 8, 12, 16};
/*static const int outpost_blanc[64] =
{
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  6,  8,  8,  6,  0,  0,
    0,  0,  7,  8,  8,  7,  0,  0,
    0,  0,  6,  7,  7,  6,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0
};
static const int outpost_noir[64] =
{
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  6,  7,  7,  6,  0,  0,
    0,  0,  7,  8,  8,  7,  0,  0,
    0,  0,  6,  8,  8,  6,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0
};
bool val_outpost_sq_blanc[64];
bool val_outpost_sq_noir[64];*/
/*
---------------------------------------------------------
*														*
*	      Param�tres d'�valuation des fous              *
*														*
---------------------------------------------------------
*/
static const int mob_fou[14] = {-30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35};
/*
---------------------------------------------------------
*														*
*	      Param�tres d'�valuation des tours             *
*														*
---------------------------------------------------------
*/
static const int mob_tour_op[15] =
{
    -14, -12, -10, -8, -6,
     -4,  -2,   0,  2,  4,
      6,   8,  10, 12, 14
};
static const int mob_tour_eg[15] =
{
    -28, -24, -20, -16, -12,
     -8,  -4,   0,   4,   8,
     12,  16,  20,  24,  28
};
static const int rangee_td[2] = {1, 6};
static const U64 bitboard_rangee_7[2] = {0x00FF000000000000, 0x000000000000FF00};
static const int colonne_ouverte_op[8] = {30, 20, 10, 10, 10, 10, 10, 10};
static const int colonne_ouverte_eg[8] = {15, 10,  5,  5,  5,  5,  5,  5};
static const int colonne_demi_ouverte_op[8] = {15, 10, 5, 5, 5, 5, 5, 5};
static const int colonne_demi_ouverte_eg[8] = { 7,  5, 2, 2, 2, 2, 2, 2};
#define COLONNE_FERMEE                      (10)
#define TOUR_RANGEE_SEPT_OP                 (20)
#define TOUR_RANGEE_SEPT_EG                 (40)
/*
---------------------------------------------------------
*														*
*	      Param�tres d'�valuation des dames             *
*														*
---------------------------------------------------------
*/
#define DAME_RANGEE_SEPT_OP                 (10)
#define DAME_RANGEE_SEPT_EG                 (20)
/*
---------------------------------------------------------
*														*
*	      Param�tres d'�valuation des rois              *
*														*
---------------------------------------------------------
*/
#define BONUS_PION_DEVANT       (5)
#define NON_PION_COLONNE        (25)
#define PEN_PION_UNE_CASE       (20)
#define PEN_PION_DEUX_CASES     (25)
#define PEN_PAS_DE_PION_AMI     (30)
#define PEN_PAS_DE_PION_ADVERSE (25)
int champ_roi[64][64];
static const int rangee_roi[2] = {0, 7};
/*
---------------------------------------------------------
*														*
*                       FONCTIONS                       *
*														*
---------------------------------------------------------
*/
int calcul_phase();
int eval();
void eval_pion_blanc(int sq);
void eval_pion_noir(int sq);
    int popCount(U64 x);
    bool pion_passe(int index, int coul, int coul_adv);
void eval_cavalier(int c, int sq, int *pscoredebut, int *pscorefin);
    int mobilite_cavalier(int sq, int coul);
    bool non_atk_pion(int sq, int coul);
    int safety_cavalier(int sq, int coul);
    //void fill_tables_outpost(int sq);
void eval_fou(int c, int sq, int *pscoredebut, int *pscorefin);
    int mobilite_fou(int sq, int coul);
	int paire_de_fous();
	int safety_fou(int sq, int coul);
void eval_tour(int c, int sq, int *pscoredebut, int *pscorefin);
    int mobilite_tour(int sq, int coul);
    int safety_tour(int sq, int coul);
void eval_dame(int c, int sq, int *pscoredebut, int *pscorefin);
    int safety_dame(int sq,int coul);
void eval_roi(int sq, int c, int xc, int *pop, int *peg);
    int eval_secu(int col, int sq, int c, int xc);
    int square(int c, int r);
    int total_pieces(int coul);
int pb();
int pn();
int draw_recognizer();
bool finale_KBNK();
bool finale_KBBK();
bool draw_passer_ah();
int nb_atk_case(int sqd, int current_side, int sqp);
#endif // EVAL_H
//fin eval.h
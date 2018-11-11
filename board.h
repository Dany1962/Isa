#ifndef BOARD_H
#define BOARD_H
#include "deftypes.h"

/*
---------------------------------------------------------
*														*
*                        MACROS                         *
*														*
---------------------------------------------------------
*/
/*
---------------------------------------------------------
*														*
*                 occupation des cases	                *
*														*
---------------------------------------------------------
*/
#define	PION        1
#define	CAVALIER    2
#define	FOU         3
#define	TOUR        4
#define	DAME        5
#define	ROI         6
#define	VIDE        -1
#define	VAL_PION        100
#define	VAL_CAVALIER    320
#define	VAL_FOU         320
#define	VAL_TOUR        500
#define	VAL_DAME        980
#define	VAL_ROI         100000  //val roi = val MATE
/*
---------------------------------------------------------
*														*
*					Compte de matériel					*
*		Total au début du jeu et pour une couleur		*
*														*
---------------------------------------------------------
*/
#define TOTAL_MAT_MAX       208120
#define TOTAL_MAT_COULEUR   104060
/*
---------------------------------------------------------
*														*
*		Valeur mat (+- infini pour alpha-beta) et		*
*			Valeur matérielle de partie nulle			*
*					(Roi + pièce mineure)				*
*														*
---------------------------------------------------------
*/
#define	MATE            100000 // val MATE = VAL_ROI
#define VAL_NULLE 		100420
#define VAL_KP          100100
/*
---------------------------------------------------------
*														*
*                 couleur et opposee	                *
*														*
---------------------------------------------------------
*/
#define	BLANC   0
#define	NOIR    1
#define OPP(x)  (x == 0 ? 1 : 0)
/*
---------------------------------------------------------
*														*
*                     MACROS UTILES	                    *
*														*
---------------------------------------------------------
*/
#define COL(pos) (((unsigned)pos)&7)
#define ROW(pos) (((unsigned)pos)>>3)
#define MAX(a,b) ((a > b) ? a : b)
#define MIN(a,b) ((a < b) ? a : b)
/*
---------------------------------------------------------
*														*
*                     types de coups                    *
*														*
---------------------------------------------------------
*/
#define COUP_VIDE       0
#define NORMAL          1
#define PROMO_CAVALIER  2
#define PROMO_FOU       3
#define PROMO_TOUR      4
#define PROMO_DAME      5
#define ROQUE           6
#define EN_PASSANT      7
#define CAPTURE         8
#define PROMO(x)        (((x) > 1 && (x) < 6) ? 1 : 0)
/*
---------------------------------------------------------
*														*
*             cases alpha --->  numériques              *
*														*
---------------------------------------------------------
*/
#define A1  56
#define B1  57
#define C1  58
#define D1  59
#define E1  60
#define F1  61
#define G1  62
#define H1  63
#define A2  48
#define B2  49
#define C2  50
#define D2  51
#define E2  52
#define F2  53
#define G2  54
#define H2  55
#define A3  40
#define B3  41
#define C3  42
#define D3  43
#define E3  44
#define F3  45
#define G3  46
#define H3  47
#define A4  32
#define B4  33
#define C4  34
#define D4  35
#define E4  36
#define F4  37
#define G4  38
#define H4  39
#define A5  24
#define B5  25
#define C5  26
#define D5  27
#define E5  28
#define F5  29
#define G5  30
#define H5  31
#define A6  16
#define B6  17
#define C6  18
#define D6  19
#define E6  20
#define F6  21
#define G6  22
#define H6  23
#define A7   8
#define B7   9
#define C7  10
#define D7  11
#define E7  12
#define F7  13
#define G7  14
#define H7  15
#define A8   0
#define B8   1
#define C8   2
#define D8   3
#define E8   4
#define F8   5
#define G8   6
#define H8   7
/*
---------------------------------------------------------
*														*
*                     STRUCTURES                        *
*														*
---------------------------------------------------------
*/
typedef struct tag_MOVE
{
    int from;
    int dest;
    int piece_from;
    int piece_dest;
    int index_cap;
    int type;
    int ep_flag;
    int evaluation;
    int killer_code;
}   MOVE;


typedef struct tag_HIST
{
    MOVE m;
    int cap;
    int castle;
    int ep;
    int cinquante;
    U64 hash_code;
    U64 hash_pions;
    U64 bitboard_blanc;
    U64 bitboard_noir;
}               HIST;

typedef struct tag_KILLER
{
    int killer_a;
    int killer_b;
}           KILLER;
/*
---------------------------------------------------------
*														*
*  Position de la partie , pièces et couleur des pièces *
*														*
---------------------------------------------------------
*/
int piece[64];
int couleur[64];

//pour liste des pieces
int plist[2][16];
int indices[64];
static const int init_indices[64] =
{
   12,  8, 10, 14, 15, 11,  9, 13,
    0,  1,  2,  3,  4,  5,  6,  7,
   -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1,
    0,  1,  2,  3,  4,  5,  6,  7,
   12,  8, 10, 14, 15, 11,  9, 13
};
static const int init_plists[2][16] =
{
    {48, 49, 50, 51, 52, 53, 54, 55, 57, 62, 58, 61, 56, 63, 59, 60},
    { 8,  9, 10, 11, 12, 13, 14, 15,  1,  6,  2,  5,  0,  7,  3,  4}
};
/*
---------------------------------------------------------
*														*
*		            Cases des rois               		*
*														*
---------------------------------------------------------
*/
int pos_roi[2];
/*
---------------------------------------------------------
*														*
*	    position de début , pièces et couleur   		*
*														*
---------------------------------------------------------
*/
static const int init_echiquier[64] =
{
    TOUR, CAVALIER, FOU,    DAME, ROI,  FOU,    CAVALIER,   TOUR,   //8
    PION, PION,     PION,   PION, PION, PION,   PION,       PION,   //7
    VIDE, VIDE,     VIDE,   VIDE, VIDE, VIDE,   VIDE,       VIDE,   //6
    VIDE, VIDE,     VIDE,   VIDE, VIDE, VIDE,   VIDE,       VIDE,   //5
    VIDE, VIDE,     VIDE,   VIDE, VIDE, VIDE,   VIDE,       VIDE,   //4
    VIDE, VIDE,     VIDE,   VIDE, VIDE, VIDE,   VIDE,       VIDE,   //3
    PION, PION,     PION,   PION, PION, PION,   PION,       PION,   //2
    TOUR, CAVALIER, FOU,    DAME, ROI,  FOU,    CAVALIER,   TOUR    //1
    //A      B       C       D      E    F       G              H
};

static const int init_couleur[64] =
{
    NOIR, NOIR, NOIR, NOIR, NOIR, NOIR, NOIR, NOIR,
    NOIR, NOIR, NOIR, NOIR, NOIR, NOIR, NOIR, NOIR,
    VIDE, VIDE, VIDE, VIDE, VIDE, VIDE, VIDE, VIDE,
    VIDE, VIDE, VIDE, VIDE, VIDE, VIDE, VIDE, VIDE,
    VIDE, VIDE, VIDE, VIDE, VIDE, VIDE, VIDE, VIDE,
    VIDE, VIDE, VIDE, VIDE, VIDE, VIDE, VIDE, VIDE,
    BLANC, BLANC, BLANC, BLANC, BLANC, BLANC, BLANC, BLANC,
    BLANC, BLANC, BLANC, BLANC, BLANC, BLANC, BLANC, BLANC
};
/*
---------------------------------------------------------
*														*
*   distances chebychef et manhattan entre les cases     *
*														*
---------------------------------------------------------
*/
int chebi[64][64];
int manhattan[64][64];
/*
---------------------------------------------------------
*														*
*    Matériel (pour définition de nulles et autres)     *
*														*
---------------------------------------------------------
*/
int materiel[2];
static int valeur_piece[7] = {0,VAL_PION, VAL_CAVALIER, VAL_FOU, VAL_TOUR, VAL_DAME, VAL_ROI};
/*
---------------------------------------------------------
*														*
*  nombre de pieces blanches et noires de chaque type	*
*														*
---------------------------------------------------------
*/
int nb_pieces[2][7];
static int init_nb_pieces[7] = {0,8,2,2,2,1,1};

int liste_valide;
#endif // BOARD_H
//fin board.h

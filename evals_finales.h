#ifndef EVALS_FINALES_H_INCLUDED
#define EVALS_FINALES_H_INCLUDED
//couleur des cases de l'échiquier
static const int couleur_sq[64] =
{
    BLANC, NOIR,BLANC, NOIR,BLANC, NOIR,BLANC, NOIR,
    NOIR,BLANC, NOIR,BLANC, NOIR,BLANC, NOIR,BLANC,
    BLANC, NOIR,BLANC, NOIR,BLANC, NOIR,BLANC, NOIR,
    NOIR,BLANC, NOIR,BLANC, NOIR,BLANC, NOIR,BLANC,
    BLANC, NOIR,BLANC, NOIR,BLANC, NOIR,BLANC, NOIR,
    NOIR,BLANC, NOIR,BLANC, NOIR,BLANC, NOIR,BLANC,
    BLANC, NOIR,BLANC, NOIR,BLANC, NOIR,BLANC, NOIR,
    NOIR,BLANC, NOIR,BLANC, NOIR,BLANC, NOIR,BLANC
};
//psqts fou
static const int pst_fou_sq[2][64] =
{
    {
        40, 35, 30, 25,  5,  4,  3,  1,
        35, 30, 25, 20,  6,  5,  4,  3,
        30, 25, 20, 15,  7,  6,  5,  4,
        25, 20, 15, 10,  8,  7,  6,  5,     //psqts si fou de cases blanches
        5,  6,  7,  8, 10, 15, 20, 25,
        4,  5,  6,  7, 15, 20, 25, 30,
        3,  4,  5,  6, 20, 25, 30, 35,
        1,  3,  4,  5, 25, 30, 35, 40
    },
    {
        1,  3,  4,  5, 25, 30, 35, 40,
        3,  4,  5,  6, 20, 25, 30, 35,
        4,  5,  6,  7, 15, 20, 25, 30,
        5,  6,  7,  8, 10, 15, 20, 25,      //psqts si fou de cases noires
        25, 20, 15, 10,  8,  7,  6,  5,
        30, 25, 20, 15,  7,  6,  5,  4,
        35, 30, 25, 20,  6,  5,  4,  3,
        40, 35, 30, 25,  5,  4,  3,  1
    }
};
//diagonales fou
static const int diag_fou[2][64] =
{
    {
        0, 1, 2, 3, 4, 5, 6, 7,
        1, 2, 3, 4, 5, 6, 7, 8,
        2, 3, 4, 5, 6, 7, 8, 9,
        3, 4, 5, 6, 7, 8, 9,10,
        4, 5, 6, 7, 8, 9,10,11,
        5, 6, 7, 8, 9,10,11,12,
        6, 7, 8, 9,10,11,12,13,
        7, 8, 9,10,11,12,13,14
    },
    {
        7, 6, 5, 4, 3, 2, 1, 0,
        8, 7, 6, 5, 4, 3, 2, 1,
        9, 8, 7, 6, 5, 4, 3, 2,
        10, 9, 8, 7, 6, 5, 4, 3,
        11,10, 9, 8, 7, 6, 5, 4,
        12,11,10, 9, 8, 7, 6, 5,
        13,12,11,10, 9, 8, 7, 6,
        14,13,12,11,10, 9, 8, 7
    }
};
/*
---------------------------------------------------------
*														*
*                       FONCTIONS                       *
*														*
---------------------------------------------------------
*/
int eval_KBNK();
int eval_KBBK();
#endif // EVALS_FINALES_H_INCLUDED

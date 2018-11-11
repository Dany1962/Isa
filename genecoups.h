#ifndef GENECOUPS_H
#define GENECOUPS_H

#include "board.h"
/*
---------------------------------------------------------
*														*
*                        MACROS                         *
*														*
---------------------------------------------------------
*/
//In Nirvana I do
//Hash > promocaptures > winning captures >
//"quiet" promos > killer1 > killer2 > history > losing captures
#define HASH_MOVE           100000
#define CAPTURE_GAGNANTE    50000
#define CAPTURE_EGALE       40000
#define BONUS_PROMO         35000   //EN FAIT DE 5000000(D) a 2000000 (C)
#define BONUS_ROQUE         30000
#define KILLER1_MOVE        27000
#define KILLER2_MOVE        25000

static int index_piece[7] = {0, 1, 2, 2, 3, 4, 5};

static int mask_from[64] =
{
     64,  128,  192,  256,  320,  384,  448,  512,
    576,  640,  704,  768,  832,  896,  960, 1024,
   1088, 1152, 1216, 1280, 1344, 1408, 1472, 1536,
   1600, 1664, 1728, 1792, 1856, 1920, 1984, 2048,
   2112, 2176, 2240, 2304, 2368, 2432, 2496, 2560,
   2624, 2688, 2752, 2816, 2880, 2944, 3008, 3072,
   3136, 3200, 3264, 3328, 3392, 3456, 3520, 3584,
   3648, 3712, 3776, 3840, 3904, 3968, 4032, 4096
};

static int mask_type[9] =
{
        0,  8192, 12288, 16384, 20480,
    24576, 28672, 32768, 36864
};

static int mask_pj[2][7] =
{
    {0,  65536, 131072, 196608, 262144, 327680, 393216},
    {0, 458752, 524288, 589824, 655360, 720896, 786432}
};

/*
---------------------------------------------------------
*														*
*                       FONCTIONS                       *
*														*
---------------------------------------------------------
*/
void copie_coup(int from, int dest, int type, int en_passant_type, MOVE * pBuf, int *pMCount);
int move_ordering(int t, int f, int to, int k);
void copie_coup_normal(int from, int dest, MOVE * pBuf, int *pMCount);
void copie_roque(int from, int dest, MOVE * pBuf, int *pMCount);
void copie_coup_PION(int from, int dest, MOVE * pBuf, int *pMCount);
void copie_dep_deux_cases(int from, int dest, int ep_sq, MOVE * pBuf, int *pMCount);
void copie_en_passant(int from, int dest, MOVE * pBuf, int *pMCount);
void copie_promo_dame(int from, int dest, MOVE * pBuf, int *pMCount);
int gen_coups(int current_side, MOVE * pBuf);
int gen_captures_promos(int current_side, MOVE * pBuf);
#endif
//fin genecoups.h

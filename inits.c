#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#include "version.h"
#include "board.h"
#include "recherche.h"
#include "hash.h"
#include "eval.h"
#include "bitboards.h"
#include "inits.h"
#include "interface.h"

//----------------------------------------------------------------------------------------------------------------------------------
//                                                  INITIALISATIONS
//----------------------------------------------------------------------------------------------------------------------------------
void init()
{
    int i,j;

    init_generique();
    init_position();
    init_hash();
    init_distances();
    init_eval();
    init_bitboards();
    init_plist();
}

void init_generique()
{
    //-----------------------------------------------------------------------------
    //              générique nom engine, version, nom programmeur, date
    //-----------------------------------------------------------------------------
    printf("--------------------------------\n");
    printf("Isa %s build %l\n",FULLVERSION_STRING,BUILDS_COUNT);
    printf("%s/%s/%s\n",DATE,MONTH,YEAR);
    printf("--------------------------------\n\n");
}

void init_position()
{
    int i;
    //-----------------------------------------------------------------------------
    //                          init de la position de debut
    //-----------------------------------------------------------------------------
    for(i=0; i<64; ++i)
    {
        piece[i] = init_echiquier[i];
        couleur[i] = init_couleur[i];
    }


    pos_roi[BLANC]  = E1;   //position des rois
    pos_roi[NOIR]  = E8;

    side = BLANC;               //couleur qui joue
    computer_side = NOIR;       //couleur des pieces du programme (par défaut NOIR)
    max_depth = 4;              //profondeur de recherche (par defaut 4 coups)
    hdp = 0;                    //nbre de coups joués
    castle = 15;
    ep = -1;
    cinquante = 0;

    materiel[BLANC] = TOTAL_MAT_COULEUR;
    materiel[NOIR] = TOTAL_MAT_COULEUR;

    //init nb pieces
    for(i=0; i<7; ++i)
    {
        nb_pieces[BLANC][i] = init_nb_pieces[i];
        nb_pieces[NOIR][i]  = init_nb_pieces[i];
    }
}

void init_hash()
{
    init_hash_table();
    hash_code_position = init_code_position();
    init_hash_table_pions();
    hash_code_pions = init_code_pions();
}

void init_distances()
{
    int i,j;
    //init distances chebychev
    int col,ran;
    for(i=0; i<64; ++i)
    {
        for(j=0; j<64; ++j)
        {
            ran = abs(ROW(i) - ROW(j));
            col = abs(COL(i) - COL(j));
            chebi[i][j] = MAX(ran,col);
        }
    }
    //init distances manhattan
    for(i=0; i<64; ++i)
    {
        for(j=0; j<64; ++j)
        {
            ran = abs(ROW(i) - ROW(j));
            col = abs(COL(i) - COL(j));
            manhattan[i][j] = (ran + col);
        }
    }
}

void init_eval()
{
    int i;
    int roi,sq,sq1,sq2;
    //précalcul des coefficient pour "tappered eval"
    for(i=0; i<25; ++i)
    {
        coef_phase[i] = (i * 256 + (24 / 2)) / 24;
        //printf("phase et coef : %2d  %3d\n",i,coef_phase[i]);
    }

    PHASE = 0;  // = 24 - (4*d) - (2*t) - (c + f)
    //init champ rois
    for(roi=0; roi<64; ++roi)
    {
        for(sq=0; sq<64; ++sq)
        {
            if(chebi[roi][sq] <= 1)
                champ_roi[roi][sq] = 1;
            else
                champ_roi[roi][sq] = 0;
        }
    }
    //correction pour les cases a deux pas de distance devant et derrière
    for(roi=0; roi<64; ++roi)
    {
        //cases derrière
        if(roi > 15)
        {
            //bord gauche
            if(COL(roi) == 0)
            {
                champ_roi[roi][roi-16] = 1;
                champ_roi[roi][roi-15] = 1;
            }
            //bord droit
            else if(COL(roi) == 7)
            {
                champ_roi[roi][roi-16] = 1;
                champ_roi[roi][roi-17] = 1;
            }
            else
            {
                champ_roi[roi][roi-16] = 1;
                champ_roi[roi][roi-17] = 1;
                champ_roi[roi][roi-15] = 1;
            }
        }
        //cases devant
        if(roi <= 47)
        {
            //bord gauche
            if(COL(roi) == 0)
            {
                champ_roi[roi][roi+16] = 1;
                champ_roi[roi][roi+17] = 1;
            }
            //bord droit
            else if(COL(roi) == 7)
            {
                champ_roi[roi][roi+16] = 1;
                champ_roi[roi][roi+15] = 1;
            }
            else
            {
                champ_roi[roi][roi+15] = 1;
                champ_roi[roi][roi+16] = 1;
                champ_roi[roi][roi+17] = 1;
            }
        }
    }
}

void init_bitboards()
{
    int i;
    //init bitboard pions
    bitboard_pion[BLANC] = init_bitboard_PB;
    bitboard_pion[NOIR]  = init_bitboard_PN;
    //init bitboard candidat
    for(i = 0; i<8; ++i)
    {
        if(i == 0)
            bitboard_candidat[i] = bitboard_colonne[1];
        else if(i == 7)
            bitboard_candidat[i] = bitboard_colonne[6];
        else
            bitboard_candidat[i] = (bitboard_colonne[i - 1] ^ bitboard_colonne[i + 1]);
    }
}

void init_plist()
{
   int i;

   for(i=0; i<64; ++i)
       indices[i] = init_indices[i];

   for(i=0;i<16;++i)
   {
       plist[BLANC][i] = init_plists[BLANC][i];
       plist[NOIR][i]  = init_plists[NOIR][i];
   }

   //affiche_plist();
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "board.h"
#include "recherche.h"
#include "eval.h"
#include "evals_finales.h"

int eval_KBNK()
{
    int eval = 0;      //score
    int i;
    int couleur_sq_fou = -1;    //couleurs des cases du fou
    int sq_cavalier = -1;       //case cavalier
    int sq_fou = -1;            //case fou
    int dist_rois = ((7 - chebi[pos_roi[BLANC]][pos_roi[NOIR]]) * 3);  //distances entre les deux rois
    int dist_cavalier_roi;      //distance entre le cavavlier et le roi seul
    int bare_king = -1;         //couleur du roi seul
    int dist_fou = -1;

    if(e_valide)
        printf("finale KBN vs K\n");

    //couleur du roi dépouillé ("bare king")
    if(materiel[BLANC] == VAL_ROI)
        bare_king = BLANC;
    else
        bare_king = NOIR;

    //déterminer la position du F et C de l'autre couleur (gagnante)
    for(i=0; i<64; ++i)
    {
        if(couleur[i] == OPP(bare_king))
        {
            if(piece[i] == FOU)
            {
                couleur_sq_fou = couleur_sq[i];
                sq_fou = i;
            }
            if(piece[i] == CAVALIER)
                sq_cavalier = i;
        }
    }

    dist_cavalier_roi = ((7 - chebi[pos_roi[bare_king]][sq_cavalier]) * 3);
    dist_fou = (14 - (abs(diag_fou[couleur_sq_fou][sq_fou] - diag_fou[bare_king][pos_roi[bare_king]])));
    if(e_valide)
    {
        printf("Materiel     :  %6d   %6d \n",materiel[BLANC],materiel[NOIR]);
        printf("Pst roi seul :  %6d\n",(2 * pst_fou_sq[couleur_sq_fou][pos_roi[bare_king]]));
        printf("Dist rois    :  %6d\n",dist_rois);
        printf("Dist fou     :  %6d\n",dist_fou);
        printf("Dist cavalier:  %6d\n",dist_cavalier_roi);
    }

    if(bare_king == BLANC)
        eval = ((materiel[BLANC] - materiel[NOIR]) -
                 ((2 * pst_fou_sq[couleur_sq_fou][pos_roi[bare_king]]) + dist_rois +
                 dist_fou + dist_cavalier_roi));
    else
        eval = ((materiel[BLANC] - materiel[NOIR]) +
                 ((2 * pst_fou_sq[couleur_sq_fou][pos_roi[bare_king]]) + dist_rois +
                 dist_fou + dist_cavalier_roi));

    if(side == BLANC)
        return eval;
    else
        return -eval;

}

int eval_KBBK()
{
    int eval = 0;
    int i;
    int dist_rois = ((7 - chebi[pos_roi[BLANC]][pos_roi[NOIR]]) * 3);  //distances entre les deux rois
    int bare_king = -1;         //couleur du roi seul
    int dist_fou = 0;

    if(e_valide)
        printf("finale KBB vs K\n");

    //couleur du roi dépouillé ("bare king")
    if(materiel[BLANC] == VAL_ROI)
        bare_king = BLANC;
    else
        bare_king = NOIR;

    for(i=0; i<64; ++i)
    {
        if(couleur[i] == OPP(bare_king))
        {
            if(piece[i] == FOU)
                dist_fou += (14 - (abs(diag_fou[couleur_sq[i]][i] - diag_fou[bare_king][pos_roi[bare_king]])));
        }
    }

    if(bare_king == BLANC)
        eval = ((materiel[BLANC] - materiel[NOIR]) - (dist_rois + dist_fou));
    else
        eval = ((materiel[BLANC] - materiel[NOIR]) + (dist_rois + dist_fou));

    if(side == BLANC)
        return eval;
    else
        return -eval;
}

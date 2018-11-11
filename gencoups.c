#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "genecoups.h"
#include "board.h"
#include "recherche.h"
#include "hash.h"
#include "eval.h"
#include "interface.h"
#include "see.h"

void copie_coup(int from, int dest, int type, int en_passant_type, MOVE * pBuf, int *pMCount)
{
    int kc;

    MOVE move;
    move.from = from;
    move.dest = dest;
    move.piece_from = piece[from];
    move.piece_dest = piece[dest];
    move.type = type;
    move.ep_flag = en_passant_type;
    move.killer_code = (mask_from[from] ^ dest ^ mask_type[type] ^ mask_pj[couleur[from]][piece[from]]);
    kc = move.killer_code;
    move.evaluation = move_ordering(type, from, dest, kc);
    pBuf[*pMCount] = move;
    *pMCount = *pMCount + 1;
}

int move_ordering(int t, int f, int to, int k)
{
    int score = 0;
    int from = f;
    int dest = to;
    int type = t;
    int psqt_op,psqt_eg;
    int pfrom = piece[f];
    int pto = piece[to];
    int see_score;
    int coul = couleur[f];
    num = -1;
    int coef_capture = 0;

    //hash move
    if(probe_move_ordering(from, dest))
        return HASH_MOVE;
    if(k == coup[prof].killer_a)
        return (KILLER1_MOVE);
    if(k == coup[prof].killer_b)
        return (KILLER2_MOVE);

    switch(t)
    {
    case NORMAL :
        score = history[f][to];
        break;
    case PROMO_CAVALIER :
        score = (BONUS_PROMO + PROMO_CAVALIER);
        if(pto != VIDE)
            score += (64 * index_piece[pto] - index_piece[pfrom]);
        break;
    case PROMO_FOU :
        score = (BONUS_PROMO + PROMO_FOU);
        if(pto != VIDE)
            score += (64 * index_piece[pto] - index_piece[pfrom]);
        break;
    case PROMO_TOUR :
        score = (BONUS_PROMO + PROMO_TOUR);
        if(pto != VIDE)
            score += (64 * index_piece[pto] - index_piece[pfrom]);
        break;
    case PROMO_DAME :
        score = (BONUS_PROMO + PROMO_DAME);
        if(pto != VIDE)
            score += (64 * index_piece[pto] - index_piece[pfrom]);
        break;
    case ROQUE :
        score = BONUS_ROQUE;
        break;
    case EN_PASSANT :
        score = CAPTURE_EGALE;
        break;
    case CAPTURE :
        memset(see_tree, 0, sizeof(see_tree));
        memset(gsa, 0, sizeof(gsa));
        if(liste_valide)
            printf("coup   %d  %d  (%d)\n",f,to,val_see[pto]);
        coef_capture = (64 * index_piece[pto] - index_piece[pfrom]);
        make_capture(f, to, coul);
        coul = OPP(coul);
        //affiche_echiquier();
        see_score = val_see[pto] - see(to, coul);
        coul = OPP(coul);
        unmake_capture(f, to, coul);
        //affiche_echiquier();
        if(liste_valide)
            printf("      see = %d\n",see_score);
        if(see_score > 0)
            score = (CAPTURE_GAGNANTE + coef_capture + see_score);
        else if(see_score == 0)
            score = (CAPTURE_EGALE + coef_capture);
        else
            score = 0;
        break;
    default:
        break;
    }
    return score;
}


void copie_roque(int from, int dest, MOVE * pBuf, int *pMCount)
{
    copie_coup(from, dest, ROQUE, -1,pBuf, pMCount);
}

void copie_coup_normal(int from, int dest, MOVE * pBuf, int *pMCount)
{
    if(piece[dest] != VIDE)
    {
        copie_coup(from, dest, CAPTURE, -1,pBuf, pMCount);
    }
    else
    {
        copie_coup(from, dest, NORMAL, -1,pBuf, pMCount);
    }

}

/* PION can promote */
void copie_coup_PION(int from, int dest, MOVE * pBuf, int *pMCount)
{
    if (dest > 7 && dest < 56)
    {
        if(piece[dest] != VIDE)
        {
            copie_coup(from, dest, CAPTURE, -1,pBuf, pMCount);
        }
        else
        {
            copie_coup(from, dest, NORMAL, -1,pBuf, pMCount);
        }
    }

    else
    {
        copie_coup(from, dest, PROMO_DAME,-1, pBuf, pMCount);
        copie_coup(from, dest, PROMO_TOUR, -1, pBuf, pMCount);
        copie_coup(from, dest, PROMO_FOU, -1, pBuf, pMCount);
        copie_coup(from, dest, PROMO_CAVALIER, -1, pBuf, pMCount);
    }
}

void copie_dep_deux_cases(int from, int dest, int ep_sq, MOVE * pBuf, int *pMCount)
{
    copie_coup(from, dest, NORMAL, ep_sq,pBuf, pMCount);
}


void copie_en_passant(int from, int dest, MOVE * pBuf, int *pMCount)
{
    copie_coup(from, dest, EN_PASSANT, -1,pBuf, pMCount);
}

void copie_promo_dame(int from, int dest, MOVE * pBuf, int *pMCount)
{
    copie_coup(from, dest, PROMO_DAME, -1,pBuf, pMCount);
}

/* gen_coups all moves of current_side to move and push them to pBuf, return number of moves */
int gen_coups(int current_side, MOVE * pBuf)
{
    int i,k,y,row,col,movecount;
    int cp;

    movecount = 0;

    //-----------------------------------------------------------------------------------------------------------------
    //                                      generation roques
    if(current_side == BLANC)
    {
        if (castle & 1)
            copie_roque(E1, G1, pBuf, &movecount);
        if (castle & 2)
            copie_roque(E1, C1, pBuf, &movecount);
    }
    else
    {
        if (castle & 4)
            copie_roque(E8, G8, pBuf, &movecount);
        if (castle & 8)
            copie_roque(E8, C8, pBuf, &movecount);
    }

    for (cp=0; cp<16; ++cp)
    {
        i = plist[current_side][cp];
        if (i != -1)
        {
            switch (piece[i])
            {
            case PION:
                col = COL(i);
                row = ROW(i);
                if (current_side == NOIR)
                {
                    if (couleur[i + 8] == VIDE)
                        copie_coup_PION(i, i + 8, pBuf, &movecount);
                    if (row == 1 && couleur[i + 8] == VIDE && couleur[i + 16] == VIDE)
                        copie_dep_deux_cases(i, i + 16, i + 8, pBuf, &movecount);
                    if (col && couleur[i + 7] == BLANC)
                        copie_coup_PION(i, i + 7, pBuf, &movecount);
                    if (col < 7 && couleur[i + 9] == BLANC)
                        copie_coup_PION(i, i + 9, pBuf, &movecount);
                }
                else
                {
                    if (couleur[i - 8] == VIDE)
                        copie_coup_PION(i, i - 8, pBuf, &movecount);
                    if (row == 6 && couleur[i - 8] == VIDE && couleur[i - 16] == VIDE)
                        copie_dep_deux_cases(i, i - 16, i - 8, pBuf, &movecount);
                    if (col && couleur[i - 9] == NOIR)
                        copie_coup_PION(i, i - 9, pBuf, &movecount);
                    if (col < 7 && couleur[i - 7] == NOIR)
                        copie_coup_PION(i, i - 7, pBuf, &movecount);
                }
                break;

            case DAME:         /* == FOU+TOUR */
            case FOU:
                for (y = i - 9; y >= 0 && COL(y) != 7; y -= 9)          /* go left up */
                {
                    if (couleur[y] != current_side)
                        copie_coup_normal(i, y, pBuf, &movecount);
                    if (couleur[y] != VIDE)
                        break;

                }
                for (y = i - 7; y >= 0 && COL(y) != 0; y -= 7)          /* go right up */
                {
                    if (couleur[y] != current_side)
                        copie_coup_normal(i, y, pBuf, &movecount);
                    if (couleur[y] != VIDE)
                        break;

                }
                for (y = i + 9; y < 64 && COL(y) != 0; y += 9)          /* go right down */
                {
                    if (couleur[y] != current_side)
                        copie_coup_normal(i, y, pBuf, &movecount);
                    if (couleur[y] != VIDE)
                        break;

                }
                for (y = i + 7; y < 64 && COL(y) != 7; y += 7)          /* go right down */
                {
                    if (couleur[y] != current_side)
                        copie_coup_normal(i, y, pBuf, &movecount);
                    if (couleur[y] != VIDE)
                        break;

                }
                if (piece[i] == FOU)
                    break;

            /* FALL THROUGH FOR DAME {I meant to do that!} ;-) */
            case TOUR:
                col = COL(i);
                for (k = i - col, y = i - 1; y >= k; y--)    /* go left */
                {
                    if (couleur[y] != current_side)
                        copie_coup_normal(i, y, pBuf, &movecount);
                    if (couleur[y] != VIDE)
                        break;

                }
                for (k = i - col + 7, y = i + 1; y <= k; y++)        /* go right */
                {
                    if (couleur[y] != current_side)
                        copie_coup_normal(i, y, pBuf, &movecount);
                    if (couleur[y] != VIDE)
                        break;

                }
                for (y = i - 8; y >= 0; y -= 8)         /* go up */
                {
                    if (couleur[y] != current_side)
                        copie_coup_normal(i, y, pBuf, &movecount);
                    if (couleur[y] != VIDE)
                        break;

                }
                for (y = i + 8; y < 64; y += 8)         /* go down */
                {
                    if (couleur[y] != current_side)
                        copie_coup_normal(i, y, pBuf, &movecount);
                    if (couleur[y] != VIDE)
                        break;

                }
                break;

            case CAVALIER:
                col = COL(i);
                y = i - 6;
                if (y >= 0 && col < 6 && couleur[y] != current_side)
                    copie_coup_normal(i, y, pBuf, &movecount);
                y = i - 10;
                if (y >= 0 && col > 1 && couleur[y] != current_side)
                    copie_coup_normal(i, y, pBuf, &movecount);
                y = i - 15;
                if (y >= 0 && col < 7 && couleur[y] != current_side)
                    copie_coup_normal(i, y, pBuf, &movecount);
                y = i - 17;
                if (y >= 0 && col > 0 && couleur[y] != current_side)
                    copie_coup_normal(i, y, pBuf, &movecount);
                y = i + 6;
                if (y < 64 && col > 1 && couleur[y] != current_side)
                    copie_coup_normal(i, y, pBuf, &movecount);
                y = i + 10;
                if (y < 64 && col < 6 && couleur[y] != current_side)
                    copie_coup_normal(i, y, pBuf, &movecount);
                y = i + 15;
                if (y < 64 && col > 0 && couleur[y] != current_side)
                    copie_coup_normal(i, y, pBuf, &movecount);
                y = i + 17;
                if (y < 64 && col < 7 && couleur[y] != current_side)
                    copie_coup_normal(i, y, pBuf, &movecount);
                break;

            case ROI:
                col = COL(i);
                if (col && couleur[i - 1] != current_side)
                    copie_coup_normal(i, i - 1, pBuf, &movecount); /* left */
                if (col < 7 && couleur[i + 1] != current_side)
                    copie_coup_normal(i, i + 1, pBuf, &movecount); /* right */
                if (i > 7 && couleur[i - 8] != current_side)
                    copie_coup_normal(i, i - 8, pBuf, &movecount); /* up */
                if (i < 56 && couleur[i + 8] != current_side)
                    copie_coup_normal(i, i + 8, pBuf, &movecount); /* down */
                if (col && i > 7 && couleur[i - 9] != current_side)
                    copie_coup_normal(i, i - 9, pBuf, &movecount); /* left up */
                if (col < 7 && i > 7 && couleur[i - 7] != current_side)
                    copie_coup_normal(i, i - 7, pBuf, &movecount); /* right up */
                if (col && i < 56 && couleur[i + 7] != current_side)
                    copie_coup_normal(i, i + 7, pBuf, &movecount); /* left down */
                if (col < 7 && i < 56 && couleur[i + 9] != current_side)
                    copie_coup_normal(i, i + 9, pBuf, &movecount); /* right down */
                break;
            default:
                //printf("gen i piece couleur : %d  %d  %d \n",i,piece[i],couleur[i]);
                //puts("piece type unknown");
                //assert(FAUX);
                break;
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------
    //                                          en passant
    if(ep != -1)
    {
        if(current_side == BLANC)
        {
            if((COL(ep) != 0) && (couleur[ep + 7] == BLANC) && (piece[ep + 7] == PION))
                copie_en_passant(ep + 7, ep,  pBuf, &movecount);
            if((COL(ep) != 7) && (couleur[ep + 9] == BLANC) && (piece[ep + 9] == PION))
                copie_en_passant(ep + 9, ep,  pBuf, &movecount);
        }
        else
        {
            if((COL(ep) != 0) && (couleur[ep - 9] == NOIR) && (piece[ep - 9] == PION))
                copie_en_passant(ep - 9, ep,  pBuf, &movecount);
            if((COL(ep) != 7) && (couleur[ep - 7] == NOIR) && (piece[ep - 7] == PION))
                copie_en_passant(ep - 7, ep,  pBuf, &movecount);
        }
    }

    return movecount;
}



int gen_captures_promos(int current_side, MOVE * pBuf)
{
    int i,k,y,col,movecount;
    int cp;
    movecount = 0;
    int xside = OPP(side);

    for(cp=0; cp<16; ++cp)
    {
        i = plist[current_side][cp];
        if (i != -1)
        {
            switch (piece[i])
            {
            case PION:
                col = COL(i);
                if (current_side == NOIR)
                {
                    if ((col && couleur[i + 8] == VIDE) && (i>=48))
                        copie_promo_dame(i, i + 8, pBuf, &movecount);
                    if (col && couleur[i + 7] == BLANC)
                        copie_coup_PION(i, i + 7, pBuf, &movecount);
                    if (col < 7 && couleur[i + 9] == BLANC)
                        copie_coup_PION(i, i + 9, pBuf, &movecount);
                }
                else
                {
                    if ((col && couleur[i - 8] == VIDE) && (i<=15))
                        copie_promo_dame(i, i - 8, pBuf, &movecount);
                    if (col && couleur[i - 9] == NOIR)
                        copie_coup_PION(i, i - 9, pBuf, &movecount);
                    if (col < 7 && couleur[i - 7] == NOIR)
                        copie_coup_PION(i, i - 7, pBuf, &movecount);
                }
                break;

            case DAME:         /* == FOU+TOUR */
            case FOU:
                for (y = i - 9; y >= 0 && COL(y) != 7; y -= 9)          /* go left up */
                {
                    if (couleur[y] != VIDE)
                    {
                        if(couleur[y] == xside)
                        {
                            copie_coup_normal(i, y, pBuf, &movecount);
                        }
                        break;
                    }
                }
                for (y = i - 7; y >= 0 && COL(y) != 0; y -= 7)          /* go right up */
                {
                    if (couleur[y] != VIDE)
                    {
                        if(couleur[y] == xside)
                        {
                            copie_coup_normal(i, y, pBuf, &movecount);
                        }
                        break;
                    }
                }
                for (y = i + 9; y < 64 && COL(y) != 0; y += 9)          /* go right down */
                {
                    if (couleur[y] != VIDE)
                    {
                        if(couleur[y] == xside)
                        {
                            copie_coup_normal(i, y, pBuf, &movecount);
                        }
                        break;
                    }
                }
                for (y = i + 7; y < 64 && COL(y) != 7; y += 7)          /* go right down */
                {
                    if (couleur[y] != VIDE)
                    {
                        if(couleur[y] == xside)
                        {
                            copie_coup_normal(i, y, pBuf, &movecount);
                        }
                        break;
                    }
                }
                if (piece[i] == FOU)
                    break;

            /* FALL THROUGH FOR DAME {I meant to do that!} ;-) */
            case TOUR:
                col = COL(i);
                for (k = i - col, y = i - 1; y >= k; y--)    /* go left */
                {
                    if (couleur[y] != VIDE)
                    {
                        if(couleur[y] == xside)
                        {
                            copie_coup_normal(i, y, pBuf, &movecount);
                        }
                        break;
                    }
                }
                for (k = i - col + 7, y = i + 1; y <= k; y++)        /* go right */
                {
                    if (couleur[y] != VIDE)
                    {
                        if(couleur[y] == xside)
                        {
                            copie_coup_normal(i, y, pBuf, &movecount);
                        }
                        break;
                    }
                }
                for (y = i - 8; y >= 0; y -= 8)         /* go up */
                {
                    if (couleur[y] != VIDE)
                    {
                        if(couleur[y] == xside)
                        {
                            copie_coup_normal(i, y, pBuf, &movecount);
                        }
                        break;
                    }
                }
                for (y = i + 8; y < 64; y += 8)         /* go down */
                {
                    if (couleur[y] != VIDE)
                    {
                        if(couleur[y] == xside)
                        {
                            copie_coup_normal(i, y, pBuf, &movecount);
                        }
                        break;
                    }
                }
                break;

            case CAVALIER:
                col = COL(i);
                y = i - 6;
                if (y >= 0 && col < 6 && couleur[y] == xside)
                    copie_coup_normal(i, y, pBuf, &movecount);
                y = i - 10;
                if (y >= 0 && col > 1 && couleur[y] == xside)
                    copie_coup_normal(i, y, pBuf, &movecount);
                y = i - 15;
                if (y >= 0 && col < 7 && couleur[y] == xside)
                    copie_coup_normal(i, y, pBuf, &movecount);
                y = i - 17;
                if (y >= 0 && col > 0 && couleur[y] == xside)
                    copie_coup_normal(i, y, pBuf, &movecount);
                y = i + 6;
                if (y < 64 && col > 1 && couleur[y] == xside)
                    copie_coup_normal(i, y, pBuf, &movecount);
                y = i + 10;
                if (y < 64 && col < 6 && couleur[y] == xside)
                    copie_coup_normal(i, y, pBuf, &movecount);
                y = i + 15;
                if (y < 64 && col > 0 && couleur[y] == xside)
                    copie_coup_normal(i, y, pBuf, &movecount);
                y = i + 17;
                if (y < 64 && col < 7 && couleur[y] == xside)
                    copie_coup_normal(i, y, pBuf, &movecount);
                break;

            case ROI:
                col = COL(i);
                if (col && couleur[i - 1] == xside)
                    copie_coup_normal(i, i - 1, pBuf, &movecount); /* left */
                if (col < 7 && couleur[i + 1] == xside)
                    copie_coup_normal(i, i + 1, pBuf, &movecount); /* right */
                if (i > 7 && couleur[i - 8] == xside)
                    copie_coup_normal(i, i - 8, pBuf, &movecount); /* up */
                if (i < 56 && couleur[i + 8] == xside)
                    copie_coup_normal(i, i + 8, pBuf, &movecount); /* down */
                if (col && i > 7 && couleur[i - 9] == xside)
                    copie_coup_normal(i, i - 9, pBuf, &movecount); /* left up */
                if (col < 7 && i > 7 && couleur[i - 7] == xside)
                    copie_coup_normal(i, i - 7, pBuf, &movecount); /* right up */
                if (col && i < 56 && couleur[i + 7] == xside)
                    copie_coup_normal(i, i + 7, pBuf, &movecount); /* left down */
                if (col < 7 && i < 56 && couleur[i + 9] == xside)
                    copie_coup_normal(i, i + 9, pBuf, &movecount); /* right down */
                break;
            default:
                //printf("gen i piece couleur : %d  %d  %d \n",i,piece[i],couleur[i]);
                //puts("piece type unknown");
                //assert(FAUX);
                break;
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------
    //                                          en passant
    if(ep != -1)
    {
        if(current_side == BLANC)
        {
            if((COL(ep) != 0) && (couleur[ep + 7] == BLANC) && (piece[ep + 7] == PION))
                copie_en_passant(ep + 7, ep,  pBuf, &movecount);
            if((COL(ep) != 7) && (couleur[ep + 9] == BLANC) && (piece[ep + 9] == PION))
                copie_en_passant(ep + 9, ep,  pBuf, &movecount);
        }
        else
        {
            if((COL(ep) != 0) && (couleur[ep - 9] == NOIR) && (piece[ep - 9] == PION))
                copie_en_passant(ep - 9, ep,  pBuf, &movecount);
            if((COL(ep) != 7) && (couleur[ep - 7] == NOIR) && (piece[ep - 7] == PION))
                copie_en_passant(ep - 7, ep,  pBuf, &movecount);
        }
    }

    return movecount;
}


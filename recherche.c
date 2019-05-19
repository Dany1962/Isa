#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "board.h"
#include "recherche.h"
#include "hash.h"
#include "temps.h"
#include "bitboards.h"
#include "deftypes.h"
#include "eval.h"
#include "genecoups.h"
#include "interface.h"
#include "see.h"

/*
---------------------------------------------------------
*														*
*                   ALPHA BETA  + PVS                   *
*														*
---------------------------------------------------------
*/
int pvs(int alpha, int beta, int depth, MOVE * pBestMove, bool nulmove)
{
    int i,j,k;
    int value;
    int havemove;
    int movecnt;
    int hashf = hashfALPHA;
    int rep;
    bool echec = FAUX;
    bool echec2 = FAUX;
    bool ext = FAUX;
    MOVE moveBuf[200];
    MOVE tmpMove;
    int margin[4] = {0, 125, 325, 525};
    //int margin[6] = {0, 125, 275, 325, 525, 995};
    int score;
    int red;

    //controle temps depasse ou non (jeu au temps)
    fin_recherche = controle_si_temps_depasse();
    if(fin_recherche)
        return 0;

    //longueur pv
    long_pv[prof] = prof;

    //profondeur limite ateinte : retourne eval()
    if(prof >= MAXPLY-1)
        return eval();

    //nulle règle des 50 coups? ?
    if(cinquante == 100)
        return 0;

    //nulle règle des triples répétitions ?
    rep = triple_repetition();
    if(prof && rep)
        return 0;

    //extension d'une profondeur si la couleur en cours est en échec
    echec = roi_attaque(pos_roi[side], side);
    if(echec)
        depth++;

    //on ateint la profondeur en cours , quiescence
    if(depth <= 0)
    {
        value =  quiesce(alpha, beta, &tmpMove, depth-1);
        return value;
    }

    //position dans la table de hashage ?
    if(prof)
    {
        value = probe_hash(depth, alpha, beta);
        if(value != valINCONNUE)
        {
            return value;
        }
    }

    //génération des coups pseudo-légaux
    PHASE = calcul_phase();
    movecnt = gen_coups(side, moveBuf);
    if(follow_pv)
        tri_pv(moveBuf, movecnt);

    //FUTILITY PRUNING
    //conditions : depth >0 et < 4 , !echec , !pv
    if((!echec) && (!follow_pv) && (depth < 4))
    {
        score = eval();
        if(score <= alpha-margin[depth])
            return (quiesce(alpha, beta, &tmpMove, depth-1));
        if(score >= beta+margin[depth])
            return beta;
    }

    //NULLMOVE
    //conditions :    pas de finale de pion
    //pas en échec
    //profondeur >=2
    if((!echec) && (ok_pour_nul_move()) && (depth >= 2) && (nulmove) && (!follow_pv))
    {
        jouer_coup_nul();
        value = -pvs(-beta, -(beta-1), depth - NULL_DEPTH - 1, &tmpMove, NO_NULL);
        dejouer_coup_nul();
        if (value >= beta)
        {
            return beta;
        }
    }

    //init du pointeur et flag "au moins un coup jouable"
    havemove = 0;
    pBestMove->type = COUP_VIDE;

    //boucle coups normaux
    for (i = 0; i < movecnt; ++i)
    {
        ext = FAUX;
        meilleur_coup_suivant(moveBuf, movecnt, i);

        if (jouer_coup(moveBuf[i]))
            continue;

        //si le coup donne échec
        echec2 = roi_attaque(pos_roi[side], side);

        //compteur de coups légaux
        havemove++;

        //compteurs de positions
        nodes++;

        //value = -pvs(-beta, -alpha, depth - 1, &tmpMove, OK_NULL);

        //premier coup de la liste , full search
        if(havemove == 1)
            value = -pvs(-beta, -alpha, depth - 1, &tmpMove, OK_NULL);
        else
        {
            //lmr possible?
            if((depth >= START_PROF) && (!echec) && (!echec2) &&
                    (!follow_pv) && (moveBuf[i].type == NORMAL))
            {
                if(havemove > 5)
                    value = -pvs(-(alpha+1), -alpha, (depth - 2), &tmpMove, OK_NULL);
                else
                    value = -pvs(-(alpha+1), -alpha, (depth - 1), &tmpMove, OK_NULL);
            }
            else
                value = alpha + 1;
            //research
            if(value > alpha)
            {
                value = -pvs(-(alpha+1), -alpha, depth - 1, &tmpMove, NO_NULL);
                if(value > alpha && value < beta)
                {
                    value = -pvs(-beta, -alpha, depth - 1, &tmpMove, OK_NULL);
                }
            }
        }
        dejouer_coup();

        if (value > alpha)
        {
            *pBestMove = moveBuf[i];
            if (value >= beta)     //" cutoff "
            {
                save_hash(depth, beta, hashfBETA, pBestMove);

                if(moveBuf[i].type == NORMAL)
                {
                    //update killers
                    if(moveBuf[i].killer_code != coup[prof].killer_a)
                    {
                        coup[prof].killer_b = coup[prof].killer_a;
                        coup[prof].killer_a = moveBuf[i].killer_code;
                    }

                    //update history
                    history[moveBuf[i].from][moveBuf[i].dest] += (depth * depth);
                    if(history[moveBuf[i].from][moveBuf[i].dest] >= 20000)
                        history[moveBuf[i].from][moveBuf[i].dest] /= 2;
                    for(k=0; k<i; ++k)
                    {
                        if(moveBuf[k].type == NORMAL)
                        {
                            history[moveBuf[k].from][moveBuf[k].dest] -= depth;
                            if(history[moveBuf[k].from][moveBuf[k].dest] < -20000)
                                history[moveBuf[k].from][moveBuf[k].dest] /= 2;
                        }
                    }
                }
                return beta;
            }
            hashf = hashfEXACT;
            alpha = value;

            //mise a jour pv
            pv[prof][prof] = *pBestMove;
            for (j = prof + 1; j < long_pv[prof + 1]; ++j)
            {
                pv[prof][j] = pv[prof + 1][j];
            }
            long_pv[prof] = long_pv[prof + 1];
        }
    }

    //si aucun coups , situation de MAT ou de PAT
    if (!havemove)
    {
        if(roi_attaque(pos_roi[side], side))
        {
            save_hash(depth, (-MATE + prof), hashf, pBestMove);
            return -MATE + prof;
        }

        else
        {
            save_hash(depth, 0, hashf, pBestMove);
            return 0;
        }
    }
    save_hash(depth, alpha, hashf, pBestMove);
    return alpha;
}

void tri_pv(MOVE *ptab, int ctr)
{
    int i;

    follow_pv = FAUX;
    for(i = 0; i < ctr; ++i)
    {
        if ((ptab[i].from == pv[0][prof].from) && (ptab[i].dest == pv[0][prof].dest))
        {
            follow_pv = VRAI;
            ptab[i].evaluation += 20000000;
            return;
        }
    }
}


bool ok_pour_nul_move()
{
    int p;
    p = calcul_phase();
    if(p < 16)
    {
        return VRAI;
    }
    return FAUX;
}


void jouer_coup_nul()
{
    //printf("jouer coup nul \n");

    hist[hdp].castle = castle;
    hist[hdp].ep = ep;
    hist[hdp].cinquante = cinquante;
    hist[hdp].hash_code = hash_code_position;
    hist[hdp].bitboard_blanc = bitboard_pion[BLANC];
    hist[hdp].bitboard_noir = bitboard_pion[NOIR];

    prof++;
    hdp++;

    hash_code_position ^=  val_couleur[side];
    side = (BLANC + NOIR) - side;
    hash_code_position ^=  val_couleur[side];

    if(ep != -1)
    {
        hash_code_position ^= val_en_passant[ep];
    }
    ep = -1;
    cinquante++;
}


void dejouer_coup_nul()
{
    //printf("dejouer coup nul \n");
    side = (BLANC + NOIR) - side;
    hdp--;
    prof--;

    castle = hist[hdp].castle;
    ep = hist[hdp].ep;
    cinquante = hist[hdp].cinquante;
    hash_code_position = hist[hdp].hash_code;
    bitboard_pion[BLANC] = hist[hdp].bitboard_blanc;
    bitboard_pion[NOIR] = hist[hdp].bitboard_noir;
}

int quiesce(int alpha, int beta, MOVE * pBestMove, int depth)
{
    int i,j;
    int val;
    int movecnt;
    MOVE moveBuf[200],tmpMove;
    int see_score;
    int score;

    //controle temps depasse ou non
    fin_recherche = controle_si_temps_depasse();
    if(fin_recherche)
        return 0;

    //evaluation de la position en cours
    score = eval();

    //longueur pv
    long_pv[prof] = prof;
    pBestMove->type = COUP_VIDE;

    //profondeur limite ateinte : retourne eval()
    if(prof >= MAXPLY-1)
        return score;

    //stand pat ?
    if(score >= beta)
        return beta;
    if(alpha < score)
        alpha = score;

    //limite qs ateinte
    if(depth < QS_LIMITE)
        return score;


    //generation captures + promos dame
    movecnt = gen_captures_promos(side, moveBuf);

    for (i = 0; i < movecnt; ++i)
    {
        meilleur_coup_suivant(moveBuf, movecnt, i);

        //SEE prunning
        if(moveBuf[i].piece_from > moveBuf[i].piece_dest)
        {
            if(moveBuf[i].evaluation == 0)
                continue;
        }

        if (jouer_coup(moveBuf[i]))
            continue;

        //compteur de positions
        nodes++;

        //compteur de qs positions
        q_nodes++;

        val = -quiesce(-beta, -alpha, &tmpMove, depth - 1);

        dejouer_coup();

        if (val > alpha)
        {
            if (val >= beta)
                return beta;
            alpha = val;
            *pBestMove = moveBuf[i];

            //mise a jour pv
            pv[prof][prof] = *pBestMove;
            for (j = prof + 1; j < long_pv[prof + 1]; ++j)
            {
                pv[prof][j] = pv[prof + 1][j];
            }
            long_pv[prof] = long_pv[prof + 1];
        }
    }
    return alpha;
}

void meilleur_coup_suivant(MOVE *ptable, int nb_coups, int debut)
{
    int ms;
    int mi;
    int i;
    MOVE temp;

    ms = -MATE;
    for(i = debut; i < nb_coups; ++i)
    {
        if(ptable[i].evaluation > ms)
        {
            mi = i;
            ms = ptable[i].evaluation;
        }
    }

    temp = ptable[debut];
    ptable[debut] = ptable[mi];
    ptable[mi] = temp;
}


int test_see(MOVE *ptab, int ctr)
{
    int f = ptab[ctr].from;
    int to = ptab[ctr].dest;
    int coul = couleur[f];
    int score = 0;
    int pto = ptab[ctr].piece_dest;

    memset(see_tree, 0, sizeof(see_tree));
    memset(gsa, 0, sizeof(gsa));
    make_capture(f, to, coul);
    coul = OPP(coul);
    //affiche_echiquier();
    score = val_see[pto] - see(to, coul);
    coul = OPP(coul);
    unmake_capture(f, to, coul);

    return score;
}




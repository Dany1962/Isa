#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "board.h"
#include "recherche.h"
#include "hash.h"
#include "temps.h"
#include "book.h"
#include "eval.h"
#include "bitboards.h"
#include "progjoue.h"
#include "genecoups.h"
#include "interface.h"

MOVE programme_joue(int max_depth, int output)
{
    MOVE m;
    MOVE tmp;
    MOVE moveBuf[200];
    int movecnt;
    int score;
    int i,j;
    int temps_intermediaire;
    int ply;

    memset(tt, 0, sizeof(tt));
    memset(history, 0, sizeof(history));
    memset(eval_table, 0, sizeof(eval_table));
    memset(eval_struct_pions, 0, sizeof(eval_struct_pions));
    memset(pv, 0, sizeof(pv));
    for(ply=0;ply<MAXPLY+1;++ply)
    {
        coup[ply].killer_a = 0;
        coup[ply].killer_b = 0;
    }

    movecnt = gen_coups(side, moveBuf);

    //----------------------------------------------------------------------
    //  recherche d'un coup dans la bibliothèque d'ouvertures (8 1/2 coups)
    //----------------------------------------------------------------------
    if(hdp <= 20)
    {
        //printf("recherche dans biblio\n");
        int b = biblio();
        //printf("cd et ca : %d  %d\n",cd,ca);
        if(b)
        {
            coup_biblio = VRAI;
            for (i = 0; i < movecnt; ++i)
            {
                if((moveBuf[i].from == cd) && (moveBuf[i].dest == ca))
                {

                    if(jouer_coup(moveBuf[i]))
                    {
                        dejouer_coup();
                        break;
                    }
                    else
                    {
                         dejouer_coup();
                         tmp = moveBuf[i];
                         return tmp;
                    }
                }
            }
        }
        coup_biblio = FAUX;
    }

    printf("Prof Score   Noeuds   Temps   PV\n");
    printf("-----------------------------------------------\n");

    //--------------------------------------------------------------------------------
    //                  calcul du temps imparti pour jouer son coup
    //--------------------------------------------------------------------------------
    debut = get_ms();
    fin = debut + time_left;

    fin_recherche = FAUX;

    printf("temps max : %d\n",time_left);

    positions_testees = 0;
    positions_trouvees = 0;
    positions_sauvees = 0;

    for(i=1;i<=max_depth;++i)
    {
        prof = 0;
        nodes = 0;
        q_nodes = 0;

        follow_pv = VRAI;
        score = pvs(-MATE, MATE, i, &m, 0);

        temps_intermediaire = get_ms();
        if(fin_recherche)
            break;

        tmp = m;

        if(output == 1) // mode console
        {
            //printf("%d  %d \n",temps_intermediaire,debut);
            float val1 = (float)score/100;
            float val2 = ((float)(temps_intermediaire - debut)/1000);
            printf("%3d  %-+2.2f  %2.2f  %15d  (%8d)",
                   i,
                   val1,
                   val2,
                   nodes,q_nodes);
            for (j = 0; j < long_pv[0]; ++j)
				printf(" %s", move_str(pv[0][j]));
			printf("\n");
        }
        else if(output == 2)        //mode GUI  (exemple : 9 ply, score=1.56, time = 10.84 seconds, nodes=48000, PV = "Nf3 Nc6 Nc3 Nf6" )
        {
            printf("%d %d %d %9d  ",i,score, (temps_intermediaire-debut)/1000, nodes);
            for (j = 0; j < long_pv[0]; ++j)
				printf(" %s", move_str(pv[0][j]));
			printf("\n");
			fflush(stdout);
        }

        if((score > 90000) || (score < -90000))
            break;
    }
    printf("-----------------------------------------------\n");
    fin = get_ms();
    return tmp;
}

//-------------------------------------------------------------------------------------
//                                    JOUER COUP
//-------------------------------------------------------------------------------------
int jouer_coup(MOVE m)
{
    int            echec;
    int xside = OPP(side);
    int dep = -1,arr = -1;
    int from = m.from,dest = m.dest;
    int pfrom = m.piece_from,pdest = m.piece_dest;
    int type = m.type;
    int tmp;

    //sauvegardes
    hist[hdp].m = m;
    hist[hdp].cap = piece[m.dest];
    hist[hdp].m.index_cap = ((piece[m.dest]) ? indices[m.dest] : VIDE);
    hist[hdp].castle = castle;
    hist[hdp].ep = ep;
    hist[hdp].cinquante = cinquante;
    hist[hdp].hash_code = hash_code_position;
    hist[hdp].hash_pions = hash_code_pions;
    hist[hdp].bitboard_blanc = bitboard_pion[BLANC];
    hist[hdp].bitboard_noir = bitboard_pion[NOIR];

    //déplacement d'un pion ? updates
    if(pfrom == PION)
    {
        //update bitboard
        bitboard_pion[side] ^= pion_sur_case[from];
        bitboard_pion[side] ^= pion_sur_case[dest];
        //update hashcode pions
        hash_code_pions ^= hash_table_pions[side][from];
        hash_code_pions ^= hash_table_pions[side][dest];
    }

    //roques?
    if(type == ROQUE)
    {
        //printf("oui roque\n");
        if(roi_attaque(pos_roi[side], side))
        {
            //printf("impossible , roi attaqué\n");
            return VRAI;
        }
        switch (m.dest)
        {
        case G1:
            if (couleur[F1] != VIDE || couleur[G1] != VIDE ||
                    roi_attaque(F1, BLANC) || roi_attaque(G1, BLANC))
            {
                //printf("roque impossible : %d  %d  %d   %d\n",couleur[F1],couleur[G1],roi_attaque(F1, BLANC),
                //roi_attaque(G1, BLANC));
                return VRAI;
            }
            dep = H1;
            arr = F1;
            break;
        case    C1:
            if (couleur[B1] != VIDE || couleur[C1] != VIDE || couleur[D1] != VIDE ||
                    roi_attaque(C1, BLANC) || roi_attaque(D1, BLANC))
            {
                return VRAI;
            }
            dep = A1;
            arr = D1;
            break;
        case G8:
            if (couleur[F8] != VIDE || couleur[G8] != VIDE ||
                    roi_attaque(F8, NOIR) || roi_attaque(G8, NOIR))
            {
                return VRAI;
            }
            dep = H8;
            arr = F8;
            break;
        case C8:
            if (couleur[B8] != VIDE || couleur[C8] != VIDE || couleur[D8] != VIDE ||
                    roi_attaque(C8, NOIR) || roi_attaque(D8, NOIR))
            {
                return VRAI;
            }
            dep = A8;
            arr = D8;
            break;
        default:
            dep = -1;
            arr = -1;
            break;
        }
        //déplacer la tour
		hash_code_position ^= hash_table[side][TOUR][dep];
        piece[dep] = VIDE;
        couleur[dep] = VIDE;
        hash_code_position ^= hash_table[side][TOUR][arr];
        piece[arr] = TOUR;
        couleur[arr] = side;
        //update plist tour roque
        tmp = indices[dep];
        indices[arr] = tmp;
        indices[dep] = VIDE;
        plist[side][tmp] = arr;
    }

    //update plist
    if(pdest != VIDE)
        plist[xside][indices[dest]] = VIDE;
    tmp = indices[from];
    indices[dest] = tmp;
    indices[from] = VIDE;
    plist[side][tmp] = dest;


    //déplacer la piece jouee ,vider la case départ
    hash_code_position ^= hash_table[side][pfrom][from];
    piece[from] = VIDE;
    couleur[from] = VIDE;

    //case arrivée piece adverse ?
    if(pdest != VIDE)
    {
		hash_code_position ^= hash_table[xside][pdest][dest];
        materiel[xside] -= valeur_piece[pdest];
        nb_pieces[xside][pdest]--;

        //pion capturé? updates
        if(pdest == PION)
        {
            //update bitboards
            bitboard_pion[xside] ^= pion_sur_case[dest];
            //update hashcode pions
            hash_code_pions ^= hash_table_pions[xside][dest];
        }
    }

    //-------------------------------------------------------------
    //                       promo?
    //-------------------------------------------------------------
    if(PROMO(type))
    {
        hash_code_position ^= hash_table[side][type][dest];
        piece[dest] = type;
        couleur[dest] = side;
        materiel[side] += (valeur_piece[type] - VAL_PION);
        nb_pieces[side][PION]--;
        nb_pieces[side][type]++;
        hash_code_pions ^= hash_table_pions[side][dest];
    }

    //déplacement de la pièce jouée
    else
    {
        hash_code_position ^= hash_table[side][pfrom][dest];
        piece[dest] = pfrom;
        couleur[dest] = side;
    }

    //prise en passant?
    if(type == EN_PASSANT)
    {
        materiel[xside] -= VAL_PION;
        nb_pieces[xside][PION]--;
        if(side == BLANC)
        {
            hash_code_pions ^= hash_table_pions[NOIR][(dest+8)];
            piece[(dest + 8)] = VIDE;
            couleur[(dest + 8)] = VIDE;
            bitboard_pion[NOIR] ^= pion_sur_case[(dest + 8)];
            hash_code_position ^= hash_table[xside][PION][(dest + 8)];
            hist[hdp].m.index_cap = indices[(dest + 8)];
            plist[xside][indices[(dest + 8)]] = VIDE;
            indices[(dest + 8)] = VIDE;
        }
        else
        {
            hash_code_pions ^= hash_table_pions[BLANC][(dest-8)];
            piece[(dest - 8)] = VIDE;
            couleur[(dest - 8)] = VIDE;
            bitboard_pion[BLANC] ^= pion_sur_case[(dest - 8)];
            hash_code_position ^= hash_table[xside][PION][(dest - 8)];
            hist[hdp].m.index_cap = indices[(dest - 8)];
            plist[xside][indices[(dest - 8)]] = VIDE;
            indices[(dest - 8)] = VIDE;
        }
    }

    //déplacement du roi ? on change sa position
    if(piece[dest] == ROI)
        pos_roi[side] = dest;

    //update profondeur et nbre de coups
    prof++;
    hdp++;

    //mise a jour de la case de prise en passant
    if(ep != -1)
        hash_code_position ^= val_en_passant[ep];
    ep = m.ep_flag;
    if(ep != -1)
        hash_code_position ^= val_en_passant[ep];

    //mise a jour de cinquante (pour la règle des 50 coups
    //sans coups de pions ou prise de piece
    if((piece[m.dest] == PION) || (m.type == CAPTURE) || (m.type == EN_PASSANT))
        cinquante = 0;
    else
        ++cinquante;

    //mise a jour pour droits aux roques
    hash_code_position ^= droit_aux_roques[castle];
    castle &= castle_mask[(int)m.from] & castle_mask[(int)m.dest];
    hash_code_position ^= droit_aux_roques[castle];

    echec = roi_attaque(pos_roi[side], side);
    hash_code_position ^=  val_couleur[side];
    side = (BLANC + NOIR) - side;
    hash_code_position ^=  val_couleur[side];

    if(echec)
    {
        dejouer_coup();
        return VRAI;
    }

    return FAUX;
}

//DEJOUER COUP
void dejouer_coup()
{
    side = (BLANC + NOIR) - side;
    int xside = OPP(side);
    int dep = -1, arr = -1;
    int from,dest,index,pfrom,pdest,type;

    hdp--;
    prof--;

    from  = hist[hdp].m.from;
    dest  = hist[hdp].m.dest;
    index = hist[hdp].m.index_cap;
    pfrom = hist[hdp].m.piece_from;
    pdest = hist[hdp].m.piece_dest;
    type  = hist[hdp].m.type;

    //vider la case arrivee
    piece[dest] = VIDE;
    couleur[dest] = VIDE;

    //roque ?
    if (type == ROQUE)
    {
        switch(dest)
        {
        case G1:
            dep = F1;
            arr = H1;
            break;
        case C1:
            dep = D1;
            arr = A1;
            break;
        case G8:
            dep = F8;
            arr = H8;
            break;
        case C8:
            dep = D8;
            arr = A8;
            break;
        default:  /* shouldn't get here */
            dep = -1;
            arr = -1;
            break;
        }

        //replacer la tour
        piece[dep] = VIDE;
        couleur[dep] = VIDE;
        piece[arr] = TOUR;
        couleur[arr] = side;
        //update plist
        indices[arr] = indices[dep];
        indices[dep] = VIDE;
        plist[side][indices[arr]] = arr;
    }

    //update plist
    indices[from] = indices[dest];
    indices[dest] = VIDE;
    plist[side][indices[from]] = from;
    if(pdest != VIDE)
    {
        indices[dest] = index;
        plist[xside][index] = dest;
    }

    //promo?
    if(PROMO(type))
    {
        piece[from] = PION;
        couleur[from] = side;
        materiel[side] -= (valeur_piece[type] - VAL_PION);
        nb_pieces[side][PION]++;
        nb_pieces[side][type]--;
    }
    //replacement de la pièce jouée
    else
    {
        piece[from] = pfrom;
        couleur[from] = side;
    }

    //capture?
    if(pdest > VIDE)
    {
        piece[dest] = pdest;
        couleur[dest] = xside;
        materiel[xside] += valeur_piece[pdest];
        nb_pieces[xside][pdest]++;
    }

    //prise en passant?
    if(type == EN_PASSANT)
    {
        materiel[xside] += VAL_PION;
        nb_pieces[xside][PION]++;
        if(side == BLANC)
        {
            piece[(dest + 8)] = PION;
            couleur[(dest + 8)] = xside;
            indices[(dest + 8)] = index;
            plist[xside][index] = (dest + 8);
        }
        else
        {
            piece[(dest - 8)] = PION;
            couleur[(dest - 8)] = xside;
            indices[(dest - 8)] = index;
            plist[xside][index] = (dest - 8);
        }
    }

    //BACKUPS
    castle = hist[hdp].castle;
    ep = hist[hdp].ep;
    cinquante = hist[hdp].cinquante;
    hash_code_position = hist[hdp].hash_code;
    bitboard_pion[BLANC] = hist[hdp].bitboard_blanc;
    bitboard_pion[NOIR] = hist[hdp].bitboard_noir;
    hash_code_pions = hist[hdp].hash_pions;

    //déplacement du roi ? on change sa position
    if(piece[hist[hdp].m.from] == ROI)
        pos_roi[side] = hist[hdp].m.from;
}

//************************************************************************************************************************

int roi_attaque(int sq, int current_side)
{
    int             k,
                    h,
                    y,
                    row,
                    col,
                    xside;
    xside = (BLANC + NOIR) - current_side;     /* opposite current_side, who may be checROI */
    //-----------------------------------------------------------------------------------------------------------------
    //                          on récupère la case dont on cherche les attaques
    k = sq;

    row = ROW(k), col = COL(k);
    //-----------------------------------------------------------------------------------------------------------------
    //                                  coups d'échecs cavalier
    if (col > 0 && row > 1 && couleur[k - 17] == xside && piece[k - 17] == CAVALIER)
        return 1;
    if (col < 7 && row > 1 && couleur[k - 15] == xside && piece[k - 15] == CAVALIER)
        return 1;
    if (col > 1 && row > 0 && couleur[k - 10] == xside && piece[k - 10] == CAVALIER)
        return 1;
    if (col < 6 && row > 0 && couleur[k - 6] == xside && piece[k - 6] == CAVALIER)
        return 1;
    if (col > 1 && row < 7 && couleur[k + 6] == xside && piece[k + 6] == CAVALIER)
        return 1;
    if (col < 6 && row < 7 && couleur[k + 10] == xside && piece[k + 10] == CAVALIER)
        return 1;
    if (col > 0 && row < 6 && couleur[k + 15] == xside && piece[k + 15] == CAVALIER)
        return 1;
    if (col < 7 && row < 6 && couleur[k + 17] == xside && piece[k + 17] == CAVALIER)
        return 1;
    /* Check horizontal and vertical lines for attacROI of DAME, TOUR, ROI */
    /* go down */
    y = k + 8;
    if (y < 64)
    {
        if (couleur[y] == xside && (piece[y] == ROI || piece[y] == DAME || piece[y] == TOUR))
            return 1;
        if (piece[y] == VIDE)
        {
            for (y += 8; y < 64; y += 8)
            {
                if (couleur[y] == xside && (piece[y] == DAME || piece[y] == TOUR))
                    return 1;
                if (piece[y] != VIDE)
                    break;
            }
        }
    }
    /* go left */
    y = k - 1;
    h = k - col;
    if (y >= h)
    {
        if (couleur[y] == xside && (piece[y] == ROI || piece[y] == DAME || piece[y] == TOUR))
            return 1;
        if (piece[y] == VIDE)
        {
            for (y--; y >= h; y--)
            {
                if (couleur[y] == xside && (piece[y] == DAME || piece[y] == TOUR))
                    return 1;
                if (piece[y] != VIDE)
                    break;
            }
        }
    }
    /* go right */
    y = k + 1;
    h = k - col + 7;
    if (y <= h)
    {
        if (couleur[y] == xside && (piece[y] == ROI || piece[y] == DAME || piece[y] == TOUR))
            return 1;
        if (piece[y] == VIDE)
        {
            for (y++; y <= h; y++)
            {
                if (couleur[y] == xside && (piece[y] == DAME || piece[y] == TOUR))
                    return 1;
                if (piece[y] != VIDE)
                    break;
            }
        }
    }
    /* go up */
    y = k - 8;
    if (y >= 0)
    {
        if (couleur[y] == xside && (piece[y] == ROI || piece[y] == DAME || piece[y] == TOUR))
            return 1;
        if (piece[y] == VIDE)
        {
            for (y -= 8; y >= 0; y -= 8)
            {
                if (couleur[y] == xside && (piece[y] == DAME || piece[y] == TOUR))
                    return 1;
                if (piece[y] != VIDE)
                    break;
            }
        }
    }
    /* Check diagonal lines for attacROI of DAME, FOU, ROI, PION */
    /* go right down */
    y = k + 9;
    if (y < 64 && COL(y) != 0)
    {
        if (couleur[y] == xside)
        {
            if (piece[y] == ROI || piece[y] == DAME || piece[y] == FOU)
                return 1;
            if (current_side == NOIR && piece[y] == PION)
                return 1;
        }
        if (piece[y] == VIDE)
        {
            for (y += 9; y < 64 && COL(y) != 0; y += 9)
            {
                if (couleur[y] == xside && (piece[y] == DAME || piece[y] == FOU))
                    return 1;
                if (piece[y] != VIDE)
                    break;
            }
        }
    }
    /* go left down */
    y = k + 7;
    if (y < 64 && COL(y) != 7)
    {
        if (couleur[y] == xside)
        {
            if (piece[y] == ROI || piece[y] == DAME || piece[y] == FOU)
                return 1;
            if (current_side == NOIR && piece[y] == PION)
                return 1;
        }
        if (piece[y] == VIDE)
        {
            for (y += 7; y < 64 && COL(y) != 7; y += 7)
            {
                if (couleur[y] == xside && (piece[y] == DAME || piece[y] == FOU))
                    return 1;
                if (piece[y] != VIDE)
                    break;

            }
        }
    }
    /* go left up */
    y = k - 9;
    if (y >= 0 && COL(y) != 7)
    {
        if (couleur[y] == xside)
        {
            if (piece[y] == ROI || piece[y] == DAME || piece[y] == FOU)
                return 1;
            if (current_side == BLANC && piece[y] == PION)
                return 1;
        }
        if (piece[y] == VIDE)
        {
            for (y -= 9; y >= 0 && COL(y) != 7; y -= 9)
            {
                if (couleur[y] == xside && (piece[y] == DAME || piece[y] == FOU))
                    return 1;
                if (piece[y] != VIDE)
                    break;

            }
        }
    }
    /* go right up */
    y = k - 7;
    if (y >= 0 && COL(y) != 0)
    {
        if (couleur[y] == xside)
        {
            if (piece[y] == ROI || piece[y] == DAME || piece[y] == FOU)
                return 1;
            if (current_side == BLANC && piece[y] == PION)
                return 1;
        }
        if (piece[y] == VIDE)
        {
            for (y -= 7; y >= 0 && COL(y) != 0; y -= 7)
            {
                if (couleur[y] == xside && (piece[y] == DAME || piece[y] == FOU))
                    return 1;
                if (piece[y] != VIDE)
                    break;
            }
        }
    }
    return 0;
}


int triple_repetition()
{
    int occurences = 0;
    int i;

    for(i = hdp - cinquante; i < hdp; ++i)
    {
        //printf("hist hash et cur hash  : %016llX   %016llX\n",hist[i].hash_code,hash_code_position);
        if(hist[i].hash_code == hash_code_position)
            ++occurences;
    }
    //printf("      occurence : %d\n",occurences);
    return occurences;
}

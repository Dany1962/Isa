#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "eval.h"
#include "board.h"
#include "recherche.h"
#include "temps.h"
#include "eval.h"
#include "evals_finales.h"
#include "bitboards.h"
#include "deftypes.h"
#include "hash.h"
#include "interface.h"
#include "edit.h"
#include "progjoue.h"

/*
---------------------------------------------------------
*														*
*		        calcul de la phase en cours     		*
*														*
---------------------------------------------------------
*/
int calcul_phase()
{
    return (24 - (4 * (nb_pieces[BLANC][DAME] + nb_pieces[NOIR][DAME])) - (2 * (nb_pieces[BLANC][TOUR] + nb_pieces[NOIR][TOUR])) -
            (nb_pieces[BLANC][CAVALIER] + nb_pieces[NOIR][CAVALIER] + nb_pieces[BLANC][FOU] + nb_pieces[NOIR][FOU]));
}


/*
---------------------------------------------------------
*														*
*		    évaluation statique de la position			*
*														*
---------------------------------------------------------
*/
int eval()
{
    char pieceName[] = " PCFTDR";
    int i,cp;
    signed int score_blanc_debut = 0, score_blanc_fin = 0;
    signed int score_noir_debut  = 0, score_noir_fin  = 0;
    signed int score_debut,score_fin;
    int eval;
    bool eval_hash_valide;
    bool eval_pion_valide = valINCONNUE;
    int pattern_blanc = 0;
    int pattern_noir  = 0;
    int tempo_bonus = (side == BLANC ? 10 : -10);
    int pce;
    int fi;

    memset(somme_attaques,0,sizeof(somme_attaques));
    memset(nombre_attaquants,0,sizeof(nombre_attaquants));
    memset(eval_pions_op, 0, sizeof(eval_pions_op));
    memset(eval_pions_eg, 0, sizeof(eval_pions_eg));

    PHASE = calcul_phase();

    //recherche de la position dans la table Hash_Eval
    if(!e_valide)
    {
        eval_hash_valide = probe_eval();
        if(eval_hash_valide != valINCONNUE)
        {
            if (side == BLANC)
            {
                return (eval_hash_valide);
            }
            return -(eval_hash_valide);
        }
    }

    //si KvsK ou KBvsK ou KNvsK --> position nulle
    if(draw_recognizer())
    {
        save_eval(0);
        return 0;
    }

    //test si pion passé colonne a ou h et roi dépouillé
    //et roi dépouillé case promo ou distance promo == 1--->draw
    if(draw_passer_ah())
    {
        save_eval(0);
        return 0;
    }

    //si finale KBN vs K , eval séparée
    if(finale_KBNK())
        return (eval_KBNK());

    //si finale KBB vs K , eval séparée
    if(finale_KBBK())
        return (eval_KBBK());

    //recherche de la position des pions dans la table
    if(!e_valide)
        eval_pion_valide = probe_pions();
    else
        eval_pion_valide = valINCONNUE;

    //sinon eval complète
    if(e_valide)
    {
        printf("...............................\n");
        printf("      EVALUATION STATIQUE\n");
    }

    //boucle les 64 cases de l'échiquier
    for (cp=0; cp<16; ++cp)
    {
        i = plist[BLANC][cp];
        if(i != VIDE)
        {
            pce = piece[i];
            if(e_valide)
                printf("--------------%c%s-------------\n",pieceName[pce],coord_une_case(i));
            //Score PST
            score_blanc_debut += pst_op[pce][i];
            score_blanc_fin += pst_eg[pce][i];
            //Score Matériel
            score_blanc_debut += materiel_op[pce];
            score_blanc_fin   += materiel_eg[pce];
            if(e_valide)
                printf("Psqt                : (%d , %d)\n",pst_op[pce][i],pst_eg[pce][i]);
            switch(pce)
            {
            case PION:
                if(eval_pion_valide == valINCONNUE)
                    eval_pion_blanc(i);
                break;
            case CAVALIER:
                eval_cavalier(BLANC, i, &score_blanc_debut, &score_blanc_fin);
                break;
            case FOU:
                eval_fou(BLANC, i, &score_blanc_debut, &score_blanc_fin);
                break;
            case TOUR:
                eval_tour(BLANC, i, &score_blanc_debut, &score_blanc_fin);
                break;
            case DAME:
                eval_dame(BLANC, i, &score_blanc_debut, &score_blanc_fin);
                break;
            case ROI:
                eval_roi(i, BLANC, NOIR, &score_blanc_debut, &score_blanc_fin);
                break;
            default:
                break;
            }
        }
        i = plist[NOIR][cp];
        if(i != VIDE)
        {
            pce = piece[i];
            fi = flip[i];
            if(e_valide)
                printf("--------------%c%s-------------\n",pieceName[pce],coord_une_case(i));
            //Score PST
            score_noir_debut += pst_op[pce][fi];
            score_noir_fin += pst_eg[piece[i]][fi];
            //Score Matériel
            score_noir_debut += materiel_op[pce];
            score_noir_fin   += materiel_eg[pce];
            if(e_valide)
                printf("Psqt                : (%d , %d)\n",pst_op[pce][fi],pst_eg[pce][fi]);
            switch(pce)
            {
            case PION:
                if(eval_pion_valide == valINCONNUE)
                    eval_pion_noir(i);
                break;
            case CAVALIER:
                eval_cavalier(NOIR, i, &score_noir_debut, &score_noir_fin);
                break;
            case FOU:
                eval_fou(NOIR, i, &score_noir_debut, &score_noir_fin);
                break;
            case TOUR:
                eval_tour(NOIR, i, &score_noir_debut, &score_noir_fin);
                break;
            case DAME:
                eval_dame(NOIR, i, &score_noir_debut, &score_noir_fin);
                break;
            case ROI:
                eval_roi(i, NOIR, BLANC, &score_noir_debut, &score_noir_fin);
                break;
            default:
                break;
            }
        }
    }

    if(e_valide)
    {
        printf("-----------Criteres generaux de la position-----------\n");
    }

    //score des pions
    int op,eg,in;
    if(eval_pion_valide == valINCONNUE)
    {
        op = (eval_pions_op[BLANC] - eval_pions_op[NOIR]);
        eg = (eval_pions_eg[BLANC] - eval_pions_eg[NOIR]);
        in = (((op * (256 - coef_phase[PHASE])) + (eg * coef_phase[PHASE])) / 256);
        save_pions(in);
    }
    else
        in = eval_pion_valide;

    if(e_valide)
    {
        printf("structure pions debut : %5d  %5d\n",eval_pions_op[BLANC],eval_pions_op[NOIR]);
        printf("structure pions fin   : %5d  %5d\n",eval_pions_eg[BLANC],eval_pions_eg[NOIR]);
        printf("Interpolee            : (%5d , %5d) = %5d\n",op,eg,in);
    }

    //si un roi est dépouillé , gros bonus pour l'autre
    //couleur plus le roi est proche du bord
    if(materiel[BLANC] == VAL_ROI)
    {
        score_noir_fin += roi_edge[pos_roi[BLANC]];
        if(e_valide)
            printf("Bonus aux noirs roi dépouille blanc : %d\n",roi_edge[pos_roi[BLANC]]);
    }

    if(materiel[NOIR] == VAL_ROI)
    {
        score_blanc_fin += roi_edge[pos_roi[NOIR]];
        if(e_valide)
            printf("Bonus aux blancs roi dépouille noir : %d\n",roi_edge[pos_roi[NOIR]]);
    }

    //Pénalité si une couleur n'a plus de pions
    //(promotion en Dame impossible)
    if(nb_pieces[BLANC][PION] == 0)
    {
        score_blanc_debut -= ZERO_PIONS;
        score_blanc_fin   -= ZERO_PIONS;
        if(e_valide)
        {
            printf("Penalite aux blancs , plus de pions ! (%d,%d)\n",-ZERO_PIONS,-ZERO_PIONS);
        }
    }

    if(nb_pieces[NOIR][PION] == 0)
    {
        score_noir_debut -= ZERO_PIONS;
        score_noir_fin   -= ZERO_PIONS;
        if(e_valide)
        {
            printf("Penalite aux noirs , plus de pions ! (%d,%d)\n",-ZERO_PIONS,-ZERO_PIONS);
        }
    }

    //Bonus paire de fous
    score_blanc_debut += (PAIRE_FOU * paire_de_fous());
    score_blanc_fin   += (PAIRE_FOU * paire_de_fous());
    if(e_valide)
        printf("paire de fous : (%d,%d) \n",
               (PAIRE_FOU * paire_de_fous()),(PAIRE_FOU * paire_de_fous()));

    //Pattern : situation de pièces mauvaises
    pattern_blanc = pb();
    score_blanc_debut += pattern_blanc;
    score_blanc_fin   += pattern_blanc;
    pattern_noir  = pn();
    score_noir_debut  += pattern_noir;
    score_noir_fin    += pattern_noir;

    //scores attaques du roi
    if(e_valide)
    {
        printf("Attaques et nombre d'attaquants et valeurs : \n");
        printf("Blanc : %d %d %d\n",somme_attaques[BLANC],nombre_attaquants[BLANC],
               safety_table[nombre_attaquants[BLANC]][somme_attaques[BLANC]]);
        printf("Noir : %d %d %d\n",somme_attaques[NOIR],nombre_attaquants[NOIR],
               safety_table[nombre_attaquants[NOIR]][somme_attaques[NOIR]]);
    }
    score_blanc_debut += safety_table[nombre_attaquants[BLANC]][somme_attaques[BLANC]];
    score_noir_debut  += safety_table[nombre_attaquants[NOIR]][somme_attaques[NOIR]];

    //scores Interpolé
    score_debut = ((score_blanc_debut - score_noir_debut) + tempo_bonus);
    score_fin   = (score_blanc_fin   - score_noir_fin);
    eval = ((score_debut * (256 - coef_phase[PHASE])) + (score_fin * coef_phase[PHASE])) / 256;
    eval += in;  //+ score structure pions interpolée

    //Affichage evaluations
    if(e_valide)
    {
        printf("Score blanc et noir debut : %5d  %5d\n",score_blanc_debut,score_noir_debut);
        printf("Score blanc et noir fin   : %5d  %5d\n",score_blanc_fin,score_noir_fin);
        printf("Score debut               : %5d  \n",score_debut);
        printf("Score fin                 : %5d  \n",score_fin);
        printf("EVAL                      : %5d\n",eval);
        printf("...............................\n");
        printf("hashcode from scratch               : %016llX\n",init_code_position());
    }

    //sauvegarde de la position dans la table Hash_Eval
    save_eval(eval);

    if (side == BLANC)
    {
        return (eval);
    }
    return -(eval);
}

//EVALUATION PIONS//
void eval_pion_blanc(int sq)
{
    bool open = FAUX;
    int row = ROW(sq);
    int col = COL(sq);
    int passe_op = 0,passe_eg = 0;
    int d1 = (20 * chebi[pos_roi[NOIR]][(sq - 8)]);
    int d2 = (5 * chebi[pos_roi[BLANC]][(sq - 8)]);
    U64 pb = (bitboard_candidat[col] & bitboard_pion[BLANC]);
    U64 pn = (bitboard_candidat[col] & bitboard_pion[NOIR]);
    int nbpb = 0;
    int nbpn = 0;
    int sqd = sq - 8;
    int score_op = 0,score_eg = 0;
    int pla = (nb_pieces[NOIR][DAME] + nb_pieces[NOIR][TOUR]); //nbre pieces lourdes adverses
    bool opn = ((bitboard_colonne[col] & bitboard_pion[NOIR]) == 0);

    //Test  si pion open (pas de pion adverse sur la même
    //colonne) pour eval isolé et arriéré
    if(opn && pla)
    {
        open = VRAI;
        if(e_valide)
            printf("pion open\n");
    }

    //Pion isolé ? PENALITE
    if((BB_colonne[COL(sq)] & bitboard_pion[BLANC]) == 0 )
    {
        if(open)
        {
            score_op -= (ISOLE_OPEN_OP);
            score_eg -= (ISOLE_OPEN_EG);
            if(e_valide)
                printf("Pion isole open          : (%d , %d)\n",-ISOLE_OPEN_OP,-ISOLE_OPEN_EG);
        }
        else
        {
            score_op -= (ISOLE_OP);
            score_eg -= (ISOLE_EG);
            if(e_valide)
                printf("Pion isole          : (%d , %d)\n",-ISOLE_OP,-ISOLE_EG);
        }
    }

    //Pion doublé ? PENALITE
    U64 nb_pions_colonne;
    nb_pions_colonne = arr_file[COL(sq)] & bitboard_pion[BLANC];
    if(nb_pions_colonne & (nb_pions_colonne-1))
    {
        score_op -= PDOUBLE_OP;
        score_eg -= PDOUBLE_EG;
        if(e_valide)
            printf("Pion double         : (%d , %d)\n",-PDOUBLE_OP,-PDOUBLE_EG);
    }

    //Pion arriéré ? Pénalité
    //Condition : pions derrière ses voisins adjacents
    //Case devant lui controlé par un pion adverse
    //Pénalité supplémentaire si colonne demi ouverte
    if(row >= 4)  //rangées 2 à 4
    {
        if((mask_backwards[BLANC][sq] & bitboard_pion[BLANC]) == 0)
        {
            if(COL(sq) != 0 && (si_piece(PION, NOIR, sq - 17)))
            {
                if(open)
                {
                    score_op -= BACKWARD_OPEN_OP;
                    score_eg -= BACKWARD_OPEN_EG;
                    if(e_valide)
                        printf("Pion arriere open: (%d , %d)\n",-(BACKWARD_OPEN_OP),-(BACKWARD_OPEN_EG));
                }
                else
                {
                    score_op -= BACKWARD_OP;
                    score_eg -= BACKWARD_EG;
                    if(e_valide)
                        printf("Pion arriere        : (%d , %d)\n",-(BACKWARD_OP),-(BACKWARD_EG));
                }
            }
            if(COL(sq) != 7 && (si_piece(PION, NOIR, sq - 15)))
            {
                if(open)
                {
                    score_op -= BACKWARD_OPEN_OP;
                    score_eg -= BACKWARD_OPEN_EG;
                    if(e_valide)
                        printf("Pion arriere col 1/2: (%d , %d)\n",-(BACKWARD_OPEN_OP),-(BACKWARD_OPEN_EG));
                }
                else
                {
                    score_op -= BACKWARD_OP;
                    score_eg -= BACKWARD_EG;
                    if(e_valide)
                        printf("Pion arriere        : (%d , %d)\n",-(BACKWARD_OP),-(BACKWARD_EG));
                }
            }
        }
    }

    //Validation du pion passé
    if(pion_passe(sq, BLANC, NOIR))
    {
        int atk,def;
        int dra,dre,score_proxi_roi;

        //Bonus rangée dépendant
        passe_op += pop[BLANC][row];
        passe_eg += peg[BLANC][row];
        if(e_valide)
            printf("Bonus row : (%d,%d)\n",pop[BLANC][row],peg[BLANC][row]);

        //bonus case vide devant le pion
        if(piece[sq-8] == VIDE)
        {
            passe_op += (pop[BLANC][row] / 5);
            passe_eg += (peg[BLANC][row] / 5);
            if(e_valide)
                printf("Bonus case vide devant : (%d,%d)\n",(pop[BLANC][row] / 5),(peg[BLANC][row] / 5));
        }

        //bonus controle case devant (ou penalité)
        atk = nb_atk_case(sq-8, BLANC, sq);
        def = nb_atk_case(sq-8, NOIR, sq);
        passe_op += ((def - atk) * 3);
        passe_eg += ((def - atk) * 6);
        if(e_valide)
            printf("Bonus controle case devant : (%d,%d) [%d,%d]\n",((def - atk) * 3),((def - atk) * 6),def,atk);

        //Pion passé instoppable (règle du carré) + bonus proximité du roi
        if(PHASE == 24)
        {
            //règle de carré
            if (MIN(5,chebi[sq][sq_promo_blanc[col]]) <
                    (chebi[pos_roi[NOIR]][sq_promo_blanc[col]] - BLACKKING2MOVE(side)))
            {
                passe_eg += (INSTOPPABLE1);
                if(e_valide)
                    printf("  Passe Instoppable : (%d)\n",INSTOPPABLE1);
            }
            //bonus si le roi est adjacent a la case de promo et adjacent au pion
            if((chebi[pos_roi[BLANC]][sq_promo_blanc[col]] == 1) && (chebi[pos_roi[BLANC]][sq] == 1))
            {
                passe_eg += (INSTOPPABLE2);
                if(e_valide)
                    printf("  Passe Instoppable 2 : (%d)\n",INSTOPPABLE2);
            }
            //bonus proximité du roi
            dra = chebi[pos_roi[BLANC]][sq-8];
            dre = chebi[pos_roi[NOIR]][sq-8];
            score_proxi_roi = (table_distances[dra][dre] * coef_row[BLANC][row]);
            if(e_valide)
                printf("Bonus proximite roi : %d  (dra = %d,dre = %d)  (table = %d) (coef row = %d) \n",
                       score_proxi_roi,dra,dre,table_distances[dra][dre],coef_row[BLANC][row]);
            passe_eg += score_proxi_roi;
        }
        score_op += passe_op;
        score_eg += passe_eg;
        if(e_valide)
            printf("Score pion passe : (%d,%d)\n",passe_op,passe_eg);
    }

    //Si pion pas passé , test si pion candidat
    //conditions : pion open et nbre de pions amis
    //supérieur ou egal auxx pions adverses sur les col adjacentes                                          *
    else
    {
        if(opn)
        {
            if(e_valide)
                printf("Le pion n'est pas passe mais open\n");
            nbpb = popCount(pb);
            nbpn = popCount(pn);
            if(nbpb >= nbpn)
            {
                score_eg += candidat_eg[BLANC][row];
                if(e_valide)
                    printf("pion candidat : (%d,%d)\n",candidat_op[BLANC][row],candidat_eg[BLANC][row]);
            }
        }
    }
    eval_pions_op[BLANC] += score_op;
    eval_pions_eg[BLANC] += score_eg;
}

void eval_pion_noir(int sq)
{
    bool open = FAUX;
    int row = ROW(sq);
    int col = COL(sq);
    int passe_op = 0,passe_eg = 0;
    U64 pb = (bitboard_candidat[col] & bitboard_pion[BLANC]);
    U64 pn = (bitboard_candidat[col] & bitboard_pion[NOIR]);
    int nbpb = 0;
    int nbpn = 0;
    int sqd = sq + 8;
    int score_op = 0,score_eg = 0;
    int pla = (nb_pieces[BLANC][DAME] + nb_pieces[BLANC][TOUR]); //nbre pieces lourdes adverses
    bool opn = ((bitboard_colonne[col] & bitboard_pion[BLANC]) == 0);

    //Test  si pion open (pas de pion adverse sur la même
    //colonne) pour eval isolé et arriéré
    if(opn && pla)
    {
        open = VRAI;
        if(e_valide)
            printf("pion open\n");
    }

    //Pion isolé ? PENALITE
    if((BB_colonne[COL(sq)] & bitboard_pion[NOIR]) == 0)
    {
        if(open)
        {
            score_op -= (ISOLE_OPEN_OP);
            score_eg -= (ISOLE_OPEN_EG);
            if(e_valide)
                printf("Pion isole open          : (%d , %d)\n",-ISOLE_OPEN_OP,-ISOLE_OPEN_EG);
        }
        else
        {
            score_op -= (ISOLE_OP);
            score_eg -= (ISOLE_EG);
            if(e_valide)
                printf("Pion isole          : (%d , %d)\n",-ISOLE_OP,-ISOLE_EG);
        }
    }

    //Pion doublé ? PENALITE
    U64 nb_pions_colonne;
    nb_pions_colonne = arr_file[COL(sq)] & bitboard_pion[NOIR];
    if(nb_pions_colonne & (nb_pions_colonne-1))
    {
        score_op -= PDOUBLE_OP;
        score_eg -= PDOUBLE_EG;
        if(e_valide)
            printf("Pion double         : (%d , %d)\n",-PDOUBLE_OP,-PDOUBLE_EG);
    }

    //Pion arriéré ? Pénalité
    //Condition : pions derrière ses voisins adjacents
    //Case devant lui controlé par un pion adverse
    //Pénalité supplémentaire si colonne demi ouverte
    if(row <= 3)  //rangées 5 à 8
    {
        if((mask_backwards[NOIR][sq] & bitboard_pion[NOIR]) == 0)
        {
            if(COL(sq) != 0 && (si_piece(PION, BLANC, sq + 15)))
            {
                if(open)
                {
                    score_op -= BACKWARD_OPEN_OP;
                    score_eg -= BACKWARD_OPEN_EG;
                    if(e_valide)
                        printf("Pion arriere open: (%d , %d)\n",-(BACKWARD_OPEN_OP),-(BACKWARD_OPEN_EG));
                }
                else
                {
                    score_op -= BACKWARD_OP;
                    score_eg -= BACKWARD_EG;
                    if(e_valide)
                        printf("Pion arriere        : (%d , %d)\n",-(BACKWARD_OP),-(BACKWARD_EG));
                }
            }
            if(COL(sq) != 7 && (si_piece(PION, BLANC, sq + 17)))
            {
                if(open)
                {
                    score_op -= BACKWARD_OPEN_OP;
                    score_eg -= BACKWARD_OPEN_EG;
                    if(e_valide)
                        printf("Pion arriere open: (%d , %d)\n",-(BACKWARD_OPEN_OP),-(BACKWARD_OPEN_EG));
                }
                else
                {
                    score_op -= BACKWARD_OP;
                    score_eg -= BACKWARD_EG;
                    if(e_valide)
                        printf("Pion arriere        : (%d , %d)\n",-(BACKWARD_OP),-(BACKWARD_EG));
                }
            }
        }
    }

    //Validation du pion passé
    if(pion_passe(sq, NOIR, BLANC))
    {
        int atk,def;
        int dra,dre,score_proxi_roi;

        //Bonus rangée dépendant
        passe_op += pop[NOIR][row];
        passe_eg += peg[NOIR][row];
        if(e_valide)
            printf("Bonus row : (%d,%d)\n",pop[NOIR][row],peg[NOIR][row]);

        //bonus case vide devant le pion
        if(piece[sq+8] == VIDE)
        {
            passe_op += (pop[NOIR][row] / 5);
            passe_eg += (peg[NOIR][row] / 5);
            if(e_valide)
                printf("Bonus case vide devant : (%d,%d)\n",(pop[NOIR][row] / 5),(peg[NOIR][row] / 5));
        }

        //bonus controle case devant (ou penalité)
        atk = nb_atk_case(sq+8, NOIR, sq);
        def = nb_atk_case(sq+8, BLANC, sq);
        passe_op += ((def - atk) * 3);
        passe_eg += ((def - atk) * 6);
        if(e_valide)
            printf("Bonus controle case devant : (%d,%d) [%d,%d]\n",((def - atk) * 3),((def - atk) * 6),def,atk);

        //Pion passé instoppable (règle du carré)  + bonus proximité du roi
        if(PHASE == 24)
        {
            //règle du carré
            if (MIN(5,chebi[sq][sq_promo_noir[col]]) <
                    (chebi[pos_roi[BLANC]][sq_promo_noir[col]] - WHITEKING2MOVE(side)))
            {
                passe_eg += (INSTOPPABLE1);
                if(e_valide)
                    printf("  Passe Instoppable : (%d)\n",INSTOPPABLE1);
            }
            //bonus si le roi est adjacent a la case de promo et adjacent au pion
            if((chebi[pos_roi[NOIR]][sq_promo_noir[col]] == 1) && (chebi[pos_roi[NOIR]][sq] == 1))
            {
                passe_eg += (INSTOPPABLE2);
                if(e_valide)
                    printf("  Passe Instoppable 2 : (%d)\n",INSTOPPABLE2);
            }
            //bonus proximité du roi
            dra = chebi[pos_roi[NOIR]][sq+8];
            dre = chebi[pos_roi[BLANC]][sq+8];
            score_proxi_roi = (table_distances[dra][dre] * coef_row[NOIR][row]);
            if(e_valide)
                printf("Bonus proximite roi : %d  (dra = %d,dre = %d)  (table = %d) (coef row = %d) \n",
                       score_proxi_roi,dra,dre,table_distances[dra][dre],coef_row[NOIR][row]);
            passe_eg += score_proxi_roi;
        }
        score_op += passe_op;
        score_eg += passe_eg;

        if(e_valide)
            printf("Score pion passe : (%d,%d)\n",passe_op,passe_eg);
    }

    //Si pion pas passé , test si pion candidat
    //conditions : pion open et nbre de pions amis
    //supérieur ou egal auxx pions adverses sur les col adjacentes                                          *
    else
    {
        if(opn)
        {
            if(e_valide)
                printf("Le pion n'est pas passe mais open\n");
            nbpb = popCount(pb);
            nbpn = popCount(pn);
            if(nbpn >= nbpb)
            {
                score_eg += candidat_eg[NOIR][row];
                if(e_valide)
                    printf("pion candidat : (%d,%d)\n",candidat_op[NOIR][row],candidat_eg[NOIR][row]);
            }
        }
    }
    eval_pions_op[NOIR] += score_op;
    eval_pions_eg[NOIR] += score_eg;
}

int popCount(U64 x)
{
    int count = 0;
    while (x)
    {
        count++;
        x &= x - 1; // reset LS1B
    }
    return count;
}

bool pion_passe(int index, int coul, int coul_adv)
{
    return((bitboard_front[coul][index] & bitboard_pion[coul_adv]) == 0);
}

//EVALUATION CAVALIER//
void eval_cavalier(int c, int sq, int *pscoredebut, int *pscorefin)
{
    int mobilite = 0;
    int xc = OPP(c);
    int col = COL(sq);
    int y;
    int score_attaque = 0;
    int index = sq;

    //Mobilité
    mobilite = mobilite_cavalier(sq, c);
    *pscoredebut += mob_cavalier[mobilite];
    *pscorefin   += mob_cavalier[mobilite];
    if(e_valide)
        printf("Mobilite            : (%d , %d)\n",mob_cavalier[mobilite],mob_cavalier[mobilite]);

    //Eval attaques sur le roi
    score_attaque = safety_cavalier(index, xc);
    if(score_attaque)
    {
        somme_attaques[c] += score_attaque;
        ++nombre_attaquants[c];
        if(e_valide)
            printf("Score d'attaque : %d : \n",score_attaque);
    }
}

//Mobilité cavalier
int mobilite_cavalier(int sq, int coul)
{
    int col = COL(sq);
    int y;
    int mob = 0;
    int opp_coul = OPP(coul);

    y = sq - 6;
    if ((y >= 0 && col < 6) && (couleur[y] == VIDE) && (non_atk_pion(y, opp_coul)))
        ++mob;
    y = sq - 10;
    if ((y >= 0 && col > 1) && (couleur[y] == VIDE) && (non_atk_pion(y, opp_coul)))
        ++mob;
    y = sq - 15;
    if ((y >= 0 && col < 7) && (couleur[y] == VIDE) && (non_atk_pion(y, opp_coul)))
        ++mob;
    y = sq - 17;
    if ((y >= 0 && col > 0) && (couleur[y] == VIDE) && (non_atk_pion(y, opp_coul)))
        ++mob;
    y = sq + 6;
    if ((y < 64 && col > 1) && (couleur[y] == VIDE) && (non_atk_pion(y, opp_coul)))
        ++mob;
    y = sq + 10;
    if ((y < 64 && col < 6) && (couleur[y] == VIDE) && (non_atk_pion(y, opp_coul)))
        ++mob;
    y = sq + 15;
    if ((y < 64 && col > 0) && (couleur[y] == VIDE) && (non_atk_pion(y, opp_coul)))
        ++mob;
    y = sq + 17;
    if ((y < 64 && col < 7) && (couleur[y] == VIDE) && (non_atk_pion(y, opp_coul)))
        ++mob;
    return mob;
}

//Attaques cavalier
//Renvoie le nombre de cases que le cavalier controle
//autour du roi adverse
int safety_cavalier(int sq, int coul)
{
    int score = 0;
    int y;
    int col = COL(sq);
    int ra = pos_roi[coul];

    y = sq - 6;
    if (y >= 0 && col < 6)
        score += (champ_roi[ra][y] * KCAVALIER);
    y = sq - 10;
    if (y >= 0 && col > 1)
        score += (champ_roi[ra][y] * KCAVALIER);
    y = sq - 15;
    if (y >= 0 && col < 7)
        score += (champ_roi[ra][y] * KCAVALIER);
    y = sq - 17;
    if (y >= 0 && col > 0)
        score += (champ_roi[ra][y] * KCAVALIER);
    y = sq + 6;
    if (y < 64 && col > 1)
        score += (champ_roi[ra][y] * KCAVALIER);
    y = sq + 10;
    if (y < 64 && col < 6)
        score += (champ_roi[ra][y] * KCAVALIER);
    y = sq + 15;
    if (y < 64 && col > 0)
        score += (champ_roi[ra][y] * KCAVALIER);
    y = sq + 17;
    if (y < 64 && col < 7)
        score += (champ_roi[ra][y] * KCAVALIER);
    return score;
}

//Routine renvoie vrai si une case
//est attaquée par un pion
bool non_atk_pion(int sq, int coul)
{
    int col = COL(sq);

    if(coul == BLANC)
    {
        if((col != 0) && (si_piece(PION, BLANC, sq+7)))
            return FAUX;
        if((col != 7) && (si_piece(PION, BLANC, sq+9)))
            return FAUX;
    }
    else
    {
        if((col != 0) && (si_piece(PION, NOIR, sq-9)))
            return FAUX;
        if((col != 7) && (si_piece(PION, NOIR, sq-7)))
            return FAUX;
    }
    return VRAI;
}

//EVALUATION FOU//
void eval_fou(int c, int sq, int *pscoredebut, int *pscorefin)
{
    int mobilite = 0;
    int score_attaque = 0;
    int index = sq;
    int xc = OPP(c);

    //Mobilité
    mobilite = mobilite_fou(index, c);
    *pscoredebut += mob_fou[mobilite];
    *pscorefin   += mob_fou[mobilite];
    if(e_valide)
        printf("Mobilite            : (%d , %d)\n",mob_fou[mobilite],mob_fou[mobilite]);

    //Eval attaques sur le roi
    score_attaque = safety_fou(index, xc);
    if(score_attaque)
    {
        somme_attaques[c] += score_attaque;
        ++nombre_attaquants[c];
        if(e_valide)
            printf("Score d'attaque : %d : \n",score_attaque);
    }
}

//Mobilité fou
int mobilite_fou(int sq, int coul)
{
    int y;
    int opp_coul = OPP(coul);
    int mob = 0;

    for (y = sq - 9; y >= 0 && COL(y) != 7; y -= 9)          /* go left up */
    {
        if((couleur[y] == VIDE) && (non_atk_pion(y, opp_coul)))
            mob++;
        if(couleur[y] != VIDE)
            break;
    }
    for (y = sq - 7; y >= 0 && COL(y) != 0; y -= 7)          /* go right up */
    {
        if((couleur[y] == VIDE) && (non_atk_pion(y, opp_coul)))
            mob++;
        if (couleur[y] != VIDE)
            break;
    }
    for (y = sq + 9; y < 64 && COL(y) != 0; y += 9)          /* go right down */
    {
        if((couleur[y] == VIDE) && (non_atk_pion(y, opp_coul)))
            mob++;
        if(couleur[y] != VIDE)
            break;
    }
    for (y = sq + 7; y < 64 && COL(y) != 7; y += 7)          /* go right down */
    {
        if((couleur[y] == VIDE) && (non_atk_pion(y, opp_coul)))
            mob++;
        if(couleur[y] != VIDE)
            break;
    }
    return mob;
}

//Attaques fou
//Renvoie le nombre de cases que le cavalier controle
//autour du roi adverse
int safety_fou(int sq, int coul)
{
    int y;
    int score = 0;
    int ra = pos_roi[coul];

    for (y = sq - 9; y >= 0 && COL(y) != 7; y -= 9)          /* go left up */
    {
        score += (KFOU * champ_roi[ra][y]);
        if(couleur[y] != VIDE)
            break;
    }
    for (y = sq - 7; y >= 0 && COL(y) != 0; y -= 7)          /* go right up */
    {
        score += (KFOU * champ_roi[ra][y]);
        if (couleur[y] != VIDE)
            break;
    }
    for (y = sq + 9; y < 64 && COL(y) != 0; y += 9)          /* go right down */
    {
        score += (KFOU * champ_roi[ra][y]);
        if(couleur[y] != VIDE)
            break;
    }
    for (y = sq + 7; y < 64 && COL(y) != 7; y += 7)          /* go right down */
    {
        score += (KFOU * champ_roi[ra][y]);
        if(couleur[y] != VIDE)
            break;
    }
    return score;
}

//Evaluation paire de fous
//Fonction polynomiale de type
//(B * B-1) - (b * b-1)
int paire_de_fous()
{
    return (nb_pieces[BLANC][FOU] *(nb_pieces[BLANC][FOU]-1) -
            nb_pieces[NOIR][FOU]*(nb_pieces[NOIR][FOU]-1));
}

//EVALUATION TOUR//
void eval_tour(int c, int sq, int *pscoredebut, int *pscorefin)
{
    int mobilite = 0;
    int coul = c;
    int xc = OPP(coul);
    int index = sq;
    int col = COL(sq);
    int row = ROW(sq);
    int col_roi = COL(pos_roi[xc]);
    int dist = distance(col,col_roi);
    int p = 0;
    int score_attaque = 0;

    //Calcule le type de colonne :
    //2 : colonne ouverte
    //1 : colonne demi-ouverte
    //0 : colonne fermée
    p += ((bitboard_colonne[col] & bitboard_pion[coul]) == 0);
    p += ((bitboard_colonne[col] & bitboard_pion[xc]) == 0);
    if(e_valide)
        printf("%d   %d\n",((bitboard_colonne[col] & bitboard_pion[BLANC]) == 0),
               ((bitboard_colonne[col] & bitboard_pion[NOIR]) == 0));

    //Mobilité
    mobilite = mobilite_tour(sq, c);
    *pscoredebut += mob_tour_op[mobilite];
    *pscorefin   += mob_tour_eg[mobilite];
    if(e_valide)
        printf("Mobilite            : (%d , %d)\n",mob_tour_op[mobilite],mob_tour_eg[mobilite]);

    //Eval tour colonne ouverte / semi ouverte
    switch(p)
    {
    case 0:     //colonne fermée
        *pscoredebut -= COLONNE_FERMEE;
        *pscorefin   -= COLONNE_FERMEE;
        if(e_valide)
            printf("Tour sur colonne fermee : (%d,%d)\n",
                   -COLONNE_FERMEE,-COLONNE_FERMEE);
        break;
    case 1:     //colonne demi-ouverte
        *pscoredebut += colonne_demi_ouverte_op[dist];
        *pscorefin   += colonne_demi_ouverte_eg[dist];
        if(e_valide)
            printf("Tour sur colonne semi ouverte : (%d,%d)\n",
                   colonne_demi_ouverte_op[dist],
                   colonne_demi_ouverte_eg[dist]);
        break;
    case 2:     //colonne ouverte
        *pscoredebut += colonne_ouverte_op[dist];
        *pscorefin   += colonne_ouverte_eg[dist];
        if(e_valide)
            printf("Tour sur colonne ouverte : (%d,%d)\n",
                   colonne_ouverte_op[dist],
                   colonne_ouverte_eg[dist]);
        break;
    default:
        break;
    }

    //Eval tour 7eme rangée
    //Condition : présence de pions adverses
    if((row == rangee_td[coul]) && ((bitboard_rangee_7[coul] & bitboard_pion[xc]) != 0))
    {
        *pscoredebut += TOUR_RANGEE_SEPT_OP;
        *pscorefin   += TOUR_RANGEE_SEPT_EG;
        if(e_valide)
            printf("Tour rangee 7 : (%d,%d)\n",TOUR_RANGEE_SEPT_OP,TOUR_RANGEE_SEPT_EG);
    }

    //Eval attaques sur le roi
    score_attaque = safety_tour(index, xc);
    if(score_attaque)
    {
        somme_attaques[c] += score_attaque;
        ++nombre_attaquants[c];
        if(e_valide)
            printf("Score d'attaque : %d : \n",score_attaque);
    }
}

//Mobilité tour
int mobilite_tour(int sq, int coul)
{
    int col;
    int k;
    int y;
    int opp_coul = OPP(coul);
    int mob = 0;

    col = COL(sq);
    for (k = sq - col, y = sq - 1; y >= k; y--)    /* go left */
    {
        if((couleur[y] == VIDE) && (non_atk_pion(y, opp_coul)))
            mob++;
        if(couleur[y] != VIDE)
            break;
    }
    for (k = sq - col + 7, y = sq + 1; y <= k; y++)        /* go right */
    {
        if((couleur[y] == VIDE) && (non_atk_pion(y, opp_coul)))
            mob++;
        if(couleur[y] != VIDE)
            break;
    }
    for (y = sq - 8; y >= 0; y -= 8)         /* go up */
    {
        if((couleur[y] == VIDE) && (non_atk_pion(y, opp_coul)))
            mob++;
        if(couleur[y] != VIDE)
            break;
    }
    for (y = sq + 8; y < 64; y += 8)         /* go down */
    {
        if((couleur[y] == VIDE) && (non_atk_pion(y, opp_coul)))
            mob++;
        if(couleur[y] != VIDE)
            break;
    }

    return mob;
}

//Attaques tour
//Renvoie le nombre de cases que le cavalier controle
//autour du roi adverse
int safety_tour(int sq, int coul)
{
    int col;
    int k;
    int y;
    int ra = pos_roi[coul];
    int score = 0;

    col = COL(sq);
    for (k = sq - col, y = sq - 1; y >= k; y--)    /* go left */
    {
        score += (KTOUR * champ_roi[ra][y]);
        if(couleur[y] != VIDE)
            break;
    }
    for (k = sq - col + 7, y = sq + 1; y <= k; y++)        /* go right */
    {
        score += (KTOUR * champ_roi[ra][y]);
        if(couleur[y] != VIDE)
            break;
    }
    for (y = sq - 8; y >= 0; y -= 8)         /* go up */
    {
        score += (KTOUR * champ_roi[ra][y]);
        if(couleur[y] != VIDE)
            break;
    }
    for (y = sq + 8; y < 64; y += 8)         /* go down */
    {
        score += (KTOUR * champ_roi[ra][y]);
        if(couleur[y] != VIDE)
            break;
    }
    return score;
}

//EVALUATION DAME//
void eval_dame(int c, int sq, int *pscoredebut, int *pscorefin)
{
    int mobilite = 0;
    int coul = c;
    int xc = OPP(coul);
    int row = ROW(sq);
    int rangee_dame = ((c == BLANC) ? 1 : 6);
    int rangee_roi_adv = ((c == BLANC) ? 0 : 7);
    int score_attaque = 0;
    int index = sq;

    //Proximité dame --> roi adverse (distance manhattan)
    *pscoredebut += (10 - manhattan[sq][pos_roi[xc]]);
    *pscorefin   += (10 - manhattan[sq][pos_roi[xc]]);
    if(e_valide)
        printf("proximite dame --> roi adverse : (%d,%d)\n",(10 - manhattan[sq][pos_roi[xc]]),(10 - manhattan[sq][pos_roi[xc]]));

    //Eval dame 7eme rangée
    //Condition : présence du roi rangée du bord
    if((row == rangee_td[coul]) && (ROW(pos_roi[xc]) == rangee_roi[coul]))
    {
        *pscoredebut += DAME_RANGEE_SEPT_OP;
        *pscorefin   += DAME_RANGEE_SEPT_EG;
        if(e_valide)
            printf("Dame rangee 7 : (%d,%d)\n",DAME_RANGEE_SEPT_OP,DAME_RANGEE_SEPT_EG);
    }

    //Eval attaques sur le roi
    score_attaque = safety_dame(index, xc);
    if(score_attaque)
    {
        somme_attaques[c] += score_attaque;
        ++nombre_attaquants[c];
        if(e_valide)
            printf("Score d'attaque : %d : \n",score_attaque);
    }
}

//Attaques dame
int safety_dame(int sq,int coul)
{
    int k;
    int y;
    int ra = pos_roi[coul];
    int col;
    int score = 0;
    //gauche en haut
    for (y = sq - 9; y >= 0 && COL(y) != 7; y -= 9)
    {
        score += (KDAME * champ_roi[ra][y]);
        if(piece[y] != VIDE)
            break;
    }
    //droite en haut
    for (y = sq - 7; y >= 0 && COL(y) != 0; y -= 7)
    {
        score += (KDAME * champ_roi[ra][y]);
        if(piece[y] != VIDE)
            break;
    }
    //droite en bas
    for (y = sq + 9; y < 64 && COL(y) != 0; y += 9)
    {
        score += (KDAME * champ_roi[ra][y]);
        if(piece[y] != VIDE)
            break;
    }
    //gauche en bas
    for (y = sq + 7; y < 64 && COL(y) != 7; y += 7)
    {
        score += (KDAME * champ_roi[ra][y]);
        if(piece[y] != VIDE)
            break;
    }
    col = COL(sq);
    // a gauche
    for (k = sq - col, y = sq - 1; y >= k; y--)
    {
        score += (KDAME * champ_roi[ra][y]);
        if(piece[y] != VIDE)
            break;
    }
    //a droite
    for (k = sq - col + 7, y = sq + 1; y <= k; y++)
    {
        score += (KDAME * champ_roi[ra][y]);
        if(piece[y] != VIDE)
            break;
    }
    //en haut
    for (y = sq - 8; y >= 0; y -= 8)
    {
        score += (KDAME * champ_roi[ra][y]);
        if(piece[y] != VIDE)
            break;
    }
    //en bas
    for (y = sq + 8; y < 64; y += 8)
    {
        score += (KDAME * champ_roi[ra][y]);
        if(piece[y] != VIDE)
            break;
    }
    return score;
}

//EVALUATION ROI//
void eval_roi(int sq, int c, int xc, int *pop, int *peg)
{
    int score = 0;
    int i;

    if((nb_pieces[xc][DAME]) && (total_pieces(xc) != 0))
    {
        //securité du  roi (castled)
        if (COL(sq) < 3)
        {
            score += eval_secu(0, sq, c, xc);
            score += eval_secu(1, sq, c, xc);
            score += eval_secu(2, sq, c, xc);
        }
        else if (COL(sq) > 4)
        {
            score += eval_secu(5,sq, c, xc);
            score += eval_secu(6,sq, c, xc);
            score += eval_secu(7,sq, c, xc);
        }
        // no castled
        else
        {
            for (i = COL(sq); i <= COL(sq) + 2; ++i)
            {
                if((bitboard_colonne[i] & bitboard_pion[BLANC]) == 0)
                    score -= NON_PION_COLONNE;
                if((bitboard_colonne[i] & bitboard_pion[NOIR]) == 0)
                    score -= NON_PION_COLONNE;
            }
        }
        *pop += score;
        if(e_valide)
            printf("Securite            : (%d)\n",score);
    }
}

int eval_secu(int col, int sq, int c, int xc)
{
    int score = 0;
    int row = ROW(sq);
    int sqp  = ((c == BLANC) ? (square(col, row - 1))  : (square(col, row + 1)));
    int sqp2 = ((c == BLANC) ? (square(col, row - 2))  : (square(col, row + 2)));
    int sqp3 = ((c == BLANC) ? (square(col, row - 3))  : (square(col, row + 3)));

    //le pion devant a bougé ou non ?
    if(si_piece(PION, c, sqp))
    {
        score += BONUS_PION_DEVANT;
        if(e_valide)
            printf("    Le pion devant n'a pas bouge , bonus : %d\n",BONUS_PION_DEVANT);
    }

    else if((si_piece(PION, c, sqp2)))
    {
        score -= PEN_PION_UNE_CASE;
        if(e_valide)
            printf("    Le pion devant a bouge d'une case , penalite : %d\n",PEN_PION_UNE_CASE);
    }
    else if((si_piece(PION, c, sqp3)))
    {
        score -= PEN_PION_DEUX_CASES;
        if(e_valide)
            printf("    Le pion devant a bouge de 2 cases , penalite : %d\n",PEN_PION_DEUX_CASES);
    }
    if((bitboard_colonne[col] & bitboard_pion[c]) == 0)
    {
        score -= PEN_PAS_DE_PION_AMI;
        if(e_valide)
            printf("    Aucun pion ami sur cette colonne (%d) , penalite : %d\n",col,PEN_PAS_DE_PION_AMI);
    }

    //aucun pion adverse sur la colonne
    if((bitboard_colonne[col] & bitboard_pion[xc]) == 0)
    {
        score -= PEN_PAS_DE_PION_ADVERSE;
        if(e_valide)
        {
            printf("    Aucun pion adverse sur cette colonne (%d) , penalite : %d\n",col,PEN_PAS_DE_PION_ADVERSE);
        }
    }
    return score;
}

int square(int c, int r)
{
    return(c + 8*r);
}

int total_pieces(int coul)
{
    return(nb_pieces[coul][CAVALIER] + nb_pieces[coul][FOU] + nb_pieces[coul][TOUR]);
}

//EVALUATION PATTERNS//
int pb()
{
    int score = 0;
    //fou a7 , pion adverse b6
    if((si_piece(FOU,BLANC,A7)) && (si_piece(PION,NOIR,B6)))
        score -= 100;
    //fou b8 , pion adverse c7
    if((si_piece(FOU,BLANC,B8)) && (si_piece(PION,NOIR,C7)))
        score -= 100;
    //fou h7 , pion adverse g6
    if((si_piece(FOU,BLANC,H7)) && (si_piece(PION,NOIR,G6)))
        score -= 100;
    //fou g8 , pion adverse f7
    if((si_piece(FOU,BLANC,G8)) && (si_piece(PION,NOIR,F7)))
        score -= 100;
    //fou a6 , pion adverse b5
    if((si_piece(FOU,BLANC,A6)) && (si_piece(PION,NOIR,B5)))
        score -= 100;
    //fou c1 , pion blanc d2 , case d3 occupée
    if((si_piece(FOU,BLANC,C1)) && (si_piece(PION,BLANC,D2)) && (piece[D3] != VIDE))
        score -= 50;
    //fou f1 , pion blanc e2 , case e3 occupée
    if((si_piece(FOU,BLANC,F1)) && (si_piece(PION,BLANC,E2)) && (piece[E3] != VIDE))
        score -= 50;
    //tour a1 ou a2 ou b1 , roi b1 ou c1
    if(((si_piece(TOUR,BLANC,A1)) || (si_piece(TOUR,BLANC,A2)) || (si_piece(TOUR,BLANC,B1))) &&
            ((si_piece(ROI,BLANC,B1)) || (si_piece(ROI,BLANC,C1))))
        score -= 50;
    //tour h1 ou h2 ou g1 , roi f1 ou g1
    if(((si_piece(TOUR,BLANC,H1)) || (si_piece(TOUR,BLANC,H2)) || (si_piece(TOUR,BLANC,G1))) &&
            ((si_piece(ROI,BLANC,F1)) || (si_piece(ROI,BLANC,G1))))
        score -= 50;
    if(e_valide)
        printf("Score patterns blanc : %d\n",score);
    return score;
}
int pn()
{
    int score = 0;
    //fou a2 , pion adverse b3
    if((si_piece(FOU,NOIR,A2)) && (si_piece(PION,BLANC,B3)))
        score -= 100;
    //fou b1 , pion adverse c2
    if((si_piece(FOU,NOIR,B1)) && (si_piece(PION,BLANC,C2)))
        score -= 100;
    //fou h2 , pion adverse g3
    if((si_piece(FOU,NOIR,H2)) && (si_piece(PION,BLANC,G3)))
        score -= 100;
    //fou g1 , pion adverse f2
    if((si_piece(FOU,NOIR,G1)) && (si_piece(PION,BLANC,F2)))
        score -= 100;
    //fou a3 , pion adverse b4
    if((si_piece(FOU,NOIR,A3)) && (si_piece(PION,BLANC,B4)))
        score -= 100;
    //fou c8 , pion noir d7 , case d6 occupée
    if((si_piece(FOU,NOIR,C8)) && (si_piece(PION,NOIR,D7)) && (piece[D6] != VIDE))
        score -= 50;
    //fou f8 , pion blanc e7 , case e6 occupée
    if((si_piece(FOU,NOIR,F8)) && (si_piece(PION,NOIR,E7)) && (piece[E6] != VIDE))
        score -= 50;
    //tour a8 ou a7 ou b8 , roi b8 ou c8
    if(((si_piece(TOUR,NOIR,A8)) || (si_piece(TOUR,NOIR,A7)) || (si_piece(TOUR,NOIR,B8))) &&
            ((si_piece(ROI,NOIR,B8)) || (si_piece(ROI,NOIR,C8))))
        score -= 50;
    //tour h8 ou h7 ou g8 , roi f8 ou g8
    if(((si_piece(TOUR,NOIR,H8)) || (si_piece(TOUR,NOIR,H7)) || (si_piece(TOUR,NOIR,G8))) &&
            ((si_piece(ROI,NOIR,F8)) || (si_piece(ROI,NOIR,G8))))
        score -= 50;
    if(e_valide)
        printf("Score patterns noir : %d\n",score);
    return score;
}
//Routine reconnaissance d'une position nulle//
//Pour l'instant prises en compte
//K+K , K+KB , K+KN
//KB+KB , KB+KN , KN+KB , KN+KN
//A VOIR 	: KR+KB , KR+KN	...
int draw_recognizer()
{
    //printf("%d  %d \n",materiel[BLANC],materiel[NOIR]);

    if((materiel[BLANC] == VAL_ROI) && (materiel[NOIR] == VAL_ROI))
        return VRAI;
    if((nb_pieces[BLANC][PION] | nb_pieces[NOIR][PION]) == 0)
    {
        if((materiel[BLANC] < VAL_NULLE) && (materiel[NOIR] < VAL_NULLE))
            return VRAI;
    }
    return FAUX;
}

//Routine reconnaissance finale KBN vs K
bool finale_KBNK()
{
    if(((nb_pieces[BLANC][PION] | nb_pieces[NOIR][PION]) == 0) &&
            ((nb_pieces[BLANC][DAME] | nb_pieces[NOIR][DAME]) == 0) &&
            ((nb_pieces[BLANC][TOUR] | nb_pieces[NOIR][TOUR]) == 0))
    {
        if((materiel[BLANC] == VAL_ROI) && (nb_pieces[NOIR][FOU] == 1) &&
                (nb_pieces[NOIR][CAVALIER] == 1))
            return VRAI;
        else if((materiel[NOIR] == VAL_ROI) && (nb_pieces[BLANC][FOU] == 1) &&
                (nb_pieces[BLANC][CAVALIER] == 1))
            return VRAI;
        else
            return FAUX;
    }
    return FAUX;
}

//Routine reconnaissance finale KBB vs K//
bool finale_KBBK()
{
    if(((nb_pieces[BLANC][PION] | nb_pieces[NOIR][PION]) == 0) &&
            ((nb_pieces[BLANC][DAME] | nb_pieces[NOIR][DAME]) == 0) &&
            ((nb_pieces[BLANC][TOUR] | nb_pieces[NOIR][TOUR]) == 0) &&
            ((nb_pieces[BLANC][CAVALIER] | nb_pieces[NOIR][CAVALIER]) == 0))
    {
        if((materiel[BLANC] == VAL_ROI) && (nb_pieces[NOIR][FOU] == 2))
            return VRAI;
        else if((materiel[NOIR] == VAL_ROI) && (nb_pieces[BLANC][FOU] == 2))
            return VRAI;
        else
            return FAUX;
    }
    return FAUX;
}

//test si pion passé colonne a ou h et roi dépouillé//
//et roi dépouillé case promo ou distance promo == 1 ---> draw
bool draw_passer_ah()
{
    //si K vs KP
    if((materiel[BLANC] == VAL_ROI) && (materiel[NOIR] == VAL_KP))
    {
        //si le pion est sur la colonne a ou h
        if((bitboard_pion[NOIR] & bitboard_colonne[0]) &&
                (chebi[pos_roi[BLANC]][sq_promo_noir[0]] <= 1))
            return VRAI;
        else if((bitboard_pion[NOIR] & bitboard_colonne[7]) &&
                (chebi[pos_roi[BLANC]][sq_promo_noir[7]] <= 1))
            return VRAI;
        else
            return FAUX;
    }
    else if((materiel[NOIR] == VAL_ROI) && (materiel[BLANC] == VAL_KP))
    {
        //si le pion est sur la colonne a ou h
        if((bitboard_pion[BLANC] & bitboard_colonne[0]) &&
                (chebi[pos_roi[NOIR]][sq_promo_blanc[0]] <= 1))
            return VRAI;
        else if((bitboard_pion[BLANC] & bitboard_colonne[7]) &&
                (chebi[pos_roi[NOIR]][sq_promo_blanc[7]] <= 1))
            return VRAI;
        else
            return FAUX;
    }
    return FAUX;
}

int nb_atk_case(int sqd, int current_side, int sqp)
{
    int k = sqd;    //case devant le pion passé
    int p = sqp;    //case du pion passé
    int h,y;
    int row = ROW(k);
    int col = COL(k);
    int xside = OPP(current_side);
    int score = 0;
    int val[7] = {0, 9, 7, 7, 5, 3, 1};

    //-----------------------------------------------------------------------------------------------------------------
    //                                  coups d'échecs cavalier
    if (col > 0 && row > 1 && couleur[k - 17] == xside && piece[k - 17] == CAVALIER)
        score += val[CAVALIER];
    if (col < 7 && row > 1 && couleur[k - 15] == xside && piece[k - 15] == CAVALIER)
        score += val[CAVALIER];
    if (col > 1 && row > 0 && couleur[k - 10] == xside && piece[k - 10] == CAVALIER)
        score += val[CAVALIER];
    if (col < 6 && row > 0 && couleur[k - 6] == xside && piece[k - 6] == CAVALIER)
        score += val[CAVALIER];
    if (col > 1 && row < 7 && couleur[k + 6] == xside && piece[k + 6] == CAVALIER)
        score += val[CAVALIER];
    if (col < 6 && row < 7 && couleur[k + 10] == xside && piece[k + 10] == CAVALIER)
        score += val[CAVALIER];
    if (col > 0 && row < 6 && couleur[k + 15] == xside && piece[k + 15] == CAVALIER)
        score += val[CAVALIER];
    if (col < 7 && row < 6 && couleur[k + 17] == xside && piece[k + 17] == CAVALIER)
        score += val[CAVALIER];
    /* Check horizontal and vertical lines for attacROI of DAME, TOUR, ROI */
    /* go down */
    y = k + 8;
    if (y < 64)
    {
        if (couleur[y] == xside && (piece[y] == ROI || piece[y] == DAME || piece[y] == TOUR))
            score += val[piece[y]];
        if (piece[y] == VIDE || y == sqp)
        {
            for (y += 8; y < 64; y += 8)
            {
                if (couleur[y] == xside && (piece[y] == DAME || piece[y] == TOUR))
                    score += val[piece[y]];
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
            score += val[piece[y]];
        if (piece[y] == VIDE || y ==sqp)
        {
            for (y--; y >= h; y--)
            {
                if (couleur[y] == xside && (piece[y] == DAME || piece[y] == TOUR))
                    score += val[piece[y]];
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
            score += val[piece[y]];
        if (piece[y] == VIDE || y == sqp)
        {
            for (y++; y <= h; y++)
            {
                if (couleur[y] == xside && (piece[y] == DAME || piece[y] == TOUR))
                    score += val[piece[y]];
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
            score += val[piece[y]];
        if (piece[y] == VIDE || y == sqp)
        {
            for (y -= 8; y >= 0; y -= 8)
            {
                if (couleur[y] == xside && (piece[y] == DAME || piece[y] == TOUR))
                    score += val[piece[y]];
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
                score += val[piece[y]];
            if (current_side == NOIR && piece[y] == PION)
                score += val[PION];
        }
        if (piece[y] == VIDE || y == sqp)
        {
            for (y += 9; y < 64 && COL(y) != 0; y += 9)
            {
                if (couleur[y] == xside && (piece[y] == DAME || piece[y] == FOU))
                    score += val[piece[y]];
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
                score += val[piece[y]];
            if (current_side == NOIR && piece[y] == PION)
                score += val[PION];
        }
        if (piece[y] == VIDE || y == sqp)
        {
            for (y += 7; y < 64 && COL(y) != 7; y += 7)
            {
                if (couleur[y] == xside && (piece[y] == DAME || piece[y] == FOU))
                    score += val[piece[y]];
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
                score += val[piece[y]];
            if (current_side == BLANC && piece[y] == PION)
                score += val[PION];
        }
        if (piece[y] == VIDE || y == sqp)
        {
            for (y -= 9; y >= 0 && COL(y) != 7; y -= 9)
            {
                if (couleur[y] == xside && (piece[y] == DAME || piece[y] == FOU))
                    score += val[piece[y]];
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
                score += val[piece[y]];
            if (current_side == BLANC && piece[y] == PION)
                score += val[PION];
        }
        if (piece[y] == VIDE || y == sqp)
        {
            for (y -= 7; y >= 0 && COL(y) != 0; y -= 7)
            {
                if (couleur[y] == xside && (piece[y] == DAME || piece[y] == FOU))
                    score += val[piece[y]];
                if (piece[y] != VIDE)
                    break;
            }
        }
    }
    return score;
}

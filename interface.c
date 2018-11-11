#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "board.h"
#include "recherche.h"
#include "hash.h"
#include "temps.h"
#include "bitboards.h"
#include "eval.h"
#include "deftypes.h"
#include "interface.h"
#include "progjoue.h"
#include "fen.h"
#include "genecoups.h"
#include "hash.h"


void affiche_echiquier()
{
    char            pieceName[] = " PNBRQKpnbrqk";
    int             i;

    for (i = 0; i < 64; i++)
    {
        if ((i & 7) == 0)
        {
            printf("   +---+---+---+---+---+---+---+---+\n");
            if (i <= 56)
            {
                printf(" %d |", 8 - (((unsigned)i) >> 3));
            }
        }
        if (piece[i] == VIDE)
            printf("   |");
        else
        {
            printf(" %c |", pieceName[piece[i] + (couleur[i] == BLANC ? 0 : 6)]);
        }
        if ((i & 7) == 7)
            printf("\n");
    }
    printf("   +---+---+---+---+---+---+---+---+\n     a   b   c   d   e   f   g   h\n\n");

    position_to_fen();
}


void affiche_infos()
{
    printf("------------------------------------------------\n");
    printf("PHASE :  %d\n",calcul_phase());
    printf("------------------------------------------------\n");
    printf("Materiel               : %5d  %5d\n",materiel[BLANC],materiel[NOIR]);
    printf("Nombre de coups        : %4d\n",hdp);
    printf("cinquante              : %4d\n",cinquante);
    printf("ep                     : %4d\n",ep);
    printf("Couleur qui joue       : %4d\n",side);
    printf("Profondeur             : %4d\n",max_depth);
    printf("roque                  : %2d\n",castle);
    printf("------------------------------------------------\n");
    printf("hashcode               : %016llX\n",hash_code_position);
    printf("Index                  : %d\n",(int)(hash_code_position % HASH_TABLE_SIZE));
    printf("Positions_testees  : %10d\n",positions_testees);
    printf("Positions_trouvees : %10d\n",positions_trouvees);
    printf("Positions_sauvees  : %10d\n",positions_sauvees);
    printf("\n------------------------------------------------\n");
    printf("Bitboards pions : %016llX    %016llX\n",bitboard_pion[BLANC],bitboard_pion[NOIR]);
    printf("Hashcode pions         : %016llX\n",hash_code_pions);
    printf("------------------------------------------------\n");
}

char *coord_une_case(int sq)
{
    static char str[3];

    sprintf(str,"%c%d",COL(sq) + 'a', 8 - ROW(sq));

    return str;
}

char *move_str(MOVE m)
{
    static char str[5];

    sprintf(str,"%c%d%c%d",COL(m.from) + 'a', 8 - ROW(m.from),COL(m.dest) + 'a', 8 - ROW(m.dest));
    if(m.type > NORMAL)
    {
        switch(m.type)
        {
        case PROMO_CAVALIER:
            strcat(str,"N");
            break;
        case PROMO_FOU:
            strcat(str,"B");
            break;
        case PROMO_TOUR:
            strcat(str,"R");
            break;
        case PROMO_DAME:
            strcat(str,"Q");
            break;
        default:
            break;
        }
    }
    return str;
}


int verif_coup(char *s)
{
    int from,dest;
    MOVE moveBuf[200];
    int movecnt;
    int i;

    from = s[0] - 'a';
    from += 8 * (8 - (s[1] - '0'));
    dest = s[2] - 'a';
    dest += 8 * (8 - (s[3] - '0'));

    prof = 0;
    movecnt = gen_coups(side, moveBuf);
    /* loop through the moves to see if it's legal */
    for (i = 0; i < movecnt; i++)
        if (moveBuf[i].from == from && moveBuf[i].dest == dest)
        {
            if (piece[from] == PION && (dest < 8 || dest > 55))     /* Promotion move? */
            {
                switch (s[4])
                {
                case 'q':
                    moveBuf[i].type = PROMO_DAME;
                    break;

                case 'r':
                    moveBuf[i].type = PROMO_TOUR;
                    break;

                case 'b':
                    moveBuf[i].type = PROMO_FOU;
                    break;

                case 'n':
                    moveBuf[i].type = PROMO_CAVALIER;
                    break;
                default:
                    moveBuf[i].type = PROMO_DAME;
                }
            }
            if (jouer_coup(moveBuf[i]))
            {
                return ILLEGAL;
            }
            else
            {
                return LEGAL;
            }
            break;
        }

    return 0;
}


void affiche_liste()
{
    MOVE moveBuf[200];
    int  movecnt;
    int i;
    int temps_debut,temps_fin;

    movecnt = gen_coups(side, moveBuf);//gen_coups(side, moveBuf);

    printf("index   coup      type    piece    ep type     couleur    evaluation  \n");
    printf("-------------------------------------------------------------\n");
    for (i = 0; i < movecnt; ++i)
    {
        printf("%2d   %c%d%c%d      %2d        %d         %d         %d       %d\n",i,'a' + COL(moveBuf[i].from),
               8 - ROW(moveBuf[i].from),
               'a' + COL(moveBuf[i].dest),
               8 - ROW(moveBuf[i].dest),moveBuf[i].type,
               piece[moveBuf[i].from],moveBuf[i].ep_flag,couleur[moveBuf[i].from],moveBuf[i].evaluation);
    }
    liste_valide = FAUX;
}

void affiche_captures()
{
    MOVE            moveBuf[200];
    int             movecnt;
    int i;

    movecnt = gen_captures_promos(side, moveBuf);
    printf("coup      type    piece    ep type   couleur     evaluation\n");
    printf("-----------------------------------------------------------\n");
    for (i = 0; i < movecnt; ++i)
    {
        printf("%c%d%c%d      %2d        %d         %d        %d      %d\n",'a' + COL(moveBuf[i].from),
               8 - ROW(moveBuf[i].from),
               'a' + COL(moveBuf[i].dest),
               8 - ROW(moveBuf[i].dest),moveBuf[i].type,
               piece[moveBuf[i].from],moveBuf[i].ep_flag,couleur[moveBuf[i].from],moveBuf[i].evaluation);
    }
}

void affiche_resultat()
{
    MOVE moveBuf[200];
    int movecnt;
    int i;
    int legal = FAUX;
    int rep;

    PHASE = calcul_phase();

    //-----------------------------------------------------------------
    //   on vérifie si la partie est gagnée par un joueur ou nulle
    //-----------------------------------------------------------------
    movecnt = gen_coups(side, moveBuf);

    for (i = 0; i < movecnt; ++i)
    {
        if (jouer_coup(moveBuf[i]))
        {
            continue;
        }
        legal = VRAI;
        dejouer_coup();
        break;
    }

    if(!legal)
    {
        if(roi_attaque(pos_roi[side], side))
        {
            if (side == BLANC)
            {
                printf("0-1 {Les blancs sont mat}\n");
                computer_side = NONE;
            }
            else
            {
                printf("1-0 {Les noirs sont mat}\n");
                computer_side = NONE;
            }
        }
        else
        {
            printf("1/2-1/2 {Pat}\n");
            computer_side = NONE;
        }
    }

    //-----------------------------------------------------------------
    //               on vérifie la règle de triple réptition
    //-----------------------------------------------------------------

    rep = triple_repetition();
    if(rep == 3)
    {
        printf("La partie est nulle par triple repetition\n");
        computer_side = NONE;
    }

    //-----------------------------------------------------------------
    //               on vérifie la règle des 50 coups
    //-----------------------------------------------------------------
    if(cinquante >= 100)
    {
        printf("La partie est nulle par la règle des cinquante coups\n");
        computer_side = NONE;
    }

    //-----------------------------------------------------------------
    //            on vérifie la nulle par matériel insufisant
    //-----------------------------------------------------------------
    if(draw_recognizer())
    {
        printf("La partie est nulle par materiel insufisant\n");
    }
}


U64 perft (int p)
{
    int i;
    int movecnt;
    U64 nodes = 0;

    if (!p)
        return 1;
    MOVE moveBuf[200];
    movecnt = gen_coups(side, moveBuf);
    for (i = 0; i < movecnt; ++i)
    {
        if (jouer_coup(moveBuf[i]))
        {
            continue;
        }
        nodes += perft (p - 1);
        dejouer_coup();
    }
    return nodes;
}


void affiche_infos_coup(MOVE m)
{
    printf("=========================================\n");
    printf("coup     couleur    piece    type   \n");
    printf("%s       %d         %d       %d\n",move_str(m),couleur[m.from],piece[m.from],m.type);
    printf("=========================================\n");

}

void print_champ_roi(int sq)
{
    int i;
    for (i = 0; i < 64; i++)
    {
        if ((i & 7) == 0)
        {
            printf("   +---+---+---+---+---+---+---+---+\n");
            if (i <= 56)
            {
                printf(" %d |", 8 - (((unsigned)i) >> 3));
            }
        }
        printf(" %d |",champ_roi[sq][i]);
        if ((i & 7) == 7)
            printf("\n");
    }
    printf("   +---+---+---+---+---+---+---+---+\n     a   b   c   d   e   f   g   h\n\n");
}

void affiche_hash_move()
{
    HASHE *phashe = &tt[hash_code_position % HASH_TABLE_SIZE];;
    MOVE *b = phashe->best;

    printf("hashcode   : %016llX\n",phashe->key);
    printf("profondeur : %d\n",phashe->depth);
    printf("flag       : %d\n",phashe->flag);
    printf("valeur     : %d\n",phashe->value);
    printf("coup: \n");
    printf("from et dest             : %d  %d \n",b->from,b->dest);
    printf("piece from et piece dest : %d  %d\n",b->piece_from,b->piece_dest);
    printf("type coup                : %d\n",b->type);
    printf("ep flag                  : %d\n",b->ep_flag);
    printf("evaluation               : %d\n",b->evaluation);
    printf("killer code              : %d\n",b->killer_code);


}

void affiche_plist()
{
    char            pieceName[] = " PNBRQKpnbrqk";
    int             i;

    for (i = 0; i < 64; i++)
    {
        if ((i & 7) == 0)
        {
            printf("    +----+----+----+----+----+----+----+----+\n");
            if (i <= 56)
            {
                printf(" %2d |", 8 - (((unsigned)i) >> 3));
            }
        }
        if (indices[i] == VIDE)
            printf(" .. |");
        else
        {
            printf(" %2d |", indices[i]);
        }
        if ((i & 7) == 7)
            printf("\n");
    }
    printf("    +----+----+----+----+----+----+----+----+\n     a   b   c   d   e   f   g   h\n\n");

    for(i=0;i<16;++i)
        printf(" %d ",plist[BLANC][i]);
    printf("\n");
    for(i=0;i<16;++i)
        printf(" %d ",plist[NOIR][i]);
    printf("\n");
}


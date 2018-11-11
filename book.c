#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "board.h"
#include "recherche.h"
#include "hash.h"
#include "temps.h"
#include "book.h"
#include "eval.h"
#include "bitboards.h"
#include "interface.h"
#include "edit.h"
#include "inits.h"
#include "genecoups.h"
#include "progjoue.h"

int biblio()
{
    char     p[65];     /*positions du fichier livre.txt*/
    char     c[5];      /*coup jouable à partir d'une position p*/
    char     e[65]="";  /*position sur l'echiquier de la partie en cours*/
    char     pieceName[] = " PCFTDRpcftdr";
    int      i;
    int      x = -1;
    int      k;
    int      valeur = -10000;
    int      note_coup;
    int      occur;
    int      pos = 0;

    srand(time(NULL));

    FILE *bib;       /*pointeur fichier bibliotheque d'ouvertures*/
    bib = fopen("graham_book.txt","r");
    if(!bib)
    {
        printf("No book found \n");
        return FAUX;
    }
    /*scan de la position et copie dans e*/
    for (i=0; i<64; ++i)
    {
        x++;
        if (piece[i] > VIDE)
        {
            e[x] = pieceName[piece[i]+(couleur[i] == BLANC ? 0 : 6)];
        }
        else
        {
            e[x] = '.';
        }
    }
    //printf("position : %s\n",e);

    k = feof(bib);
    while(!k)
    {
        fscanf(bib,"%s",p);
        pos++;
        fscanf(bib,"%s",c);
        k = feof(bib);
        occur = 0;
        for(i=0; i<=63; i++)
        {
            if(p[i] == e[i])
                occur++;
        }
        if(occur == 64)
        {
            note_coup = ((rand() * rand()) / rand());
            printf("%s  --->  %d  ----> Bibliotheque\n",c,note_coup);
            if(note_coup > valeur)
            {
                valeur = note_coup;
                cd = c[0] - 'a';
                cd += 8 * (8 - (c[1] - '0'));
                ca = c[2] - 'a';
                ca += 8 * (8 - (c[3] - '0'));
            }
        }
    }

    printf("%d positions dans la bibliotheque\n",pos);

    fclose(bib);
    if(valeur != -10000)
        return VRAI;
    else
        return FAUX;
}

void scan_biblio()
{
    char     p[65];     /*positions du fichier livre.txt*/
    char     c[5];      /*coup jouable à partir d'une position p*/
    FILE *bib;       /*pointeur fichier bibliotheque d'ouvertures*/
    FILE *bob;
    int k;
    int i;
    int prb = 0,grb = 0,prn = 0,grn = 0;
    int dep = 0,arr = 0;
    MOVE moveBuf[200];
    int movecnt;
    int coup_a_jouer;
    int bestScore;

    computer_side = VIDE;

    bib = fopen("YODAbiblio.txt","r"); //ouvre le fichier biblio en lecture
    bob = fopen("ISAbook.txt","a");
    k = feof(bib);

    while(!k)
    {
        init();
        vider_l_echiquier();
        fscanf(bib,"%s",p);   //lecture position
        printf("%s\n",p);
        fscanf(bib,"%s",c);   //lecture du coup
        printf("%s\n",c);
        k = feof(bib);//scanf("%d",&x);
        //copie de la position
        for(i=0; i<64; ++i)
        {
            switch(p[i])
            {
            case 'P':
                piece[i] = PION;
                couleur[i] = BLANC;
                nb_pieces[BLANC][PION]++;
                bitboard_pion[BLANC] ^= pion_sur_case[i];
                break;
            case 'C':
                piece[i] = CAVALIER;
                couleur[i] = BLANC;
                nb_pieces[BLANC][CAVALIER]++;
                break;
            case 'F':
                piece[i] = FOU;
                couleur[i] = BLANC;
                nb_pieces[BLANC][FOU]++;
                break;
            case 'T':
                piece[i] = TOUR;
                couleur[i] = BLANC;
                nb_pieces[BLANC][TOUR]++;
                break;
            case 'D':
                piece[i] = DAME;
                couleur[i] = BLANC;
                nb_pieces[BLANC][DAME]++;
                break;
            case 'R':
                piece[i] = ROI;
                couleur[i] = BLANC;
                pos_roi[BLANC] = i;
                nb_pieces[BLANC][ROI]++;
                break;
            case 'p':
                piece[i] = PION;
                couleur[i] = NOIR;
                bitboard_pion[NOIR] ^= pion_sur_case[i];
                nb_pieces[NOIR][PION]++;
                break;
            case 'c':
                piece[i] = CAVALIER;
                couleur[i] = NOIR;
                nb_pieces[NOIR][CAVALIER]++;
                break;
            case 'f':
                piece[i] = FOU;
                couleur[i] = NOIR;
                nb_pieces[NOIR][FOU]++;
                break;
            case 't':
                piece[i] = TOUR;
                couleur[i] = NOIR;
                nb_pieces[NOIR][TOUR]++;
                break;
            case 'd':
                piece[i] = DAME;
                couleur[i] = NOIR;
                nb_pieces[NOIR][DAME]++;
                break;
            case 'r':
                piece[i] = ROI;
                couleur[i] = NOIR;
                pos_roi[NOIR] = i;
                nb_pieces[NOIR][ROI]++;
                break;
            case '.':
                break;
            default:
                break;
            }
        }
        //on determine "castle"
        if(pos_roi[BLANC] == E1)
        {
            if(si_piece(TOUR, BLANC, H1))
                prb = 1;
            if(si_piece(TOUR, BLANC, A1))
                grb = 2;
        }
        if(pos_roi[NOIR] == E8)
        {
            if(si_piece(TOUR, NOIR, H8))
                prn = 4;
            if(si_piece(TOUR, NOIR, A8))
                grn = 8;
        }
        castle = (prb ^ grb ^ prn ^ grn);
        hash_code_position = init_code_position();
        //determine la phase de jeu
        PHASE = calcul_phase();
        //determine la case depart et d'arrivee du coup
        dep = c[0] - 'a';
        dep += 8 * (8 - (c[1] - '0'));
        arr = c[2] - 'a';
        arr += 8 * (8 - (c[3] - '0'));
        //determine la couleur qui joue
        side = couleur[dep];
        //jouer le coup déterminé
        movecnt = gen_coups(side, moveBuf);
        coup_a_jouer = -1;
        for (i = 0; i < movecnt; i++)
        {
            if (moveBuf[i].from == dep && moveBuf[i].dest == arr)
            {
                coup_a_jouer = i;
                break;
            }
        }
        if(coup_a_jouer < 0)
            printf("y a un bleme !\n");
        else
            jouer_coup(moveBuf[coup_a_jouer]);
        affiche_echiquier();
        affiche_infos();
        //analyser la position issue du coup (5 sec / coup)
        computer_side = side;
        bestScore = programme_teste(max_depth, 1);
        computer_side = VIDE;
        printf("score = %4d\n",bestScore);
        //score traduit du point de vue du camp qui a joué le coup de la biblio
        bestScore = -(bestScore);
        //test si l'analyse a été bonne , on garde la position et le coup
        if(bestScore > -10)
        {
            fprintf(bob,"%s\n",p);
            fprintf(bob,"%s\n",c);
            //fprintf(bob,"%3d\n",bestScore);
        }
    }
    fclose(bob);
    fclose(bib);

}

void vider_l_echiquier()
{
    int i;

    memset(materiel, 0, sizeof(materiel));
    memset(nb_pieces, 0, sizeof(nb_pieces));

    for(i=0; i<64; ++i)
    {
        couleur[i] = VIDE;
        piece[i] = VIDE;
    }
    ep = -1;
    castle = 0;
    bitboard_pion[BLANC] = 0;
    bitboard_pion[NOIR] = 0;
    hash_code_position = 0;
}


int programme_teste(int max_depth, int output)
{
    MOVE m;
    MOVE tmp;
    int score;
    int i,j;
    int temps_intermediaire;
    int bestScore;

    memset(tt, 0, sizeof(tt));
    memset(history, 0, sizeof(history));
    memset(pv, 0, sizeof(pv));

    printf("Prof Score   Noeuds   Temps   PV\n");
    printf("-----------------------------------------------\n");

    //--------------------------------------------------------------------------------
    //                  calcul du temps imparti pour jouer son coup
    //--------------------------------------------------------------------------------
    debut = get_ms();
    fin = debut + temps_max;

    fin_recherche = FAUX;

    printf("temps max : %d\n",temps_max);

    positions_testees = 0;
    positions_trouvees = 0;
    positions_sauvees = 0;

    for(i=1;i<=max_depth;++i)
    {
        prof = 0;
        nodes = 0;

        follow_pv = VRAI;
        score = pvs(-MATE, MATE, i, &m, 0);

        temps_intermediaire = get_ms();
        if(fin_recherche)
            break;
        bestScore = score;
        tmp = m;

        if(output == 1) // mode console
        {
            //printf("%d  %d \n",temps_intermediaire,debut);
            float val2 = ((float)(temps_intermediaire - debut)/1000);
            printf("%3d  %-+3d  %2.2f  %15d",i,score,val2,nodes);
            for (j = 0; j < long_pv[0]; ++j)
				printf(" %s", move_str(pv[0][j]));
			printf("\n");
        }
        if((score > 90000) || (score < -90000))
            break;
    }
    printf("-----------------------------------------------\n");
    fin = get_ms();
    return bestScore;
}

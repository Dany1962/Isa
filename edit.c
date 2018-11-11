#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "board.h"
#include "recherche.h"
#include "hash.h"
#include "eval.h"
#include "bitboards.h"
#include "deftypes.h"
#include "edit.h"
#include "interface.h"

//----------------------------------------------------------------------------------------------------------------
//                              entrée d'une position (manuellement)
//----------------------------------------------------------------------------------------------------------------
void edit()
{
    int coul = BLANC;           //par défaut couleur BLANC
    int d;                      //case selectionnée par l'utilisateur
    char s[256];                //chaine entrée par l'utilisateur
    bool fin_saisie = FAUX;     //booléen de fin de saisie
    int i;
    int prb = 0,grb = 0,prn = 0,grn = 0;

    e_valide = FAUX;

    for(;;)                     //boucle infinie de saisie
    {
        //affiche_echiquier();
        //affiche_infos();
        scanf("%s", s);
        d = s[1] - 'a';
        d += 8 * (8 - (s[2] - '0'));
        switch(s[0])
        {
        case 'P':
            if(nb_pieces[coul][PION] == 8)
                printf("deja 8 pions , impossible !\n");
            else
            {
                vider_sq(couleur[d], piece[d], d);
                occuper_sq(coul, PION, d);
            }
            break;
        case 'N':
            vider_sq(couleur[d], piece[d], d);
            occuper_sq(coul, CAVALIER, d);
            break;
        case 'B':
            vider_sq(couleur[d], piece[d], d);
            occuper_sq(coul, FOU, d);
            break;
        case 'R':
            vider_sq(couleur[d], piece[d], d);
            occuper_sq(coul, TOUR, d);
            break;
        case 'Q':
            vider_sq(couleur[d], piece[d], d);
            occuper_sq(coul, DAME, d);
            break;
        case 'K':
            vider_sq(couleur[d], piece[d], d);
            occuper_sq(coul, ROI, d);
            pos_roi[coul] = d;
            break;
        case '.':
            ep = -1;
            fin_saisie = VRAI;
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
            break;
        case 'x':
            vider_sq(couleur[d], piece[d], d);
            break;
        case '#':
            for(i=0; i<64; ++i)
            {
                vider_sq(couleur[i], piece[i], i);
            }
            ep = -1;
            castle = 0;
            bitboard_pion[BLANC] = 0;
            bitboard_pion[NOIR] = 0;
            break;
        case 'c':
            if(coul == BLANC)
                coul = NOIR;
            else
                coul = BLANC;
            break;
        default :
            break;
        }
        if(fin_saisie)
            break;

        affiche_echiquier();
        affiche_infos();
    }
    hdp = 1;
}


void vider_sq(int c, int p, int sq)
{
    if(piece[sq] != VIDE)
    {
        //update plist
        plist[c][indices[sq]] = VIDE;
        indices[sq] = VIDE;
        if(piece[sq] == ROI)
        {
            pos_roi[c] = -1;
        }
        if(piece[sq] == PION)
        {
            bitboard_pion[couleur[sq]] ^= pion_sur_case[sq];

        }
        nb_pieces[c][p]--;
        materiel[c] -= valeur_piece[piece[sq]];
        piece[sq]  = VIDE;
        couleur[sq]  = VIDE;
    }
}

void occuper_sq(int c, int p, int sq)
{
    int index;
    int c1,p1,sq1;

    //enlève l'ancienne pièce du jeu (si besoin)
    if(piece[sq] != VIDE)
    {
        c1 = couleur[sq];
        p1 = piece[sq];
        sq1 = sq;
        vider_sq(c1, p1, sq1);
    }

    //place la nouvelle pièce
    piece[sq] = p;
    couleur[sq] = c;
    nb_pieces[c][p]++;
    materiel[c] += valeur_piece[p];
    //update plist
    index = (ind_start[p] + (nb_pieces[c][p] - 1));
    plist[c][index] = sq;
    indices[sq] = index;
    if(p == PION)
        bitboard_pion[c] ^= pion_sur_case[sq];
    if(p == ROI)
        pos_roi[c] = sq;
}


int si_piece(int p, int c, int k)
{
    return(((piece[k] == p) && (couleur[k] == c)) ? VRAI : FAUX);  //optimisation voir gprof1
}


void set_board(char *fen)
{

}

void reinit_plist()
{
    int i;

}

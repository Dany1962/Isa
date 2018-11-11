#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "board.h"
#include "recherche.h"
#include "interface.h"

void position_to_fen()
{
    int x, y, l=0, i=0, sq;
    char  ROW[8];
    char string[256];

    strcpy(string,"");

    for (y=0; y<8; y++)
    {
        i=l=0;
        strcpy(ROW,"");
        for (x=0; x<8; x++)
        {
            sq = (y*8)+x;
            if (couleur[sq] == VIDE) l++;
            else
            {
                if (l>0)
                {
                    ROW[i] = (char) (l+48);
                    i++;
                }
                l=0;
                if (couleur[sq] == BLANC)
                {
                    switch (piece[sq])
                    {
                    case PION	 :
                        ROW[i]='P';
                        break;
                    case CAVALIER :
                        ROW[i]='N';
                        break;
                    case FOU	 :
                        ROW[i]='B';
                        break;
                    case TOUR	 :
                        ROW[i]='R';
                        break;
                    case DAME	 :
                        ROW[i]='Q';
                        break;
                    case ROI	 :
                        ROW[i]='K';
                        break;
                    }
                }
                else
                {
                    switch (piece[sq])
                    {
                    case PION	 :
                        ROW[i]='p';
                        break;
                    case CAVALIER :
                        ROW[i]='n';
                        break;
                    case FOU	 :
                        ROW[i]='b';
                        break;
                    case TOUR	 :
                        ROW[i]='r';
                        break;
                    case DAME	 :
                        ROW[i]='q';
                        break;
                    case ROI	 :
                        ROW[i]='k';
                        break;
                    }
                }
                i++;
            }
        }
        if (l>0)
        {
            ROW[i] = (char) (l+48);
            i++;
        }
        strncat(string,ROW,i);
        if (y<7) strcat(string,"/");
    }

    if (side == BLANC)
    {
        strcat(string," w ");
    }
    else
    {
        strcat(string," b ");
    }


    if(castle & 1)
    {
        strcat(string,"K");
    }
    if(castle & 2)
    {
        strcat(string,"Q");
    }
    if(castle & 4)
    {
        strcat(string,"k");
    }
    if(castle & 8)
    {
        strcat(string,"q");
    }
    if(!castle)
        strcat(string,"-");

    strcat(string," ");

    if (ep == -1)
    {
        strcat(string,"-");
    }
    else
    {
       strcat(string,coord_une_case(ep));
    }

    strcat(string," ");

    printf("%s\n",string);
}

void fen_to_position(char *fen)
{
	int n = strlen(fen);
	int i;
	int sq = 0;
	int z;
	int a = 0;

	for(i=0;i<64;++i)
	{
		couleur[i] = VIDE;
		piece[i] = VIDE;
	}
	/*
	---------------------------------------------------------
	*														*
	*					Pièces en jeu						*
	*														*
	---------------------------------------------------------
	*/
	for(i=0,z=0;i<n && z==0;++i)
	{
		switch(fen[i])
		{
			case '1': sq += 1;break;
			case '2': sq += 2;break;
			case '3': sq += 3;break;
			case '4': sq += 4;break;
			case '5': sq += 5;break;
			case '6': sq += 6;break;
			case '7': sq += 7;break;
			case '8': sq += 8;break;
			case 'p': piece[sq] = PION;couleur[sq] = NOIR;++sq;break;
			case 'n': piece[sq] = CAVALIER;couleur[sq] = NOIR;++sq;break;
			case 'b': piece[sq] = FOU;couleur[sq] = NOIR;++sq;break;
			case 'r': piece[sq] = TOUR;couleur[sq] = NOIR;++sq;break;
			case 'q': piece[sq] = DAME;couleur[sq] = NOIR;++sq;break;
			case 'k': piece[sq] = ROI;couleur[sq] = NOIR;++sq;break;
			case 'P': piece[sq] = PION;couleur[sq] = BLANC;++sq;break;
			case 'N': piece[sq] = CAVALIER;couleur[sq] = BLANC;++sq;break;
			case 'B': piece[sq] = FOU;couleur[sq] = BLANC;++sq;break;
			case 'R': piece[sq] = TOUR;couleur[sq] = BLANC;++sq;break;
			case 'Q': piece[sq] = DAME;couleur[sq] = BLANC;++sq;break;
			case 'K': piece[sq] = ROI;couleur[sq] = BLANC;++sq;break;
			case '/': break;
			default : z = 1 ; break;
		}
		a = i;
	}
	/*
	---------------------------------------------------------
	*														*
	*				Couleur qui joue						*
	*														*
	---------------------------------------------------------
	*/
	side  = -1;
	++a;

	for (i=a, z = 0; i<n && z == 0; ++i)
	{
		switch(fen[i])
		{
			case 'w': side = BLANC; break;
			case 'b': side = NOIR; break;
			default: z = 1; break;
		}
		a = i;
	}
	/*
	---------------------------------------------------------
	*														*
	*							Roques						*
	*														*
	---------------------------------------------------------
	*/
	castle = 0;
	++a;

	castle = 0;

	for (i=a+1, z = 0; i<n && z == 0; ++i)
	{
		switch(fen[i])
		{
			case 'K': castle |= 1; break;
			case 'Q': castle |= 2; break;
			case 'k': castle |= 4; break;
			case 'q': castle |= 8; break;
			case '-': break;
			default: z = 1; break;
		}
		a = i;
	}
	/*
	---------------------------------------------------------
	*														*
	*						En passant						*
	*														*
	---------------------------------------------------------
	*/
	ep = -1;
	++a;

	for (i=a+1, z = 0; i<n && z == 0; ++i)
	{
		switch(fen[i])
		{
			case '-': break;
			case 'a': ep = 0; break;
			case 'b': ep = 1; break;
			case 'c': ep = 2; break;
			case 'd': ep = 3; break;
			case 'e': ep = 4; break;
			case 'f': ep = 5; break;
			case 'g': ep = 6; break;
			case 'h': ep = 7; break;
			case '1': ep += 56; break;
			case '2': ep += 48; break;
			case '3': ep += 40; break;
			case '4': ep += 32; break;
			case '5': ep += 24; break;
			case '6': ep += 16; break;
			case '7': ep +=  8; break;
			case '8': ep +=  0; break;
			default: z = 1; break;
		}
	}
	affiche_echiquier();
	affiche_infos();
}


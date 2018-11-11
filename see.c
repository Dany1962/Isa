#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "board.h"
#include "see.h"
#include "edit.h"
#include "interface.h"

int see(int sq, int coul)
{
    int value = 0;
    int square   = -1;
    int val;

    //printf("entree see : sq = %d , coul = %d \n ",sq,coul);

    if(!gsa[num])
        square = get_smallest_attacker(sq, coul);

    if(square != -1)
    {
        val = val_see[piece[sq]];
        if(liste_valide)
            printf("piece trouvee : %d  %d  val : %d\n",square,piece[square],val);
        make_capture(square, sq, coul);
        coul = OPP(coul);
        gsa[num] = VRAI;
        value = MAX(0,  val - see(sq, coul));
        coul = OPP(coul);
        unmake_capture(square, sq, coul);
        return value;
    }
    else
    {
        gsa[num] = FAUX;
        return value;
    }

}

void make_capture(int from, int to, int coul)
{
    int x;

    ++num;

    //printf("coup dans make : from,to,piece[from],piece[to] , NUM , coul %d %d %d %d %d\n",
    //from,to,piece[from],piece[to],num);

    see_tree[num].fr = from;
    see_tree[num].to = to;
    see_tree[num].pj = piece[from];
    see_tree[num].cp = piece[to];

    piece[to]     = piece[from];
    piece[from]   = VIDE;
    couleur[to]   = coul;
    couleur[from] = VIDE;

    //affiche_echiquier();

    //scanf("%d",&x);
}

void unmake_capture(int from, int to, int coul)
{
    int x;

    piece[from]   = see_tree[num].pj;
    piece[to]     = see_tree[num].cp;
    couleur[to]   = OPP(coul);
    couleur[from] = coul;

    //printf("coup dans unmake : from,to,piece[from],piece[to] , NUM %d %d %d %d %d\n",
    //from,to,piece[from],piece[to],num);

    //affiche_echiquier();

    //scanf("%d",&x);
    --num;
}

int get_smallest_attacker(int sq, int coul)
{
    int col = COL(sq);
    int row = ROW(sq);
    int k,h,y;
    int min_piece = 10;
    int square = -1;

    k = sq;

    //pion le plus petit attaquant------------------------------------------------
    if(coul == BLANC)
    {
        if((col != 0) && si_piece(PION, BLANC, sq+7))
            return (sq+7);
        if((col != 7) && si_piece(PION, BLANC, sq+9))
            return (sq+9);
    }
    else
    {
        if((col != 0) && si_piece(PION, NOIR, sq-9))
            return (sq-9);
        if((col != 7) && si_piece(PION, NOIR, sq-7))
            return (sq-7);
    }
    //cavalier------------------------------------------------------------------------
    y = sq - 6;
    if (y >= 0 && col < 6 && si_piece(CAVALIER, coul, y))
        return y;
    y = sq - 10;
    if (y >= 0 && col > 1 && si_piece(CAVALIER, coul, y))
        return y;
    y = sq - 15;
    if (y >= 0 && col < 7 && si_piece(CAVALIER, coul, y))
        return y;
    y = sq - 17;
    if (y >= 0 && col > 0 && si_piece(CAVALIER, coul, y))
        return y;
    y = sq + 6;
    if (y < 64 && col > 1 && si_piece(CAVALIER, coul, y))
        return y;
    y = sq + 10;
    if (y < 64 && col < 6 && si_piece(CAVALIER, coul, y))
        return y;
    y = sq + 15;
    if (y < 64 && col > 0 && si_piece(CAVALIER, coul, y))
        return y;
    y = sq + 17;
    if (y < 64 && col < 7 && si_piece(CAVALIER, coul, y))
        return y;
    //fou , dame ou roi---------------------------------------------------------------
    /* Check diagonal lines for attacROI of DAME, FOU, ROI, PION */
    /* go right down */
    y = k + 9;
    if (y < 64 && COL(y) != 0)
    {
        if (couleur[y] == coul)
        {
            if (piece[y] == ROI || piece[y] == DAME || piece[y] == FOU)
            {
                if(piece[y] < min_piece)
                {
                    min_piece = piece[y];
                    square = y;
                }
            }
        }
        if (piece[y] == VIDE)
        {
            for (y += 9; y < 64 && COL(y) != 0; y += 9)
            {
                if (couleur[y] == coul && (piece[y] == DAME || piece[y] == FOU))
                {
                    if(piece[y] < min_piece)
                    {
                        min_piece = piece[y];
                        square = y;
                    }
                }
                if (piece[y] != VIDE)
                    break;
            }
        }
    }
    // go left down
    y = k + 7;
    if (y < 64 && COL(y) != 7)
    {
        if (couleur[y] == coul)
        {
            if (piece[y] == ROI || piece[y] == DAME || piece[y] == FOU)
            {
                if(piece[y] < min_piece)
                {
                    min_piece = piece[y];
                    square = y;
                }
            }
        }
        if (piece[y] == VIDE)
        {
            for (y += 7; y < 64 && COL(y) != 7; y += 7)
            {
                if (couleur[y] == coul && (piece[y] == DAME || piece[y] == FOU))
                {
                    if(piece[y] < min_piece)
                    {
                        min_piece = piece[y];
                        square = y;
                    }
                }
                if (piece[y] != VIDE)
                    break;
            }
        }
    }
    // go left up
    y = k - 9;
    if (y >= 0 && COL(y) != 7)
    {
        if (couleur[y] == coul)
        {
            if (piece[y] == ROI || piece[y] == DAME || piece[y] == FOU)
            {
                if(piece[y] < min_piece)
                {
                    min_piece = piece[y];
                    square = y;
                }
            }
        }
        if (piece[y] == VIDE)
        {
            for (y -= 9; y >= 0 && COL(y) != 7; y -= 9)
            {
                if (couleur[y] == coul && (piece[y] == DAME || piece[y] == FOU))
                {
                    if(piece[y] < min_piece)
                    {
                        min_piece = piece[y];
                        square = y;
                    }
                }
                if (piece[y] != VIDE)
                    break;
            }
        }
    }
    // go right up
    y = k - 7;
    if (y >= 0 && COL(y) != 0)
    {
        if (couleur[y] == coul)
        {
            if (piece[y] == ROI || piece[y] == DAME || piece[y] == FOU)
            {
                if(piece[y] < min_piece)
                {
                    min_piece = piece[y];
                    square = y;
                }
            }
        }
        if (piece[y] == VIDE)
        {
            for (y -= 7; y >= 0 && COL(y) != 0; y -= 7)
            {
                if (couleur[y] == coul && (piece[y] == DAME || piece[y] == FOU))
                {
                    if(piece[y] < min_piece)
                    {
                        min_piece = piece[y];
                        square = y;
                    }
                }
                if (piece[y] != VIDE)
                    break;
            }
        }
    }
    //dame , tour , roi---------------------------------------------------------------------
    // go down
    y = k + 8;
    if (y < 64)
    {
        if (couleur[y] == coul && (piece[y] == ROI || piece[y] == DAME || piece[y] == TOUR))
        {
            if(piece[y] < min_piece)
            {
                min_piece = piece[y];
                square = y;
            }
        }
        if (piece[y] == VIDE)
        {
            for (y += 8; y < 64; y += 8)
            {
                if (couleur[y] == coul && (piece[y] == DAME || piece[y] == TOUR))
                {
                    if(piece[y] < min_piece)
                    {
                        min_piece = piece[y];
                        square = y;
                    }
                }
                if (piece[y] != VIDE)
                    break;
            }
        }
    }
    // go left
    y = k - 1;
    h = k - col;
    if (y >= h)
    {
        if (couleur[y] == coul && (piece[y] == ROI || piece[y] == DAME || piece[y] == TOUR))
        {
            if(piece[y] < min_piece)
            {
                min_piece = piece[y];
                square = y;
            }
        }
        if (piece[y] == VIDE)
        {
            for (y--; y >= h; y--)
            {
                if (couleur[y] == coul && (piece[y] == DAME || piece[y] == TOUR))
                {
                    if(piece[y] < min_piece)
                    {
                        min_piece = piece[y];
                        square = y;
                    }
                }
                if (piece[y] != VIDE)
                    break;
            }
        }
    }
    // go right
    y = k + 1;
    h = k - col + 7;
    if (y <= h)
    {
        if (couleur[y] == coul && (piece[y] == ROI || piece[y] == DAME || piece[y] == TOUR))
        {
            if(piece[y] < min_piece)
            {
                min_piece = piece[y];
                square = y;
            }
        }
        if (piece[y] == VIDE)
        {
            for (y++; y <= h; y++)
            {
                if (couleur[y] == coul && (piece[y] == DAME || piece[y] == TOUR))
                {
                    if(piece[y] < min_piece)
                    {
                        min_piece = piece[y];
                        square = y;
                    }
                }
                if (piece[y] != VIDE)
                    break;
            }
        }
    }
    // go up
    y = k - 8;
    if (y >= 0)
    {
        if (couleur[y] == coul && (piece[y] == ROI || piece[y] == DAME || piece[y] == TOUR))
        {
            if(piece[y] < min_piece)
            {
                min_piece = piece[y];
                square = y;
            }
        }
        if (piece[y] == VIDE)
        {
            for (y -= 8; y >= 0; y -= 8)
            {
                if (couleur[y] == coul && (piece[y] == DAME || piece[y] == TOUR))
                {
                    if(piece[y] < min_piece)
                    {
                        min_piece = piece[y];
                        square = y;
                    }
                }
                if (piece[y] != VIDE)
                    break;
            }
        }
    }
    return square;
}

int atacked_by_piece(int sq, int coul, int pce)
{
    int col = COL(sq);
    int row = ROW(sq);
    int y,k;

    switch(pce)
    {
    case PION:
        if(coul == BLANC)
        {
            if((col != 0) && si_piece(PION, BLANC, sq+7))
                return (sq+7);
            if((col != 7) && si_piece(PION, BLANC, sq+9))
                return (sq+9);
        }
        else
        {
            if((col != 0) && si_piece(PION, NOIR, sq-9))
                return (sq-9);
            if((col != 7) && si_piece(PION, NOIR, sq-7))
                return (sq-7);
        }
        break;
    case CAVALIER:
        y = sq - 6;
        if (y >= 0 && col < 6 && couleur[y] == coul)
            return y;
        y = sq - 10;
        if (y >= 0 && col > 1 && couleur[y] == coul)
            return y;
        y = sq - 15;
        if (y >= 0 && col < 7 && couleur[y] == coul)
            return y;
        y = sq - 17;
        if (y >= 0 && col > 0 && couleur[y] == coul)
            return y;
        y = sq + 6;
        if (y < 64 && col > 1 && couleur[y] == coul)
            return y;
        y = sq + 10;
        if (y < 64 && col < 6 && couleur[y] == coul)
            return y;
        y = sq + 15;
        if (y < 64 && col > 0 && couleur[y] == coul)
            return y;
        y = sq + 17;
        if (y < 64 && col < 7 && couleur[y] == coul)
            return y;
        break;
    case FOU:
        for (y = sq - 9; y >= 0 && COL(y) != 7; y -= 9)          /* go left up */
        {
            if(si_piece(FOU, coul, y))
                return y;
            if (couleur[y] != VIDE)
                break;
        }
        for (y = sq - 7; y >= 0 && COL(y) != 0; y -= 7)          /* go right up */
        {
            if(si_piece(FOU, coul, y))
                return y;
            if (couleur[y] != VIDE)
                break;
        }
        for (y = sq + 9; y < 64 && COL(y) != 0; y += 9)          /* go right down */
        {
            if(si_piece(FOU, coul, y))
                return y;
            if (couleur[y] != VIDE)
                break;
        }
        for (y = sq + 7; y < 64 && COL(y) != 7; y += 7)          /* go right down */
        {
            if(si_piece(FOU, coul, y))
                return y;
            if (couleur[y] != VIDE)
                break;
        }
        break;
    case TOUR:
        for (k = sq - col, y = sq - 1; y >= k; y--)    /* go left */
        {
            if(si_piece(TOUR, coul, y))
                return y;
            if (couleur[y] != VIDE)
                break;
        }
        for (k = sq - col + 7, y = sq + 1; y <= k; y++)        /* go right */
        {
            if(si_piece(TOUR, coul, y))
                return y;
            if (couleur[y] != VIDE)
                break;
        }
        for (y = sq - 8; y >= 0; y -= 8)         /* go up */
        {
            if(si_piece(TOUR, coul, y))
                return y;
            if (couleur[y] != VIDE)
                break;
        }
        for (y = sq + 8; y < 64; y += 8)         /* go down */
        {
            if(si_piece(TOUR, coul, y))
                return y;
            if (couleur[y] != VIDE)
                break;
        }
        break;
    case DAME:
        for (y = sq - 9; y >= 0 && COL(y) != 7; y -= 9)          /* go left up */
        {
            if(si_piece(DAME, coul, y))
                return y;
            if (couleur[y] != VIDE)
                break;
        }
        for (y = sq - 7; y >= 0 && COL(y) != 0; y -= 7)          /* go right up */
        {
            if(si_piece(DAME, coul, y))
                return y;
            if (couleur[y] != VIDE)
                break;
        }
        for (y = sq + 9; y < 64 && COL(y) != 0; y += 9)          /* go right down */
        {
            if(si_piece(DAME, coul, y))
                return y;
            if (couleur[y] != VIDE)
                break;
        }
        for (y = sq + 7; y < 64 && COL(y) != 7; y += 7)          /* go right down */
        {
            if(si_piece(DAME, coul, y))
                return y;
            if (couleur[y] != VIDE)
                break;
        }
        for (k = sq - col, y = sq - 1; y >= k; y--)    /* go left */
        {
            if(si_piece(DAME, coul, y))
                return y;
            if (couleur[y] != VIDE)
                break;
        }
        for (k = sq - col + 7, y = sq + 1; y <= k; y++)        /* go right */
        {
            if(si_piece(DAME, coul, y))
                return y;
            if (couleur[y] != VIDE)
                break;
        }
        for (y = sq - 8; y >= 0; y -= 8)         /* go up */
        {
            if(si_piece(DAME, coul, y))
                return y;
            if (couleur[y] != VIDE)
                break;
        }
        for (y = sq + 8; y < 64; y += 8)         /* go down */
        {
            if(si_piece(DAME, coul, y))
                return y;
            if (couleur[y] != VIDE)
                break;
        }
        break;
    case ROI:
        if (col && si_piece(ROI, coul, (sq-1)))
            return (sq-1); /* left */
        if (col < 7 && si_piece(ROI, coul, (sq+1)))
            return (sq+1); /* right */
        if (sq > 7 && si_piece(ROI, coul, (sq-8)))
            return (sq-8); /* up */
        if (sq < 56 && si_piece(ROI, coul, (sq+8)))
            return (sq+8); /* down */
        if (col && sq > 7 && si_piece(ROI, coul, (sq-9)))
            return (sq-9); /* left up */
        if (col < 7 && sq > 7 && si_piece(ROI, coul, (sq-7)))
            return (sq-7); /* right up */
        if (col && sq < 56 && si_piece(ROI, coul, (sq+7)))
            return (sq+7); /* left down */
        if (col < 7 && sq < 56 && si_piece(ROI, coul, (sq+9)))
            return (sq+9); /* right down */
        break;
    default:
        break;
    }

    return -1;
}

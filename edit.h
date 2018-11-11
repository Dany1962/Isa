#ifndef EDIT_H
#define EDIT_H
static int ind_start[7] = {0, 0, 8, 10, 12, 14, 15};
/*
---------------------------------------------------------
*														*
*                       FONCTIONS                       *
*														*
---------------------------------------------------------
*/
void edit();
void vider_sq(int c, int p, int sq);
void occuper_sq(int c, int p, int sq);
int si_piece(int p, int c, int k);
void set_board(char *fen);
void reinit_plist();
#endif // EDIT_H
//fin edit.h

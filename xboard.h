#ifndef XBOARD_H
#define XBOARD_H
/*
---------------------------------------------------------
*														*
*                           MACROS                      *
*														*
---------------------------------------------------------
*/
#define ON          2
#define OFF         0
#define NONE        -1
#define ANALYZE     3
#define INVALID     666
#define MAXMOVES    500  /* maximum game length  */
#define MAXPLY      100   /* maximum search depth */
#define DEFAULT_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
int ponder;
int randomize;
int postThinking;
int resign;         // engine-defined option
int contemptFactor; // likewise
int temps_par_coup;
/*
---------------------------------------------------------
*														*
*                       FONCTIONS                       *
*														*
---------------------------------------------------------
*/
int xboard();
#endif // XBOARD_H
//fin xboard.h

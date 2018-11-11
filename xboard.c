#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <time.h>
#include <math.h>

#include "version.h"
#include "recherche.h"
#include "temps.h"
#include "xboard.h"
#include "eval.h"
#include "deftypes.h"
#include "edit.h"
#include "board.h"
#include "interface.h"
#include "progjoue.h"
#include "inits.h"


int xboard()
{
    char inBuf[80], command[80];
    MOVE bestMove;
    int min,sec;
    int mps,inc;
    int moves_left;
    int nbcoups = 0;
    int i;
    int coups_joues = 0;
    int inc_wb = 0;

    while(1)
    {
        fflush(stdout);

        if(side == computer_side)   // programme joue
        {
            bestMove = programme_joue(max_depth, postThinking);
            printf("move   %s\n", move_str(bestMove));
            jouer_coup(bestMove);
            //affiche_echiquier();
            //affiche_infos();
            affiche_resultat();
            continue;
        }

        fflush(stdout);

        //ponder mode à developper plus tard
        if(computer_side == ANALYZE)
        {

        }
        else
        {

        }

noPonder:
        if (!fgets(inBuf, 256, stdin))
            return 0;
        if (inBuf[0] == '\n')
            continue;

        // extract the first word
        sscanf(inBuf, "%s", command);

        // recognize the command,and execute it
        if(!strcmp(command, "quit"))
        {
            break;    // breaks out of infinite loop
        }
        if(!strcmp(command, "force"))
        {
            computer_side = NONE;
            continue;
        }
        if(!strcmp(command, "analyze"))
        {
            computer_side = ANALYZE;
            continue;
        }
        if(!strcmp(command, "exit"))
        {
            computer_side = NONE;
            continue;
        }
        if(!strcmp(command, "level"))   //pour jeu blitz ou tournament (dans la GUI) entrée de MPS, BASE , INC
        {
            sscanf(inBuf,"level %d %d %d",&mps,&base,&inc);
            //if(mps)
            //    temps_par_coup = (((base * 60) / (mps - hdp)) * 1000);
            max_depth = 100;
            continue;
        }
        if(!strcmp(command, "otim"))
        {
            goto noPonder;    // do not start pondering after receiving time commands, as move will follow immediately
        }
        if(!strcmp(command, "time"))
        {
            sscanf(inBuf, "time %d", &temps_max);
            if(mps == 0)
            {
                temps_max *=10;
                if(inc == 0)
                {
                    if(hdp > 50)
                        temps_max /= 50;
                    else
                        temps_max /= 40;
                }
                else
                    (temps_max /= 30) + inc;
                time_left = temps_max;
            }
            else
            {
                inc_wb = (inc * 1000);
                nbcoups = (hdp / 2 + 1);
                for(i=mps;; i+=mps)
                {
                    if(nbcoups == i) //dernier coups de la session time_left - 10 secondes
                    {
                        time_left = (temps_max / 2);
                        time_left += inc_wb;
                        break;
                    }
                    else if(nbcoups < i)
                    {
                        moves_left = (i - nbcoups );
                        moves_left++;
                        time_left  = (temps_max / moves_left);
                        time_left *= 10;
                        time_left += inc;
                        break;
                    }
                }
                temps_max -= time_left;
                printf("HDP ET MPS : %d %d\n",hdp,mps);
                printf("NBCOUPS    : %d\n",nbcoups);
                printf("COUPS JOUES: %d\n",coups_joues);
                printf("i          : %d\n",i);
                printf("MOVE LEFT  : %d\n",moves_left);
                printf("TIME LEFT  : %d\n",time_left);
                temps_max -= time_left;
                temps_max += inc_wb;
            }
            max_depth = 100;
            continue;
        }
        if (!strcmp(command, "protover"))
        {
            printf("feature done=0\n");
            printf("feature myname=\"Isa %d.%d.%d\"\n",MAJOR,MINOR,BUILD);
            printf("feature done=1\n");
            continue;
        }
        if(!strcmp(command, "option"))   // setting of engine-define option; find out which
        {
            //if(scanf(inBuf+7, "Resign=%d",   &resign)         == 1) continue;
            //if(scanf(inBuf+7, "Contempt=%d", &contemptFactor) == 1) continue;
            continue;
        }
        if(!strcmp(command, "sd"))
        {
            sscanf(inBuf, "sd %d", &max_depth);
            temps_max = 2000000;
            time_left = 2000000;
            continue;
        }
        if(!strcmp(command, "st"))
        {
            sscanf(inBuf, "st %d", &temps_max);
            temps_max *= 1000;
            time_left = temps_max;
            max_depth = 100;
            continue;
        }
        if(!strcmp(command, "memory"))
        {
            //SetMemorySize(atoi(inBuf+7));
            continue;
        }
        if(!strcmp(command, "ping"))
        {
            //printf("pong%s", inBuf+4);
            continue;
        }
//  if(!strcmp(command, ""))        { scanf(inBuf, " %d", &); continue; }
        if(!strcmp(command, "new"))
        {
            computer_side = NONE;
            init();
            continue;
        }
        if(!strcmp(command, "setboard"))
        {
            //engineSide = NONE;
            //stm = Setup(inBuf+9);
            continue;
        }
        if(!strcmp(command, "easy"))
        {
            //ponder = OFF;
            continue;
        }
        if(!strcmp(command, "hard"))
        {
            //ponder = ON;
            continue;
        }
        if(!strcmp(command, "undo"))
        {
            if(hdp)
            {
                dejouer_coup();
                computer_side = NONE;
            }
            continue;
        }
        if(!strcmp(command, "remove"))
        {
            //stm = TakeBack(2);
            continue;
        }
        if(!strcmp(command, "go"))
        {
            computer_side = side;
            continue;
        }
        if(!strcmp(command, "post"))
        {
            postThinking = ON;
            continue;
        }
        if(!strcmp(command, "nopost"))
        {
            postThinking = OFF;
            continue;
        }
        if(!strcmp(command, "edit"))
        {
            edit();
            continue;
        }
        if(!strcmp(command, "random"))
        {
            //randomize = ON;
            continue;
        }
        if(!strcmp(command, "hint"))
        {
            //if(ponderMove != INVALID) printf("Hint: %s\n", MoveToText(ponderMove));
            continue;
        }
        if(!strcmp(command, "book"))
        {
            continue;
        }
        if(!strcmp(command, "xboard"))
        {
            continue;
        }
        if(!strcmp(command, "computer"))
        {
            continue;
        }
        if(!strcmp(command, "name"))
        {
            continue;
        }
        if(!strcmp(command, "ics"))
        {
            continue;
        }
        if(!strcmp(command, "accepted"))
        {
            continue;
        }
        if(!strcmp(command, "rejected"))
        {
            continue;
        }
        if(!strcmp(command, "variant"))
        {
            continue;
        }
        if(!strcmp(command, ""))
        {
            continue;
        }
        //--------------------------------------------------------------------------------
        //                                  commandes persos
        //--------------------------------------------------------------------------------
        if (!strcmp(command, "perft"))
        {
            sscanf (inBuf, "perft %d", &max_depth);
            debut = get_ms();
            int count = perft(max_depth);
            /* Stop timer */
            printf("sortie perft\n");
            fin = get_ms();
            float val2 = ((float)(fin - debut)/1000);
            printf ("Noeuds = %d\n", count);
            printf ("Temps  = %2.2f secondes\n", val2);
            continue;
        }
        if (!strcmp(command, "e"))     //evaluation statique détaillée
        {
            int score;
            e_valide = VRAI;
            score = eval();
            printf("SCORE : %d\n",score);
            e_valide = FAUX;
            continue;
        }
        if (!strcmp(command, "liste"))    //affiche les coups plausibles de la position en cours
        {
            affiche_liste();
            continue;
        }
        if(command[1] >= '0' && command[1] <= '9')
        {
            printf("%c  \n",command[1]);
            bool coup_legal;
            coup_legal = verif_coup(command);
            if(coup_legal)
            {
                //affiche_echiquier();
                affiche_resultat();
            }
            else
            {
                printf("Illegal move.\n");
            }
            continue;
        }
    }
    return 0;
}

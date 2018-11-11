//---------------------------------------------------------------------------------------------------------------------
//                                                      ISA
//---------------------------------------------------------------------------------------------------------------------
// base firstchess fam ngyen
//---------------------------------------------------------------------------------------------------------------------
//                                              HISTORIQUE:
//---------------------------------------------------------------------------------------------------------------------
/*
version 0.0.5  :    Base de d�part avec datas , affichage du jeu , g�n�rateur de coups sans ep et roque
                    fonction recherche , alpha beta , jouer et dejouer coup
                    Evaluation basique (mat�riel) , fonction d'attaque
version 0.0.39 :    Ajout des options go (le programme joue avec la couleur en cours) et sd (change la profondeur de recherche)
                    Ajout option undo (retour en arri�re)
                    Ajout table init echiquier et couleur , ajout tables pos_roi[BLANC/NOIR],roques(bug)
version 0.1.27 :    Ajout g�n�ration roques et en passant
version 0.2.14 :    G�n�rateur de 64 bits , table de hashage , mise a jour non incr�mentale hashcode
version 0.2.34 :    On a le jeu en temps chronom�tr� op�rationnel , m�me boucle pour la recherche fixe ,
                    L'entr�e du temps imparti se fait en millisecondes (st 5000 = 5sec/cps , st 500 = 1/2 sec/cps etc...)
                    Mise a jour incr�mentale hashcode
version 0.3.12 :    On a la mise a jour incr�mentale du mat�riel
                    Lazy evaluation : mat�riel + psqt
version 0.3.36 :    history heuristic avec history[couleur][64][64]
                    Move ordering bas� sur history et valeur + grande pour captures promos en passant
                    Quiescence
version 0.4.66 :    Bug important mise a jour mat�riel dans le cas des promos !! arrive a trouver qques finales de pions gagnantes
version 0.5.33 :    Hashtables qui ont l'air de fonctionner (positions classiques r�solues) , ISA tourne sous Arena , fonction edit
                    (ancienne m�thode)
version 0.6.38 :    Square rule impl�ment�e
version 0.6.61 :    Perft impl�ment� , fonctions d'�valuation de tests : eval mat+pst ,full eval mat+pst+square rule
                    eval mat , eval mat+centre . Impl�mentation perft()
version 0.6.79 :    Gen coups promo d�bugu�
version 0.6.85 :    Impl�mentation tables colonnes et rangee , mises � jour incr�mentales, utiles par la suite pour l'�val et autre
                    R�vision pst. Dans le cas de finales K+Pi�ces vs K d�pouill� , un bonus dans l'�val au camp sup�rieur pour le roi
                    proche d'un bord (table roi_edge[64]
version 0.6.99 :    Implementation des fonctions distinctes dans eval , eval pion ,eval cavalier ... eval roi .Ajout peire de fous
                    dans l'eval et p�nalit� position sans pions
version 0.7.75 :    J'ai mes deux routines mises � jour dans jouer/dejouer coup (pour mettre a jour le plus possible de
                                                                                  param�tres de mani�re incr�mentale)
version 0.8.18 :    Bitboards pour les pions pass�s impl�ment�s , mise a jour de ceux ci incr�mental , Eval des pions pass�s dans la fe
version 0.8.82 :    On a les pions isol�s et doubl�s dans l'�val , D�buguage de l'�val
version 0.9.12 :    Ajout extension d'une profondeur si en �chec (pvs) , fix bug dans check_blocages() (eval.c)
version 1.0.82 :    Impl�mentation du nullmove, autre m�thode ds jouer dejouer coup pour mises a jour hash , table mat�riel ,
                    Table des pi�ces , bitboard pions
version 1.1.12 :    Gros changement : biblioth�que d'ouvertures(m�me impl�mentation que jars et yoda) sur 5 1/2 coups
                                      , et essai de correction d'imballance mat�rielle (id�e de Crafty)
version 1.1.47 :    Impl�mentation de killer heuristic (deux coups meurtrier) gain de temps notable , parfois 2 profondeurs de plus
version 1.1.77 :    Affiche la variation principale (pv) / pion bloqu� : penalit� baiss�e , p�nalit� double pion bloqu� cr�e
version 1.2.50 :    Tentative lmr (pas convaincant) / cr�ation fonction d'attaque NBR / pst modifi�es / essai de calculer la S�curit�
                    du roi / p�nalit� si le roi bouge avant d'avoir roqu�
version 1.2.54 :    Delta prunning ds quiesce (fa�on Danasah) , r�introduction lmr , On attaque a revoir les bonus d'attaque
                    de cavalier tour et fous , Essai ajout de "soutien d'un pion pass� par son roi : bonus roi proche pion pass�
                    et proche case promo pion pass� pour les finales de pions et PHASE >= 21

version 1.2.58 :    paire de fou d�but = 60 fin = 30 , pst pions chang�es , pst roi debut chang� (val o-o)
version 1.2.62 :    tri , tables killer chang�es(type MOVE) , table history chang�e en history[64][64] au lieu de history[side][64][64],
version 1.2.63 :    Imballance corrig�e , pions pass�s corrig�s , pions li�s corrig�s (bonus si !isl et !double)
                    Ajout mobilit� cavalier
                    fonction edit debugu�e , draw recognizer debugu� , bitboards corrig�s , deux tables (op et eg) pour avance pion pass�
                    roi soutient pion pass� corrig� ,modif val pi�ces (pour �viter imballances : QvsRR , RP vs NB ...) ,
                    on �value pas la structure des fous et cavalier s'ils sont sur leur rang�e d'origine ,
version 1.2.85 :    essai securit� roi mani�re tscp ,essai attaque principe toga ,
version 1.2.94 :    valeurs p�nalit�s pions isol�s et doubl�s invers�es (valeur plus forte au d�but , plus faible en fin de partie
                    suppressions redondances : si on calcule les attaques avec toga m�thode plus les bonus des pi�ces proches du roi
                    ca fait beaucoup !
                    nulmove valide si phase = 0 a 15 , gros bug dans l'�val des colonnes ouvertes et des diagonales !!
                    pion pass� eval am�lior�e (type danasah) + ajout bonus si case devant pp libre et !attaqu�e par l'adv
version 1.4.33 :    p�nalit� si le roi est en �chec (eval()) -50 , g�n�rateur de 64 bits chang� (danasah)
                    conditions lmr chang�es (compteur history) , conditions pions li�s chang�es (pion ami a une rang�e d'�cart)
                    extension si coup de pion 7th rang�e (et pas lmr) , ajout eval() retourn�e (pvs & qs) si profondeur max ateinte
version 1.4.33 :    changement de valeur de valinconnue(utilis� par les tt)(= -1 pr�c�demment ) valeur arbitraire
                                                                               900000( > a MATE)
version 1.6.36 :    Cette version a particip� � son premier tournoi , le 6 Juin 2015 , le On-line engine blitz
                    tourney du Hollandais HGMuller (20eme ex aequo / 26 , 3.5/9 , +3=1-5)
version 1.6.36 :    Correction de la table des pions pass�s , un gros illogisme !!
    avant :                                     apr�s :
    int pion_passe_debut[64] =                  int pion_passe_debut[64] =
    {                                           {
        0,  0,  0,  0,  0,  0,  0,  0,              0,  0,  0,  0,  0,  0,  0,  0,
        62, 60, 58, 56, 56, 58, 60, 62,             62, 60, 58, 56, 56, 58, 60, 62,
        36, 32, 32, 28, 28, 32, 32, 36,             36, 32, 32, 28, 28, 32, 32, 36,
        21, 21, 21, 15, 15, 21, 21, 21,             21, 21, 21, 15, 15, 21, 21, 21,
        13, 13, 13,  7,  7, 13, 13, 13,             13, 13, 13, 13, 13, 13, 13, 13,
        12, 12, 12, 19, 19, 12, 12, 12,             12, 12, 12, 12, 12, 12, 12, 12,
        10, 10, 10, 45, 45, 10, 10, 10,             10, 10, 10, 10, 10, 10, 10, 10,
        0,  0,  0,  0,  0,  0,  0,  0               0,  0,  0,  0,  0,  0,  0,  0
    };

	nodes est incr�ment� dans recherche : maintenant apr�s chaque coup jou� dans pvs , dans quiesce , et nullmove
	chang� les noms des defines (plus courts et con�is)                                          };

*/

//fix bug : pas de probe hash si prof = 0
//fix bug : promotion , maintenant Isa envoie a la GUI la promo choisie (N,B,R ou Q)
//fix bug : computer_side = NOIR au lieu de VIDE par d�faut
//UPDATE 01/02/2016 : eval s�par�e pour les finales KBNvsK et KBBvsK isa mate maintenant avec ce mat�riel
//fix bug : extension promo et avance 7eme prise en compte maintenant ouf !!!8
//fix bug : deux valeurs interverties dans datas.c : bitboards
/*      const U64 bitboard_colonne[8] =
{
   0x0101010101010101, 0x0202020202020202, 0x0808080808080808, 0x0404040404040404,
   0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080
};
au lieu de
const U64 bitboard_colonne[8] =
{
   0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808,
   0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080
};
*/
//fix bug valeurs pions arri�r�s doivent �tre > en fin de partie et inf�rieurs en d�but
//fix bug eval.c ligne  927 if(piece[sq+8] == VIDE) au lieu de if(piece[sq-8] == VIDE)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#include "version.h"
#include "recherche.h"
#include "temps.h"
#include "eval.h"
#include "deftypes.h"
#include "board.h"
#include "book.h"
#include "edit.h"
#include "interface.h"
#include "inits.h"
#include "progjoue.h"
#include "see.h"
#include "genecoups.h"

int main()
{
    char s[256];
    MOVE bestMove;
    int i;
    MOVE moveBuf[200];
    int debut,fin;
    int temps_par_coup;
    int real_mps;

    init();
    affiche_echiquier();
    affiche_infos();

    for (;;)
    {
        if (side == computer_side)      // programme joue
        {
            //recherche du meilleur coup a profondeur max_depth
            bestMove = programme_joue(max_depth, 1);
            jouer_coup(bestMove);
            affiche_echiquier();
            affiche_infos();
            affiche_resultat();
            continue;
        }
        //atente commande
        printf("isa> ");
        //printf("Taper help pour voir les commandes dispos\n");
        if (scanf("%s", s) == EOF)  //ferme le programme
            return 0;
        if (!strcmp(s, "d"))        //affichage de l'�chiquier et des infos
        {
            affiche_echiquier();
            continue;
        }
        if (!strcmp(s, "go"))       //le programme joue avec la couleur en cours
        {
            computer_side = side;
            affiche_resultat();
            continue;
        }
        if (!strcmp(s, "sd"))       //profondeur fixe
        {
            scanf("%d", &max_depth);
            affiche_echiquier();
            time_left = 2000000;
            continue;
        }
        if (!strcmp(s, "st"))       //jeu chronom�tr�
        {
            scanf("%d", &time_left);
            //temps_max *= 1000;
            max_depth = 100;
            continue;
        }
        if (!strcmp(s, "level"))       //pour jeu blitz ou tournament (dans la GUI) entr�e de MPS, BASE , INC
        {
            scanf("%d %d %d",&mps,&base,&inc);
            if(mps)
            {
                temps_max = (((base * 60) / (mps - hdp)) * 1000);
                printf("Temps Par Coup : %d\n",temps_max);
            }
            max_depth = 100;
            continue;
        }
        if (!strcmp(s, "undo"))     //annule un coup
        {
            if(hdp)
            {
                dejouer_coup();
                computer_side = VIDE;
                affiche_echiquier();
                affiche_infos();
            }
            continue;
        }
        if (!strcmp(s, "new"))      //commence une nouvelle partie
        {
            init();
            affiche_echiquier();
        }
        if (!strcmp(s, "quit"))     //quitte l'application
        {
            printf("bye ! ;-) \n");
            return 0;
        }
        if (!strcmp(s, "liste"))    //affiche les coups plausibles de la position en cours
        {
            liste_valide = VRAI;
            affiche_liste();
            continue;
        }
        if (!strcmp(s, "xxx"))      //affiches les captures plausibles de la position en cours
        {
            affiche_captures();
            continue;
        }
        if (!strcmp(s, "xboard"))   //passe en mode GUI
        {
            xboard();
            continue;
        }
        if (!strcmp(s, "stop"))     //arrete le programme avec la couleur en cours
        {
            computer_side = VIDE;
            continue;
        }
        if (!strcmp(s, "edit"))     //entree manuelle d'une position
        {
            edit();
            continue;
        }
        if (!strcmp(s, "setboard"))     //entree manuelle d'une position
        {
            continue;
        }
        if (!strcmp(s, "e"))     //evaluation statique d�taill�e
        {
            int score;
            e_valide = VRAI;
            score = eval();
            printf("SCORE : %d\n",score);
            e_valide = FAUX;
            continue;
        }
        if (!strcmp(s, "help"))     //aide
        {
            printf("-------------------------------AIDE-------------------------------\n");
            printf("d       : Affichage de l'echiquier et des infos \n");
            printf("go      : Le programme joue avec la couleur en cours\n");
            printf("sd      : Change la profondeur de recherche\n");
            printf("undo    : Annule un coup\n");
            printf("new     : Commence une nouvelle partie\n");
            printf("quit    : Quitte l'application\n");
            printf("liste   : Affiche les coups plausibles de la position en cours\n");
            printf("xxx     : Affiches les captures plausibles de la position en cours\n");
            printf("xboard  : Passe en mode GUI\n");
            printf("stop    : Arrete le programme avec la couleur en cours\n");
            printf("edit    : Entree manuelle d'une position\n");
            printf("help    : Aide\n");
            printf("mode    : 1 : alpha beta , 2 : negascout\n");
            printf("------------------------------------------------------------------\n");
            continue;
        }
        if (!strcmp(s, "perft"))
        {
            scanf ("%d", &max_depth);
            debut = get_ms();
            U64 count = perft(max_depth);
            /* Stop timer */
            fin = get_ms();
            float val2 = ((float)(fin - debut)/1000);
            printf ("Noeuds = %d\n", count);
            printf ("Temps  = %2.2f secondes\n", val2);
            continue;
        }
        if (!strcmp(s, "cr"))
        {
            int sq;
            scanf("%d",&sq);
            if(sq < 0 || sq > 63)
                continue;
            print_champ_roi(sq);
            continue;
        }
        if (!strcmp(s, "hashsize"))
        {
            int hash_size;

            scanf("%d",&hash_size);
            continue;
        }
        if (!strcmp(s, "scan"))
        {
            scan_biblio();
        }
        if (!strcmp(s, "test"))
        {
            int d;
            int nbcoups;
            scanf("%d",&d);
            scanf("%d",&nbcoups);
            printf("reduction  = %d\n",1 + (int) (sqrt((d-3) * nbcoups) / 4.0));
        }
        if (!strcmp(s, "tps"))
        {
            printf("base , mps , et nbcoups\n");
            int b,m,n,x,y;
            scanf("%d %d %d",&b,&m,&n);
            if(side == BLANC)
                x = (m - (n / 2 + 1));
            else
                x = (m - (n / 2));
            y = (((b * 60) / x) * 1000);
            printf("temps pour jouer le coup : %d (moves left : %d) \n",y,x);
            continue;
        }
        /*
        scanf ("%d", &max_depth);
            clock_t start;
            clock_t stop;
            double t = 0.0;
            start = clock ();
            unsigned long long count = perft (max_depth);
            stop = clock ();
            t = (double) (stop - start) / CLOCKS_PER_SEC;
            printf ("nodes = %llu\n", count);
            printf ("time = %.2f s\n", t);
            continue;*/
        //check si l'utilisateur a entr� un coup
        bool coup_legal;
        coup_legal = verif_coup(s);
        if(coup_legal)
        {
            affiche_echiquier();
            affiche_infos();
            affiche_resultat();
        }
        else
        {
            printf("Illegal move.\n");
        }
    }
    return 0;
}

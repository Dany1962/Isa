#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>



#include "board.h"
#include "recherche.h"
#include "hash.h"
#include "deftypes.h"

void init_hash_table()
{
    int c, p, sq;
    int ctr = 0;

    memset(hash_table, 0, sizeof(hash_table));

    //code couleur/piece/case
    for(c = BLANC; c <= NOIR; ++c)
    {
        for(p = PION; p <= ROI; ++p)
        {
            for(sq = 0; sq < 64; ++sq)
            {
                hash_table[c][p][sq] = jeu_de_valeurs_64_bits[ctr];
                ++ctr;
            }
        }
    }

    //droit aux roques
    for(c = 0; c < 16; ++c)
    {
        droit_aux_roques[c] = jeu_de_valeurs_64_bits[ctr];
        ++ctr;
    }

    //codes cases en passant
    for(c = 0; c < 64; ++c)
    {
        val_en_passant[c] = jeu_de_valeurs_64_bits[ctr];
        ++ctr;
    }
    //codes couleurs
    val_couleur[BLANC] = jeu_de_valeurs_64_bits[ctr];
    ++ctr;
    val_couleur[NOIR]  = jeu_de_valeurs_64_bits[ctr];
    ++ctr;
}

void init_hash_table_pions()
{
    int ctr = 0;
    int i,j;

    for(i=BLANC;i<=NOIR;++i)
    {
        for(j=0;j<64;++j)
        {
            hash_table_pions[i][j] = jeu_de_valeurs_64_bits[ctr];
            ++ctr;
        }
    }
}

U64 init_code_position()
{
    int c;
    U64 code = 0;

    for(c = 0; c < 64; ++c)
    {
        if(piece[c] != VIDE)
            code ^= hash_table[couleur[c]][piece[c]][c];
    }
    code ^= droit_aux_roques[castle];
    if(ep != -1)
    {
		code ^= val_en_passant[ep];
    }
    code ^= val_couleur[side];
    return code;
}

U64 init_code_pions()
{
    int c;
    U64 code = 0;

    for(c=0;c<64;++c)
    {
        if(piece[c] == PION)
            code ^= hash_table_pions[couleur[c]][c];
    }

    return code;
}

int probe_hash(int depth, int alpha, int beta)
{
    HASHE *phashe = &tt[hash_code_position % HASH_TABLE_SIZE];

    positions_testees++;

    if(phashe->key == hash_code_position)
    {
        positions_trouvees++;
        if(phashe->depth >= depth)
        {
            if(phashe->flag == hashfEXACT)
                return phashe->value;
            if((phashe->flag == hashfALPHA) && (phashe->value <= alpha))
                return alpha;
            if((phashe->flag == hashfBETA) && (phashe->value >= beta))
                return beta;
        }
    }
    return valINCONNUE;
}


void save_hash(int depth, int val, int hashf, MOVE *pbest)
{
    HASHE *phashe = &tt[hash_code_position % HASH_TABLE_SIZE];

    positions_sauvees++;

    phashe->key     = hash_code_position;
    phashe->depth   = depth;
    phashe->flag    = hashf;
    phashe->value   = val;
    phashe->best    = pbest;
}

int probe_move_ordering(int f, int d)
{
    HASHE *phashe = &tt[hash_code_position % HASH_TABLE_SIZE];
    MOVE *b = phashe->best;

    if(phashe->key == hash_code_position)
    {
        if(f == b->from && d == b->dest)
            return VRAI;
    }
    return FAUX;
}

int probe_eval()
{
    HASH_EVAL *phashe = &eval_table[hash_code_position % HASH_TABLE_SIZE];
    if(phashe->key == hash_code_position)
        return phashe->eval_hash;
    return valINCONNUE;
}

int probe_pions()
{
    HASH_PIONS *phashe = &eval_struct_pions[hash_code_pions % HASH_TABLE_SIZE_PIONS];
    if(phashe->key_pion == hash_code_pions)
        return phashe->eval_hash_pions;
    return valINCONNUE;
}

void save_eval(int score)
{
    HASH_EVAL *phashe = &eval_table[hash_code_position % HASH_TABLE_SIZE];
    phashe->key = hash_code_position;
    phashe->eval_hash = score;
}

void save_pions(int score)
{
    HASH_PIONS *phashe = &eval_struct_pions[hash_code_pions % HASH_TABLE_SIZE_PIONS];
    phashe->key_pion = hash_code_pions;
    phashe->eval_hash_pions = score;
}


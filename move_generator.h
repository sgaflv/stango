#ifndef MOVE_GENERATOR_H_INCLUDED
#define MOVE_GENERATOR_H_INCLUDED


#include "common.h"


void setup_board(char *fen);
int generate_moves(struct chess_state *moves);

U64 perft(int depth);

int test_all_moves();
void test_thinking();


int alphaBetaMax( int alpha, int beta, int depth, bool first);
int alphaBetaMin( int alpha, int beta, int depth, bool first);
void make_smart_move();

#endif // MOVE_GENERATOR_H_INCLUDED

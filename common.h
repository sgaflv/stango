#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef unsigned char bool;
#define true 1
#define false 0

typedef unsigned long long U64;

#define PIECE2CHAR(p)           ("01BbRrQqPpNnKk#!.?"[p]) 
#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)<(b) ? (a) : (b))

#define R(square)               ((square) >> 3)        
#define F(square)               ((square) & 7)        
#define SQ(f,r)                 (((r) << 3) | (f))     
#define WTM                     (~ply & 1)

#define CAS_WK 1
#define CAS_BK 2
#define CAS_WQ 4
#define CAS_BQ 8

#define WHITE 0
#define BLACK 1

#define INF 10000000
#define MAX_PLY 100


extern int piece_value[];

enum piece_t {
	white, // all white
	black, // all black
	Bishop, 
	bishop,
	Rook, 
	rook, 
	Queen, 
	queen, 
	Pawn,
	pawn, 
	Knight, 
	knight, 
	King, 
	king,
	
	enpas,
	all, empty, error
};



enum pos_t { /* position constants */
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8
};

enum rank_t {
	R1, R2, R3, R4, R5, R6, R7, R8
};

enum file_t {
	FA, FB, FC, FD, FE, FF, FG, FH, NONPAS
};


// for now chess state and chess move is the same thing
struct chess_state {
	U64 key; // computed hash key
	/* dynamic part, to recover the old state */
	enum pos_t from;
	enum pos_t to;
	enum piece_t promo;
	bool is_capture;
	int move_value;
	char cas; // state of the enabled castles
	
	// bit fields must be in the same order as enum piece_t
	U64 all_white;
	U64 all_black;
	
	U64 bBishop;
	U64 bbishop;
	U64 bRook;
	U64 brook;
	U64 bQueen;
	U64 bqueen;
	
	U64 bPawn;
	U64 bpawn;
	U64 bKnight;
	U64 bknight;
	U64 bKing;
	U64 bking;
	
	U64 benpas;
	U64 all;
	
	U64 all_white_attacks;
	
	U64 all_black_attacks;
};

int count_bits(U64 bits);

void add_bit(U64 *board, int x, int y);
bool get_bit(U64 *value, int x, int y);

inline int popCount (U64 x);
inline int bitScanForward(U64 bb);

U64 random_U64();

bool states_equal(struct chess_state *a, struct chess_state *b);

#include "messaging.h"
#include "hash.h"
#include "chess_game.h"
#include "magic.h"
#include "evaluator.h"
#include "move_generator.h"
#include "commands.h"

#endif // COMMON_H_INCLUDED

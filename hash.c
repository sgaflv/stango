
#include "hash.h"

struct hash_t hash[HASH_SIZE];


static U64 zobrist[12][64];  /* Hash-key construction */


static long rnd_seed = 115;

static long rnd(void)
{
        long r = rnd_seed;

        r = 16807 * (r % 127773L) - 2836 * (r / 127773L);
        if (r < 0) r += 0x7fffffffL;

        return rnd_seed = r;
}

void init_hash() {
	unsigned int i;
	for (i=0; i<sizeof(zobrist); i++) {
		( (unsigned char*)zobrist )[i] = rnd();
	}
}

static inline U64 compute_piece(U64 positions, enum piece_t piece) {
	U64 pos;
	int idx;
	U64 ret=0;
	piece-=2; //adjust smallest piece to 0 coordinate
	
	//piece=0;
	while (positions) {
		pos=positions&(0-positions);
		idx = bitScanForward(pos);
		ret^=zobrist[piece][idx];
		positions^=pos;
	}
	return ret;
}

inline U64 compute_hash(struct chess_state *state) {
	U64 h = 0;
	
	h^=compute_piece(state->bQueen, Queen);
	h^=compute_piece(state->bBishop, Bishop);
	h^=compute_piece(state->bRook, Rook);
	h^=compute_piece(state->bPawn, Pawn);
	h^=compute_piece(state->bKnight, Knight);
	
	h^=compute_piece(state->bqueen, queen);
	h^=compute_piece(state->bbishop, bishop);
	h^=compute_piece(state->brook, rook);
	h^=compute_piece(state->bpawn, pawn);
	h^=compute_piece(state->bknight, knight);
	
	h^=zobrist[king-2][bitScanForward(state->bking)];
	h^=zobrist[King-2][bitScanForward(state->bKing)];
//	h^=compute_piece(state->bKing, King);
	
	h^=state->benpas | state->benpas>>24;
	
	h^=state->cas<<1 | WTM;
	
	return h;
}

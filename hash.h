#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

#include "common.h"

#define HASH_SIZE (1<<21)

#define HASH(h) (h & (HASH_SIZE - 1))

#define MOVE(from,to,promo) (((from)<<10)|((to)<<4)|(promo))
#define FROM(a) ((a)>>10)
#define TO(a)  (((a)>>6)&63)
#define PROMO(a)  ((a)&15)


enum score_t {s_empty, s_PV, s_alpha, s_beta};

struct hash_t {
	U64 key;
	char depth;
	int score;
	enum score_t flag;
	struct chess_state move;
};

extern struct hash_t hash[];

void init_hash();
U64 compute_hash(struct chess_state *state);

void record_hash(int depth, int score, enum score_t flag, U64 key, struct chess_state *move, int ply);

#endif // HASH_H_INCLUDED

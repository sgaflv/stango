#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

#include "common.h"

#define HASH_SIZE (1<<21)

#define HASH(h) (h & (HASH_SIZE - 1))



enum score_t {s_PV, s_cut, s_all};

struct hash_t {
	U64 key;
	char depth;
	int score;
};

extern struct hash_t hash[];

void init_hash();
U64 compute_hash();


#endif // HASH_H_INCLUDED

#include "common.h"

int piece_value[] = {0,0,330,-330,500,-500,900,-900,100,-100,320,-320,20000,-20000};


static const int index64[64] = {
   63,  0, 58,  1, 59, 47, 53,  2,
   60, 39, 48, 27, 54, 33, 42,  3,
   61, 51, 37, 40, 49, 18, 28, 20,
   55, 30, 34, 11, 43, 14, 22,  4,
   62, 57, 46, 52, 38, 26, 32, 41,
   50, 36, 17, 19, 29, 10, 13, 21,
   56, 45, 25, 31, 35, 16,  9, 12,
   44, 24, 15,  8, 23,  7,  6,  5
};

inline int bitScanForward(U64 bb) {
	/* here we assume there is only one bit on the field */
   return index64[(bb * 0x07EDD5E59A4E28C2ULL) >> 58];
}


void add_bit(U64 *board, int x, int y) {
	if (x<0) return;
	if (x>7) return;
	if (y<0) return;
	if (y>7) return;
	
	(*board)|=1ULL<<(x+y*8);
}


bool get_bit(U64 *value, int x, int y) {
	if (x<0) return 0;
	if (x>7) return 0;
	if (y<0) return 0;
	if (y>7) return 0;
	
	return (*value&(1ULL<<(x+y*8)))>0;
}


inline int popCount (U64 x) {
	int count = 0;
	while (x) {
		count++;
		x &= x - 1;
	}
	return count;
}

U64 random_U64() {
	U64 u1, u2, u3, u4;
	u1 = (U64)(random()) & 0xFFFF; 
	u2 = (U64)(random()) & 0xFFFF;
	u3 = (U64)(random()) & 0xFFFF;
	u4 = (U64)(random()) & 0xFFFF;
	return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}


bool states_equal(struct chess_state *a, struct chess_state *b) {
	if (memcmp(&a->bBishop, &b->bBishop, 12*sizeof (U64))) return false;
	if (a->benpas!=b->benpas) return false;
	if (a->cas!=b->cas) return false;
	return true;
}

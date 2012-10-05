#ifndef MAGIC_H_INCLUDED
#define MAGIC_H_INCLUDED

#include "common.h"
#include "chess_game.h"

extern int RPopBits[64];
 
extern int BPopBits[64];

extern int RShiftBits[64];
extern int BShiftBits[64];

extern const U64 RMagic[64];
extern const U64 BMagic[64];
extern U64 *RAttack[64]; /* 64 pointers to rook magic number arrays */
extern U64 *BAttack[64]; /* 64 pointers to bishop magic number arrays */

extern void initPopBits();
extern void initMagic();

extern void generateAllRookMagic();
extern void generateAllBishopMagic();

#endif // MAGIC_H_INCLUDED

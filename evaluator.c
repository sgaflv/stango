#include "evaluator.h"


int BPawnPSQ[64] = {
		0,  0,  0,  0,  0,  0,  0,  0,
		50, 50, 50, 50, 50, 50, 50, 50,
		10, 10, 20, 30, 30, 20, 10, 10,
		5,  5, 10, 25, 25, 10,  5,  5,
		0,  0,  0, 20, 20,  0,  0,  0,
		5, -5,-10,  0,  0,-10, -5,  5,
		5, 10, 10,-20,-20, 10, 10,  5,
		0,  0,  0,  0,  0,  0,  0,  0
	};
int WPawnPSQ[64];
	
int BKnightPSQ[64] = {
		-50,-40,-30,-30,-30,-30,-40,-50,
		-40,-20,  0,  0,  0,  0,-20,-40,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30,
		-30,  0, 15, 20, 20, 15,  0,-30,
		-30,  5, 10, 15, 15, 10,  5,-30,
		-40,-20,  0,  5,  5,  0,-20,-40,
		-50,-40,-30,-30,-30,-30,-40,-50
	};
int WKnightPSQ[64];


int BBishopPSQ[64] = {
		-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-20,-10,-10,-10,-10,-10,-10,-20
	};
int WBishopPSQ[64];

int BRookPSQ[64] = {
		0,  0,  0,  0,  0,  0,  0,  0,
		5, 10, 10, 10, 10, 10, 10,  5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		0,  0,  0,  5,  5,  0,  0,  0	
	};
int WRookPSQ[64];


int BQueenPSQ[64] = {
		-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5,  5,  5,  5,  0,-10,
		 -5,  0,  5,  5,  5,  5,  0, -5,
		  0,  0,  5,  5,  5,  5,  0, -5,
		-10,  5,  5,  5,  5,  5,  0,-10,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20
	};
int WQueenPSQ[64];

int BKingPSQ[64] = {
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-20,-30,-30,-40,-40,-30,-30,-20,
		-10,-20,-20,-20,-20,-20,-20,-10,
		20, 20,  0,  0,  0,  0, 20, 20,
		20, 30, 10,  0,  0, 10, 30, 20
	};
int WKingPSQ[64];

int BKingEndPSQ[64] = {
	-50,-40,-30,-20,-20,-30,-40,-50,
	-30,-20,-10,  0,  0,-10,-20,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-30,  0,  0,  0,  0,-30,-30,
	-50,-30,-30,-30,-30,-30,-30,-50
};
int WKingEndPSQ[64];



void flip(int *p, int *p2) {
	int i=0;
	int tmp;
	while (i<64) {
		p[i]=p2[63-i];
		i++;
	}
}

// PSQ generation
void init_psq() {
	flip(WKingPSQ, BKingPSQ);
	flip(WPawnPSQ, BPawnPSQ);
	flip(WRookPSQ, BRookPSQ);
	flip(WQueenPSQ, BQueenPSQ);
	
	flip(WKnightPSQ, BKnightPSQ);
	flip(WBishopPSQ, BBishopPSQ);
	
	flip(WKingPSQ, BKingPSQ);
	flip(WKingEndPSQ, BKingEndPSQ);
	
}


int positionToScore(int *scores, U64 bitboard) {
	U64 bit;
	int idx;
	int ret=0;
	
	while (bitboard) {
		bit=bitboard&(0-bitboard);
		idx=bitScanForward(bit);
		
		ret+=scores[idx];
		
		bitboard^=bit;
	}
	return ret;
}


/* evaluates current position */
int evaluate_position() {
	/* for now a very simple evaluation */
	int w = 0;
	int b = 0;
	int i;
	U64 tmp;
	U64 pawns;
	bool isEndgame=false;
	int r = rand()&15;
	
	b+=popCount(bcurrent->brook)*500;
	b+=popCount(bcurrent->bqueen)*900;
	w+=popCount(bcurrent->bRook)*500;
	w+=popCount(bcurrent->bQueen)*900;
	
	if (b+w<1800) isEndgame=true;
	
	b+=popCount(bcurrent->bpawn)*100;
	b+=popCount(bcurrent->bknight)*320;
	b+=popCount(bcurrent->bbishop)*330;
	b+=popCount(bcurrent->bking)*40000;
	
	//b+=( (bcurrent->cas&CAS_BK) + (bcurrent->cas&CAS_BQ) )*10;
	
	b+=positionToScore(BPawnPSQ, bcurrent->bpawn);
	b+=positionToScore(BRookPSQ, bcurrent->brook);
	b+=positionToScore(BBishopPSQ, bcurrent->bbishop);
	b+=positionToScore(BKnightPSQ, bcurrent->bknight);
	b+=positionToScore(BQueenPSQ, bcurrent->bqueen);
	
	
	w+=popCount(bcurrent->bPawn)*100;
	w+=popCount(bcurrent->bKnight)*320;
	w+=popCount(bcurrent->bBishop)*330;
	w+=popCount(bcurrent->bKing)*40000;
	
//	w+=( (bcurrent->cas&CAS_WK) + (bcurrent->cas&CAS_WQ) )*10;
	
	
	w+=positionToScore(WPawnPSQ, bcurrent->bPawn);
	w+=positionToScore(WRookPSQ, bcurrent->bRook);
	w+=positionToScore(WBishopPSQ, bcurrent->bBishop);
	w+=positionToScore(WKnightPSQ, bcurrent->bKnight);
	w+=positionToScore(WQueenPSQ, bcurrent->bQueen);
	
	
	
	
	tmp=leftBorder;
	// penalty for doubled pawns
	for (i=0;i<8;i++) {
		pawns=tmp&bcurrent->bPawn;
		if (pawns&(pawns-1)) {
			w-=50;
		}
		pawns=tmp&bcurrent->bpawn;
		if (pawns&(pawns-1)) {
			b-=50;
		}
		
		tmp<<=1;
	}
	
	
	if (isEndgame) {
		b+=positionToScore(BKingEndPSQ, bcurrent->bking);
	} else {
		b+=positionToScore(BKingPSQ, bcurrent->bking);
	}
	if (isEndgame) {
		w+=positionToScore(WKingEndPSQ, bcurrent->bking);
	} else {
		w+=positionToScore(WKingPSQ, bcurrent->bking);
	}
	
	return w-b+r-7;
}

#include "common.h"
#include "chess_game.h"

/* all movement patterns */
U64 mKing[64];
U64 mKnight[64];

/* sliding attacks */
U64 mBishop[64];
U64 mRook[64];

/* internal sliding attacks */
U64 iBishop[64];
U64 iRook[64];

U64 mWPawnMove[64];
U64 mWPawnCapture[64];
U64 mBPawnMove[64];
U64 mBPawnCapture[64];

/* helpful boards */
U64 leftBorder   = 0x0101010101010101ULL;
U64 rightBorder  = 0x8080808080808080ULL;
U64 topBorder    = 0xff00000000000000ULL;
U64 bottomBorder = 0x00000000000000ffULL;


struct game_setup game_setup;

struct chess_state *bcurrent;
struct chess_state history[MAX_PLY_COUNT];
int ply; // total number of moves made

unsigned char MP0_254[255];

/*
void read_bboard() {
	
	// load piece data from current board to the set of bitboards 
	U64 b = 1LL;
	int pos = 0;
	move_count=0;
	memset(history, 0, sizeof history);
	bcurrent=history;
	
	if (current.enpas!=NOPAS) {
		bcurrent->benpas=1<<current.enpas;
		bcurrent->benpas<<=16;
		if (current.white_to_move) {
			bcurrent->benpas<<=24;
		}
	}
	
	bcurrent->white_to_move = current.white_to_move;
	
	{
		U64 ws = (current.cas&WS)!=0;
		U64 wl = (current.cas&WL)!=0;
		U64 bs = (current.cas&BS)!=0;
		U64 bl = (current.cas&BL)!=0;
		
		bcurrent->bcas = (ws<<G1)|(wl<<C1)|(bs<<G8)|(bl<<C8);
	}
	
	while (pos<64) {
		
		enum piece_t p=char_to_piece(current.board[pos]);
		
		switch (p) {
			case Pawn:   bcurrent->bPawn|=b;   break;
			case Knight: bcurrent->bKnight|=b; break;
			case Bishop: bcurrent->bBishop|=b; break;
			case Rook:   bcurrent->bRook|=b;   break;
			case Queen:  bcurrent->bQueen|=b;  break;
			case King:   bcurrent->bKing|=b;   break;
			case pawn:   bcurrent->bpawn|=b;   break;
			case knight: bcurrent->bknight|=b; break;
			case bishop: bcurrent->bbishop|=b; break;
			case rook:   bcurrent->brook|=b;   break;
			case queen:  bcurrent->bqueen|=b;  break;
			case king:   bcurrent->bking|=b;   break;
			default: 
			
			break;
		}
			
		b<<=1;
		pos++;
	}
	
	bcurrent->all_white = 
		bcurrent->bPawn | 
		bcurrent->bKnight | 
		bcurrent->bBishop | 
		bcurrent->bRook | 
		bcurrent->bQueen | 
		bcurrent->bKing;
	
	bcurrent->all_black =
		bcurrent->bpawn | 
		bcurrent->bknight | 
		bcurrent->bbishop | 
		bcurrent->brook | 
		bcurrent->bqueen | 
		bcurrent->bking;
	
	bcurrent->all = bcurrent->all_white | bcurrent->all_black;
	
	update_all_attack_bits();
}
*/

/*
void new_game() {
	game_setup.book_mode=true;
	game_setup.play_white=false;

	current.white_to_move=true;
	current.enpas=NOPAS;
	
	game_setup.moves_left = 50;
	
	game_setup.half_move_count = 0;
	
	memcpy(
		current.board, // the board is upside-down, A1 = 0th element, e.t.c. 
		"RNBQKBNR"
		"PPPPPPPP"
		"........"
		"........"
		"........"
		"........"
		"pppppppp"
		"rnbqkbnr"
		, 64);
	
	current.cas=BL|BS|WL|WS;
}
*/

void init() {
	int x, y, i, j;
	
	for (i=0;i<=255;i++) {
		MP0_254[i]=i;
	}
	
	for (y=0;y<8;y++) {
		for (x=0;x<8;x++) {
			int idx = x+y*8;
			/* pawn move */
			add_bit(&mWPawnMove[idx], x, y+1);
			if (y==1) add_bit(&mWPawnMove[idx], x, y+2);
			
			add_bit(&mBPawnMove[idx], x, y-1);
			if (y==6) add_bit(&mBPawnMove[idx], x, y-2);
			
			/* pawn capture */
			add_bit(&mWPawnCapture[idx], x-1, y+1);
			add_bit(&mWPawnCapture[idx], x+1, y+1);
			
			add_bit(&mBPawnCapture[idx], x-1, y-1);
			add_bit(&mBPawnCapture[idx], x+1, y-1);
			
			/* moves for knight */
			for (i=-2;i<3;i++) {
				for (j=-2;j<3;j++) {
					if (abs(j)+abs(i)!=3) continue;
					add_bit(&mKnight[idx], x+i, y+j);
				}
			}
			/* moves for king */
			for (i=-1;i<2;i++) {
				for (j=-1;j<2;j++) {
					if (j==0&&i==0) continue;
					add_bit(&mKing[idx], x+i, y+j);
				}
			}
			/* moves for rook */
			for (i=-7;i<8;i++) {
				for (j=-7;j<8;j++) {
					if (j!=0&&i!=0) continue;
					if (j==0&&i==0) continue;
					
					add_bit(&mRook[idx], x+i, y+j);
				}
			}
			/* moves for bishop */
			for (i=-7;i<8;i++) {
				for (j=-7;j<8;j++) {
					if (abs(j)!=abs(i)) continue;
					if (j==0&&i==0) continue;
					
					add_bit(&mBishop[idx], x+i, y+j);
				}
			}
			
			/* assign inner attack bits */
			iRook[idx]=mRook[idx];
			iBishop[idx]=mBishop[idx];
			
			if (x>0) {
				iRook[idx]&=~leftBorder;
				iBishop[idx]&=~leftBorder;
			}
			
			if (x<7) {
				iRook[idx]&=~rightBorder;
				iBishop[idx]&=~rightBorder;
			}
			
			if (y>0) {
				iRook[idx]&=~bottomBorder;
				iBishop[idx]&=~bottomBorder;
			}
			
			if (y<7) {
				iRook[idx]&=~topBorder;
				iBishop[idx]&=~topBorder;
			}
			
		}
	}
	
	
	initPopBits();
	initMagic();
	init_hash();

}

void output_bitboard(U64 b) {
	int x, y;
	sprintf(outbuf, "output for field 0x%llX\n", b);
	send_message(outbuf);
	
	for (y=7;y>=0;y--) {
		for (x=0;x<8;x++) {
			if (get_bit(&b, x, y)) 
				outbuf[x]='1';
			else 
				outbuf[x]='.';
			
		}
		
		outbuf[8]='\n';
		outbuf[9]=0;
		send_message(outbuf);
	}
	
}


char piece_to_char(enum piece_t pp) {
	return PIECE2CHAR(pp);
};


enum piece_t char_to_piece(char a) {
	
	enum piece_t ret=error;
	switch (a) {
		case 'p': ret=pawn;   break;
		case 'n': ret=knight; break;
		case 'b': ret=bishop; break;
		case 'r': ret=rook;   break;
		case 'q': ret=queen;  break;
		case 'k': ret=king;   break;
		
		case 'P': ret=Pawn;   break;
		case 'N': ret=Knight; break;
		case 'B': ret=Bishop; break;
		case 'R': ret=Rook;   break;
		case 'Q': ret=Queen;  break;
		case 'K': ret=King;   break;

		default: ret=error;
	}
	return ret;
}


void send_move(enum pos_t from, enum pos_t to, enum piece_t promo) {
	char c = ' ';
	if (promo!=empty) {
		c=PIECE2CHAR(promo);
		if (c>='A'&&c<='Z') {
			c=c-'A'+'a';
		}
	}
	
	sprintf(outbuf, "move %c%c%c%c%c\n", 
		(char)(from&7)+'a', 
		(char)(from>>3)+'1', 
		(char)(to&7)+'a', 
		(char)(to>>3)+'1',
		(char)c);
	send_message(outbuf);
}


char getCharAt(struct chess_state *bcurrent, U64 at) {
	if (bcurrent->benpas & at)   return '#';
	
	if (bcurrent->bpawn & at)   return 'p';
	if (bcurrent->bknight & at) return 'n';
	if (bcurrent->bking & at)   return 'k';
	if (bcurrent->bqueen & at)  return 'q';
	if (bcurrent->bbishop & at) return 'b';
	if (bcurrent->brook & at)   return 'r';
	
	if (bcurrent->bPawn & at)   return 'P';
	if (bcurrent->bKnight & at) return 'N';
	if (bcurrent->bKing & at)   return 'K';
	if (bcurrent->bQueen & at)  return 'Q';
	if (bcurrent->bBishop & at) return 'B';
	if (bcurrent->bRook & at)   return 'R';
	
	return '.';
}

/* additional debug information */
/*
void print_board() {
	int i, j;
	char board[8][8];
	
	for(i=0;i<8;i++) {
		for (j=0;j<8;j++) {
			board[7-i][j]=piece_to_char(bcurrent->piece_board[i*8+j]);
		}
	}
	for (i=0;i<8;i++) {
		sprintf(outbuf, "#         \n");
		memcpy(outbuf+2, board[i], 8);
		send_message(outbuf);
	}
	send_message("#\n");
	
	sprintf(outbuf, "# new %c old %c promo %c", 
			piece_to_char(bcurrent->new_piece), piece_to_char(bcurrent->old_piece), 
			piece_to_char(bcurrent->promo));
	
	send_message(outbuf);
	send_move(bcurrent->from, bcurrent->to);
	
	if (bcurrent->is_enpas_cap) send_message("# enpas capture\n");
	if (bcurrent->is_long_castle) send_message("# long castle\n");
	if (bcurrent->is_short_castle) send_message("# short castle\n");
	
	send_message("all pieces");    output_bitboard(bcurrent->all);
	send_message("black_attacks"); output_bitboard(bcurrent->all_black_sliding);
	send_message("black_static");  output_bitboard(bcurrent->all_black_static);
	send_message("white_sliding"); output_bitboard(bcurrent->all_white_sliding);
	send_message("white_static");  output_bitboard(bcurrent->all_white_static);
}
*/



inline bool is_static(enum piece_t p) {
	return p>=Pawn&&p<=king;
}

inline bool is_sliding(enum piece_t p) {
	return p>=Bishop&&p<=Pawn;
}

inline bool is_white(enum piece_t p) {
	return (~p)&1;
}

inline bool is_black(enum piece_t p) {
	return p&1;
}

inline bool is_piece(enum piece_t p) {
	return p>=Bishop&&p<=king;
}


bool parse_move(char *move, enum pos_t *from, enum pos_t *to, char *promo) {
	// parse algebraic notation
	while (*move&&*move==' ') move++;
	if (!move) return false;
	
	if (move[0]<'a'||move[0]>'h') return false;
	if (move[1]<'1'||move[1]>'8') return false;
	if (move[2]<'a'||move[2]>'h') return false;
	if (move[3]<'1'||move[3]>'8') return false;
	
	if (move[4]!=0) {
		*promo=move[4];
		
		if (*promo==error) return 0;
		if (move[5]!=0) return 0;
	} else if (move[4]!=0) return false;
	
	*from = (move[0]-'a')+(move[1]-'1')*8;
	*to = (move[2]-'a')+(move[3]-'1')*8;
	
	return true;
}


bool move_if_can(char *move) {
	enum pos_t from=0;
	enum pos_t to=0;
	char promo=0;
	int mcnt, i;
	
	if (!parse_move(move, &from, &to, &promo)) {
		send_message("# move rejected by parser\n");
		return false;
	}
	
	struct chess_state moves[255];
	
	bcurrent=&history[ply];
	mcnt=generate_moves(moves);
	
	for (i=0;i<mcnt;i++) {
		if (from==moves[i].from&&to==moves[i].to) {
			send_message("# movement is accepted");
			send_move(moves[i].from, moves[i].to, moves[i].promo);
			
			ply++;
			memcpy(&history[ply], &moves[i], sizeof (struct chess_state));
			bcurrent = &moves[i];
			
			print_game(bcurrent);
			return true;
		}
	}
	
	send_message("# movement is not legal\n");
	
	for (i=0;i<mcnt;i++) {
		send_message("# ");
		send_move(moves[i].from, moves[i].to, moves[i].promo);
	}
	
	return false;
}


inline U64 get_rook_attack_bits(enum pos_t idx) {
	U64 index = bcurrent->all & iRook[idx];
	index *=RMagic[idx];
	index>>=RShiftBits[idx];
	return RAttack[idx][index];
}

inline U64 get_bishop_attack_bits(enum pos_t idx) {
	U64 index = bcurrent->all & iBishop[idx];
	index *=BMagic[idx];
	index>>=BShiftBits[idx];
	return BAttack[idx][index];
}


void update_static_attacks(bool is_white) {
	U64 attack=0;
	U64 tmp;
	U64 iso;
	int idx;
	
	if (is_white) {
		/* all static attacks */
		attack=0;
		/* all pawns*/
		attack|=(bcurrent->bPawn<<9)&~leftBorder;
		attack|=(bcurrent->bPawn<<7)&~rightBorder;
		
		tmp = attack;
		while (tmp) {
			iso=tmp & (0-tmp);
			idx=bitScanForward(iso);
			tmp^=iso;
		}
		
		/* all knights */
		tmp = bcurrent->bKnight;
		while (tmp) {
			iso=tmp & (0-tmp);
			idx=bitScanForward(iso);
			attack|=mKnight[idx];
			tmp^=iso;
		}
		
		/* the king */
		idx=bitScanForward(bcurrent->bKing);
		attack|=mKing[idx];
//		bcurrent->piece_board[idx]=King;
		
		bcurrent->all_white_attacks = attack;
	} else {
		
		attack=0;
		/* all black pawns*/
		attack|=(bcurrent->bpawn>>9)&~rightBorder;
		attack|=(bcurrent->bpawn>>7)&~leftBorder;
		
		tmp = attack;
		while (tmp) {
			iso=tmp & (0-tmp);
			idx=bitScanForward(iso);
//			bcurrent->piece_board[idx]=pawn;
			tmp^=iso;
		}
		
		/* all knights */
		tmp = bcurrent->bknight;
		while (tmp) {
			iso=tmp & (0-tmp);
			idx=bitScanForward(iso);
			attack|=mKnight[idx];
//			bcurrent->piece_board[idx]=knight;
			tmp^=iso;
		}
		/* the black king */
		idx=bitScanForward(bcurrent->bking);
		attack|=mKing[idx];
//		bcurrent->piece_board[idx]=king;
		
		bcurrent->all_black_attacks = attack;
	}
}

void update_all_attack_bits() {
	U64 attack=0;
	U64 tmp;
	U64 iso;
	
	int idx;
	
/*	for (idx=0;idx<64;idx++) {
		bcurrent->piece_board[idx]=empty;
	}
	*/
	
	update_static_attacks(true);
	update_static_attacks(false);
	
	attack=0;
	/* all rook attacks */
	tmp = bcurrent->bRook;
	while (tmp) {
		iso=tmp & (0-tmp);
		idx=bitScanForward(iso);
		attack|=get_rook_attack_bits(idx);
//		bcurrent->piece_board[idx]=Rook;
		tmp^=iso;
	}
	/* all bishop attacks */
	tmp = bcurrent->bBishop;
	while (tmp) {
		iso=tmp & (0-tmp);
		idx=bitScanForward(iso);
		attack|=get_bishop_attack_bits(idx);
//		bcurrent->piece_board[idx]=Bishop;
		tmp^=iso;
	}
	/* all queens */
	tmp = bcurrent->bQueen;
	while (tmp) {
		iso=tmp & (0-tmp);
		idx=bitScanForward(iso);
		attack|=get_bishop_attack_bits(idx);
		attack|=get_rook_attack_bits(idx);
//		bcurrent->piece_board[idx]=Queen;
		tmp^=iso;
	}
//	bcurrent->all_white_sliding = attack;
	
	bcurrent->all_white_attacks |= attack;
	
	attack = 0;
	/* all rook attacks */
	tmp = bcurrent->brook;
	while (tmp) {
		iso=tmp & (0-tmp);
		idx=bitScanForward(iso);
		attack|=get_rook_attack_bits(idx);
//		bcurrent->piece_board[idx]=rook;
		tmp^=iso;
	}
	/* all bishop attacks */
	tmp = bcurrent->bbishop;
	while (tmp) {
		iso=tmp & (0-tmp);
		idx=bitScanForward(iso);
		attack|=get_bishop_attack_bits(idx);
//		bcurrent->piece_board[idx]=bishop;
		tmp^=iso;
	}
	/* all queens */
	tmp = bcurrent->bqueen;
	while (tmp) {
		iso=tmp & (0-tmp);
		idx=bitScanForward(iso);
		attack|=get_bishop_attack_bits(idx);
		attack|=get_rook_attack_bits(idx);
//		bcurrent->piece_board[idx]=queen;
		tmp^=iso;
	}
//	bcurrent->all_black_sliding = attack;
	bcurrent->all_black_attacks |= attack;
	
/*	tmp = bcurrent->all;
	while (tmp) {
		iso=tmp & (0-tmp);
		idx=bitScanForward(iso);
		
		bcurrent->piece_board[idx]=getPieceFromBitboard(getBitboardAt(iso));
		
		tmp^=iso;
	}*/
	
}

/*
void update_attack(bool is_undo) {
	U64 attack=0;
	U64 tmp;
	U64 iso;
	int idx;
	enum pos_t from = bcurrent->from;
	enum pos_t to   = bcurrent->to;
	
	bool update_black_static=false;
	bool update_white_static=false;
	
	bool update_black_sliding=false;
	bool update_white_sliding=false;
	
	update_all_attack_bits();
	return;
	
	if (bcurrent->promo!=empty||bcurrent->is_enpas_cap||bcurrent->is_enpas_cap||bcurrent->is_long_castle) {
		update_all_attack_bits();
		return;
	} else {
		
		if (!is_undo) {
			if (is_static(bcurrent->new_piece)) {
				if (is_white(bcurrent->new_piece)) {
					update_white_static=true;
				} else {
					update_black_static=true;
				}
			}
			if (is_static(bcurrent->old_piece)) {
				if (is_white(bcurrent->old_piece)) {
					update_white_static=true;
				} else {
					update_black_static=true;
				}
			}
			if (is_sliding(bcurrent->new_piece)) {
				if (is_white(bcurrent->new_piece)) {
					update_white_sliding=true;
				} else {
					update_black_sliding=true;
				}
			}
			if (is_sliding(bcurrent->old_piece)) {
				if (is_white(bcurrent->old_piece)) {
					update_white_sliding=true;
				} else {
					update_black_sliding=true;
				}
			}
		} else {
			if (is_static(bcurrent->old_piece)) {
				if (is_white(bcurrent->old_piece)) {
					update_white_static=true;
				} else {
					update_black_static=true;
				}
			}
			if (is_static(bcurrent->new_piece)) {
				if (is_white(bcurrent->new_piece)) {
					update_white_static=true;
				} else {
					update_black_static=true;
				}
			}
			if (is_sliding(bcurrent->old_piece)) {
				if (is_white(bcurrent->old_piece)) {
					update_white_sliding=true;
				} else {
					update_black_sliding=true;
				}
			}
			if (is_sliding(bcurrent->new_piece)) {
				if (is_white(bcurrent->new_piece)) {
					update_white_sliding=true;
				} else {
					update_black_sliding=true;
				}
			}
		}
		
	}
	
	bcurrent->sliding_board[from]=0;
	bcurrent->sliding_board[to]=0;
	
	tmp=(bcurrent->bQueen|bcurrent->bqueen|bcurrent->bBishop|bcurrent->bbishop);
	attack  = get_bishop_attack_bits(from)&tmp;
	attack |= get_bishop_attack_bits(to)&tmp;
	
	tmp=(bcurrent->bQueen|bcurrent->bqueen|bcurrent->bRook|bcurrent->brook);
	attack |= get_rook_attack_bits(from)&tmp;
	attack |= get_rook_attack_bits(to)&tmp;
	
	if (!is_undo) {
		if (is_sliding(bcurrent->new_piece))
			attack|=1ULL<<to;
	} else {
		if (is_sliding(bcurrent->new_piece))
			attack|=1ULL<<from;
		if (is_sliding(bcurrent->old_piece))
			attack|=1ULL<<to;
	}
	
	while (attack) {
		iso=attack & (0-attack);
		idx=bitScanForward(iso);
		
		switch (piece_board[idx]) {
			case Rook:
				update_white_sliding=true;
				bcurrent->sliding_board[idx]=get_rook_attack_bits(idx);
				break;
			case rook:
				update_black_sliding=true;
				bcurrent->sliding_board[idx]=get_rook_attack_bits(idx);
				break;
			case Bishop:
				update_white_sliding=true;
				bcurrent->sliding_board[idx]=get_bishop_attack_bits(idx);
				break;
			case bishop:
				update_black_sliding=true;
				bcurrent->sliding_board[idx]=get_bishop_attack_bits(idx);
				break;
			case Queen:
				update_white_sliding=true;
				bcurrent->sliding_board[idx]=get_rook_attack_bits(idx);
				bcurrent->sliding_board[idx]|=get_bishop_attack_bits(idx);
				break;
			case queen:
				update_black_sliding=true;
				bcurrent->sliding_board[idx]=get_rook_attack_bits(idx);
				bcurrent->sliding_board[idx]|=get_bishop_attack_bits(idx);
				break;
			default:
				sprintf(outbuf, "# ERROR, non sliding figure %c updated at %d\n", piece_board[idx], idx);
				send_message(outbuf);
				
		}
		attack^=iso;
	}
	
	if (update_white_static) update_static_attacks(true);
	if (update_black_static) update_static_attacks(false);
	
	if (update_white_sliding) {
		bcurrent->all_white_sliding = 0;
		tmp=bcurrent->bRook | bcurrent->bBishop | bcurrent->bQueen;
		while (tmp) {
			iso=tmp & (0-tmp);
			idx=bitScanForward(iso);
			bcurrent->all_white_sliding|=bcurrent->sliding_board[idx];
			tmp^=iso;
		}
	}
	
	if (update_black_sliding) {
		bcurrent->all_black_sliding = 0;
		tmp=bcurrent->brook | bcurrent->bbishop | bcurrent->bqueen;
		while (tmp) {
			iso=tmp & (0-tmp);
			idx=bitScanForward(iso);
			bcurrent->all_black_sliding|=bcurrent->sliding_board[idx];
			tmp^=iso;
		}
	}

	bcurrent->all_black_attacks = bcurrent->all_black_static | bcurrent->all_black_sliding;
	bcurrent->all_white_attacks = bcurrent->all_white_static | bcurrent->all_white_sliding;
	
}
*/



void print_game(struct chess_state *state) {
	int i, j;
	U64 bto=1;
	char board[8][8];
	U64 *bp[20];
	int bpn = 13;
	char bc[] = {'#', 'p', 'n', 'k', 'q', 'b', 'r', 'P', 'N', 'K', 'Q', 'B', 'R'};
	bp[0]=&state->benpas;
	
	bp[1]=&state->bpawn;
	bp[2]=&state->bknight;
	bp[3]=&state->bking;
	bp[4]=&state->bqueen;
	bp[5]=&state->bbishop;
	bp[6]=&state->brook;
	
	bp[7]=&state->bPawn;
	bp[8]=&state->bKnight;
	bp[9]=&state->bKing;
	bp[10]=&state->bQueen;
	bp[11]=&state->bBishop;
	bp[12]=&state->bRook;
	
	memset(board, '.', sizeof board);
	
	
	while (bto) {
		char c=getCharAt(state, bto);
		i=bitScanForward(bto);
		if (c=='p') c='d';
		board[7-(i>>3)][i&7]=c;
		bto<<=1;
	}
	
	for (i=0;i<8;i++) {
		sprintf(outbuf, "#%d                  \n", 8-i);
		for (j=0;j<8;j++) outbuf[j*2+3]=board[i][j];
		send_message(outbuf);
	}
	send_message("#  a b c d e f g h\n");
	
	sprintf(outbuf, "# Castling: WQ WK BQ BK\n");
	if (!(state->cas&CAS_WQ)) {outbuf[12]='_'; outbuf[13]='_';}
	if (!(state->cas&CAS_WK)) {outbuf[15]='_'; outbuf[16]='_';}
	if (!(state->cas&CAS_BQ)) {outbuf[18]='_'; outbuf[19]='_';}
	if (!(state->cas&CAS_BK)) {outbuf[21]='_'; outbuf[22]='_';}
	
	send_message(outbuf);
	
	if (WTM) {
		send_message("# White is to move\n\n");
	} else {
		send_message("# Black is to move\n\n");
	}
	
	for (i=0;i<bpn-1;i++) {
		for (j=i+1;j<bpn;j++) {
			if (((*bp[i])&(*bp[j]))!=0) {
				sprintf(outbuf, "# ERROR: bitboards %c and %c collide!\n", bc[i], bc[j]);
				send_message(outbuf);
				output_bitboard(*bp[i]);
				output_bitboard(*bp[j]);
				//usleep(2000000);
			}
		}
	}
	
	//print_board();
}


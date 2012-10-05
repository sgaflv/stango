
#include "move_generator.h"



struct chess_state *best_move_found;
//struct chess_move history[500];
int current_move;


int nodes_checked;

int root_score;
enum pos_t last_capture_square;   // square of the last capture


U64 *get_bitboard(struct chess_state *state, enum piece_t piece) {
	U64 *t= &state->all_white;
	return t+piece;
}


enum piece_t get_bpiece(struct chess_state *state, U64 *bitboard) {
	if (&state->benpas == bitboard) return enpas;
	
	if (&state->bKing == bitboard) return King;
	if (&state->bQueen == bitboard) return Queen;
	if (&state->bBishop == bitboard) return Bishop;
	if (&state->bRook == bitboard) return Rook;
	if (&state->bPawn == bitboard) return Pawn;
	if (&state->bKnight == bitboard) return Knight;

	if (&state->bking == bitboard) return king;
	if (&state->bqueen == bitboard) return queen;
	if (&state->bbishop == bitboard) return bishop;
	if (&state->brook == bitboard) return rook;
	if (&state->bpawn == bitboard) return pawn;
	if (&state->bknight == bitboard) return knight;
	
	return empty;
	
	/*U64 *t=&state->all_white;
	return bitboard - t;
	*/
}

enum piece_t get_bto_piece(struct chess_state *state, U64 bto) {
	if (state->benpas & bto) return enpas;
	if (!(state->all & bto)) return empty;
	
	if (state->all_white & bto) {
		if (state->bPawn & bto) return Pawn;
		
		if (state->bKing & bto) return King;
		if (state->bQueen & bto) return Queen;
		if (state->bBishop & bto) return Bishop;
		if (state->bRook & bto) return Rook;
		if (state->bKnight & bto) return Knight;
		
	} else {
		if (state->bpawn & bto) return pawn;
		
		if (state->bking & bto) return king;
		if (state->bqueen & bto) return queen;
		if (state->bbishop & bto) return bishop;
		if (state->brook & bto) return rook;
		if (state->bknight & bto) return knight;
	}

	
	return empty;
	
	/*U64 *t=&state->all_white;
	return bitboard - t;
	*/
}


void setup_board(char *fen)
{
	int rank, file;
	
	while (*fen==' '||*fen=='\t') fen++;
	
	bcurrent=&history[0];
	
	memset(bcurrent, 0, sizeof (struct chess_state));
	
	rank=R8; file=FA;
	
	while (rank>R1 || file<=FH) {
		switch (*fen) {
			case 'K': add_bit(&bcurrent->bKing, file++, rank);   break;
			case 'Q': add_bit(&bcurrent->bQueen, file++, rank);  break;
			case 'B': add_bit(&bcurrent->bBishop, file++, rank); break;
			case 'R': add_bit(&bcurrent->bRook, file++, rank);   break;
			case 'N': add_bit(&bcurrent->bKnight, file++, rank); break;
			case 'P': add_bit(&bcurrent->bPawn, file++, rank);   break;
			
			case 'k': add_bit(&bcurrent->bking, file++, rank);   break;
			case 'q': add_bit(&bcurrent->bqueen, file++, rank);  break;
			case 'b': add_bit(&bcurrent->bbishop, file++, rank); break;
			case 'r': add_bit(&bcurrent->brook, file++, rank);   break;
			case 'n': add_bit(&bcurrent->bknight, file++, rank); break;
			case 'p': add_bit(&bcurrent->bpawn, file++, rank);   break;
			
			case '/': rank--; file = FA; break;
			
			case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8':
				file+=  *fen - '0';
				break;
			default:
			send_message("# fen error\n");
			return;
		}
		
		fen++;
	}
		
	if (fen[1]=='w') 
		ply = 0;
	else {
		ply = 1;
	}
	
	fen += 2;

	while (*fen) {
		switch (*fen) {
			case 'K': bcurrent->cas|=CAS_WK; break;
			case 'Q': bcurrent->cas|=CAS_WQ; break;
			case 'k': bcurrent->cas|=CAS_BK; break;
			case 'q': bcurrent->cas|=CAS_BQ; break;
			case 'a': case 'b': case 'c': case 'd':
			case 'e': case 'f': case 'g': case 'h':
				bcurrent->benpas = 1ULL<<SQ(*fen-'a',WTM?R6:R3);
				break;
			default:
				break;
		}
		fen++;
	}
	
	if (ply>0)
		memcpy(&history[ply], &history[0], sizeof (struct chess_state));
		
	bcurrent=&history[ply];
	
	bcurrent->all_black = bcurrent->bbishop | bcurrent->brook | bcurrent->bqueen | bcurrent->bking | bcurrent->bpawn | bcurrent->bknight;
	bcurrent->all_white = bcurrent->bBishop | bcurrent->bRook | bcurrent->bQueen | bcurrent->bKing | bcurrent->bPawn | bcurrent->bKnight;
	bcurrent->all = bcurrent->all_black | bcurrent->all_white;
}


void get_fen(char *fen) {
	int rank, file;
	
	rank=R8; file=FA;
	for (rank=R8;rank>=R1;rank--) {
		int cnt=0;
		for (file=FA;file<=FH;file++) {
			char c = getCharAt(bcurrent, 1ULL<<SQ(file, rank));
			if (c=='#'||c=='.') {
				cnt++;
				continue;
			}
			
			if (cnt>0) {
				*fen++=((char)cnt)+'0';
				cnt=0;
			}
			*fen++=c;
		}
		if (cnt>0) {
			*fen++=((char)cnt)+'0';
			cnt=0;
		}
		if (rank>R1) *fen++='/';
	}
	
	*fen++=' ';
	if (WTM) *fen++='w';
	else *fen++='b';
	
	*fen++=' ';
	
	if (bcurrent->cas&CAS_WK) *fen++='K';
	if (bcurrent->cas&CAS_WQ) *fen++='Q';
	if (bcurrent->cas&CAS_BK) *fen++='k';
	if (bcurrent->cas&CAS_BQ) *fen++='q';
	
	if (bcurrent->benpas) {
		int idx = bitScanForward(bcurrent->benpas);
		char fl = F(idx);
		*fen++='a'+fl;
	}
	
	*fen++=' ';
	*fen++=0;
	
	
}


void fillAttacks(enum piece_t piece, U64 bfrom, U64 attacks, struct chess_state *moves, int *ret);
	

int generate_moves(struct chess_state *moves) {
	U64 pieces;
	U64 attacks;
	U64 bfrom;
	enum pos_t from;
	
	int ret=0;
	
	update_all_attack_bits();
	
	if (WTM) {
	
		pieces=bcurrent->bPawn;
		while (pieces) {
			bfrom=pieces&(0-pieces);
			
			from = bitScanForward(bfrom);
			attacks=mWPawnCapture[from]&(bcurrent->all_black|bcurrent->benpas);
			
			fillAttacks(Pawn, bfrom, attacks, moves, &ret);
			
			attacks=mWPawnMove[from]&(~bcurrent->all);
			if ((bfrom<<8)&attacks)
				fillAttacks(Pawn, bfrom, attacks, moves, &ret);
			
			pieces^=bfrom;
		}
	
		pieces=bcurrent->bKnight;
		while (pieces) {
			bfrom=pieces&(0-pieces);
			from = bitScanForward(bfrom);
			
			attacks=mKnight[from]&(~(bcurrent->all_white));
			fillAttacks(Knight, bfrom, attacks, moves, &ret);
			
			pieces^=bfrom;
		}
		
		pieces=bcurrent->bBishop;
		while (pieces) {
			bfrom=pieces&(0-pieces);
			from = bitScanForward(bfrom);
			
			attacks=get_bishop_attack_bits(from)&(~bcurrent->all_white);
			fillAttacks(Bishop, bfrom, attacks, moves, &ret);
			
			pieces^=bfrom;
		}
		
		pieces=bcurrent->bRook;
		while (pieces) {
			bfrom=pieces&(0-pieces);
			from = bitScanForward(bfrom);
			
			attacks=get_rook_attack_bits(from)&(~bcurrent->all_white);
			fillAttacks(Rook, bfrom, attacks, moves, &ret);
			
			pieces^=bfrom;
		}
		
		
		pieces=bcurrent->bQueen;
		while (pieces) {
			bfrom=pieces&(0-pieces);
			from = bitScanForward(bfrom);
			
			attacks=(get_bishop_attack_bits(from)|get_rook_attack_bits(from))&(~bcurrent->all_white);
			fillAttacks(Queen, bfrom, attacks, moves, &ret);
			
			pieces^=bfrom;
		}
		
		from = bitScanForward(bcurrent->bKing);
		attacks=mKing[from];
		if (from==E1) attacks|=0x44;

		attacks&=(~bcurrent->all_white);
		
		fillAttacks(King, bcurrent->bKing, attacks, moves, &ret);
		
	} else {
		
		pieces=bcurrent->bpawn;
		while (pieces) {
			/* get the last bit to get the figure */
			bfrom= pieces&(0-pieces);
			from = bitScanForward(bfrom);
			
			attacks=mBPawnCapture[from]&(bcurrent->all_white|bcurrent->benpas);
			
			
			fillAttacks(pawn, bfrom, attacks, moves, &ret);
			
			attacks=mBPawnMove[from]&(~bcurrent->all);
			if ((bfrom>>8)&attacks)
				fillAttacks(pawn, bfrom, attacks, moves, &ret);
			
			
			pieces^=bfrom;
		}

		pieces=bcurrent->bknight;
		while (pieces) {
			bfrom=pieces&(0-pieces);
			from = bitScanForward(bfrom);
			attacks=mKnight[from]&(~bcurrent->all_black);
			fillAttacks(knight, bfrom, attacks, moves, &ret);
			
			pieces^=bfrom;
		}
		
		pieces=bcurrent->bbishop;
		while (pieces) {
			bfrom=pieces&(0-pieces);
			from = bitScanForward(bfrom);
			
			attacks=get_bishop_attack_bits(from)&(~bcurrent->all_black);
			fillAttacks(bishop, bfrom, attacks, moves, &ret);
			
			pieces^=bfrom;
		}
		
		
		pieces=bcurrent->brook;
		while (pieces) {
			bfrom=pieces&(0-pieces);
			from = bitScanForward(bfrom);
			
			attacks=get_rook_attack_bits(from)&(~bcurrent->all_black);
			fillAttacks(rook, bfrom, attacks, moves, &ret);
			
			pieces^=bfrom;
		}
		
		
		pieces=bcurrent->bqueen;
		while (pieces) {
			bfrom=pieces&(0-pieces);
			from = bitScanForward(bfrom);
			
			attacks=(get_bishop_attack_bits(from)|get_rook_attack_bits(from))&(~bcurrent->all_black);
			fillAttacks(queen, bfrom, attacks, moves, &ret);
			
			pieces^=bfrom;
		}


		from = bitScanForward(bcurrent->bking);
		attacks=(mKing[from]);
		if (from==E8) attacks|=0x4400000000000000;
		attacks&=(~bcurrent->all_black);
		fillAttacks(king, bcurrent->bking, attacks, moves, &ret);
	}
	
	return ret;
}



inline bool is_hit(int idx, bool is_white) {
	U64 bits;
	if (is_white) {
		bits = get_rook_attack_bits(idx)&(bcurrent->bqueen | bcurrent->brook);
		if (bits) return true;
		bits = get_bishop_attack_bits(idx)&(bcurrent->bqueen | bcurrent->bbishop);
		if (bits) return true;
		bits = mKnight[idx]&bcurrent->bknight;
		if (bits) return true;
		
		bits=(bcurrent->bpawn>>9)&~rightBorder;
		bits|=(bcurrent->bpawn>>7)&~leftBorder;
		bits|=mKing[bitScanForward(bcurrent->bking)];
		
		if (bits&(1ULL<<idx)) return true;
	} else {
		bits = get_rook_attack_bits(idx)&(bcurrent->bQueen | bcurrent->bRook);
		if (bits) return true;
		bits = get_bishop_attack_bits(idx)&(bcurrent->bQueen | bcurrent->bBishop);
		if (bits) return true;
		bits = mKnight[idx]&bcurrent->bKnight;
		if (bits) return true;
		
		bits=(bcurrent->bPawn<<9)&~leftBorder;
		bits|=(bcurrent->bPawn<<7)&~rightBorder;
		bits|=mKing[bitScanForward(bcurrent->bKing)];
		if (bits&(1ULL<<idx)) return true;
	}
	
	return false;
}


inline bool is_king_safe(struct chess_state *move)
{
	bool failed = false;
	/* apply move, and see if the king gets in check */
	struct chess_state *tmp;
	tmp = bcurrent;
	
	bcurrent = move;
	
	if (WTM) {
		enum pos_t idx = bitScanForward(bcurrent->bKing);
		if (is_hit(idx, true)) {
			failed=true;
		}
	} else {
		enum pos_t idx = bitScanForward(bcurrent->bking);
		if (is_hit(idx, false)) {
			failed=true;
		}
	}
	
	bcurrent=tmp;
	
	if (failed) return false;
	return true;
}


void fillAttacks(enum piece_t piece, U64 bfrom, U64 attacks, struct chess_state *moves, int *ret) {
	
	U64 bto;
	U64 hits;
	enum pos_t from = bitScanForward(bfrom);
	enum pos_t to;
	U64 *b_piece;
	struct chess_state *move;
	
	enum piece_t new_piece = piece;
	enum piece_t old_piece;
	
	
	while (attacks) {
		
		bto= attacks&(0-attacks);
		to = bitScanForward(bto);
		move = &moves[*ret];
		memcpy(move, bcurrent, sizeof (struct chess_state));
		move->move_value-=piece_value[new_piece];
		move->from = from;
		move->to = to;
		
		old_piece = get_bto_piece(move, bto);
		
		if (old_piece==enpas) old_piece=empty;
		move->promo = empty;
		move->is_capture = false;
		
		if (old_piece!=empty) {
			b_piece = get_bitboard(move, old_piece);
			*b_piece ^= bto;
			if (WTM) {
				move->all_black ^= bto;
			} else {
				move->all_white ^= bto;
			}
			
			move->move_value+=piece_value[old_piece];
			move->is_capture=true;
		}
		
		
		b_piece = get_bitboard(move, new_piece);
		if (b_piece!=NULL) {
			*b_piece ^= bfrom|bto;
			if (WTM) {
				move->all_white ^= bfrom|bto;
			} else {
				move->all_black ^= bfrom|bto;
			}
		}
		
		move->benpas = 0;
		/* pawns */
		if (new_piece==Pawn||new_piece==pawn) {
			
			if (bto&bcurrent->benpas) {
//				move->is_enpas_cap = true;
				
				if (new_piece==Pawn) {
					move->bpawn ^= bto>>8;
					move->all_black ^= bto>>8;
				} else {
					move->bPawn ^= bto<<8;
					move->all_white ^= bto<<8;
				}
			}
			
			if (bto&0xff000000000000ff) {
				if (WTM) {
					move->bQueen ^= bto;
					move->bPawn  ^= bto;
					move->promo = Queen;
					
				} else {
					move->bqueen ^= bto;
					move->bpawn  ^= bto;
					move->promo = queen;
				}
			}
			
			if (new_piece==pawn&&(bto<<16)==bfrom) {
				move->benpas = bto<<8;
			}
			
			if (new_piece==Pawn&&(bto>>16)==bfrom) {
				move->benpas = bto>>8;
			}
		}
		
		
		/* castling */
		if (new_piece==King) {
			if (from==E1&&(to==G1||to==C1)) {
				
				hits=bcurrent->all_black_attacks;
				
				if (bfrom&hits||
						(to==G1&&!(bcurrent->cas&CAS_WK))||
						(to==C1&&!(bcurrent->cas&CAS_WQ))
						) {
					attacks^=bto;
					continue;
				}
				
				if (to==G1&&( (0x0000000000000060) & (hits|bcurrent->all)  )) {
					attacks^=bto;
					continue;
				}
				
				if (to==C1&&( (0x000000000000000E&bcurrent->all) || (0x000000000000000C&hits)  )) {
					attacks^=bto;
					continue;
				}
				
				/* castling is accepted, apply changes to the bitboard */
				if (to==G1) {
					U64 bb = (1ULL<<H1)|(1ULL<<F1);
					move->bRook ^= bb;
					move->all_white ^= bb;
				}
				
				
				if (to==C1) {
					U64 bb = (1ULL<<A1)|(1ULL<<D1);
					move->bRook ^= bb;
					move->all_white ^= bb;
				}
			}
			
		} else if (new_piece==king) {
			if (from==E8&&(to==G8||to==C8)) {
				hits=bcurrent->all_white_attacks;
				
				if (bfrom&hits||
						(to==G8&&!(bcurrent->cas&CAS_BK))||
						(to==C8&&!(bcurrent->cas&CAS_BQ))
						) {
					attacks^=bto;
					continue;
				}
				
				if (to==G8&&( (0x6000000000000000) & (hits|bcurrent->all)  )) {
					attacks^=bto;
					continue;
				}
				
				if (to==C8&&( (0x0E00000000000000&bcurrent->all) || (0x0C00000000000000&hits)  )) {
					attacks^=bto;
					continue;
				}
				
				/* castling is accepted, apply changes to the bitboard */
				if (to==G8) {
					U64 bb = (1ULL<<H8)|(1ULL<<F8);
					move->brook ^= bb;
					move->all_black ^= bb;
				}
				if (to==C8) {
					U64 bb = (1ULL<<A8)|(1ULL<<D8);
					move->brook ^= bb;
					move->all_black ^= bb;
				}
				
//				if (to==G8) move->is_short_castle=true;
//				if (to==C8) move->is_long_castle=true;
			}
		}
		
		/* disable castling if any movement of these squares */
		if (move->cas) {
			if (from==E1||from==A1||to==A1) move->cas &= CAS_WK|CAS_BQ|CAS_BK;
			if (from==E1||from==H1||to==H1) move->cas &= CAS_WQ|CAS_BQ|CAS_BK;
			if (from==E8||from==A8||to==A8) move->cas &= CAS_WQ|CAS_WK|CAS_BK;
			if (from==E8||from==H8||to==H8) move->cas &= CAS_WQ|CAS_WK|CAS_BQ;
		}
		
		move->all = move->all_black | move->all_white;
		
		if (!is_king_safe(move)) {
			attacks^=bto;
			continue;
		}
		

		
		if (move->promo==Queen) {
			// if a pawn has gotten a promotion, every other promotion is also possible
			(*ret)++;
			memcpy(&moves[*ret], move, sizeof (struct chess_state));
			moves[*ret].promo = Bishop;
			moves[*ret].bQueen ^= bto;
			moves[*ret].bBishop ^= bto;
			
			(*ret)++; 
			memcpy(&moves[*ret], move, sizeof (struct chess_state));
			moves[*ret].promo = Rook;
			moves[*ret].bQueen ^= bto;
			moves[*ret].bRook ^= bto;
			
			(*ret)++; 
			memcpy(&moves[*ret], move, sizeof (struct chess_state));
			moves[*ret].promo = Knight;
			moves[*ret].bQueen ^= bto;
			moves[*ret].bKnight ^= bto;
			
		} else if (move->promo==queen) {
			(*ret)++;
			memcpy(&moves[*ret], move, sizeof (struct chess_state));
			moves[*ret].promo = bishop;
			moves[*ret].bqueen ^= bto;
			moves[*ret].bbishop ^= bto;
			
			(*ret)++;
			memcpy(&moves[*ret], move, sizeof (struct chess_state));
			moves[*ret].promo = rook;
			moves[*ret].bqueen ^= bto;
			moves[*ret].brook ^= bto;
			
			(*ret)++; 
			memcpy(&moves[*ret], move, sizeof (struct chess_state));
			moves[*ret].promo = knight;
			moves[*ret].bqueen ^= bto;
			moves[*ret].bknight ^= bto;
			
		}
		
		(*ret)++;
		attacks^=bto;
	}
}

static int transpositions;
static int collisions;

U64 perft(int depth)
{
	struct chess_state moves[256];
	int n_moves, i;
	U64 nodes = 0;
	int a=0;
	U64 hnum;
	struct hash_t *h;
	
	if (depth == 0) {
//		print_game();
//		output_bitboard(bcurrent->all_black_attacks);
		return 1;
	}
	
	hnum = compute_hash(bcurrent);
	h = &hash[HASH(hnum)];
	
	if (h->key==hnum&&h->depth==depth) {
		transpositions++;
		return h->score;
	}
	
	/*
	if (h->key==hnum&&!states_equal(&h->state, bcurrent)) {
		collisions++;
		
		printf("collision for %llX idx=%d and %llX idx=%d\n", hnum, HASH(hnum), h->key, HASH(h->key));
		print_game(bcurrent);
		print_game(&h->state);
	};
	*/
	
	
	n_moves = generate_moves(moves);
	
	/*
	{
		int k;
		get_fen(outbuf);
		setup_board2(outbuf);
		k = perft2(1);
		if (n_moves!=k) {
			printf("%s\n", outbuf);
			print_board2();
			cmd_list_moves(NULL);
			
			print_game(bcurrent);
			
			for (i = 0; i < n_moves; i++) {
				send_move(moves[i].from, moves[i].to, moves[i].promo);
			}
			printf("%d moves against %d moves\n", n_moves, k);
			exit(0);
		}
		
	}
*/

	//if (depth==1) return n_moves;
	//if (depth==7) a=12;
	for (i = a; i < n_moves; i++) {
		
		if (depth==7) {
			printf("%d of %d\n", i+1, n_moves);
		}
		//send_move(moves[i].from, moves[i].to);
		
		bcurrent = &moves[i];
//		send_message("## "); send_move(bcurrent->from, bcurrent->to);
		ply++;
		
		nodes += perft(depth - 1);
		ply--;
	}
	

	h->key=hnum;
	h->depth=depth;
//	memcpy(&h->state, bcurrent, sizeof(struct chess_state));
	h->score=nodes;


	return nodes;
}



U64 test_moves(char *fen, int depth)
{//test move generators
	U64 total=0;
	clock_t start, finish;
	
	setup_board(fen);
	collisions=0;
	transpositions=0;
	
	printf("testing began\n");
	
	start = clock();
	
	total = perft(depth);
	finish = clock();
	
	printf("testing ended\n");
	printf("collisions found %d   transpositions found %d \n", collisions, transpositions);
	double time = (double)(finish-start)/CLOCKS_PER_SEC;
	
	sprintf(outbuf, "# Time elapsed: %0.2lf\n# Nodes checked:                      %d\n"
		"# Nodes per second: %0.2lf\n", 
		time, (int)total, total/time);
	send_message(outbuf);
	return total;
}

int test_all_moves()
{
	char *test1="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq ";
	char *test2="r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq";
	char *test3="8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w ";
	char *test4="r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -";
	char *test4m="r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ ";
	char *test5="n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - ";
	
	char *test7="r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";

	
	if (test_moves(test5, 1)!=24)        printf("ERROR!\t\t\t\t               Test5(1) failed!\n");
	if (test_moves(test7, 1)!=48)         printf("ERROR!\t\t\t\t               Test7(1) failed!\n");
	if (test_moves(test5, 2)!=496)       printf("ERROR!\t\t\t\t               Test5(2) failed!\n");
	if (test_moves(test7, 2)!=2039)       printf("ERROR!\t\t\t\t               Test7(2) failed!\n");
	if (test_moves(test5, 3)!=9483)      printf("ERROR!\t\t\t\t               Test5(3) failed!\n");
	if (test_moves(test3, 5)!=674624)    printf("ERROR!\t\t\t\t               Test3(5) failed!\n");
	
	if (test_moves(test1, 5)!=4865609)   printf("ERROR!\t\t\t\t               Test1(5) failed!\n");

	// various promotion tests
	if (test_moves(test5, 4)!=182838)    printf("ERROR!\t\t\t\t               Test5(4) failed!\n");
	if (test_moves(test5, 5)!=3605103)   printf("ERROR!\t\t\t\t               Test5(5) failed!\n");
	
	// other tests
	if (test_moves(test7, 3)!=97862)      printf("ERROR!\t\t\t\t               Test7(3) failed!\n");
	if (test_moves(test7, 4)!=4085603)    printf("ERROR!\t\t\t\t               Test7(4) failed!\n");
	
	if (test_moves(test4, 5)!=15833292)  printf("ERROR!\t\t\t\t               Test4(5) failed!\n");
	if (test_moves(test4m, 5)!=15833292) printf("ERROR!\t\t\t\t               Test4m(5) failed!\n");
	if (test_moves(test5, 6)!=71179139)  printf("ERROR!               Test5(6) failed!\n");
	if (test_moves(test2, 5)!=193690690) printf("ERROR!               Test2(5) failed!\n");
//	if (test_moves(test7, 5)!=193690690)  printf("ERROR!               Test7(5) failed!\n");
//	if (test_moves(test7, 6)!=8031647685) printf("ERROR!               Test7(6) failed!\n");
	return 0;
}






int alphaBetaMax( int alpha, int beta, int depth, bool first ) {
	int mcnt, i, score=-INF;
	int oldalpha=alpha;
	int oldbeta=beta;
	struct chess_state moves[255];
	
	struct chess_state *tmp;
	int next_depth;
	int mv;
	int rnd = random();
	U64 hnum=0;
	struct hash_t *h;
	
	
	
	if (depth == 0 || game_setup.forced ) return evaluate_position();
	
	h = &hash[0];
	
	hnum = compute_hash(bcurrent);
	h = &hash[HASH(hnum)];
	
	if (!first&&h->key==hnum&&h->depth>=depth) {
		transpositions++;
//		if (h->score<beta) beta=h->score;
//		if (alpha>=beta) return beta;
	}

	mcnt = generate_moves(moves);
	
	if (mcnt==0) {
		if ((bcurrent->bKing&bcurrent->all_black_attacks)!=0) return -50000-depth;
		if ((bcurrent->bking&bcurrent->all_white_attacks)!=0) return 50000+depth;
		return 0;
	}
	
	{
//		unsigned char movemap[mcnt];
//		memcpy(movemap, MP0_254, sizeof mcnt);
//		qsort(movemap, mcnt, sizeof (char), compareDec);
		
		
		for (i=0;i<mcnt;i++) {
			
	/*		if (first) {
				sprintf(outbuf, "# thinking %d of %d\n", i+1, mcnt);
				send_message(outbuf);
			}*/
			
			mv=(rnd+i)%mcnt;
		
			tmp = bcurrent;
			bcurrent = &moves[mv];
			ply++;
			next_depth=depth - 1;
			
			if (moves[mv].is_capture) {
				if (depth<2) {
					if (moves[mv].to==last_capture_square) {
						next_depth++;
					} else if (WTM&&!first) {
						/* avoid unforced capture at the event horizon */
						bcurrent = tmp;
						ply--;
						continue;
					}
				}
				
				last_capture_square=moves[mv].to;
			}
			
			if (!bcurrent->bKing||!bcurrent->bking) next_depth=0;
			
			score = alphaBetaMin( alpha, beta, next_depth, false ) ;
			
			bcurrent = tmp;
			ply--;
			
			if (score<=alpha) continue;
			
			alpha = score;
			if (first) {
				if (score<=-50000||score>=50000) {
					sprintf(outbuf, "# mating %d ", score);
					send_message(outbuf);
					send_move(moves[mv].from, moves[mv].to, moves[mv].promo);
				}
				memcpy(&history[ply+1], &moves[mv], sizeof (struct chess_state));
				best_move_found = &history[ply+1];
			}
			
			if (alpha<beta) continue;
			return alpha;
		}
	}
	
	score--;
	
	h->key=hnum;
	h->depth=depth;
	h->score=alpha;
	return alpha;
}

int alphaBetaMin( int alpha, int beta, int depth, bool first ) {
	int mcnt, i, score=INF;
	int oldalpha=alpha;
	int oldbeta=beta;
	struct chess_state moves[255];
	struct chess_state *tmp;
	int next_depth;
	int mv;
	int rnd=random();
	U64 hnum;
	struct hash_t *h;
	
	nodes_checked++;
	
	if (depth==0 || game_setup.forced) return evaluate_position();

//	hnum=0;
//	h = &hash[0];

	
	hnum = compute_hash(bcurrent);
	h = &hash[HASH(hnum)];
	
	if (!first&&h->key==hnum&&h->depth>=depth) {
		transpositions++;
//		if (h->score>alpha) alpha=h->score;
//		if (alpha>=beta) return alpha;
	}



	mcnt = generate_moves(moves);
	
	
	if (mcnt==0) {
		if ((bcurrent->bKing&bcurrent->all_black_attacks)!=0) return -50000-depth;
		if ((bcurrent->bking&bcurrent->all_white_attacks)!=0) return 50000+depth;
		return 0;
	}
	
	for (i=0;i<mcnt;i++) {
		
/*		if (first) {
			sprintf(outbuf, "# thinking %d of %d\n", i+1, mcnt);
			send_message(outbuf);
		}
		*/
		
		mv=(rnd+i)%mcnt;
		
		tmp = bcurrent;
		bcurrent = &moves[mv];
		ply++;
		next_depth=depth - 1;
		
		if (moves[mv].is_capture) {
			if (depth<2) {
				if (moves[mv].to==last_capture_square) {
					next_depth++;
				} else if (!WTM&&!first) {
					/* avoid unforced capture at the event horizon */
					bcurrent = tmp;
					//undo move
					ply--;
					continue;
				}
			}
			last_capture_square=moves[mv].to;
		}
			
		if (!bcurrent->bKing||!bcurrent->bking) next_depth=0;
		
		
		score = alphaBetaMax( alpha, beta, next_depth, false ) ;
		
		bcurrent = tmp;
		ply--;
		
		
		if (score>=beta) continue;
			
		beta = score;
		
		if (first) {
			if (score<=-50000||score>=50000) {
				sprintf(outbuf, "# mating %d ", score);
				send_message(outbuf);
				send_move(moves[mv].from, moves[mv].to, moves[mv].promo);
			}
				
			memcpy(&history[ply+1], &moves[mv], sizeof (struct chess_state));
			best_move_found = &history[ply+1];
		}
		
		if (alpha<beta) continue;
		
		return beta; // fail hard alpha-cutoff
	}
	
	score++;
	
	h->key=hnum;
	h->depth=depth;
	h->score=beta;
	
	return beta;
}

#define TIME 1.5

void make_smart_move() {
	enum pos_t from=A1;
	enum pos_t to=A1;
	int estimation;
	clock_t start, finish;
	
	
	best_move_found=NULL;
	
	last_capture_square=64;
	
	root_score=evaluate_position();
	
	bcurrent=&history[ply];
	
	start = clock();
	if (WTM) {
		int depth = 6;
		finish = clock();
		
		while ((double)(finish-start)/CLOCKS_PER_SEC<TIME) {
			sprintf(outbuf, "# white depth=%d\n", depth);
			send_message(outbuf);
			nodes_checked=0;
			transpositions=0;
			
			
			estimation=alphaBetaMax(-INF, INF, depth, true);
			if (best_move_found) {
				from = best_move_found->from;
				to = best_move_found->to;
			} else {
				break;
			}
			
			depth++;
			finish = clock();
		}
		
	} else {
		int depth=12;
		finish = clock();
		
		while ((double)(finish-start)/CLOCKS_PER_SEC<TIME) 
		{
			sprintf(outbuf, "# black depth=%d\n", depth);
			send_message(outbuf);
			nodes_checked=0;
			transpositions=0;
			
			estimation=alphaBetaMin(-INF, INF, depth, true);
			if (best_move_found) {
				from = best_move_found->from;
				to = best_move_found->to;
			} else {
				break;
			}
			depth++;
			finish = clock();
			break;
		}
	}
	
	finish = clock();
	
	if (game_setup.forced) {
		send_message("# forced quit\n");
		game_setup.computer[0]=false;
		game_setup.computer[1]=false;
		return;
	}
	
	if (best_move_found==NULL) {
		send_message("# no legal move found\n");
		game_setup.computer[0]=false;
		game_setup.computer[1]=false;
		
		
		
		get_fen(outbuf);
		send_message(outbuf);
		return;
	}
	
	send_move(from, to, best_move_found->promo);
	
	ply++;
	bcurrent = &history[ply];
	
	print_game(bcurrent);

	//accept(from, to, promote, true);
	
	double time = (double)(finish-start)/CLOCKS_PER_SEC;
	sprintf(outbuf, "# Time elapsed: %0.2lf\n# Nodes checked: %d\n"
			"# Nodes per second: %0.2lf\n# Current value: %d\n# Prognosed value: %d\nTranspositions used: %d\n", 
			time, nodes_checked, nodes_checked/time, evaluate_position(), estimation, transpositions);
	send_message(outbuf);
}



void test_thinking() {
	struct chess_state moves[255];
	
	
	char *pos[] = 
		{
			"1k1r4/pp1b1R2/3q2pp/4p3/2B5/4Q3/PPP2B2/2K5 b ",
			"3r1k2/4npp1/1ppr3p/p6P/P2PPPP1/1NR5/5K2/2R5 w",
			"2q1rr1k/3bbnnp/p2p1pp1/2pPp3/PpP1P1P1/1P2BNNP/2BQ1PRK/7R b ",
			"rnbqkb1r/p3pppp/1p6/2ppP3/3N4/2P5/PPP1QPPP/R1B1KB1R w KQkq ",
			"r1b2rk1/2q1b1pp/p2ppn2/1p6/3QP3/1BN1B3/PPP3PP/R4RK1 w ",
			"2r3k1/pppR1pp1/4p3/4P1P1/5P2/1P4K1/P1P5/8 w ",
			"1nk1r1r1/pp2n1pp/4p3/q2pPp1N/b1pP1P2/B1P2R2/2P1B1PP/R2Q2K1 w ",
			"4b3/p3kp2/6p1/3pP2p/2pP1P2/4K1P1/P3N2P/8 w ",
			"2kr1bnr/pbpq4/2n1pp2/3p3p/3P1P1B/2N2N1Q/PPP3PP/2KR1B1R w ",
			"3rr1k1/pp3pp1/1qn2np1/8/3p4/PP1R1P2/2P1NQPP/R1B3K1 b ",
			"2r1nrk1/p2q1ppp/bp1p4/n1pPp3/P1P1P3/2PBB1N1/4QPPP/R4RK1 w ",
			"r3r1k1/ppqb1ppp/8/4p1NQ/8/2P5/PP3PPP/R3R1K1 b ",
			"r2q1rk1/4bppp/p2p4/2pP4/3pP3/3Q4/PP1B1PPP/R3R1K1 w ",
			"rnb2r1k/pp2p2p/2pp2p1/q2P1p2/8/1Pb2NP1/PB2PPBP/R2Q1RK1 w ",
			"2r3k1/1p2q1pp/2b1pr2/p1pp4/6Q1/1P1PP1R1/P1PN2PP/5RK1 w ",
			"r1bqkb1r/4npp1/p1p4p/1p1pP1B1/8/1B6/PPPN1PPP/R2Q1RK1 w kq ",
			"r2q1rk1/1ppnbppp/p2p1nb1/3Pp3/2P1P1P1/2N2N1P/PPB1QP2/R1B2RK1 b ",
			"r1bq1rk1/pp2ppbp/2np2p1/2n5/P3PP2/N1P2N2/1PB3PP/R1B1QRK1 b ",
			"3rr3/2pq2pk/p2p1pnp/8/2QBPP2/1P6/P5PP/4RRK1 b ",
			"r4k2/pb2bp1r/1p1qp2p/3pNp2/3P1P2/2N3P1/PPP1Q2P/2KRR3 w ",
			"3rn2k/ppb2rpp/2ppqp2/5N2/2P1P3/1P5Q/PB3PPP/3RR1K1 w ",
			"2r2rk1/1bqnbpp1/1p1ppn1p/pP6/N1P1P3/P2B1N1P/1B2QPP1/R2R2K1 b ",
			"r1bqk2r/pp2bppp/2p5/3pP3/P2Q1P2/2N1B3/1PP3PP/R4RK1 b kq ",
			"r2qnrnk/p2b2b1/1p1p2pp/2pPpp2/1PP1P3/PRNBB3/3QNPPP/5RK1 w "
		};
		
	char *bm[] = 
		{
			"Qd1+", "d5", "f5", "e6", "Nd5", "g6", "Nf6", "f5", "f5", "Ne5", 
			"f4", "Bf5", "b4", "Qd2", "Qxg7", "Ne4", "h5", "Nb3", "Rxe4",
			"g4", "Nh6", "Bxe4", "f6", "f4"
		};
	
	
	setup_board(pos[0]);
	make_smart_move();
}


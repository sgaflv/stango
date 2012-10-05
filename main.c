#include "common.h"



U64 *getBitboardAt(U64 at) {
	if (bcurrent->benpas&at)   return &bcurrent->benpas;
	
	if (bcurrent->bpawn&at)   return &bcurrent->bpawn;
	if (bcurrent->bknight&at) return &bcurrent->bknight;
	if (bcurrent->bking&at)   return &bcurrent->bking;
	if (bcurrent->bqueen&at)  return &bcurrent->bqueen;
	if (bcurrent->bbishop&at) return &bcurrent->bbishop;
	if (bcurrent->brook&at)   return &bcurrent->brook;
	
	if (bcurrent->bPawn&at)   return &bcurrent->bPawn;
	if (bcurrent->bKnight&at) return &bcurrent->bKnight;
	if (bcurrent->bKing&at)   return &bcurrent->bKing;
	if (bcurrent->bQueen&at)  return &bcurrent->bQueen;
	if (bcurrent->bBishop&at) return &bcurrent->bBishop;
	if (bcurrent->bRook&at)   return &bcurrent->bRook;
	
	return NULL;
}


enum piece_t getPieceFromBitboard(U64 *b) {
	
	if (b==&bcurrent->bPawn)   return Pawn;
	if (b==&bcurrent->bRook)   return Rook;
	if (b==&bcurrent->bKnight) return Knight;
	if (b==&bcurrent->bBishop) return Bishop;
	if (b==&bcurrent->bQueen)  return Queen;
	if (b==&bcurrent->bKing)   return King;
	
	if (b==&bcurrent->bpawn)   return pawn;
	if (b==&bcurrent->brook)   return rook;
	if (b==&bcurrent->bknight) return knight;
	if (b==&bcurrent->bbishop) return bishop;
	if (b==&bcurrent->bqueen)  return queen;
	if (b==&bcurrent->bking)   return king;
	return empty;
}



/*
inline void do_bitboard_move(struct chess_state move) {
	
	if (ply==MAX_MOVE_COUNT) {
		send_message("FATAL ERROR: Move limit reached\n");
		exit(1);
	}
	
	bcurrent->from            = move.from;
	bcurrent->to              = move.to;
	bcurrent->old_piece       = move.old_piece;
	bcurrent->new_piece       = move.new_piece;
	bcurrent->promo           = move.promo;
	bcurrent->is_enpas_cap    = move.is_enpas_cap;
	bcurrent->is_long_castle  = move.is_long_castle;
	bcurrent->is_short_castle = move.is_short_castle;
	
	memcpy(&history[ply], &bcurrent, sizeof bcurrent);
	ply++;
	
	bcurrent->benpas^=move.x_enpas;
	bcurrent->bcas^= move.x_cas;
	
	*move.b_main^=move.x_main; 
	if (move.b_cap) *move.b_cap^=move.x_cap; 
	
	if (move.b_add) *move.b_add^=move.x_add;
	
	if (bcurrent->white_to_move) {
		
		bcurrent->all_white^=move.x_main;
		if (move.b_cap) bcurrent->all_black^=move.x_cap;
		if (move.b_add) bcurrent->all_white^=move.x_add;
	} else {
		
		bcurrent->all_black^=move.x_main;
		if (move.b_cap) bcurrent->all_white^=move.x_cap;
		if (move.b_add) bcurrent->all_black^=move.x_add;
	}
	
	bcurrent->all = bcurrent->all_white | bcurrent->all_black;
	
	
	piece_board[bcurrent->to]   = bcurrent->new_piece;
	piece_board[bcurrent->from] = empty;
	
	if (bcurrent->promo!=empty) {
		piece_board[bcurrent->to]=bcurrent->promo;
	} else if (bcurrent->is_enpas_cap) {
		if (bcurrent->white_to_move) {
			piece_board[bcurrent->to-8]=empty;
		} else {
			piece_board[bcurrent->to+8]=empty;
		}
	} else if (bcurrent->is_short_castle) {
		if (bcurrent->white_to_move) {
			piece_board[H1]=empty;
			piece_board[F1]=Rook;
		} else {
			piece_board[H8]=empty;
			piece_board[F8]=rook;
		}
	} else if (bcurrent->is_long_castle) {
		if (bcurrent->white_to_move) {
			piece_board[A1]=empty;
			piece_board[D1]=Rook;
		} else {
			piece_board[A8]=empty;
			piece_board[D8]=rook;
		}
	}
	
	update_attack(false);
	
	bcurrent->white_to_move^=1;
	
}

inline void undo_bitboard_move() {
	if (ply==0) {
		send_message("FATAL ERROR: No moves in history!\n");
		exit(1);
	}
	
	ply--;
	memcpy(&bcurrent, &history[ply], sizeof bcurrent);
	
	piece_board[bcurrent->from] = bcurrent->new_piece;
	piece_board[bcurrent->to]   = bcurrent->old_piece;
	
	if (bcurrent->is_enpas_cap) {
		if (bcurrent->white_to_move) {
			piece_board[bcurrent->to-8]=pawn;
		} else {
			piece_board[bcurrent->to+8]=Pawn;
		}
	} else if (bcurrent->is_short_castle) {
		if (bcurrent->white_to_move) {
			piece_board[H1]=Rook;
			piece_board[F1]=empty;
		} else {
			piece_board[H8]=rook;
			piece_board[F8]=empty;
		}
	} else if (bcurrent->is_long_castle) {
		if (bcurrent->white_to_move) {
			piece_board[A1]=Rook;
			piece_board[D1]=empty;
		} else {
			piece_board[A8]=rook;
			piece_board[D8]=empty;
		}
	}
	
//	update_attack(true);
	
}
*/



struct chess_state *tmoves;

int compareInc(const int a, const int b) {
	return tmoves[a].move_value - tmoves[b].move_value;
}

int compareDec(const int a, const int b) {
	return tmoves[b].move_value - tmoves[a].move_value;
}

static int transpositions;




int main ()//int argc, const char * argv[])
{

	init();
	
	usleep(50000);
	
/*	if (argc>0) {
		int i;
		send_message("# Parameters given: \n");
		for (i=0;i<argc;i++) {
			sprintf(outbuf, "# %s\n", argv[i]);
			send_message(outbuf);
		}
	}
	*/
	
//	test_all_moves();
//	test_thinking();
	
//	return 0;

/*
{
	U64 attack;
	memcpy(
		current.board, 
		"RNBQKBNR"
		"PPPPPPPP"
		"........"
		"........"
		"........"
		"........"
		"ppp.pppp"
		"rnQqkqnr"
		, 64);
		
	read_board();
	attack=get_all_attack_bits(true);
	output_bitboard(attack);
	return 0;
}
*/
    
    while(1){
		if (!get_message()) return 0;
		
		if (!strcmp(inbuf, "xboard")) {
			game_setup.xboard=true;
			get_message();
			
			if (strcmp(command, "protover")) 
				return 0; /* do not work with the old protocol */
			else
				sscanf(argument, "%d", &game_setup.protover);
			
			if (game_setup.protover!=2) return 0;
			
			send_message("\n");
			send_message("feature usermove=1\n");
			get_message();
			if (strcmp(command, "accepted")) return 0; /* we rely on 'usermove' feature*/
			
			send_message("feature ping=1\n"); 
			get_message(); send_message("feature variants=\"normal\"\n");
			get_message(); send_message("feature sigint=0\n");
			get_message(); send_message("feature sigterm=1\n");
			get_message(); send_message("feature colors=0\n");
			get_message(); send_message("feature nps=0\n");
			get_message(); send_message("feature setboard=1\n");
			get_message();
			
			send_message("feature done=1\n");
			get_message();
			
			send_message("\n");
			continue;
		}
		
		if (!game_setup.xboard) return 0; /* Unsupported host protocol */
		
		if (!strcmp(command, "hard"))  game_setup.pondering=true;
		if (!strcmp(command, "easy"))  game_setup.pondering=false;
		if (!strcmp(command, "new"))   {
			setup_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq ");
			print_game(bcurrent);
			game_setup.computer[0]=false;
			game_setup.computer[1]=false;
			game_setup.forced=false;
		}
		
		if (!strcmp(command, "setboard"))   {
			setup_board(argument);
			print_game(bcurrent);
		}
		
		
		if (!strcmp(command, "force")) {
			game_setup.computer[0]=false;
			game_setup.computer[1]=false;
			game_setup.forced=true;
		}
		
		if (!strcmp(command, "quit"))  return 0;
		
		if (!strcmp(command, "ping")) {
			int ping;
			sscanf(argument, "%d", &ping);
			sprintf(outbuf, "pong %d\n", ping);
			send_message(outbuf);
		}
		
		if (!strcmp(command, "usermove")) {
			
			if (!move_if_can(argument)) {
				sprintf(outbuf, "Illegal move:%s\n", argument);
				send_message(outbuf);
			}
			game_setup.computer[!WTM]=true;
			game_setup.computer[WTM] = false;
			
		}
		
		if (!strcmp(command, "go")) {
			if (WTM) {
				send_message("# received go, playing white\n");
			} else {
				send_message("# received go, playing black\n");
			}
			game_setup.computer[!WTM]=true;
			game_setup.computer[WTM] = false;
			game_setup.forced=false;
		}
		
		while (!game_setup.forced&&game_setup.computer[!WTM]) {
			make_smart_move();
		}
    }
    
    return 0;
}


#ifndef CHESS_GAME_H_INCLUDED
#define CHESS_GAME_H_INCLUDED

#include "common.h"

#define MAX_PLY_COUNT 500


struct game_setup {
	/* protocol setup */
	bool xboard;
	int  protover;
	

	// determines, which side computer is playing
	bool computer[2];
	
	bool pondering;  /* think while opponent thinks */
	

	/* game state */
	int  mtime;
	int  otime;
	int  moves_left;  /* the 50 move rule */
	
	bool forced;     /* current move was forced */
	bool book_mode;  /* if true, load book openings */
	
	int	  half_move_count; /* number of half-moves made */
};

extern struct game_setup game_setup;


extern struct chess_state current; /* current board position */

extern int ply;
extern struct chess_state history[MAX_PLY_COUNT];
extern struct chess_state *bcurrent; /* collection of bitboards presenting current state */

//extern U64   sliding_board[64];
//extern enum piece_t piece_board[64];

extern unsigned char MP0_254[255];

/* all movement patterns */
extern U64 mKing[64];
extern U64 mKnight[64];

/* sliding attacks */
extern U64 mBishop[64];
extern U64 mRook[64];
/* sliding attacks with edges cut*/
extern U64 iBishop[64];
extern U64 iRook[64];


extern U64 mWPawnMove[64];
extern U64 mWPawnCapture[64];
extern U64 mBPawnMove[64];
extern U64 mBPawnCapture[64];

/* helpful boards */
extern U64 rightBorder;
extern U64 leftBorder;
extern U64 topBorder;
extern U64 bottomBorder;

void init();
void output_bitboard(U64 b);
enum piece_t char_to_piece(char a);
char piece_to_char(enum piece_t pp);
void send_move(enum pos_t from, enum pos_t to, enum piece_t piece);

//extern void printgame(struct chess_state *current);
//extern void print_bgame();

inline bool is_static(enum piece_t p);
inline bool is_sliding(enum piece_t p);
inline bool is_white(enum piece_t p);
inline bool is_black(enum piece_t p);
inline bool is_piece(enum piece_t p);


bool is_legal_move(enum pos_t from, enum pos_t to, enum piece_t move_piece, char promo, bool debug);
inline void accept(enum pos_t from, enum pos_t to, char promo, bool debug);

void new_game();
void read_bboard();

bool parse_move(char *move, enum pos_t *from, enum pos_t *to, char *promo);
bool move_if_can(char *move);

//extern inline void apply_bitboard_move(struct chess_move move, bool is_white);

//extern void update_attack(bool undo);
void update_all_attack_bits();
void print_game(struct chess_state *state);

//extern inline void do_bitboard_move(struct bitboard_state move);
//extern inline void undo_bitboard_move();

U64 get_rook_attack_bits(enum pos_t idx);
U64 get_bishop_attack_bits(enum pos_t idx);
U64 get_all_attack_bits(bool is_white);

enum piece_t getPieceFromBitboard(U64 *b);
U64 *getBitboardAt(U64 at);

char getCharAt(struct chess_state *bcurrent, U64 at);

#endif // CHESS_GAME_H_INCLUDED

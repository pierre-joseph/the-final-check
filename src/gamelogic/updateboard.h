#ifndef UPDATEBOARD_H
#define UPDATEBOARD_H

#include <stdint.h>
#include <stdbool.h>

typedef uint64_t Bitboard;

extern Bitboard pawn_attacks[2][64];
extern Bitboard knight_attacks[64];
extern Bitboard king_attacks[64];

typedef struct {
    char type;
    Bitboard bb;
} Piece; 

typedef struct {
    Piece pieces[12];
} Pieces;

typedef uint32_t Move;  // A move is a 32-bit packed value

// Bit layout:
// bits  0–5:   from square
// bits  6–11:  to square
// bits 12–14:  promotion piece (0 = none, 1 = R, 2 = N, 3 = B, 4 = Q)
// bits 15–17:  flags (none(0), capture(1), en passant(2), castling(3), promotion(4), double pawn push (5))

#define MOVE_FROM(m)        ((m) & 0x3F)  
#define MOVE_TO(m)          (((m) >> 6) & 0x3F)
#define MOVE_PROMO(m)       (((m) >> 12) & 0x7)
#define MOVE_FLAGS(m)       (((m) >> 15) & 0x7)

#define MAKE_MOVE(from, to, promo, flags) \
    (((from) & 0x3F) | (((to) & 0x3F) << 6) | (((promo) & 0x7) << 12) | (((flags) & 0x7) << 15))

#define MAX_MOVES 256
typedef struct {
    Move moves[MAX_MOVES];
    int count;
} MoveList;
extern MoveList cur_all_moves; 

#define SET_BIT(bb, sq) ((bb) |= (1ULL << (sq)))
#define CLEAR_BIT(bb, sq) ((bb) &= ~(1ULL << (sq)))
#define GET_BIT(bb, sq) (((bb) >> (sq)) & 1ULL)

typedef struct {
    Pieces board_pieces;
    bool white_turn;
    char* can_castle;
    int en_passant;
    MoveList all_moves;
    int halfmove_clock;         
    int fullmove_number;  
} Position;
extern Position global_position;

typedef struct {
    MoveList all_moves;
    char* can_castle;
    int en_passant;
    int halfmove_clock;
    int captured_piece;
    int fullmove_number;  
} UndoInfo;

#define MAX_DEPTH 512
extern UndoInfo pos_stack[MAX_DEPTH];
extern int stack_top;

void print_bitboard(Bitboard bb);
void start_game(char* fen, bool white_turn, char* can_castle);
MoveList* find_possible_board_moves();
char* can_castle(char* prev, Move move);
int can_enpassant(Move move);
void make_board_move(Move move);
void unmake_board_move(Move move);
bool is_square_attacked(int king_pos, bool white_attack);
bool is_move_legal(Move cur_move, bool white_turn);

#endif
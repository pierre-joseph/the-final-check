#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdint.h>

typedef uint64_t Bitboard;

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
// bits 12–14:  promotion piece (0 = none, 1 = N, 2 = B, 3 = R, 4 = Q)
// bits 15–17:  flags (none(0), capture(1), en passant(2), castling(3), promotion(4))

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

#define SET_BIT(bb, sq) ((bb) |= (1ULL << (sq)))
#define CLEAR_BIT(bb, sq) ((bb) &= ~(1ULL << (sq)))
#define GET_BIT(bb, sq) (((bb) >> (sq)) & 1ULL)

void print_bitboard(Bitboard bb);
#endif
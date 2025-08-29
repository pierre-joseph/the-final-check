#ifndef FEN_H
#define FEN_H

#include <stdint.h>

enum { PAWN = 0, ROOK = 1, KNIGHT = 2, BISHOP = 3, QUEEN = 4, KING = 5};
enum { WHITE_PAWN = 0, WHITE_ROOK = 1, WHITE_KNIGHT = 2, WHITE_BISHOP = 3, WHITE_QUEEN = 4, WHITE_KING = 5, 
    BLACK_PAWN = 6, BLACK_ROOK = 7, BLACK_KNIGHT = 8, BLACK_BISHOP = 9, BLACK_QUEEN = 10, BLACK_KING = 11};

Pieces get_bitboards(const char* fen);
const char* get_fen(Pieces board_pieces);
const char *get_move_notation(Move move);

#endif
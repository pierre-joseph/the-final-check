#ifndef FEN_H
#define FEN_H

#include <stdint.h>

Pieces get_bitboards(const char* fen);
const char* get_fen(Pieces board_pieces);
const char *get_move_notation(Move move);

#endif
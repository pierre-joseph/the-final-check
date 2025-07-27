#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <stdint.h>

void get_all_moves_from_square(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, 
int sq, char* can_castle, int can_enpassant, MoveList* movelist);

#endif
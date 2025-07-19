#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <stdint.h>

MoveList* get_pawn_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq, int can_enpassant);
MoveList* get_knight_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq); 
MoveList* get_bishop_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq);
MoveList* get_rook_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq);
MoveList* get_queen_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq);
MoveList* get_king_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq, char* can_castle);
MoveList* get_all_moves_from_square(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, 
int sq, char* can_castle, int can_enpassant);

#endif
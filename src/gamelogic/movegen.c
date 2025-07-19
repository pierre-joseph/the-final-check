#include "bitboard.h"
#include "fen.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

MoveList* get_pawn_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq, int can_enpassant){
    static MoveList movelist; 
    movelist.count = 0;
    int row = floor(sq / 8);
    int col = sq - row * 8; 
    
    if (cur_piece.type == 'P'){
        if (row == 1 && GET_BIT(my_pieces, sq + 8) == 0x0ULL && GET_BIT(opp_pieces, sq + 8) == 0x0ULL
            && GET_BIT(my_pieces, sq + 16) == 0x0ULL && GET_BIT(opp_pieces, sq + 16) == 0x0ULL) {
                movelist.moves[movelist.count] = MAKE_MOVE(sq, sq + 16, 0, 0);
                movelist.count++;
            }

        if (GET_BIT(my_pieces, sq + 8) == 0x0ULL && GET_BIT(opp_pieces, sq + 8) == 0x0ULL) {
            if (row + 1 == 8){
                for (int i = 0; i < 4; i++){
                    movelist.moves[movelist.count + i] = MAKE_MOVE(sq, sq + 8, i + 1, 0);
                }
                movelist.count += 4;
            } else {
                movelist.moves[movelist.count] = MAKE_MOVE(sq, sq + 8, 0, 0);
                movelist.count++;
            }
        }

        if (col < 7 && GET_BIT(opp_pieces, sq + 7) == 0x1ULL) {
            if (row + 1 == 8){
                for (int i = 0; i < 4; i++){
                    movelist.moves[movelist.count + i] = MAKE_MOVE(sq, sq + 7, i + 1, 1);
                }
                movelist.count += 4;
            } else {
                movelist.moves[movelist.count] = MAKE_MOVE(sq, sq + 7, 0, 1);
                movelist.count++;
            }
        }

        if (col > 0 && GET_BIT(opp_pieces, sq + 9) == 0x1ULL) {
            if (row + 1 == 8){
                for (int i = 0; i < 4; i++){
                    movelist.moves[movelist.count + i] = MAKE_MOVE(sq, sq + 9, i + 1, 1);
                }
                movelist.count += 4;
            } else {
                movelist.moves[movelist.count] = MAKE_MOVE(sq, sq + 9, 0, 1);
                movelist.count++;
            }
        }
    } else {
        if (row == 6 && GET_BIT(my_pieces, sq - 8) == 0x0ULL && GET_BIT(opp_pieces, sq - 8) == 0x0ULL
            && GET_BIT(my_pieces, sq - 16) == 0x0ULL && GET_BIT(opp_pieces, sq - 16) == 0x0ULL) {
                movelist.moves[movelist.count] = MAKE_MOVE(sq, sq - 16, 0, 0);
                movelist.count++;
            }

        if (GET_BIT(my_pieces, sq - 8) == 0x0ULL && GET_BIT(opp_pieces, sq - 8) == 0x0ULL) {
            if (row + 1 == 8){
                for (int i = 0; i < 4; i++){
                    movelist.moves[movelist.count + i] = MAKE_MOVE(sq, sq - 8, i + 1, 0);
                }
                movelist.count += 4;
            } else {
                movelist.moves[movelist.count] = MAKE_MOVE(sq, sq - 8, 0, 0);
                movelist.count++;
            }
        }

        if (col < 7 && GET_BIT(opp_pieces, sq - 9) == 0x1ULL) {
            if (row + 1 == 8){
                for (int i = 0; i < 4; i++){
                    movelist.moves[movelist.count + i] = MAKE_MOVE(sq, sq - 9, i + 1, 1);
                }
                movelist.count += 4;
            } else {
                movelist.moves[movelist.count] = MAKE_MOVE(sq, sq - 9, 0, 1);
                movelist.count++;
            }
        }

        if (col > 0 && GET_BIT(opp_pieces, sq - 7) == 0x1ULL) {
            if (row + 1 == 8){
                for (int i = 0; i < 4; i++){
                    movelist.moves[movelist.count + i] = MAKE_MOVE(sq, sq - 7, i + 1, 1);
                }
                movelist.count += 4;
            } else {
                movelist.moves[movelist.count] = MAKE_MOVE(sq, sq - 7, 0, 1);
                movelist.count++;
            }
        }
    }
    return &movelist;
}

MoveList* get_knight_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq){
    static MoveList movelist; 
    return &movelist;
}

MoveList* get_bishop_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq){
    static MoveList movelist; 
    return &movelist;
}

MoveList* get_rook_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq){
    static MoveList movelist; 
    return &movelist;
}

MoveList* get_queen_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq){
    static MoveList movelist; 
    return &movelist;
}

MoveList* get_king_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq, char* can_castle){
    static MoveList movelist; 
    return &movelist;
}

MoveList* get_all_moves_from_square(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, 
int sq, char* can_castle, int can_enpassant){
    char given_type = toupper(cur_piece.type);
    switch (given_type) {
      case 'P':
        return get_pawn_moves(cur_piece, my_pieces, opp_pieces, sq, can_enpassant);
      case 'N':
        return get_knight_moves(cur_piece, my_pieces, opp_pieces, sq);
      case 'B':
        return get_bishop_moves(cur_piece, my_pieces, opp_pieces, sq);
      case 'R':
        return get_rook_moves(cur_piece, my_pieces, opp_pieces, sq);
      case 'Q':
        return get_queen_moves(cur_piece, my_pieces, opp_pieces, sq);
      case 'K':
        return get_king_moves(cur_piece, my_pieces, opp_pieces, sq, can_castle);
    }

    return NULL;
}
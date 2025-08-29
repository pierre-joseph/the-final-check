#include "updateboard.h"
#include "fen.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

void get_pawn_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq, int can_enpassant, MoveList* movelist){
    int row = floor(sq / 8);
    int col = sq - row * 8; 
    
    if (cur_piece.type == 'P'){
        if (row == 1 && GET_BIT(my_pieces, sq + 8) == 0x0ULL && GET_BIT(opp_pieces, sq + 8) == 0x0ULL
            && GET_BIT(my_pieces, sq + 16) == 0x0ULL && GET_BIT(opp_pieces, sq + 16) == 0x0ULL) {
                movelist->moves[movelist->count] = MAKE_MOVE(sq, sq + 16, NO_PROMOTE, DOUBLE_PUSH, 0);
                movelist->count++;
            }

        if (GET_BIT(my_pieces, sq + 8) == 0x0ULL && GET_BIT(opp_pieces, sq + 8) == 0x0ULL) {
            if (row == 6){
                for (int i = 0; i < 4; i++){
                    movelist->moves[movelist->count + i] = MAKE_MOVE(sq, sq + 8, i + 1, PROMOTION_FLAG, 0);
                }
                movelist->count += 4;
            } else {
                movelist->moves[movelist->count] = MAKE_MOVE(sq, sq + 8, NO_PROMOTE, NO_FLAG, 0);
                movelist->count++;
            }
        }

        if (col < 7 && GET_BIT(opp_pieces, sq + 9) == 0x1ULL) {
            if (row == 6){
                for (int i = 0; i < 4; i++){
                    movelist->moves[movelist->count + i] = MAKE_MOVE(sq, sq + 9, i + 1, PROMOTION_FLAG, 0);
                }
                movelist->count += 4;
            } else {
                movelist->moves[movelist->count] = MAKE_MOVE(sq, sq + 9, NO_PROMOTE, CAPTURE_FLAG, 0);
                movelist->count++;
            }
        } else if (col < 7 && can_enpassant > 0 && sq + 9 == can_enpassant){
            movelist->moves[movelist->count] = MAKE_MOVE(sq, sq + 9, NO_PROMOTE, ENPASSANT_FLAG, 0);
            movelist->count++;
        }

        if (col > 0 && GET_BIT(opp_pieces, sq + 7) == 0x1ULL) {
            if (row == 6){
                for (int i = 0; i < 4; i++){
                    movelist->moves[movelist->count + i] = MAKE_MOVE(sq, sq + 7, i + 1, PROMOTION_FLAG, 0);
                }
                movelist->count += 4;
            } else {
                movelist->moves[movelist->count] = MAKE_MOVE(sq, sq + 7, NO_PROMOTE, CAPTURE_FLAG, 0);
                movelist->count++;
            }
        } else if (col > 0 && can_enpassant > 0 && sq + 7 == can_enpassant){
            movelist->moves[movelist->count] = MAKE_MOVE(sq, sq + 7, NO_PROMOTE, ENPASSANT_FLAG, 0);
            movelist->count++;
        }

    } else {
        if (row == 6 && GET_BIT(my_pieces, sq - 8) == 0x0ULL && GET_BIT(opp_pieces, sq - 8) == 0x0ULL
            && GET_BIT(my_pieces, sq - 16) == 0x0ULL && GET_BIT(opp_pieces, sq - 16) == 0x0ULL) {
                movelist->moves[movelist->count] = MAKE_MOVE(sq, sq - 16, NO_FLAG, DOUBLE_PUSH, 0);
                movelist->count++;
            }

        if (GET_BIT(my_pieces, sq - 8) == 0x0ULL && GET_BIT(opp_pieces, sq - 8) == 0x0ULL) {
            if (row == 1){
                for (int i = 0; i < 4; i++){
                    movelist->moves[movelist->count + i] = MAKE_MOVE(sq, sq - 8, i + 1, PROMOTION_FLAG, 0);
                }
                movelist->count += 4;
            } else {
                movelist->moves[movelist->count] = MAKE_MOVE(sq, sq - 8, NO_PROMOTE, NO_FLAG, 0);
                movelist->count++;
            }
        }

        if (col < 7 && GET_BIT(opp_pieces, sq - 7) == 0x1ULL) {
            if (row == 1){
                for (int i = 0; i < 4; i++){
                    movelist->moves[movelist->count + i] = MAKE_MOVE(sq, sq - 7, i + 1, PROMOTION_FLAG, 0);
                }
                movelist->count += 4;
            } else {
                movelist->moves[movelist->count] = MAKE_MOVE(sq, sq - 7, NO_PROMOTE, CAPTURE_FLAG, 0);
                movelist->count++;
            }
        } else if (col < 7 && can_enpassant > 0 && sq - 7 == can_enpassant){
            movelist->moves[movelist->count] = MAKE_MOVE(sq, sq - 7, NO_PROMOTE, ENPASSANT_FLAG, 0);
            movelist->count++;
        }

        if (col > 0 && GET_BIT(opp_pieces, sq - 9) == 0x1ULL) {
            if (row == 1){
                for (int i = 0; i < 4; i++){
                    movelist->moves[movelist->count + i] = MAKE_MOVE(sq, sq - 9, i + 1, PROMOTION_FLAG, 0);
                }
                movelist->count += 4;
            } else {
                movelist->moves[movelist->count] = MAKE_MOVE(sq, sq - 9, NO_PROMOTE, CAPTURE_FLAG, 0);
                movelist->count++;
            }
        } else if (col > 0 && can_enpassant > 0 && sq - 9 == can_enpassant){
            movelist->moves[movelist->count] = MAKE_MOVE(sq, sq - 9, NO_PROMOTE, ENPASSANT_FLAG, 0);
            movelist->count++;
        }
    }
}

void get_knight_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq, MoveList* movelist){
    int row = floor(sq / 8);
    int col = sq - row * 8; 
    for (int curRow = row - 2; curRow <= row + 2; curRow++) {
      for (int curCol = col - 2; curCol <= col + 2; curCol++) {
        int to = curRow * 8 + curCol;
        if (
          curRow >= 0 &&
          curRow < 8 &&
          curCol >= 0 &&
          curCol < 8 &&
          abs(row - curRow) != abs(col - curCol) &&
          GET_BIT(my_pieces, to) == 0x0ULL &&
          curRow != row &&
          curCol != col
        ) {
            if (GET_BIT(opp_pieces, to) == 0x1ULL) {
                movelist->moves[movelist->count] = MAKE_MOVE(sq, to, NO_PROMOTE, CAPTURE_FLAG, 0);
                movelist->count++;
            } else {
                movelist->moves[movelist->count] = MAKE_MOVE(sq, to, NO_PROMOTE, NO_FLAG, 0);
                movelist->count++;
            }
        }
      }
    }
}

void get_bishop_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq, MoveList* movelist){
    int row = floor(sq / 8);
    int col = sq - row * 8; 
    int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    for (int idx = 0; idx < 4; idx++){
        for (int i = 1; i < 8; i++) {
            int curRow = row + directions[idx][0] * i;
            int curCol = col + directions[idx][1] * i;
            int to = curRow * 8 + curCol;
            if (curRow < 0 || curCol < 0 || curRow > 7 || curCol > 7) {
                break;
            } else if (GET_BIT(my_pieces, to) == 0x0ULL && GET_BIT(opp_pieces, to) == 0x0ULL) {
                movelist->moves[movelist->count] = MAKE_MOVE(sq, to, NO_PROMOTE, NO_FLAG, 0);
                movelist->count++;
            } else if (GET_BIT(opp_pieces, to) == 0x1ULL) {
                movelist->moves[movelist->count] = MAKE_MOVE(sq, to, NO_PROMOTE, CAPTURE_FLAG, 0);
                movelist->count++;
                break;
            } else {
                break;
            }
        }
    }
}

void get_rook_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq, MoveList* movelist){
    int row = floor(sq / 8);
    int col = sq - row * 8; 
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (int idx = 0; idx < 4; idx++){
        for (int i = 1; i < 8; i++) {
            int curRow = row + directions[idx][0] * i;
            int curCol = col + directions[idx][1] * i;
            int to = curRow * 8 + curCol;
            if (curRow < 0 || curCol < 0 || curRow > 7 || curCol > 7) {
                break;
            } else if (GET_BIT(my_pieces, to) == 0x0ULL && GET_BIT(opp_pieces, to) == 0x0ULL) {
                movelist->moves[movelist->count] = MAKE_MOVE(sq, to, NO_PROMOTE, NO_FLAG, 0);
                movelist->count++;
            } else if (GET_BIT(opp_pieces, to) == 0x1ULL) {
                movelist->moves[movelist->count] = MAKE_MOVE(sq, to, NO_PROMOTE, CAPTURE_FLAG, 0);
                movelist->count++;
                break;
            } else {
                break;
            }
        }
    }
}

void get_queen_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq, MoveList* movelist){
    get_bishop_moves(cur_piece, my_pieces, opp_pieces, sq, movelist);
    get_rook_moves(cur_piece, my_pieces, opp_pieces, sq, movelist);
}

void get_king_moves(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, int sq, char* can_castle, MoveList* movelist){
    int row = floor(sq / 8);
    int col = sq - row * 8; 
    for (int curRow = row - 1; curRow <= row + 1; curRow++) {
      for (int curCol = col - 1; curCol <= col + 1; curCol++) {
        int to = curRow * 8 + curCol;
        if (
          curRow >= 0 &&
          curRow < 8 &&
          curCol >= 0 &&
          curCol < 8 &&
          (GET_BIT(my_pieces, to) == 0x0ULL) &&
          (curRow != row || curCol != col)
        ) {
          if (GET_BIT(opp_pieces, to) == 0x1ULL) {
                movelist->moves[movelist->count] = MAKE_MOVE(sq, to, NO_PROMOTE, CAPTURE_FLAG, 0);
                movelist->count++;
            } else {
                movelist->moves[movelist->count] = MAKE_MOVE(sq, to, NO_PROMOTE, NO_FLAG, 0);
                movelist->count++;
            }
        }
      }
    }

    if (cur_piece.type == 'K'){
        if (
        strchr(can_castle, 'K') &&
        (GET_BIT(my_pieces, 1) == 0x0ULL && GET_BIT(opp_pieces, 1) == 0x0ULL) &&
        (GET_BIT(my_pieces, 2) == 0x0ULL && GET_BIT(opp_pieces, 2) == 0x0ULL) &&
        !is_square_attacked(3, false) &&
        is_move_legal(MAKE_MOVE(sq, 2, NO_PROMOTE, NO_FLAG, 0), true)
        ) {
            movelist->moves[movelist->count] = MAKE_MOVE(sq, 1, NO_PROMOTE, CASTLING_FLAG, 0);
            movelist->count++;
        }

        if (
        strchr(can_castle, 'Q') &&
        (GET_BIT(my_pieces, 4) == 0x0ULL && GET_BIT(opp_pieces, 4) == 0x0ULL) &&
        (GET_BIT(my_pieces, 5) == 0x0ULL && GET_BIT(opp_pieces, 5) == 0x0ULL) && 
        (GET_BIT(my_pieces, 6) == 0x0ULL && GET_BIT(opp_pieces, 6) == 0x0ULL) && 
        !is_square_attacked(3, false) &&
        is_move_legal(MAKE_MOVE(sq, 4, NO_PROMOTE, NO_FLAG, 0), true) &&
        is_move_legal(MAKE_MOVE(sq, 5, NO_PROMOTE, NO_FLAG, 0), true)
        ) {
            movelist->moves[movelist->count] = MAKE_MOVE(sq, 5, NO_PROMOTE, CASTLING_FLAG, 0);
            movelist->count++;
        }
    } else {
        if (
        strchr(can_castle, 'k') &&
        (GET_BIT(my_pieces, 57) == 0x0ULL && GET_BIT(opp_pieces, 57) == 0x0ULL) &&
        (GET_BIT(my_pieces, 58) == 0x0ULL && GET_BIT(opp_pieces, 58) == 0x0ULL) &&
        !is_square_attacked(59, true) &&
        is_move_legal(MAKE_MOVE(sq, 58, NO_PROMOTE, NO_FLAG, 0), false)
        ) {
            movelist->moves[movelist->count] = MAKE_MOVE(sq, 57, NO_PROMOTE, CASTLING_FLAG, 0);
            movelist->count++;
        }

        if (
        strchr(can_castle, 'q') &&
        (GET_BIT(my_pieces, 60) == 0x0ULL && GET_BIT(opp_pieces, 60) == 0x0ULL) &&
        (GET_BIT(my_pieces, 61) == 0x0ULL && GET_BIT(opp_pieces, 61) == 0x0ULL) && 
        (GET_BIT(my_pieces, 62) == 0x0ULL && GET_BIT(opp_pieces, 62) == 0x0ULL) && 
        !is_square_attacked(59, true) &&
        is_move_legal(MAKE_MOVE(sq, 60, NO_PROMOTE, NO_FLAG, 0), false) &&
        is_move_legal(MAKE_MOVE(sq, 61, NO_PROMOTE, NO_FLAG, 0), false)
        ) {
            movelist->moves[movelist->count] = MAKE_MOVE(sq, 61, NO_PROMOTE, CASTLING_FLAG, 0);
            movelist->count++;
        }
    }
}

void get_all_moves_from_square(Piece cur_piece, Bitboard my_pieces, Bitboard opp_pieces, 
int sq, char* can_castle, int can_enpassant, MoveList* movelist){
    char given_type = toupper(cur_piece.type);
    switch (given_type) {
      case 'P':
        get_pawn_moves(cur_piece, my_pieces, opp_pieces, sq, can_enpassant, movelist);
        break;
      case 'N':
        get_knight_moves(cur_piece, my_pieces, opp_pieces, sq, movelist);
        break;
      case 'B':
        get_bishop_moves(cur_piece, my_pieces, opp_pieces, sq, movelist);
        break;
      case 'R':
        get_rook_moves(cur_piece, my_pieces, opp_pieces, sq, movelist);
        break;
      case 'Q':
        get_queen_moves(cur_piece, my_pieces, opp_pieces, sq, movelist);
        break;
      case 'K':
        get_king_moves(cur_piece, my_pieces, opp_pieces, sq, can_castle, movelist);
        break;
    }
}

void make_move_helper(Move move){
    int from = MOVE_FROM(move);
    int to =  MOVE_TO(move);
    int promote = MOVE_PROMO(move);
    int type = MOVE_FLAGS(move);

    if (global_position.board[from] != -1){
        int idx = global_position.board[from];
        char piece_type = global_position.board_pieces.pieces[idx].type;
        uint64_t* cur_bb = &global_position.board_pieces.pieces[idx].bb;
        pos_stack[stack_top].moving_piece = (char) idx; 
        CLEAR_BIT(*cur_bb, from);
        global_position.hash ^= zobrist_table[global_position.board[from]][from];
        if (type != PROMOTION_FLAG){
            SET_BIT(*cur_bb, to);
        }
    }

    if (global_position.board[to] != -1){
        int idx = global_position.board[to];
        char piece_type = global_position.board_pieces.pieces[idx].type;
        uint64_t* cur_bb = &global_position.board_pieces.pieces[idx].bb;
        pos_stack[stack_top].captured_piece = (char) idx; 
        global_position.hash ^= zobrist_table[global_position.board[to]][to];
        CLEAR_BIT(*cur_bb, to); 
    }

    global_position.hash ^= zobrist_table[global_position.board[from]][to];
    global_position.board[to] = global_position.board[from];
    global_position.board[from] = -1;

    if (type == ENPASSANT_FLAG){
        if (to < 32){
            CLEAR_BIT(global_position.board_pieces.pieces[WHITE_PAWN].bb, to + 8); 
            global_position.board[to + 8] = -1;
            global_position.hash ^= zobrist_table[WHITE_PAWN][to + 8];
        } else {
            CLEAR_BIT(global_position.board_pieces.pieces[BLACK_PAWN].bb, to - 8); 
            global_position.board[to - 8] = -1;
            global_position.hash ^= zobrist_table[BLACK_PAWN][to - 8];
        }
    } else if (type == CASTLING_FLAG){
        if (from == 3){
            if (from > to){
                CLEAR_BIT(global_position.board_pieces.pieces[WHITE_ROOK].bb, 0); 
                SET_BIT(global_position.board_pieces.pieces[WHITE_ROOK].bb, 2);   
                global_position.board[0] = -1;
                global_position.board[2] = 1;
                global_position.hash ^= zobrist_table[WHITE_ROOK][0];
                global_position.hash ^= zobrist_table[WHITE_ROOK][2];
            } else {
                CLEAR_BIT(global_position.board_pieces.pieces[WHITE_ROOK].bb, 7); 
                SET_BIT(global_position.board_pieces.pieces[WHITE_ROOK].bb, 4);
                global_position.board[7] = -1;
                global_position.board[4] = 1;
                global_position.hash ^= zobrist_table[WHITE_ROOK][7];
                global_position.hash ^= zobrist_table[WHITE_ROOK][4];
            }
        } else {
            if (from > to){
                CLEAR_BIT(global_position.board_pieces.pieces[BLACK_ROOK].bb, 56); 
                SET_BIT(global_position.board_pieces.pieces[BLACK_ROOK].bb, 58);   
                global_position.board[56] = -1;
                global_position.board[58] = 7;
                global_position.hash ^= zobrist_table[BLACK_ROOK][56];
                global_position.hash ^= zobrist_table[BLACK_ROOK][58];
            } else {
                CLEAR_BIT(global_position.board_pieces.pieces[BLACK_ROOK].bb, 63); 
                SET_BIT(global_position.board_pieces.pieces[BLACK_ROOK].bb, 60);
                global_position.board[63] = -1;
                global_position.board[60] = 7;
                global_position.hash ^= zobrist_table[BLACK_ROOK][63];
                global_position.hash ^= zobrist_table[BLACK_ROOK][60];
            }
        }
    } else if (type == PROMOTION_FLAG){
        if (to > 31){
            SET_BIT(global_position.board_pieces.pieces[promote].bb, to);
            global_position.board[to] = promote;
            global_position.hash ^= zobrist_table[WHITE_PAWN][to];
            global_position.hash ^= zobrist_table[promote][to];
        } else {
            SET_BIT(global_position.board_pieces.pieces[promote + 6].bb, to);
            global_position.board[to] = promote + 6;
            global_position.hash ^= zobrist_table[BLACK_PAWN][to];
            global_position.hash ^= zobrist_table[promote + 6][to];
        }
    }
}

void unmake_move_helper(Move move){
    int from = MOVE_FROM(move);
    int to =  MOVE_TO(move);
    int promote = MOVE_PROMO(move);
    int type = MOVE_FLAGS(move);

    if (global_position.board[to] != -1){
        int idx = global_position.board[to];
        char piece_type = global_position.board_pieces.pieces[idx].type;
        uint64_t* cur_bb = &global_position.board_pieces.pieces[idx].bb;
        pos_stack[stack_top].moving_piece = (char) idx; 
        CLEAR_BIT(*cur_bb, to); 
        if (type != PROMOTION_FLAG){
            SET_BIT(*cur_bb, from);
        }
    }

    global_position.board[from] = global_position.board[to];
    global_position.board[to] = -1;

    if ((unsigned int) pos_stack[stack_top].captured_piece < 12){
        int idx = pos_stack[stack_top].captured_piece;
        char piece_type = global_position.board_pieces.pieces[idx].type;
        uint64_t* cur_bb = &global_position.board_pieces.pieces[idx].bb;
        SET_BIT(*cur_bb, to);
        global_position.board[to] = idx;
    }

    if (type == ENPASSANT_FLAG){
        if (to < 32){
            SET_BIT(global_position.board_pieces.pieces[WHITE_PAWN].bb, to + 8); 
            global_position.board[to + 8] = WHITE_PAWN;
        } else {
            SET_BIT(global_position.board_pieces.pieces[BLACK_PAWN].bb, to - 8); 
            global_position.board[to - 8] = BLACK_PAWN;
        }
    } else if (type == CASTLING_FLAG) {
        if (from == 3){
            if (from > to){
                CLEAR_BIT(global_position.board_pieces.pieces[WHITE_ROOK].bb, 2); 
                SET_BIT(global_position.board_pieces.pieces[WHITE_ROOK].bb, 0);   
                global_position.board[2] = -1;
                global_position.board[0] = WHITE_ROOK;
            } else {
                CLEAR_BIT(global_position.board_pieces.pieces[WHITE_ROOK].bb, 4); 
                SET_BIT(global_position.board_pieces.pieces[WHITE_ROOK].bb, 7);
                global_position.board[4] = -1;
                global_position.board[7] = WHITE_ROOK;
            }
        } else {
            if (from > to){
                CLEAR_BIT(global_position.board_pieces.pieces[BLACK_ROOK].bb, 58); 
                SET_BIT(global_position.board_pieces.pieces[BLACK_ROOK].bb, 56); 
                global_position.board[58] = -1;
                global_position.board[56] = BLACK_ROOK;  
            } else {
                CLEAR_BIT(global_position.board_pieces.pieces[BLACK_ROOK].bb, 60); 
                SET_BIT(global_position.board_pieces.pieces[BLACK_ROOK].bb, 63);
                global_position.board[60] = -1;
                global_position.board[63] = BLACK_ROOK;
            }
        }
    } else if (type == PROMOTION_FLAG) {
        if (from < 32){
            SET_BIT(global_position.board_pieces.pieces[BLACK_PAWN].bb, from);
            global_position.board[from] = BLACK_PAWN;
        } else {
            SET_BIT(global_position.board_pieces.pieces[WHITE_PAWN].bb, from);
            global_position.board[from] = WHITE_PAWN;
        }
    }
}

void set_board(){
    for (int sq = 0; sq < 64; sq++){
        global_position.board[sq] = -1;
        for (int i = 0; i < 12; i++){
            uint64_t* cur_bb = &global_position.board_pieces.pieces[i].bb;
            if (GET_BIT(*cur_bb, sq) == 1ULL){
                global_position.board[sq] = i;
            }
        }
    }
}
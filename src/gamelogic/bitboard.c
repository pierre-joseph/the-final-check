#include "bitboard.h"
#include "movegen.h"
#include "fen.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <time.h>

Bitboard pawn_attacks[2][64];
Bitboard knight_attacks[64];
Bitboard king_attacks[64];
Pieces board_pieces; 
char captured_piece = ' ';
MoveList all_moves; 

void print_bitboard(Bitboard bb) {
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            printf("%d ", (int)((bb >> square) & 1ULL));
        }
        printf("\n");
    }
    printf("\n");
}

void make_board_move(Move move){
    captured_piece = ' ';
    int from = MOVE_FROM(move);
    int to =  MOVE_TO(move);
    int promote = MOVE_PROMO(move);
    int type = MOVE_FLAGS(move);

    for (int i = 0; i < 12; i++){
        char piece_type = board_pieces.pieces[i].type;
        uint64_t* cur_bb = &board_pieces.pieces[i].bb;
        if (GET_BIT(*cur_bb, from) == 1ULL){
            CLEAR_BIT(*cur_bb, from); 
            SET_BIT(*cur_bb, to);
        } else if (GET_BIT(*cur_bb, to) == 1ULL){
            captured_piece = piece_type;
            CLEAR_BIT(*cur_bb, to); 
        } 
    }

    if (type == 2){
        if (to < 32){
            CLEAR_BIT(board_pieces.pieces[0].bb, to + 8); 
        } else {
            CLEAR_BIT(board_pieces.pieces[6].bb, to - 8); 
        }
    } else if (type == 3){
        if (from == 3){
            if (from > to){
                CLEAR_BIT(board_pieces.pieces[1].bb, 0); 
                SET_BIT(board_pieces.pieces[1].bb, 2);   
            } else {
                CLEAR_BIT(board_pieces.pieces[1].bb, 7); 
                SET_BIT(board_pieces.pieces[1].bb, 4);
            }
        } else {
            if (from > to){
                CLEAR_BIT(board_pieces.pieces[7].bb, 56); 
                SET_BIT(board_pieces.pieces[7].bb, 58);   
            } else {
                CLEAR_BIT(board_pieces.pieces[7].bb, 63); 
                SET_BIT(board_pieces.pieces[7].bb, 60);
            }
        }
    } else if (type == 4){
        if (to > 31){
            SET_BIT(board_pieces.pieces[promote].bb, to);
        } else {
            SET_BIT(board_pieces.pieces[promote + 6].bb, to);
        }
    }
}

void unmake_board_move(Move move){
    int from = MOVE_FROM(move);
    int to =  MOVE_TO(move);
    int promote = MOVE_PROMO(move);
    int type = MOVE_FLAGS(move);

    for (int i = 0; i < 12; i++){
        char piece_type = board_pieces.pieces[i].type;
        uint64_t* cur_bb = &board_pieces.pieces[i].bb;
        if (GET_BIT(*cur_bb, to) == 1ULL){
            CLEAR_BIT(*cur_bb, to); 
            SET_BIT(*cur_bb, from);
        } else if (piece_type == captured_piece){
            SET_BIT(*cur_bb, to);
        } 
    }

    if (type == 2){
        if (to < 32){
            SET_BIT(board_pieces.pieces[0].bb, to + 8); 
        } else {
            SET_BIT(board_pieces.pieces[6].bb, to - 8); 
        }
    } else if (type == 3) {
        if (from == 3){
            if (from > to){
                CLEAR_BIT(board_pieces.pieces[1].bb, 2); 
                SET_BIT(board_pieces.pieces[1].bb, 0);   
            } else {
                CLEAR_BIT(board_pieces.pieces[1].bb, 4); 
                SET_BIT(board_pieces.pieces[1].bb, 7);
            }
        } else {
            if (from > to){
                CLEAR_BIT(board_pieces.pieces[7].bb, 58); 
                SET_BIT(board_pieces.pieces[7].bb, 56);   
            } else {
                CLEAR_BIT(board_pieces.pieces[7].bb, 60); 
                SET_BIT(board_pieces.pieces[7].bb, 63);
            }
        }
    }

    captured_piece = ' ';
}

const char* make_react_move(Move move){
    make_board_move(move);
    return get_fen(board_pieces);
}

int can_enpassant(Move move){
    int to =  MOVE_TO(move);
    int type = MOVE_FLAGS(move);
    if (type == 5 && to < 32){
        return to - 8;
    } else if (type == 5){
        return to + 8;
    } else {
        return -1;
    }
}

char* can_castle(char* prev, Move move){
    int from = MOVE_FROM(move);
    int to =  MOVE_TO(move);
    bool white_kingside = strchr(prev, 'K') ? true : false;
    bool white_queenside = strchr(prev, 'Q') ? true : false;
    bool black_kingside = strchr(prev, 'k') ? true : false;
    bool black_queenside = strchr(prev, 'q') ? true : false;
    bool can_castle_vals[4] = {white_kingside, white_queenside, black_kingside, black_queenside};
    char can_castle_strs[4] = {'K', 'Q', 'k', 'q'};
    if (from == 3){
        can_castle_vals[0] = false; 
        can_castle_vals[1] = false;
    }

    if (from == 0 || to == 0){
        can_castle_vals[0] = false; 
    }

    if (from == 7 || to == 7){
        can_castle_vals[1] = false; 
    }

    if (from == 59){
        can_castle_vals[2] = false;
        can_castle_vals[3] = false;
    }

     if (from == 56 || to == 56){
        can_castle_vals[2] = false; 
    }

    if (from == 63 || to == 63){
        can_castle_vals[3] = false; 
    }

    static char new_can_castle[5];
    for (int i = 0; i < 4; i++){
        if (can_castle_vals[i]){
            new_can_castle[i] = can_castle_strs[i];
        } else {
            new_can_castle[i] = ' ';
        }
    }
    new_can_castle[4] = '\0';

    return new_can_castle;
}

int countSetBits(unsigned int n) {
    int count = 0;
    while (n > 0) {
        count += (n & 1); 
        n >>= 1;         
    }
    return count;
}

bool inefficient_material(){
    int white_pieces = 0;
    int black_pieces = 0;
    for (int i = 0; i < 5; i++){
        Piece white = board_pieces.pieces[i];
        Piece black = board_pieces.pieces[i + 6];
        int white_bits = countSetBits(white.bb);
        int black_bits = countSetBits(black.bb);
        if (i == 2 || i == 3){
            white_pieces += white_bits * 0.5;
            black_pieces += black_bits * 0.5;
        } else {
            white_pieces += white_bits;
            black_pieces += black_bits;
        }

        if (white_pieces >= 1 || black_pieces >= 1){
            return false;
        }
    }
    return true;
}

bool can_piece_attack_square(Piece cur_piece, int king_pos, char* color, int sq){
    Bitboard white_pieces = board_pieces.pieces[0].bb | board_pieces.pieces[1].bb | board_pieces.pieces[2].bb |
                            board_pieces.pieces[3].bb | board_pieces.pieces[4].bb | board_pieces.pieces[5].bb;
    Bitboard black_pieces = board_pieces.pieces[6].bb | board_pieces.pieces[7].bb | board_pieces.pieces[8].bb |
                            board_pieces.pieces[9].bb | board_pieces.pieces[10].bb | board_pieces.pieces[11].bb;
    Bitboard my_pieces = strcmp(color, "white") == 0 ? white_pieces : black_pieces;
    Bitboard opp_pieces = strcmp(color, "white") == 0 ? black_pieces : white_pieces;
    MoveList moves_from_sq;
    moves_from_sq.count = 0;
    get_all_moves_from_square(cur_piece, my_pieces, opp_pieces, sq, "    ", -1, &moves_from_sq);
    for (int move = 0; move < moves_from_sq.count; move++){
        if (MOVE_TO(moves_from_sq.moves[move]) == king_pos){
            return true;
        }
    }
    return false;
}

bool is_square_attacked(int king_pos, char* color){
    for (int i = 0; i < 6; i++) {
        int index = strcmp(color, "white") == 0 ? i : i + 6;
        char given_type = board_pieces.pieces[index].type;
        uint64_t bitboard = board_pieces.pieces[index].bb;
        for (int sq = 0; sq < 64; sq++){
            if (GET_BIT(bitboard, sq) == 1ULL &&
            can_piece_attack_square(board_pieces.pieces[index], king_pos, color, sq)){ 
                return true;
            }
        }  
    }
    return false;
}

bool is_king_attacked(bool white_turn){
    int king_pos = white_turn ? __builtin_ctzll(board_pieces.pieces[5].bb) : __builtin_ctzll(board_pieces.pieces[11].bb);
    char* opp_color = white_turn ? "black" : "white";
    return is_square_attacked(king_pos, opp_color);
}

bool is_move_legal(Move cur_move, bool white_turn){
    make_board_move(cur_move);
    bool not_attacked = !is_king_attacked(white_turn);
    unmake_board_move(cur_move);
    return not_attacked;
}

// 0=not over, 1=draw, 2=white, 3=black
int is_game_over(bool white_turn){
    if (all_moves.count == 0) {
        if (is_king_attacked(!white_turn)) {
            int winner = white_turn ? 2 : 3;
            return winner;
        } else {
            return 1;
        }
    }
    if (inefficient_material()){
        return 1;
    }
    return 0; 
}

MoveList* find_possible_board_moves(bool white_turn, char* can_castle, int can_enpassant){
    Bitboard white_pieces = board_pieces.pieces[0].bb | board_pieces.pieces[1].bb | board_pieces.pieces[2].bb |
                            board_pieces.pieces[3].bb | board_pieces.pieces[4].bb | board_pieces.pieces[5].bb;
    Bitboard black_pieces = board_pieces.pieces[6].bb | board_pieces.pieces[7].bb | board_pieces.pieces[8].bb |
                            board_pieces.pieces[9].bb | board_pieces.pieces[10].bb | board_pieces.pieces[11].bb;
    all_moves.count = 0;

    for (int i = 0; i < 12; i++) {
        char given_type = board_pieces.pieces[i].type;
        uint64_t bitboard = board_pieces.pieces[i].bb;
        bool is_turn = (isupper(given_type) == white_turn);
        if (is_turn) {
            Bitboard my_pieces = white_turn ? white_pieces : black_pieces;
            Bitboard opp_pieces = white_turn ? black_pieces : white_pieces;
            for (int sq = 0; sq < 64; sq++){
                if (GET_BIT(bitboard, sq) == 1ULL){
                    MoveList moves_from_sq;
                    moves_from_sq.count = 0;
                    get_all_moves_from_square(board_pieces.pieces[i], my_pieces, opp_pieces, 
                    sq, can_castle, can_enpassant, &moves_from_sq);
                    for (int move = 0; move < moves_from_sq.count; move++){
                        if (is_move_legal(moves_from_sq.moves[move], white_turn)){
                            all_moves.moves[all_moves.count] = moves_from_sq.moves[move];
                            all_moves.count++;
                        }
                    }
                }
            }
        }
    }

    return &all_moves;
}

void init_pawn_attacks(){
    for (int sq = 0; sq < 64; sq++){
        uint64_t bit = 1ULL << sq;
        int row = floor(sq / 8);
        int col = 7 - (sq % 8); 
        int start = row * 8 + col;
        //white
        pawn_attacks[0][sq] = 0;
        if (sq < 57 & col > 0){
            SET_BIT(pawn_attacks[0][sq], start + 7); 
        } 
        if (sq < 55 & col < 7){ 
            SET_BIT(pawn_attacks[0][sq], start + 9); 
        }

        // Black
        pawn_attacks[1][sq] = 0;
        if (sq > 6 & col < 7){
            SET_BIT(pawn_attacks[1][sq], start - 7); 
        } 
        if (sq > 8 & col > 0){ 
            SET_BIT(pawn_attacks[1][sq], start - 9); 
        }
    }
}

void init_knight_attacks(){
    for (int sq = 0; sq < 64; sq++){
        uint64_t bit = 1ULL << sq;
        int row = floor(sq / 8);
        int col = 7 - (sq % 8); 
        knight_attacks[sq] = 0;
        for (int curRow = row - 2; curRow <= row + 2; curRow++) {
            for (int curCol = col - 2; curCol <= col + 2; curCol++) {
                int to = curRow * 8 + curCol;
                if (
                curRow >= 0 &&
                curRow < 8 &&
                curCol >= 0 &&
                curCol < 8 &&
                abs(row - curRow) != abs(col - curCol) &&
                curRow != row &&
                curCol != col
                ) {
                    SET_BIT(knight_attacks[sq], to);
                }
            }
        }
    }
}

void init_king_attacks(){
    for (int sq = 0; sq < 64; sq++){
        uint64_t bit = 1ULL << sq;
        int row = floor(sq / 8);
        int col = 7 - (sq % 8); 
        king_attacks[sq] = 0;
        for (int curRow = row - 1; curRow <= row + 1; curRow++) {
            for (int curCol = col - 1; curCol <= col + 1; curCol++) {
                int to = curRow * 8 + curCol;
                if (
                curRow >= 0 &&
                curRow < 8 &&
                curCol >= 0 &&
                curCol < 8 &&
                (curRow != row || curCol != col)
                ) {
                    SET_BIT(king_attacks[sq], to);
                }
            }
        }
    }
}

void start_game(char* fen){
    srand(time(0));
    board_pieces = get_bitboards(fen);
    init_pawn_attacks();
    init_knight_attacks();
    init_king_attacks();
}

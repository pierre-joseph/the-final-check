#include "bitboard.h"
#include "movegen.h"
#include "fen.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

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

const char* make_move(char* fen, int* move){
    Pieces board_pieces = get_bitboards(fen);
    int from = move[0];
    int to = move[1];
    int promote = move[2];
    int type = move[3];

    for (int i = 0; i < 12; i++){
        uint64_t* cur_bb = &board_pieces.pieces[i].bb;
        if (GET_BIT(*cur_bb, from) == 1ULL){
            CLEAR_BIT(*cur_bb, from); 
            if (type != 4){
                SET_BIT(*cur_bb, to);
            }
        } else if (GET_BIT(*cur_bb, to) == 1ULL){
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

    return get_fen(board_pieces);
}

int can_enpassant(int* move){
    int to = move[1];
    int type = move[3];
    if (type == 5 && to < 32){
        return to - 8;
    } else if (type == 5){
        return to + 8;
    } else {
        return -1;
    }
}

char* can_castle(char* prev, int* move){
    int from = move[0];
    int to = move[1];
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

MoveList* find_possible_board_moves(char* fen, bool white_turn, char* can_castle, int can_enpassant){
    Pieces board_pieces = get_bitboards(fen);
    Bitboard white_pieces = board_pieces.pieces[0].bb | board_pieces.pieces[1].bb | board_pieces.pieces[2].bb |
                            board_pieces.pieces[3].bb | board_pieces.pieces[4].bb | board_pieces.pieces[5].bb;
    Bitboard black_pieces = board_pieces.pieces[6].bb | board_pieces.pieces[7].bb | board_pieces.pieces[8].bb |
                            board_pieces.pieces[9].bb | board_pieces.pieces[10].bb | board_pieces.pieces[11].bb;

    static MoveList all_moves; 
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
                    MoveList* moves_from_sq = get_all_moves_from_square(board_pieces.pieces[i], my_pieces, opp_pieces, 
                    sq, can_castle, can_enpassant);

                    for (int move = 0; move < moves_from_sq->count; move++){
                        all_moves.moves[all_moves.count] = moves_from_sq->moves[move];
                        all_moves.count++;
                    }
                }
            }
        }
    }

    return &all_moves;
}
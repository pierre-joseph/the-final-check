#include "bitboard.h"
#include "movegen.h"
#include "fen.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

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

const char* make_move(char* fen, char* move){
    Pieces board_pieces = get_bitboards(fen);
    int split = 0;
    while (move[split] != '-'){
        split += 1;
    }
    move[split] = '\0';
    int from = atoi(move);
    int to = atoi(move+split+1);
    int count;
    for (int i = 0; i < 12; i++){
        uint64_t* cur_bb = &board_pieces.pieces[i].bb;
        if (GET_BIT(*cur_bb, from) == 1ULL){
            CLEAR_BIT(*cur_bb, from); 
            SET_BIT(*cur_bb, to);
        } else if (GET_BIT(*cur_bb, to) == 1ULL){
            CLEAR_BIT(*cur_bb, to); 
        } 
    }

    return get_fen(board_pieces);
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
            Bitboard opp_pieces = white_turn ? black_pieces : black_pieces;
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
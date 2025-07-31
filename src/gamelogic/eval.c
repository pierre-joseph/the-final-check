#include "bitboard.h"
#include "fen.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

int PAWN_WEIGHT = 100;
int KNIGHT_WEIGHT = 300;
int BISHOP_WEIGHT = 325;
int ROOK_WEIGHT = 500;
int QUEEN_WEIGHT = 900;

Move get_random_move(){
    int move_count = cur_all_moves.count;
    if (move_count > 0){
        int random_idx = rand() % (move_count);
        return cur_all_moves.moves[random_idx];
    } else {
        return 0;
    }
}

char* move_to_str(Move move){
    char* move_str = malloc(4);
    char* alphabet = "hgfedcba";
    int from = MOVE_FROM(move);
    int to =  MOVE_TO(move);
    int from_row = floor(from / 8);
    int from_col = from - from_row * 8; 
    int to_row = floor(to / 8);
    int to_col = to - to_row * 8; 

    move_str[0] = alphabet[from_col]; 
    sprintf(move_str + 1, "%d", from_row + 1); 
    move_str[2] = alphabet[to_col];
    sprintf(move_str + 3, "%d", to_row + 1); 
    return move_str;
}

int eval_position(bool is_white){
    int piece_weights[5] = {PAWN_WEIGHT, ROOK_WEIGHT, KNIGHT_WEIGHT, BISHOP_WEIGHT, QUEEN_WEIGHT};
    int white_pieces = 0;
    int black_pieces = 0;
    for (int i = 0; i < 5; i++){
        for (int sq = 0; sq < 64; sq++){
            if (GET_BIT(board_pieces.pieces[i].bb, sq) == 1ULL){
                white_pieces += piece_weights[i]; 
            } else if (GET_BIT(board_pieces.pieces[i + 6].bb, sq) == 1ULL){
                black_pieces += piece_weights[i]; 
            }
        }
    }
    int perspective = is_white ? 1 : -1;

    return (white_pieces - black_pieces) * perspective;
}

int num_of_positions(int depth, bool white_turn, char* can_castle_str, int can_en_passant){
    if (depth == 0){
        return 1;
    }

    int positions = 0;
    MoveList* all_moves = find_possible_board_moves(white_turn, can_castle_str, can_en_passant);
    int move_count = all_moves->count;
    for (int i = 0; i < move_count; i++){
        char capture = make_board_move(all_moves->moves[i]);
        char* new_can_castle = can_castle(can_castle_str, all_moves->moves[i]);
        int new_can_en_passant = can_enpassant(all_moves->moves[i]);
        int additional_positions = num_of_positions(depth - 1, !white_turn, new_can_castle, new_can_en_passant);
        positions += additional_positions;
        unmake_board_move(all_moves->moves[i], capture);
        free(new_can_castle);
    }
    free(all_moves);
    return positions;
}


int move_generation_test(int depth, char* fen, bool white_turn, char* can_castle_str, int can_en_passant){
    start_game(fen);
    return num_of_positions(depth, white_turn, can_castle_str, can_en_passant);
}
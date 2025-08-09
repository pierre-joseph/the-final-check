#include "updateboard.h"
#include "fen.h"
#include "eval.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <limits.h>

int PAWN_WEIGHT = 100;
int KNIGHT_WEIGHT = 300;
int BISHOP_WEIGHT = 300;
int ROOK_WEIGHT = 500;
int QUEEN_WEIGHT = 900;
int KING_WEIGHT = 0;

int PAWN_PST[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10,-20,-20, 10, 10,  5,
    5, -5,-10,  0,  0,-10, -5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5,  5, 10, 25, 25, 10,  5,  5,
    10, 10, 20, 30, 30, 20, 10, 10,
    50, 50, 50, 50, 50, 50, 50, 50,
    0,  0,  0,  0,  0,  0,  0,  0
};

int KNIGHT_PST[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

int BISHOP_PST[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};
int ROOK_PST[64] = {
     0,  0,  5,  10, 10, 5,  0,  0,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     5,  10, 10, 10, 10, 10, 10, 5,
     0,  0,  0,  0,  0,  0,  0,  0,
};
int QUEEN_PST[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20
    -10,  0,  5,  0,  0,  0,  0,-10,
    -10,  5,  5,  5,  5,  5,  0,-10,
     0,  0,  5,  5,  5,  5,  0, -5,
    -5,  0,  5,  5,  5,  5,  0, -5,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20,
};

int KING_PST[64] = {
    20,  30,  10,  0,   0,   10,  30,  20,
    20,  20,  0,   0,   0,   0,   20,  20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30
};

Move get_random_move(){
    int move_count = global_position.all_moves.count;
    if (move_count > 0){
        int random_idx = rand() % (move_count);
        return global_position.all_moves.moves[random_idx];
    } else {
        return 0;
    }
}

int eval_position(){
    int piece_weights[6] = {PAWN_WEIGHT, ROOK_WEIGHT, KNIGHT_WEIGHT, BISHOP_WEIGHT, QUEEN_WEIGHT, KING_WEIGHT};
    int* piece_sq_tables[6][64] = {PAWN_PST, ROOK_PST, KNIGHT_PST, BISHOP_PST, QUEEN_PST, KING_PST};
    int white_pieces = 0;
    int black_pieces = 0;
    for (int i = 0; i < 6; i++){
        for (int sq = 0; sq < 64; sq++){
            if (GET_BIT(global_position.board_pieces.pieces[i].bb, sq) == 1ULL){
                white_pieces += piece_weights[i]; 
                white_pieces += (*piece_sq_tables)[i][sq];
            } else if (GET_BIT(global_position.board_pieces.pieces[i + 6].bb, sq) == 1ULL){
                black_pieces += piece_weights[i]; 
                black_pieces += (*piece_sq_tables)[i][63 - sq];
            }
        }
    }

    return white_pieces - black_pieces;
}

void order_moves(MoveList* all_moves){
    int move_count = all_moves->count;
    int good_moves = 0;

    for (int i = 0; i < move_count; i++){
       Move cur_move = all_moves->moves[i];
       int type = MOVE_FLAGS(cur_move);
       if (type == 1 || type == 4){
        all_moves->moves[i] = all_moves->moves[good_moves];
        all_moves->moves[good_moves] = cur_move;
        good_moves++;
       }
    }
}

int get_move_eval(int depth, int alpha, int beta){
    if (depth == 0){
        return eval_position(); 
    }

    bool maximizing_player = global_position.white_turn;
    int best_eval = maximizing_player ? INT_MIN : INT_MAX;

    MoveList* all_moves = find_possible_board_moves();
    order_moves(all_moves);
    int move_count = all_moves->count;

    if (move_count == 0){
        if (is_king_attacked(maximizing_player)){
            return best_eval;
        } else {
            return 0;
        }
    }

    for (int i = 0; i < move_count; i++){
        make_board_move(all_moves->moves[i]);
        int new_depth = depth - 1;
        int eval = get_move_eval(new_depth, alpha, beta);
        unmake_board_move(all_moves->moves[i]);

        if (maximizing_player){
            if (eval > best_eval){
                best_eval = eval;
            }

            if (eval > alpha){
                alpha = eval;
            }
        } else {
            if (eval < best_eval){
                best_eval = eval;
            }

            if (eval < beta){
                beta = eval;
            }
        }

        if (beta < alpha){
            break;
        }
    }

    free(all_moves);
    return best_eval; 
}

Move get_best_move(int depth){
    MoveList* all_moves = find_possible_board_moves();
    order_moves(all_moves);
    Move best_move = all_moves->moves[0];
    int move_count = all_moves->count;

    bool maximizing_player = global_position.white_turn;
    int best_eval = maximizing_player ? INT_MIN : INT_MAX;

    int alpha = INT_MIN;
    int beta = INT_MAX;

    for (int i = 0; i < move_count; i++){
        make_board_move(all_moves->moves[i]);
        int eval = get_move_eval(depth - 1, alpha, beta);
        unmake_board_move(all_moves->moves[i]);

        if (maximizing_player){
            if (eval > best_eval){
                best_eval = eval;
                best_move = all_moves->moves[i];
            }

            if (eval > alpha){
                alpha = eval;
            }
        } else {
            if (eval < best_eval){
                best_eval = eval;
                best_move = all_moves->moves[i];
            }

            if (eval < beta){
                beta = eval;
            }
        }
        if (beta < alpha){
            break;
        }
    }

    free(all_moves);
    return best_move;
}

int num_of_positions(int depth){
    if (depth == 0){
        return 1;
    }

    int positions = 0;
    MoveList* all_moves = find_possible_board_moves();
    int move_count = all_moves->count;
    for (int i = 0; i < move_count; i++){
        make_board_move(all_moves->moves[i]);
        int additional_positions = num_of_positions(depth - 1);
        positions += additional_positions;
        unmake_board_move(all_moves->moves[i]);
    }

    free(all_moves);
    return positions;
}


int move_generation_test(int depth, char* fen, bool white_turn, char* can_castle_str, int can_en_passant){
    start_game(fen, white_turn, can_castle_str);
    return num_of_positions(depth);
}
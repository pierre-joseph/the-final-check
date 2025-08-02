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
    int piece_weights[5] = {PAWN_WEIGHT, ROOK_WEIGHT, KNIGHT_WEIGHT, BISHOP_WEIGHT, QUEEN_WEIGHT, KING_WEIGHT};
    int white_pieces = 0;
    int black_pieces = 0;
    for (int i = 0; i < 6; i++){
        for (int sq = 0; sq < 64; sq++){
            if (GET_BIT(global_position.board_pieces.pieces[i].bb, sq) == 1ULL){
                white_pieces += piece_weights[i]; 
            } else if (GET_BIT(global_position.board_pieces.pieces[i + 6].bb, sq) == 1ULL){
                black_pieces += piece_weights[i]; 
            }
        }
    }

    return white_pieces - black_pieces;
}

int get_move_eval(int depth, int alpha, int beta){
    if (depth == 0){
        return eval_position(); 
    }

    bool maximizing_player = global_position.white_turn;
    int best_eval = maximizing_player ? INT_MIN : INT_MAX;

    MoveList* all_moves = find_possible_board_moves();
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
        int eval = get_move_eval(depth - 1, alpha, beta);
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
    printf("Eval: %d\n", best_eval);
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
#include "updateboard.h"
#include "fen.h"
#include "eval.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <limits.h>

const int PAWN_WEIGHT = 200;
const int KNIGHT_WEIGHT = 600;
const int BISHOP_WEIGHT = 600;
const int ROOK_WEIGHT = 1000;
const int QUEEN_WEIGHT = 1800;
const int KING_WEIGHT = 0;

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

int piece_weights[6] = {PAWN_WEIGHT, ROOK_WEIGHT, KNIGHT_WEIGHT, BISHOP_WEIGHT, QUEEN_WEIGHT, KING_WEIGHT};
int* piece_sq_tables[6][64] = {PAWN_PST, ROOK_PST, KNIGHT_PST, BISHOP_PST, QUEEN_PST, KING_PST};

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

int compare_moves(const void *a, const void *b) {
    int score_a = (int) MOVE_SCORE(*(uint32_t*)a);
    int score_b = (int) MOVE_SCORE(*(uint32_t*)b);
    return score_b - score_a;  
}

void order_moves(MoveList* all_moves){
    int move_count = all_moves->count;

    for (int i = 0; i < move_count; i++){
        Move* cur_move = &all_moves->moves[i];
        int from = MOVE_FROM(*cur_move);
        int to = MOVE_TO(*cur_move);
        int type = MOVE_FLAGS(*cur_move);
        int promote_to = MOVE_PROMO(*cur_move);

        int16_t starting_guess = 0;
        if (global_position.white_turn){
            starting_guess = (*piece_sq_tables)[global_position.board[from] % 6][to];
        } else {
            starting_guess = (*piece_sq_tables)[global_position.board[from] % 6][63 - to];
        }
        int16_t move_score_guess = starting_guess;
        
        if (type == 1){
            move_score_guess = (10 * piece_weights[global_position.board[to] % 6] / 100) \
                                - piece_weights[global_position.board[from] % 6] / 100; 
        } else if (type == 2) {
            move_score_guess = 9 * (PAWN_WEIGHT / 100);
        } else if (type == 4){
            int promote_val;
            switch (promote_to) {
                case 1:
                    promote_val = ROOK_WEIGHT / 100;
                    break;
                case 2:
                    promote_val = KNIGHT_WEIGHT / 100;
                    break;
                case 3:
                    promote_val = BISHOP_WEIGHT / 100;
                    break;
                case 4:
                    promote_val = QUEEN_WEIGHT / 100;
                    break;
            }

            move_score_guess += promote_val;
        } 

        SET_MOVE_SCORE(*cur_move, move_score_guess);
    }

    qsort(all_moves->moves, move_count, sizeof(uint32_t), compare_moves);
}

int search_all_captures(int alpha, int beta, int cur_depth){
    bool maximizing_player = global_position.white_turn;
    int cur_eval = eval_position();

    if (cur_depth > 10) return maximizing_player ? alpha : beta;

    if (maximizing_player){
        if (cur_eval > alpha) alpha = cur_eval;
        if (cur_eval >= beta) return beta;
    } else {
        if (cur_eval < beta) beta = cur_eval;
        if (cur_eval <= alpha) return alpha;
    }

    MoveList* all_moves = find_possible_board_moves();
    order_moves(all_moves);
    int move_count = all_moves->count;

    if (move_count == 0){
        int score;
        if (is_king_attacked(maximizing_player)){
            score = maximizing_player ? INT_MIN + cur_depth : INT_MAX - cur_depth;
        } else {
            score = 0;
        }
        free(all_moves);
        return score;
    }

    for (int i = 0; i < move_count; i++){
        int cur_type = MOVE_FLAGS(all_moves->moves[i]);
        if (cur_type == 1 || cur_type == 2 || cur_type == 4){
            make_board_move(all_moves->moves[i]);
            int eval = search_all_captures(alpha, beta, cur_depth + 1);
            unmake_board_move(all_moves->moves[i]);

            if (maximizing_player){
                if (eval > alpha) alpha = eval;
                if (alpha >= beta) break;
            } else if (!maximizing_player){
                if (eval < beta) beta = eval;
                if (beta <= alpha) break;
            }
        }
    }

    free(all_moves);
    return maximizing_player ? alpha : beta;
}

int get_move_eval(int depth, int alpha, int beta){
    if (depth == 0){
        return search_all_captures(alpha, beta, 4); 
    }

    bool maximizing_player = global_position.white_turn;

    MoveList* all_moves = find_possible_board_moves();
    order_moves(all_moves);
    int move_count = all_moves->count;

    if (move_count == 0){
        int score;
        if (is_king_attacked(maximizing_player)){
            score = maximizing_player ? INT_MIN + (10 - depth) : INT_MAX - (10 - depth);
        } else {
            score = 0;
        }
        free(all_moves);
        return score;
    }

    for (int i = 0; i < move_count; i++){
        make_board_move(all_moves->moves[i]);
        int new_depth = depth - 1;
        int eval = get_move_eval(new_depth, alpha, beta);
        unmake_board_move(all_moves->moves[i]);

        if (maximizing_player){
            if (eval > alpha) alpha = eval;
            if (alpha >= beta) break;
        } else if (!maximizing_player){
            if (eval < beta) beta = eval;
            if (beta <= alpha) break;
        }
    }

    free(all_moves);
    return maximizing_player ? alpha : beta; 
}

Move get_best_move(int depth){
    MoveList* all_moves = find_possible_board_moves();
    order_moves(all_moves);
    Move best_move = all_moves->moves[0];
    int move_count = all_moves->count;

    bool maximizing_player = global_position.white_turn;
    int alpha = INT_MIN;
    int beta = INT_MAX;

    for (int i = 0; i < move_count; i++){
        make_board_move(all_moves->moves[i]);
        int eval = get_move_eval(depth - 1, alpha, beta);
        unmake_board_move(all_moves->moves[i]);

        if (maximizing_player){
            if (eval > alpha){
                alpha = eval;
                best_move = all_moves->moves[i];
            }
            if (alpha >= beta) break;
        } else if (!maximizing_player && eval < beta) {
            if (eval < beta){
                beta = eval;
                best_move = all_moves->moves[i];
            }
            if (beta <= alpha) break;
        }
    }

    free(all_moves);
    printf("Cur Evaluation: %d\n", maximizing_player ? alpha : beta);
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
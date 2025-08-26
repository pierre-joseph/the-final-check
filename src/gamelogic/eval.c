#include "updateboard.h"
#include "fen.h"
#include "eval.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <time.h>

TTEntry transposition_table[1 << 20];
const int PAWN_WEIGHT = 200;
const int KNIGHT_WEIGHT = 600;
const int BISHOP_WEIGHT = 600;
const int ROOK_WEIGHT = 1000;
const int QUEEN_WEIGHT = 1800;
const int KING_WEIGHT = 0;
const int MAX_MATERIAL = 16 * PAWN_WEIGHT + 8 * KNIGHT_WEIGHT + 4 * ROOK_WEIGHT + 2 * QUEEN_WEIGHT;
const int MAX_PHASE = 24;
int searched;
int transposition_positions;

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

int PAWN_PST_ENDGAME[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    40, 40, 40,  40, 40, 40, 40, 40,
    80,  80,  80, 80, 80, 80, 80, 80,
    120, 120, 120, 120, 120, 120, 120, 120,
    160, 160, 160, 160, 160, 160, 160, 160,
    250, 250, 250, 250, 250, 250, 250, 250,
    0,  0,  0,  0,  0,  0,  0,  0
};

int KING_PST_ENDGAME[64] = {
    -50, -40, -40, -40, -40, -40, -40, -50
    -40, -20, -20, -20, -20, -20, -20, -40,
    -40, -20, -5, -5, -5, -5, -20, -40,
    -40, -20, -5, 0, 0, -5, -20, -40,
    -40, -20, -5, 0, 0, -5, -20, -40,
    -40, -20, -5, -5, -5, -5, -20, -40,
    -40, -20, -20, -20, -20, -20, -20, -40,
    -50, -40, -40, -40, -40, -40, -40, -50
};

int piece_weights[6] = {PAWN_WEIGHT, ROOK_WEIGHT, KNIGHT_WEIGHT, BISHOP_WEIGHT, QUEEN_WEIGHT, KING_WEIGHT};
int* psts[6][64] = {PAWN_PST, ROOK_PST, KNIGHT_PST, BISHOP_PST, QUEEN_PST, KING_PST};
int* endgame_psts[6][64] = {PAWN_PST_ENDGAME, ROOK_PST, KNIGHT_PST, BISHOP_PST, QUEEN_PST, KING_PST_ENDGAME};

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
    int white_material = 0;
    int black_material = 0;
    int cur_phase = MAX_PHASE;
    int white_pieces_eval = 0;
    int black_pieces_eval = 0;

    for (int sq = 0; sq < 64; sq++){
        for (int i = 1; i < 5; i++){
            if (GET_BIT(global_position.board_pieces.pieces[i].bb, sq) == 1ULL){
                if (i == 1){
                    cur_phase -= 2;
                } else if (i == 2 || i == 3){
                    cur_phase -= 1;
                } else if (i == 4){
                    cur_phase -= 4;
                }

                white_material += piece_weights[i];
                white_pieces_eval += piece_weights[i];
                white_pieces_eval += (*psts)[i][sq];
                break;
            } else if (GET_BIT(global_position.board_pieces.pieces[i + 6].bb, sq) == 1ULL){
                if (i == 1){
                    cur_phase -= 2;
                } else if (i == 2 || i == 3){
                    cur_phase -= 1;
                } else if (i == 4){
                    cur_phase -= 4;
                }

                black_material += piece_weights[i];
                black_pieces_eval += piece_weights[i];
                black_pieces_eval += (*psts)[i][63 - sq];
                break;
            }
        }
    }

    for (int sq = 0; sq < 64; sq++){
        if (GET_BIT(global_position.board_pieces.pieces[0].bb, sq) == 1ULL){
            white_material += piece_weights[0];
            white_pieces_eval += piece_weights[0];
            white_pieces_eval += floor(((MAX_PHASE - cur_phase) * (*psts)[0][sq] \
            + (cur_phase) * (*endgame_psts)[0][sq]) / MAX_PHASE);
        } else if (GET_BIT(global_position.board_pieces.pieces[6].bb, sq) == 1ULL){
            black_material += piece_weights[0];
            black_pieces_eval += piece_weights[0];
            black_pieces_eval += floor(((MAX_PHASE - cur_phase) * (*psts)[0][63 - sq] \
            + (cur_phase) * (*endgame_psts)[0][63 - sq]) / MAX_PHASE);
        } else if (GET_BIT(global_position.board_pieces.pieces[5].bb, sq) == 1ULL){
            white_pieces_eval += floor(((MAX_PHASE - cur_phase) * (*psts)[5][sq] \
            + (cur_phase) * (*endgame_psts)[5][sq]) / MAX_PHASE);
        } else if (GET_BIT(global_position.board_pieces.pieces[11].bb, sq) == 1ULL){
            black_pieces_eval += floor(((MAX_PHASE - cur_phase) * (*psts)[5][63 - sq] \
            + (cur_phase) * (*endgame_psts)[5][63 - sq]) / MAX_PHASE);
        }
    }

    int white_king_sq = __builtin_ctzll(global_position.board_pieces.pieces[5].bb);
    int black_king_sq = __builtin_ctzll(global_position.board_pieces.pieces[11].bb);
    int row_col_difference = fabs(floor(white_king_sq / 8) - floor(black_king_sq / 8)) \
                                + abs((white_king_sq % 8) - (black_king_sq % 8));

    if (white_material > black_material){
        white_pieces_eval -= floor(1.5 * row_col_difference * cur_phase);
    } else if (white_material < black_material) {
        black_pieces_eval -= floor(1.5 * row_col_difference * cur_phase);
    }

    return white_pieces_eval - black_pieces_eval;
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
            starting_guess = (*psts)[global_position.board[from] % 6][to];
        } else {
            starting_guess = (*psts)[global_position.board[from] % 6][63 - to];
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

TTEntry* probe_tt(uint64_t hash, int depth, int alpha, int beta) {
    TTEntry *entry = &transposition_table[TT_INDEX(hash)];
    if (entry->key == hash && entry->depth >= depth) {
        switch (entry->flag) {
            case 1:
                return entry;  
            case 2:
                if (entry->eval >= beta) return entry; 
                break;
            case 3:
                if (entry->eval <= alpha) return entry; 
                break;
        }
    }

    return NULL; 
}

void store_tt(uint64_t hash, int depth, int eval, int alpha, int beta) {
    int flag = 1;
    if (eval >= beta) {
        flag = 2;
    } else if (eval <= alpha) {
        flag = 3;
    }

    TTEntry *entry = &transposition_table[TT_INDEX(hash)];
    if (depth >= entry->depth) {
        entry->key = hash;
        entry->depth = depth;
        entry->eval = eval;
        entry->flag = flag;
    }
}

int search_all_threats(int cur_depth, int alpha, int beta){
    bool maximizing_player = global_position.white_turn;
    int cur_eval = eval_position();

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

    int game_over = is_game_over();
    if (game_over != 0){
        int score;
        if (game_over == 1){
            score = 0;
        } else {
            score = maximizing_player ? -100000 + cur_depth : 100000 - cur_depth;
        }
        free(all_moves);
        return score;
    }

    for (int i = 0; i < move_count; i++){
        int cur_type = MOVE_FLAGS(all_moves->moves[i]);
        make_board_move(all_moves->moves[i]);

        if (cur_type == 1 || cur_type == 2 || cur_type == 4 || is_king_attacked(global_position.white_turn)){
            searched++;

            TTEntry *entry = probe_tt(global_position.hash, cur_depth, alpha, beta);
            int eval;

            if (entry && entry->depth >= cur_depth){
                transposition_positions++;
                eval = entry->eval;
            } else {
                eval = search_all_threats(cur_depth - 1, alpha, beta);
                store_tt(global_position.hash, cur_depth, eval, alpha, beta);
            }

            unmake_board_move(all_moves->moves[i]);

            if (maximizing_player){
                if (eval > alpha) alpha = eval;
                if (alpha >= beta) break;
            } else if (!maximizing_player){
                if (eval < beta) beta = eval;
                if (beta <= alpha) break;
            }
        } else {
            unmake_board_move(all_moves->moves[i]);
        }
    }

    free(all_moves);
    return maximizing_player ? alpha : beta;
}

int get_move_eval(int depth, int alpha, int beta){
    if (depth == 0){
        return search_all_threats(depth, alpha, beta); 
    }

    bool maximizing_player = global_position.white_turn;

    MoveList* all_moves = find_possible_board_moves();
    order_moves(all_moves);
    int move_count = all_moves->count;

    int game_over = is_game_over();
    if (game_over != 0){
        int score;
        if (game_over == 1){
            score = 0;
        } else {
            score = maximizing_player ? -100000 - depth : 100000 + depth;
        }
        free(all_moves);
        return score;
    }

    for (int i = 0; i < move_count; i++){
        make_board_move(all_moves->moves[i]);
        searched++;
        TTEntry *entry = probe_tt(global_position.hash, depth, alpha, beta);
        int eval;

        if (entry && entry->depth >= depth){
            transposition_positions++;
            eval = entry->eval;
        } else {
            eval = get_move_eval(depth - 1, alpha, beta);
            store_tt(global_position.hash, depth, eval, alpha, beta);
        }

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
    clock_t start, end;
    double cpu_time_used;

    start = clock();

    searched = 0;
    transposition_positions = 0;
    MoveList *all_moves = find_possible_board_moves();
    order_moves(all_moves);
    Move best_move = all_moves->moves[0];
    int move_count = all_moves->count;

    bool maximizing_player = global_position.white_turn;
    int alpha = -100000;
    int beta = 100000;

    for (int i = 0; i < move_count; i++){
        make_board_move(all_moves->moves[i]);
        searched++;
        int eval = get_move_eval(depth - 1, alpha, beta);
        unmake_board_move(all_moves->moves[i]);

        if (maximizing_player && eval > alpha){
            alpha = eval;
            best_move = all_moves->moves[i];
        } else if (!maximizing_player && eval < beta) {
            beta = eval;
            best_move = all_moves->moves[i];
        }
    }

    free(all_moves);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Cur Evaluation: %.2f\n", maximizing_player ? (float) alpha / PAWN_WEIGHT : (float) beta / PAWN_WEIGHT);
    printf("Positions Searched: %d\n", searched);
    printf("Transpositions: %d\n", transposition_positions);
    printf("Move evaluation took %.2f seconds\n", cpu_time_used);
    return best_move;
}

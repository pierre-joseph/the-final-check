#ifndef EVAL_H
#define EVAL_H
#include "updateboard.h"

Move get_best_move(int depth);
Move get_random_move();
int num_of_positions(int depth);
int move_generation_test(int depth, char* fen, bool white_turn, char* can_castle_str, int can_en_passant);

typedef struct {
    uint64_t key;
    int depth;
    int eval;
    int flag; // type of bound (1 = EXACT, 2 = LOWERBOUND, 3 = UPPERBOUND)
} TTEntry;

extern TTEntry transposition_table[1 << 20];
#define TT_SIZE (1 << 20) 
#define TT_INDEX(hash) ((hash) & (TT_SIZE - 1))

#endif
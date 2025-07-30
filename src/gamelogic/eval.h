#ifndef EVAL_H
#define EVAL_H
#include "bitboard.h"

Move get_random_move();
int num_of_positions(int depth, bool white_turn, char* can_castle_str, int can_en_passant);
int move_generation_test(int depth, char* fen, bool white_turn, char* can_castle_str, int can_en_passant);

#endif
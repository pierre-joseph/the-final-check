#include "bitboard.h"
#include <stdlib.h>

Move get_random_move(){
    int move_count = all_moves.count;
    if (move_count > 0){
        int random_idx = rand() % (move_count);
        return all_moves.moves[random_idx];
    } else {
        return 0;
    }
}

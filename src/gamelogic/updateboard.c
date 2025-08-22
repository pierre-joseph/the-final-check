#include "updateboard.h"
#include "movegen.h"
#include "fen.h"
#include "eval.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <time.h>

Bitboard pawn_attacks[2][64];
Bitboard knight_attacks[64];
Bitboard king_attacks[64];
Position global_position;
MoveList cur_all_moves; 
UndoInfo pos_stack[MAX_DEPTH];
uint64_t zobrist_table[12][64];
uint64_t zobrist_castling[4];
uint64_t zobrist_enpassant[8];
uint64_t zobrist_white_to_move; 
uint64_t game_hashes[MAX_DEPTH];

int stack_top;

void print_bitboard(Bitboard bb) {
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 7; file >= 0; file--) {
            int square = rank * 8 + file;
            printf("%d ", (int)((bb >> square) & 1ULL));
        }
        printf("\n");
    }
    printf("\n");
}

Bitboard get_white_pieces(){
    return global_position.board_pieces.pieces[0].bb | global_position.board_pieces.pieces[1].bb | 
           global_position.board_pieces.pieces[2].bb | global_position.board_pieces.pieces[3].bb | 
           global_position.board_pieces.pieces[4].bb | global_position.board_pieces.pieces[5].bb;
}

Bitboard get_black_pieces(){
    return global_position.board_pieces.pieces[6].bb | global_position.board_pieces.pieces[7].bb | 
           global_position.board_pieces.pieces[8].bb | global_position.board_pieces.pieces[9].bb | 
           global_position.board_pieces.pieces[10].bb | global_position.board_pieces.pieces[11].bb;
}

void can_enpassant(Move move){
    int to =  MOVE_TO(move);
    int type = MOVE_FLAGS(move);
    if (type == 5 && to < 32){
        global_position.en_passant = to - 8;
    } else if (type == 5){
        global_position.en_passant = to + 8;
    } else {
        global_position.en_passant = -1;
    }
}

void can_castle(Move move){
    int from = MOVE_FROM(move);
    int to =  MOVE_TO(move);
    bool can_castle_vals[4] = {true, true, true, true};
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

    for (int i = 0; i < 4; i++){
        if (!can_castle_vals[i]){
            global_position.can_castle[i] = ' ';
        }
    }
}

void update_zobrist(){
    if (global_position.en_passant != -1) {
        global_position.hash ^= zobrist_enpassant[global_position.en_passant % 8];
    }

    char can_castle_vals[4] = {'K', 'Q', 'k', 'q'};
    for (int i = 0; i < 4; i++){
        if (global_position.can_castle[i] == can_castle_vals[i]){
            global_position.hash ^= zobrist_castling[i];
        }
    }
}

void make_board_move(Move move){
    UndoInfo cur_pos = {
        .all_moves = global_position.all_moves,
        .en_passant = global_position.en_passant,
        .halfmove_clock = global_position.halfmove_clock,
        .fullmove_number = global_position.move_count,
        .moving_piece = '-',
        .captured_piece = '-',
        .hash = global_position.hash
    };

    for (int i = 0; i < 5; i++){
        cur_pos.can_castle[i] = global_position.can_castle[i];
    }

    pos_stack[stack_top] = cur_pos;
    make_move_helper(move);
    stack_top++; 

    global_position.white_turn = !global_position.white_turn;
    global_position.hash ^= zobrist_white_to_move;
    update_zobrist();
    can_castle(move);
    can_enpassant(move);
    update_zobrist();

    if (pos_stack[stack_top - 1].captured_piece != '-' 
        || pos_stack[stack_top - 1].moving_piece == (char) 0
        || pos_stack[stack_top - 1].moving_piece == (char) 6){
        global_position.halfmove_clock = 0;
    } else {
        global_position.halfmove_clock += 1;
    }


    global_position.move_count += 1;
}

void unmake_board_move(Move move){
    stack_top--;
    global_position.white_turn = !global_position.white_turn;
    global_position.all_moves = pos_stack[stack_top].all_moves;
    global_position.en_passant = pos_stack[stack_top].en_passant;
    global_position.halfmove_clock = pos_stack[stack_top].halfmove_clock;
    global_position.move_count = pos_stack[stack_top].fullmove_number;
    global_position.hash = pos_stack[stack_top].hash;

    for (int i = 0; i < 5; i++){
        global_position.can_castle[i] = pos_stack[stack_top].can_castle[i];
    }

    unmake_move_helper(move);
}

const char* make_react_move(Move move){
    make_board_move(move);
    game_hashes[global_position.move_count - 1] = global_position.hash;
    return get_fen(global_position.board_pieces);
}

int count_set_bits(Bitboard n) {
    int count = 0;
    while (n > 0) {
        count += (n & 1); 
        n >>= 1;         
    }
    return count;
}

bool inefficient_material(){
    int white_pieces = 0;
    int black_pieces = 0;
    for (int i = 0; i < 5; i++){
        Piece white = global_position.board_pieces.pieces[i];
        Piece black = global_position.board_pieces.pieces[i + 6];
        int white_bits = count_set_bits(white.bb);
        int black_bits = count_set_bits(black.bb);
        if (i == 2 || i == 3){
            white_pieces += white_bits;
            black_pieces += black_bits;
        } else {
            white_pieces += white_bits * 2;
            black_pieces += black_bits * 2;
        }

        if (white_pieces >= 2 || black_pieces >= 2){
            return false;
        }
    }
    return true;
}

bool can_piece_attack_square(Piece cur_piece, int king_pos, bool white_attack, int sq){
    char piece_type = cur_piece.type;
    if (toupper(piece_type) == 'P'){
        int side_idx = white_attack ? 0 : 1;
        return (GET_BIT(pawn_attacks[side_idx][sq], king_pos) == 1ULL);
    } else if (toupper(piece_type) == 'N'){
        return (GET_BIT(knight_attacks[sq], king_pos) == 1ULL);
    } else if (toupper(piece_type) == 'K'){
        return (GET_BIT(king_attacks[sq], king_pos) == 1ULL);
    } else {
        Bitboard white_pieces = get_white_pieces();
        Bitboard black_pieces = get_black_pieces();
        Bitboard my_pieces = white_attack ? white_pieces : black_pieces;
        Bitboard opp_pieces = white_attack ? black_pieces : white_pieces;
        MoveList moves_from_sq;
        moves_from_sq.count = 0;
        get_all_moves_from_square(cur_piece, my_pieces, opp_pieces, sq, "    ", -1, &moves_from_sq);
        for (int move = 0; move < moves_from_sq.count; move++){
            if (MOVE_TO(moves_from_sq.moves[move]) == king_pos){
                return true;
            }
        }
        return false;
    }
}

bool is_square_attacked(int king_pos, bool white_attack){
    for (int i = 0; i < 6; i++) {
        int index = white_attack ? i : i + 6;
        char given_type = global_position.board_pieces.pieces[index].type;
        uint64_t bitboard = global_position.board_pieces.pieces[index].bb;
        while (bitboard){
            int sq = __builtin_ctzll(bitboard);
            bitboard &= bitboard - 1;
            if (can_piece_attack_square(global_position.board_pieces.pieces[index], king_pos, white_attack, sq)){ 
                return true;
            }
        }
    }
    return false;
}

bool is_king_attacked(bool white_turn){
    int king_pos = white_turn ? __builtin_ctzll(global_position.board_pieces.pieces[5].bb) 
                    : __builtin_ctzll(global_position.board_pieces.pieces[11].bb);
    bool white_attack = !white_turn;
    return is_square_attacked(king_pos, white_attack);
}

bool is_move_legal(Move cur_move, bool white_turn){
    make_board_move(cur_move);
    bool not_attacked = !is_king_attacked(white_turn);
    unmake_board_move(cur_move);
    return not_attacked;
}

// 0=not over, 1=draw, 2=white, 3=black
int is_game_over(){
    if (global_position.all_moves.count == 0) {
        if (is_king_attacked(global_position.white_turn)) {
            int winner = !global_position.white_turn ? 2 : 3;
            return winner;
        } else {
            return 1;
        }
    }

    int position_count = 0;
    for (int i = 0; i < global_position.move_count; i++){
        if (game_hashes[i] == global_position.hash){
            position_count++;
            if (position_count == 3){
                return 1; 
            }
        }
    }

    if (global_position.halfmove_clock == 50){
        return 1; 
    }

    if (inefficient_material()){
        return 1;
    }
    return 0; 
}

MoveList* find_possible_board_moves(){
    Bitboard white_pieces = get_white_pieces();
    Bitboard black_pieces = get_black_pieces();
    MoveList* all_moves = (MoveList*) malloc(sizeof(MoveList));
    all_moves->count = 0;

    for (int i = 0; i < 12; i++) {
        char given_type = global_position.board_pieces.pieces[i].type;
        uint64_t bitboard = global_position.board_pieces.pieces[i].bb;
        bool is_turn = (isupper(given_type) == global_position.white_turn);
        if (is_turn) {
            Bitboard my_pieces = global_position.white_turn ? white_pieces : black_pieces;
            Bitboard opp_pieces = global_position.white_turn ? black_pieces : white_pieces;
            while (bitboard){
                int sq = __builtin_ctzll(bitboard);
                bitboard &= bitboard - 1;
                MoveList moves_from_sq;
                moves_from_sq.count = 0;
                get_all_moves_from_square(global_position.board_pieces.pieces[i], my_pieces, opp_pieces, 
                sq, global_position.can_castle, global_position.en_passant, &moves_from_sq);
                for (int move = 0; move < moves_from_sq.count; move++){
                    if (is_move_legal(moves_from_sq.moves[move], global_position.white_turn)){
                        all_moves->moves[all_moves->count] = moves_from_sq.moves[move];
                        all_moves->count++;
                    }
                }
            }
        }
    }

    global_position.all_moves = *all_moves;
    return all_moves;
}

void init_pawn_attacks(){
    for (int sq = 0; sq < 64; sq++){
        uint64_t bit = 1ULL << sq;
        int row = floor(sq / 8);
        int col = sq % 8; 
        int start = row * 8 + col;

        //white
        pawn_attacks[0][sq] = 0;
        if (sq < 57 & col > 0){
            SET_BIT(pawn_attacks[0][sq], start + 7); 
        } 
        if (sq < 55 & col < 7){ 
            SET_BIT(pawn_attacks[0][sq], start + 9); 
        }

        // Black
        pawn_attacks[1][sq] = 0;
        if (sq > 6 & col < 7){
            SET_BIT(pawn_attacks[1][sq], start - 7); 
        } 
        if (sq > 8 & col > 0){ 
            SET_BIT(pawn_attacks[1][sq], start - 9); 
        }
    }
}

void init_knight_attacks(){
    for (int sq = 0; sq < 64; sq++){
        uint64_t bit = 1ULL << sq;
        int row = floor(sq / 8);
        int col = sq % 8; 
        knight_attacks[sq] = 0;
        for (int curRow = row - 2; curRow <= row + 2; curRow++) {
            for (int curCol = col - 2; curCol <= col + 2; curCol++) {
                int to = curRow * 8 + curCol;
                if (
                curRow >= 0 &&
                curRow < 8 &&
                curCol >= 0 &&
                curCol < 8 &&
                abs(row - curRow) != abs(col - curCol) &&
                curRow != row &&
                curCol != col
                ) {
                    SET_BIT(knight_attacks[sq], to);
                }
            }
        }
    }
}

void init_king_attacks(){
    for (int sq = 0; sq < 64; sq++){
        uint64_t bit = 1ULL << sq;
        int row = floor(sq / 8);
        int col = sq % 8; 
        king_attacks[sq] = 0;
        for (int curRow = row - 1; curRow <= row + 1; curRow++) {
            for (int curCol = col - 1; curCol <= col + 1; curCol++) {
                int to = curRow * 8 + curCol;
                if (
                curRow >= 0 &&
                curRow < 8 &&
                curCol >= 0 &&
                curCol < 8 &&
                (curRow != row || curCol != col)
                ) {
                    SET_BIT(king_attacks[sq], to);
                }
            }
        }
    }
}

uint64_t xor_shift64() {
    static uint64_t state = 88172645463325252ULL; 
    state ^= state >> 12;
    state ^= state << 25;
    state ^= state >> 27;
    return state * 2685821657736338717ULL;
}

void init_zobrist() {
    for (int piece = 0; piece < 12; piece++) {
        for (int sq = 0; sq < 64; sq++) {
            zobrist_table[piece][sq] = xor_shift64();
        }
    }

    for (int i = 0; i < 4; i++) {
        zobrist_castling[i] = xor_shift64();
    }

    for (int i = 0; i < 8; i++) {
        zobrist_enpassant[i] = xor_shift64();
    }

    zobrist_white_to_move = xor_shift64();
}

void set_starting_hash(){
    global_position.hash = 0ULL;
    for (int sq = 0; sq < 64; sq++) {
        int piece = global_position.board[sq];
        if (piece != -1) {
            global_position.hash ^= zobrist_table[piece][sq];
        }
    }

    if (global_position.white_turn) {
        global_position.hash ^= zobrist_white_to_move;
    }

    char can_castle_vals[4] = {'K', 'Q', 'k', 'q'};
    for (int i = 0; i < 4; i++){
        if (global_position.can_castle[i] == can_castle_vals[i]){
            global_position.hash ^= zobrist_castling[i];
        }
    }
    
    if (global_position.en_passant != -1) {
        global_position.hash ^= zobrist_enpassant[global_position.en_passant];
    }
}

void start_game(char* fen, bool white_turn, char* can_castle){
    srand(time(0));
    global_position.board_pieces = get_bitboards(fen);
    set_board();
    global_position.white_turn = white_turn;
    global_position.en_passant = -1;
    global_position.halfmove_clock = 0;
    global_position.move_count = 0;

    for (int i = 0; i < 5; i++){
        global_position.can_castle[i] = can_castle[i];
    }

    init_pawn_attacks();
    init_knight_attacks();
    init_king_attacks();
    init_zobrist();
    set_starting_hash();
}

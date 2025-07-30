#include "bitboard.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

Pieces get_bitboards(const char* fen){
    Pieces board_pieces = {
        .pieces = {{'P', 0x0ULL}, {'R', 0x0ULL}, {'N', 0x0ULL}, {'B',0x0ULL}, 
                    {'Q', 0x0ULL}, {'K', 0x0ULL}, {'p', 0x0ULL}, {'r', 0x0ULL}, 
                    {'n', 0x0ULL}, {'b', 0x0ULL}, {'q', 0x0ULL}, {'k', 0x0ULL}}
    };

    int sq = 63;
    for (int i = 0; i < strlen(strtok((char*) fen, " ")); i++){
        if (isalpha(fen[i])){
            for (int j = 0; j < 12; j++) {
                if (board_pieces.pieces[j].type == fen[i]){
                    SET_BIT(board_pieces.pieces[j].bb, sq);
                }
            }
            sq--; 
        } else if (isdigit(fen[i])) {
            int digit_char = fen[i] - '0';
            sq -= digit_char;
        }
    }

    return board_pieces;
}

const char* get_fen(Pieces board_pieces) {
    char beginningFenString[65] = "1111111111111111111111111111111111111111111111111111111111111111";
    for (int i = 0; i < 12; i++) {
        char given_type = board_pieces.pieces[i].type;
        uint64_t bitboard = board_pieces.pieces[i].bb;
        for (int rank = 7; rank >= 0; rank--) {
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                if (GET_BIT(bitboard, square) == 1ULL){
                    beginningFenString[square] = given_type; 
                }
            }
        }
    }

    char new_fen_string[65] = "";
    int len = 0;
    int file = 0;
    int rank = 1;
    int emptycount = 0;
    for (int i = (int) strlen(beginningFenString) - 1; i >= 0; i--){
        if (beginningFenString[i] != '1'){
            if (emptycount > 0){
                char char_digit = '0' + emptycount;
                new_fen_string[len] = char_digit;
                emptycount = 0;
                len++;
            }
            new_fen_string[len] = beginningFenString[i]; 
            len++;
            file++;
        } else {
            file++;
            emptycount++;
        }

        if (file == 8){
            if (emptycount > 0){
                char char_digit = '0' + emptycount;
                new_fen_string[len] = char_digit;
                emptycount = 0;
                len++;
            }
            if (rank < 8){
                new_fen_string[len] = '/';
                len++;
                file = 0;   
                rank++;
            }
        }
    }

    static char static_fen[100];
    strcpy(static_fen, new_fen_string);
    
    return static_fen;
}
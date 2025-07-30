#include "../../Unity/src/unity.h"
#include "eval.h"
#include <stdbool.h>

void setUp(void) {
 
}

void tearDown(void) {

}

void starting_board(void) {
    char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    bool isWhite = true; 
    char* can_castle = "KQkq";
    int can_enpassant = -1;

    TEST_ASSERT_EQUAL_INT(1, move_generation_test(0, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(20, move_generation_test(1, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(400, move_generation_test(2, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(8902, move_generation_test(3, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(197281, move_generation_test(4, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(4865609, move_generation_test(5, fen, isWhite, can_castle, can_enpassant));
}

void board_2(void) {
    char* fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R";
    bool isWhite = true; 
    char* can_castle = "KQkq";
    int can_enpassant = -1;

    TEST_ASSERT_EQUAL_INT(1, move_generation_test(0, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(48, move_generation_test(1, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(2039, move_generation_test(2, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(97862, move_generation_test(3, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(4085603, move_generation_test(4, fen, isWhite, can_castle, can_enpassant));
    //TEST_ASSERT_EQUAL_INT(193690690, move_generation_test(5, fen, isWhite, can_castle, can_enpassant));
}

void board_3(void) {
    char* fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8";
    bool isWhite = true; 
    char* can_castle = "----";
    int can_enpassant = -1;

    TEST_ASSERT_EQUAL_INT(1, move_generation_test(0, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(14, move_generation_test(1, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(191, move_generation_test(2, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(2812, move_generation_test(3, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(43238, move_generation_test(4, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(674624, move_generation_test(5, fen, isWhite, can_castle, can_enpassant));
}

void board_4(void) {
    char* fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1";
    bool isWhite = true; 
    char* can_castle = "--kq";
    int can_enpassant = -1;

    TEST_ASSERT_EQUAL_INT(1, move_generation_test(0, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(6, move_generation_test(1, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(264, move_generation_test(2, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(9467, move_generation_test(3, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(422333, move_generation_test(4, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(15833292, move_generation_test(5, fen, isWhite, can_castle, can_enpassant));
}

void board_5(void) {
    char* fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R";
    bool isWhite = true; 
    char* can_castle = "KQ--";
    int can_enpassant = -1;

    TEST_ASSERT_EQUAL_INT(1, move_generation_test(0, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(44, move_generation_test(1, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(1486, move_generation_test(2, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(62379, move_generation_test(3, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(2103487, move_generation_test(4, fen, isWhite, can_castle, can_enpassant));
    //TEST_ASSERT_EQUAL_INT(89941194, move_generation_test(5, fen, isWhite, can_castle, can_enpassant));
}

void board_6(void) {
    char* fen = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1";
    bool isWhite = true; 
    char* can_castle = "----";
    int can_enpassant = -1;

    TEST_ASSERT_EQUAL_INT(1, move_generation_test(0, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(46, move_generation_test(1, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(2079, move_generation_test(2, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(89890, move_generation_test(3, fen, isWhite, can_castle, can_enpassant));
    TEST_ASSERT_EQUAL_INT(3894594, move_generation_test(4, fen, isWhite, can_castle, can_enpassant));
    //TEST_ASSERT_EQUAL_INT(164075551, move_generation_test(5, fen, isWhite, can_castle, can_enpassant));
}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(starting_board);
    RUN_TEST((board_2));
    RUN_TEST((board_3));
    RUN_TEST((board_4));
    RUN_TEST((board_5));
    RUN_TEST((board_6));

    return UNITY_END();
}
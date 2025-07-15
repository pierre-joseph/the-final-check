import MoveGenerationTest from './AI.js';
import { convertFENToBoard } from './updateBoard.js';

test('move generation with starting board', () => {
    const color = "white";
    const enPassant = null;

    const board = convertFENToBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    const canCastle = {
        whiteQueenside: true,
        whiteKingside: true,
        blackQueenside: true,
        blackKingside: true
    };

    expect(MoveGenerationTest(1, board, color, canCastle, enPassant)).toBe(20);
    expect(MoveGenerationTest(2, board, color, canCastle, enPassant)).toBe(400);
    expect(MoveGenerationTest(3, board, color, canCastle, enPassant)).toBe(8902);
    expect(MoveGenerationTest(4, board, color, canCastle, enPassant)).toBe(197281);
}) 

test('move generation with board2', () => {
    const color = "white";
    const enPassant = null;

    const board = convertFENToBoard("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    const canCastle = {
        whiteQueenside: true,
        whiteKingside: true,
        blackQueenside: true,
        blackKingside: true
    };

    expect(MoveGenerationTest(1, board, color, canCastle, enPassant)).toBe(48);
    expect(MoveGenerationTest(2, board, color, canCastle, enPassant)).toBe(2039);
    expect(MoveGenerationTest(3, board, color, canCastle, enPassant)).toBe(97862);
    expect(MoveGenerationTest(4, board, color, canCastle, enPassant)).toBe(4085603);
})

test('move generation with board3', () => {
    const color = "white";
    const enPassant = null;

    const board = convertFENToBoard("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 ");
    const canCastle = {
        whiteQueenside: false,
        whiteKingside: false,
        blackQueenside: false,
        blackKingside: false
    };

    expect(MoveGenerationTest(1, board, color, canCastle, enPassant)).toBe(14);
    expect(MoveGenerationTest(2, board, color, canCastle, enPassant)).toBe(191);
    expect(MoveGenerationTest(3, board, color, canCastle, enPassant)).toBe(2812);
    expect(MoveGenerationTest(4, board, color, canCastle, enPassant)).toBe(43238);
})

test('move generation with board4', () => {
    const color = "white";
    const enPassant = null;

    const board = convertFENToBoard("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    const canCastle = {
        whiteQueenside: false,
        whiteKingside: false,
        blackQueenside: true,
        blackKingside: true
    };

    expect(MoveGenerationTest(1, board, color, canCastle, enPassant)).toBe(6);
    expect(MoveGenerationTest(2, board, color, canCastle, enPassant)).toBe(264);
    expect(MoveGenerationTest(3, board, color, canCastle, enPassant)).toBe(9467);
    expect(MoveGenerationTest(4, board, color, canCastle, enPassant)).toBe(422333);
})

test('move generation with board5', () => {
    const color = "white";
    const enPassant = null;

    const board = convertFENToBoard("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    const canCastle = {
        whiteQueenside: true,
        whiteKingside: true,
        blackQueenside: false,
        blackKingside: false
    };

    expect(MoveGenerationTest(1, board, color, canCastle, enPassant)).toBe(44);
    expect(MoveGenerationTest(2, board, color, canCastle, enPassant)).toBe(1486);
    expect(MoveGenerationTest(3, board, color, canCastle, enPassant)).toBe(62379);
    expect(MoveGenerationTest(4, board, color, canCastle, enPassant)).toBe(2103487);
})

test('move generation with board6', () => {
    const color = "white";
    const enPassant = null;

    const board = convertFENToBoard("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    const canCastle = {
        whiteQueenside: false,
        whiteKingside: false,
        blackQueenside: false,
        blackKingside: false
    };

    expect(MoveGenerationTest(1, board, color, canCastle, enPassant)).toBe(46);
    expect(MoveGenerationTest(2, board, color, canCastle, enPassant)).toBe(2079);
    expect(MoveGenerationTest(3, board, color, canCastle, enPassant)).toBe(89890);
    expect(MoveGenerationTest(4, board, color, canCastle, enPassant)).toBe(3894594);
})

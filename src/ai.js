import {
  makeMove,
  unmakeMove,
  findAllPossibleBoardMoves,
  checkIfCanCastle,
  checkIfCanEnPassant,
  getOppColor,
} from "./updateBoard.js";

const pawnVal = 100;
const knightVal = 300;
const bishopVal = 350;
const rookVal = 500;
const queenVal = 900;

export function randomMoves(board, possibleMoves) {
  let move = null;
  let pieceIdx = Math.floor(Math.random() * possibleMoves.length);
  let newSq = Math.floor(Math.random() * possibleMoves[pieceIdx].moves.length);
  const newRow = possibleMoves[pieceIdx].moves[newSq].row;
  const newCol = possibleMoves[pieceIdx].moves[newSq].col;
  const promoteTo = possibleMoves[pieceIdx].moves[newSq].promoteTo;
  const startSquare = {
    value:
      board[possibleMoves[pieceIdx].curRow][possibleMoves[pieceIdx].curCol],
    row: possibleMoves[pieceIdx].curRow,
    col: possibleMoves[pieceIdx].curCol,
  };
  const endSquare = {
    value: board[newRow][newCol],
    row: newRow,
    col: newCol,
    promoteTo: promoteTo,
  };
  move = { startSquare, endSquare };
  return move;
}

export function findBestMove(depth, board, color, canCastle, canEnPassant) {

}

function evaluateBoard(board) {
  let whitePieces = 0;
  let blackPieces = 0;
  for (let row = 0; row < 8; row++) {
    for (let col = 0; col < 8; col++) {
      if (board[row][col] && board[row][col].type != "k") {
        let val = 0;
        switch (props.value.type) {
          case "p":
            val = pawnVal;
            break;
          case "n":
            val = knightVal;
            break;
          case "b":
            val = bishopVal;
            break;
          case "r":
            val = rookVal;
            break;
          case "q":
            val = queenVal;
            break;
        }
        if (board[row][col].color == "white") {
          whitePieces += val;
        } else {
          blackPieces;
        }
      }
    }
  }

  return whitePieces - blackPieces;
}

export function MoveGenerationTest(
  depth,
  board,
  color,
  canCastle,
  canEnPassant
) {
  if (depth == 0) {
    return 1;
  }

  const allPieces = findAllPossibleBoardMoves(
    board,
    color,
    canCastle,
    canEnPassant
  );
  let numPositions = 0;
  for (let piece of allPieces) {
    const startSquare = {
      value: board[piece.curRow][piece.curCol],
      row: piece.curRow,
      col: piece.curCol,
    };
    for (let move of piece.moves) {
      const endSquare = {
        value: board[move.row][move.col],
        row: move.row,
        col: move.col,
      };
      makeMove(board, startSquare, endSquare, move.promoteTo);
      const newCanCastle = checkIfCanCastle(canCastle, startSquare, endSquare);
      const newCanEnpassant = checkIfCanEnPassant(startSquare, endSquare);
      numPositions += MoveGenerationTest(
        depth - 1,
        board,
        getOppColor(color),
        newCanCastle,
        newCanEnpassant
      );
      unmakeMove(board, startSquare, endSquare);
    }
  }

  return numPositions;
}

// Used to test MoveGenerationTest function in ai.test.js
//module.exports = MoveGenerationTest;

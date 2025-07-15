import {
  getNewBoard,
  findAllPossibleBoardMoves,
  checkIfCanCastle,
  checkIfCanEnPassant,
  getOppColor
} from "./updateBoard.js";

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
      const newBoard = getNewBoard(
        board.map((row) => row.slice()),
        startSquare,
        endSquare,
        move.promoteTo
      );
      const newCanCastle = checkIfCanCastle(canCastle, startSquare, endSquare);
      const newCanEnpassant = checkIfCanEnPassant(startSquare, endSquare);
      numPositions += MoveGenerationTest(
        depth - 1,
        newBoard, 
        getOppColor(color),
        newCanCastle,
        newCanEnpassant
      );
    }
  }

  return numPositions;
}

// Used to test MoveGenerationTest function in ai.test.js
// module.exports = MoveGenerationTest;

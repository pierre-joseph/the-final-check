export default function randomMoves(board, possibleMoves) {
  let move = null;
  let pieceIdx = Math.floor(Math.random() * possibleMoves.length);
  let newSq = Math.floor(Math.random() * possibleMoves[pieceIdx].moves.length);
  const newRow = possibleMoves[pieceIdx].moves[newSq].row;
  const newCol = possibleMoves[pieceIdx].moves[newSq].col;
  const promoteTo = possibleMoves[pieceIdx].moves[newSq].promoteTo;
  const startSquare = {
    value: board[possibleMoves[pieceIdx].curRow][possibleMoves[pieceIdx].curCol],
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


import { getAllMovesFromSquare } from "./allMovesFromSquare.js";

export function makeMove(board, startSquare, endSquare, promoteTo) {
  board[startSquare.row][startSquare.col] = null;
  board[endSquare.row][endSquare.col] = startSquare.value;

  if (startSquare.value.type == "k") {
    if (startSquare.col == 4 && endSquare.col == 6) {
      board[startSquare.row][5] = { color: startSquare.value.color, type: "r" };
      board[startSquare.row][7] = null;
    } else if (startSquare.col == 4 && endSquare.col == 2) {
      board[startSquare.row][3] = { color: startSquare.value.color, type: "r" };
      board[startSquare.row][0] = null;
    }
  }

  if (
    startSquare.value.type == "p" &&
    startSquare.col != endSquare.col &&
    endSquare.value == null
  ) {
    board[startSquare.row][endSquare.col] = null;
  }

  if (
    startSquare.value.type == "p" &&
    (endSquare.row == 0 || endSquare.row == 7)
  ) {
    board[endSquare.row][endSquare.col] = {
      color: startSquare.value.color,
      type: promoteTo,
    };
  }
}

export function unmakeMove(board, startSquare, endSquare) {
  board[startSquare.row][startSquare.col] = startSquare.value;
  board[endSquare.row][endSquare.col] = endSquare.value;

  if (startSquare.value.type == "k") {
    if (startSquare.col == 4 && endSquare.col == 6) {
      board[startSquare.row][5] = null;
      board[startSquare.row][7] = { color: startSquare.value.color, type: "r" };
    } else if (startSquare.col == 4 && endSquare.col == 2) {
      board[startSquare.row][3] = null;
      board[startSquare.row][0] = { color: startSquare.value.color, type: "r" };
    }
  }

  if (
    startSquare.value.type == "p" &&
    startSquare.col != endSquare.col &&
    endSquare.value == null
  ) {
    board[startSquare.row][endSquare.col] = { color: getOppColor(startSquare.value.color), type: "p" };
  }
}

export function findAllPossibleBoardMoves(
  board,
  color,
  canCastle,
  canEnPassant
) {
  let possibleMoves = [];
  for (let row = 0; row < 8; row++) {
    for (let col = 0; col < 8; col++) {
      if (board[row][col] && board[row][col].color == color) {
        possibleMoves.push({
          curRow: row,
          curCol: col,
          moves: getAllMovesFromSquare(
            board,
            { value: board[row][col], row: row, col: col },
            canCastle,
            canEnPassant
          ).filter((move) => {
            return isMoveLegal(
              board,
              row,
              col,
              move.row,
              move.col,
              color,
              move.promoteTo
            );
          }),
        });
      }
    }
  }

  const filteredPossibleMoves = possibleMoves.filter(
    (piece) => piece.moves.length != 0
  );
  return filteredPossibleMoves;
}

export function checkIfOver(newBoard, possibleMoves, color) {
  let existsMove = false;
  let whitePieces = 0;
  let blackPieces = 0;
  for (let row = 0; row < 8; row++) {
    for (let col = 0; col < 8; col++) {
      if (
        newBoard[row][col] &&
        (newBoard[row][col].type == "p" ||
          newBoard[row][col].type == "r" ||
          newBoard[row][col].type == "q")
      ) {
        if (newBoard[row][col].color == "white") {
          whitePieces += 1;
        } else {
          blackPieces += 1;
        }
      } else if (
        newBoard[row][col] &&
        (newBoard[row][col].type == "n" || newBoard[row][col].type == "b")
      ) {
        if (newBoard[row][col].color == "white") {
          whitePieces += 0.5;
        } else {
          blackPieces += 0.5;
        }
      }
    }
  }

  for (let i = 0; i < possibleMoves.length; i++) {
    if (possibleMoves[i].moves.length != 0) {
      existsMove = true;
    }
  }

  if (existsMove) {
    if (whitePieces < 1 && blackPieces < 1) {
      return "Draw";
    } else {
      return null;
    }
  } else {
    if (!isKingAttacked(newBoard, getOppColor(color))) {
      return "Draw";
    } else {
      return color;
    }
  }
}

export function checkIfCanCastle(prev, startSquare, endSquare) {
  let newValue = prev;

  if (startSquare.value.color == "white" && startSquare.value.type == "k") {
    newValue = { ...newValue, whiteQueenside: false, whiteKingside: false };
  } 
  
  if ((startSquare.row == 7 && startSquare.col == 0) ||
      (endSquare.row == 7 && endSquare.col == 0)
  ) {
    newValue = { ...newValue, whiteQueenside: false };
  } 
  
  if (
    (startSquare.row == 7 && startSquare.col == 7) ||
    (endSquare.row == 7 && endSquare.col == 7)
  ) {
    newValue = { ...newValue, whiteKingside: false };
  } 
  
  if (startSquare.value.color == "black" && startSquare.value.type == "k") {
    newValue = { ...newValue, blackQueenside: false, blackKingside: false };
  } 
  
  if (
    (startSquare.row == 0 && startSquare.col == 0) ||
    (endSquare.row == 0 && endSquare.col == 0)
  ) {
    newValue = { ...newValue, blackQueenside: false };
  } 
  
  if (
    (startSquare.row == 0 && startSquare.col == 7) ||
    (endSquare.row == 0 && endSquare.col == 7)
  ) {
    newValue = { ...newValue, blackKingside: false };
  }

  return newValue;
}

export function checkIfCanEnPassant(startSquare, endSquare) {
  if ((startSquare.value.type == "p") &&
    ((startSquare.value.color == "white" &&
      startSquare.row == 6 &&
      endSquare.row == 4) ||
    (startSquare.value.color == "black" &&
      startSquare.row == 1 &&
      endSquare.row == 3))
  ) {
    return { col: endSquare.col };
  } else {
    return null;
  }
}

export function isMoveLegal(
  board,
  fromRow,
  fromCol,
  toRow,
  toCol,
  color,
  promoteTo
) {
  const startSquare = { value: board[fromRow][fromCol], row: fromRow, col: fromCol };
  const endSquare = { value: board[toRow][toCol], row: toRow, col: toCol };
  makeMove(board, startSquare, endSquare, promoteTo);
  const notAttacked = !isKingAttacked(board, color);
  unmakeMove(board, startSquare, endSquare);
  return notAttacked;
}

export function isKingAttacked(board, color) {
  const oppColor = getOppColor(color);
  const kingPos = findKing(board, color);
  return isSquareAttacked(board, kingPos.row, kingPos.col, oppColor);
}

function findKing(board, color) {
  for (let row = 0; row < 8; row++) {
    for (let col = 0; col < 8; col++) {
      const piece = board[row][col];
      if (piece && piece.type == "k" && piece.color == color) {
        return { row, col };
      }
    }
  }
}

function isSquareAttacked(board, targetRow, targetCol, oppColor) {
  for (let row = 0; row < 8; row++) {
    for (let col = 0; col < 8; col++) {
      const startingPieceValue = board[row][col];
      if (startingPieceValue && startingPieceValue.color == oppColor) {
        if (
          canPieceAttackSquare(
            board,
            { value: startingPieceValue, row: row, col: col },
            {
              whiteQueenside: false,
              whiteKingside: false,
              blackQueenside: false,
              blackKingside: false,
            },
            null,
            targetRow,
            targetCol
          )
        ) {
          return true;
        }
      }
    }
  }
  return false;
}

function canPieceAttackSquare(
  board,
  startingPiece,
  canCastle,
  canEnPassant,
  targetRow,
  targetCol
) {
  let allMoves = getAllMovesFromSquare(
    board,
    startingPiece,
    canCastle,
    canEnPassant
  );
  return allMoves.some((sq) => sq.row === targetRow && sq.col === targetCol);
}

export function getOppColor(color) {
  return color == "white" ? "black" : "white";
}

export function convertFENToBoard(fen) {
  const fenBoard = fen.split(" ")[0];
  let board = [];
  let curRank = [];
  let col = 0;
  let row = 0;

  for (let char of fenBoard) {
    if (char == "/") {
      board.push(curRank);
      curRank = [];
      row = row + 1;
      col = 0;
    } else {
      if (Number.isInteger(Number(char))) {
        for (let i = 0; i < Number(char); i++) {
          curRank.push(null);
        }
        col = col + Number(char);
      } else {
        const isUpperCase = char === char.toUpperCase();
        const type = char.toLowerCase();
        curRank.push({ iswhite: isUpperCase, type: type });
        col += 1;
      }
    }
  }
  board.push(curRank);
  return board;
}

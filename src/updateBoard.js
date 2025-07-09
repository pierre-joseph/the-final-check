import { getAllMovesFromSquare } from "./allMovesFromSquare";

export function getNewBoard(prevBoard, startSquare, endSquare, promoteTo) {
    let newBoard = prevBoard;
    newBoard[startSquare.row][startSquare.col] = null;
    newBoard[endSquare.row][endSquare.col] = startSquare.value;
  
    if (startSquare.value.color == "white" && startSquare.value.type == "k") {
      if (startSquare.col == 4 && endSquare.col == 6) {
        newBoard[7][5] = { color: "white", type: "r" };
        newBoard[7][7] = null;
      } else if (startSquare.col == 4 && endSquare.col == 2) {
        newBoard[7][3] = { color: "white", type: "r" };
        newBoard[7][0] = null;
      }
    } else if (
      startSquare.value.color == "black" &&
      startSquare.value.type == "k"
    ) {
      if (startSquare.col == 4 && endSquare.col == 6) {
        newBoard[0][5] = { color: "black", type: "r" };
        newBoard[0][7] = null;
      } else if (startSquare.col == 4 && endSquare.col == 2) {
        newBoard[0][3] = { color: "black", type: "r" };
        newBoard[0][0] = null;
      }
    }
  
    if (
      startSquare.value.type == "p" &&
      startSquare.col != endSquare.col &&
      endSquare.value == null
    ) {
      newBoard[startSquare.row][startSquare.col] = null;
    }
  
    if (
      startSquare.value.type == "p" &&
      (endSquare.row == 0 || endSquare.row == 7)
    ) {
      newBoard[endSquare.row][endSquare.col] = {
        color: startSquare.value.color,
        type: promoteTo,
      };
    }
  
    return newBoard;
  }
  
  export function findAllPossibleBoardMoves(board, color, canEnPassant, canCastle) {
    let newPossible = [
      [[], [], [], [], [], [], [], []],
      [[], [], [], [], [], [], [], []],
      [[], [], [], [], [], [], [], []],
      [[], [], [], [], [], [], [], []],
      [[], [], [], [], [], [], [], []],
      [[], [], [], [], [], [], [], []],
      [[], [], [], [], [], [], [], []],
      [[], [], [], [], [], [], [], []],
    ];
    for (let row = 0; row < 8; row++) {
      for (let col = 0; col < 8; col++) {
        if (board[row][col] && board[row][col].color == color) {
          newPossible[row][col] = getAllMovesFromSquare(
            board,
            { value: board[row][col], row: row, col: col },
            canCastle,
            canEnPassant
          ).filter((move) => {
            return isMoveLegal(board, row, col, move.row, move.col, color, move.promoteTo);
          });
        }
      }
    }
    return newPossible;
  }

  export function checkIfOver(newBoard, moves){
      let existsMove = false;
      let whitePieces = 0;
      let blackPieces = 0;
      for (let row = 0; row < 8; row++) {
        for (let col = 0; col < 8; col++) {
          if (moves[row][col].length != 0) {
            existsMove = true;
          }

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
            (newBoard[row][col].type == "n" ||
              newBoard[row][col].type == "b")
          ) {
            if (newBoard[row][col].color == "white") {
              whitePieces += 0.5;
            } else {
              blackPieces += 0.5;
            }
          }
        }
      }
      if (existsMove) {
        if (whitePieces < 1 && blackPieces < 1) {
          return "Draw";
        } else {
          return null;
        }
      } else {
        if (!isKingAttacked(newBoard, getOppColor(curTurn))) {
          return "Draw";
        } else {
          return curTurn;
        }
      }
    }

export function checkIfCanCastle(prev, startSquare){
  if (startSquare.value.color == "white") {
    if (startSquare.value.type == "k") {
      return { ...prev, whiteQueenside: false, whiteKingside: false };
    } else if (
      startSquare.value.type == "r" &&
      startSquare.row == 7 &&
      startSquare.col == 0
    ) {
      return { ...prev, whiteQueenside: false };
    } else if (
      startSquare.value.type == "r" &&
      startSquare.row == 7 &&
      startSquare.col == 7
    ) {
      return { ...prev, whiteKingside: false };
    }
  } else {
    if (startSquare.value.type == "k") {
      return { ...prev, blackQueenside: false, blackKingside: false };
    } else if (
      startSquare.value.type == "r" &&
      startSquare.row == 0 &&
      startSquare.col == 0
    ) {
      return { ...prev, blackQueenside: false };
    } else if (
      startSquare.value.type == "r" &&
      startSquare.row == 0 &&
      startSquare.col == 7
    ) {
      return { ...prev, blackKingside: false };
    }
  }
  return prev;
}

export function checkIfCanEnPassant(startSquare, endSquare){
  if (
    (startSquare.value.color == "white" &&
      startSquare.row == 6 &&
      endSquare.row == 4) ||
    (startSquare.value.color == "black" &&
      startSquare.row == 1 &&
      endSquare.row == 3)
  ) {
    return { col: endSquare.col };
  } else {
    return null;
  }
}
  
  export function isMoveLegal(board, fromRow, fromCol, toRow, toCol, color, promoteTo) {
    const simulated = board.map((row) => row.slice());
    const simulatedNewBoard = getNewBoard(
      simulated,
      { value: board[fromRow][fromCol], row: fromRow, col: fromCol },
      { value: board[toRow][toCol], row: toRow, col: toCol },  
      promoteTo
    );
    return !isKingAttacked(simulatedNewBoard, color)
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
              null,
              {
                whiteQueenside: false,
                whiteKingside: false,
                blackQueenside: false,
                blackKingside: false,
              },
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
    canEnPassant,
    canCastle,
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
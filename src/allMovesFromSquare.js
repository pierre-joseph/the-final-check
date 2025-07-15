import { isMoveLegal, isKingAttacked } from "./updateBoard.js";

export function getAllMovesFromSquare(
    board,
    startingPiece,
    canCastle,
    canEnPassant
  ) {
    const row = startingPiece.row;
    const col = startingPiece.col;
    const type = startingPiece.value.type;
    const turn = startingPiece.value.color;
  
    switch (type) {
      case "p":
        return getPawnMoves(row, col, turn, board, canEnPassant);
      case "n":
        return getKnightMoves(row, col, turn, board);
      case "b":
        return getBishopMoves(row, col, turn, board);
      case "r":
        return getRookMoves(row, col, turn, board);
      case "q":
        return getQueenMoves(row, col, turn, board);
      case "k":
        return getKingMoves(row, col, turn, board, canCastle);
    }
  }
  
  function getPawnMoves(row, col, color, board, canEnPassant) {
    let allMoves = [];
    if (color == "white") {
      if (row == 6 && board[5][col] == null && board[4][col] == null) {
        allMoves.push({ row: 4, col: col });
      }
  
      if (board[row - 1][col] == null) {
        if (row - 1 == 0){
          allMoves.push({ row: row - 1, col: col, promoteTo: "b" });
          allMoves.push({ row: row - 1, col: col, promoteTo: "n" });
          allMoves.push({ row: row - 1, col: col, promoteTo: "r" });
          allMoves.push({ row: row - 1, col: col, promoteTo: "q" });
        } else {
          allMoves.push({ row: row - 1, col: col });
        }
      }
  
      if (
        col > 0 &&
        ((board[row - 1][col - 1] && board[row - 1][col - 1].color != color) ||
          (row == 3 && canEnPassant && canEnPassant.col == col - 1))
      ) {
        if (row - 1 == 0){
          allMoves.push({ row: row - 1, col: col - 1, promoteTo: "b" });
          allMoves.push({ row: row - 1, col: col - 1, promoteTo: "n" });
          allMoves.push({ row: row - 1, col: col - 1, promoteTo: "r" });
          allMoves.push({ row: row - 1, col: col - 1, promoteTo: "q" });
        } else {
          allMoves.push({ row: row - 1, col: col - 1 });
        }
      }
  
      if (
        col < 7 &&
        ((board[row - 1][col + 1] && board[row - 1][col + 1].color != color) ||
          (row == 3 && canEnPassant && canEnPassant.col == col + 1))
      ) {
        if (row - 1 == 0){
          allMoves.push({ row: row - 1, col: col + 1, promoteTo: "b" });
          allMoves.push({ row: row - 1, col: col + 1, promoteTo: "n" });
          allMoves.push({ row: row - 1, col: col + 1, promoteTo: "r" });
          allMoves.push({ row: row - 1, col: col + 1, promoteTo: "q" });
        } else {
          allMoves.push({ row: row - 1, col: col + 1 });
        }
      }

    } else if (color == "black") {
      if (row == 1 && board[2][col] == null && board[3][col] == null) {
        allMoves.push({ row: 3, col: col });
      }
  
      if (board[row + 1][col] == null) {
        if (row + 1 == 7){
          allMoves.push({ row: row + 1, col: col, promoteTo: "b" });
          allMoves.push({ row: row + 1, col: col, promoteTo: "n" });
          allMoves.push({ row: row + 1, col: col, promoteTo: "r" });
          allMoves.push({ row: row + 1, col: col, promoteTo: "q" });
        } else {
          allMoves.push({ row: row + 1, col: col});
        }
      }
  
      if (
        col > 0 &&
        ((board[row + 1][col - 1] && board[row + 1][col - 1].color != color) ||
          (row == 4 && canEnPassant && canEnPassant.col == col - 1))
      ) {
        if (row + 1 == 7){
          allMoves.push({ row: row + 1, col: col - 1, promoteTo: "b" });
          allMoves.push({ row: row + 1, col: col - 1, promoteTo: "n" });
          allMoves.push({ row: row + 1, col: col - 1, promoteTo: "r" });
          allMoves.push({ row: row + 1, col: col - 1, promoteTo: "q" });
        } else {
          allMoves.push({ row: row + 1, col: col - 1});
        }
      }

      if (
        col < 7 &&
        ((board[row + 1][col + 1] && board[row + 1][col + 1].color != color) ||
          (row == 4 && canEnPassant && canEnPassant.col == col + 1))
      ) {
        if (row + 1 == 7){
          allMoves.push({ row: row + 1, col: col + 1, promoteTo: "b" });
          allMoves.push({ row: row + 1, col: col + 1, promoteTo: "n" });
          allMoves.push({ row: row + 1, col: col + 1, promoteTo: "r" });
          allMoves.push({ row: row + 1, col: col + 1, promoteTo: "q" });
        } else {
          allMoves.push({ row: row + 1, col: col + 1});
        }
      }
    }

    return allMoves;
  }
  
  function getKnightMoves(row, col, color, board) {
    let allMoves = [];
    for (let curRow = row - 2; curRow <= row + 2; curRow++) {
      for (let curCol = col - 2; curCol <= col + 2; curCol++) {
        if (
          curRow >= 0 &&
          curRow < 8 &&
          curCol >= 0 &&
          curCol < 8 &&
          Math.abs(row - curRow) != Math.abs(col - curCol) &&
          (board[curRow][curCol] == null ||
            board[curRow][curCol].color != color) &&
          curRow != row &&
          curCol != col
        ) {
          allMoves.push({ row: curRow, col: curCol });
        }
      }
    }
    return allMoves;
  }
  
  function getBishopMoves(row, col, color, board) {
    let allMoves = [];
    for (let i = 1; i < 8; i++) {
      const curRow = row - i;
      const curCol = col - i;
      if (curRow < 0 || curCol < 0) {
        break;
      } else if (board[curRow][curCol] == null) {
        allMoves.push({ row: curRow, col: curCol });
      } else if (board[curRow][curCol] && board[curRow][curCol].color != color) {
        allMoves.push({ row: curRow, col: curCol });
        break;
      } else {
        break;
      }
    }
  
    for (let i = 1; i < 8; i++) {
      const curRow = row + i;
      const curCol = col - i;
      if (curRow > 7 || curCol < 0) {
        break;
      } else if (board[curRow][curCol] == null) {
        allMoves.push({ row: curRow, col: curCol });
      } else if (board[curRow][curCol] && board[curRow][curCol].color != color) {
        allMoves.push({ row: curRow, col: curCol });
        break;
      } else {
        break;
      }
    }
  
    for (let i = 1; i < 8; i++) {
      const curRow = row - i;
      const curCol = col + i;
      if (curRow < 0 || curCol > 7) {
        break;
      } else if (board[curRow][curCol] == null) {
        allMoves.push({ row: curRow, col: curCol });
      } else if (board[curRow][curCol] && board[curRow][curCol].color != color) {
        allMoves.push({ row: curRow, col: curCol });
        break;
      } else {
        break;
      }
    }
  
    for (let i = 1; i < 8; i++) {
      const curRow = row + i;
      const curCol = col + i;
      if (curRow > 7 || curCol > 7) {
        break;
      } else if (board[curRow][curCol] == null) {
        allMoves.push({ row: curRow, col: curCol });
      } else if (board[curRow][curCol] && board[curRow][curCol].color != color) {
        allMoves.push({ row: curRow, col: curCol });
        break;
      } else {
        break;
      }
    }
    return allMoves;
  }
  
  function getRookMoves(row, col, color, board) {
    let allMoves = [];
    for (let curRow = row - 1; curRow >= 0; curRow--) {
      if (board[curRow][col] == null) {
        allMoves.push({ row: curRow, col: col });
      } else if (board[curRow][col] && board[curRow][col].color != color) {
        allMoves.push({ row: curRow, col: col });
        break;
      } else {
        break;
      }
    }
  
    for (let curRow = row + 1; curRow < 8; curRow++) {
      if (board[curRow][col] == null) {
        allMoves.push({ row: curRow, col: col });
      } else if (board[curRow][col] && board[curRow][col].color != color) {
        allMoves.push({ row: curRow, col: col });
        break;
      } else {
        break;
      }
    }
  
    for (let curCol = col - 1; curCol >= 0; curCol--) {
      if (board[row][curCol] == null) {
        allMoves.push({ row: row, col: curCol });
      } else if (board[row][curCol] && board[row][curCol].color != color) {
        allMoves.push({ row: row, col: curCol });
        break;
      } else {
        break;
      }
    }
  
    for (let curCol = col + 1; curCol < 8; curCol++) {
      if (board[row][curCol] == null) {
        allMoves.push({ row: row, col: curCol });
      } else if (board[row][curCol] && board[row][curCol].color != color) {
        allMoves.push({ row: row, col: curCol });
        break;
      } else {
        break;
      }
    }
  
    return allMoves;
  }
  
  function getQueenMoves(row, col, color, board) {
    const bishopMoves = getBishopMoves(row, col, color, board);
    const rookMoves = getRookMoves(row, col, color, board);
    let allMoves = [...bishopMoves, ...rookMoves];
    return allMoves;
  }
  
  function getKingMoves(row, col, color, board, canCastle) {
    let allMoves = [];
    for (let curRow = row - 1; curRow <= row + 1; curRow++) {
      for (let curCol = col - 1; curCol <= col + 1; curCol++) {
        if (
          curRow >= 0 &&
          curRow < 8 &&
          curCol >= 0 &&
          curCol < 8 &&
          (board[curRow][curCol] == null ||
            board[curRow][curCol].color != color) &&
          (curRow != row || curCol != col)
        ) {
          allMoves.push({ row: curRow, col: curCol });
        }
      }
    }
  
    if (color == "white") {
      if (
        canCastle.whiteKingside &&
        board[7][5] == null &&
        board[7][6] == null &&
        isMoveLegal(board, row, col, 7, 5, color) &&
        isMoveLegal(board, row, col, 7, 6, color) &&
        !isKingAttacked(board, color)
      ) {
        allMoves.push({ row: 7, col: 6 });
      }
  
      if (
        canCastle.whiteQueenside &&
        board[7][3] == null &&
        board[7][2] == null &&
        board[7][1] == null &&
        isMoveLegal(board, row, col, 7, 3, color) &&
        isMoveLegal(board, row, col, 7, 2, color) &&
        !isKingAttacked(board, color)
      ) {
        allMoves.push({ row: 7, col: 2 });
      }
    }
  
    if (color == "black") {
      if (
        canCastle.blackKingside &&
        board[0][5] == null &&
        board[0][6] == null &&
        isMoveLegal(board, row, col, 0, 5, color) &&
        isMoveLegal(board, row, col, 0, 6, color) &&
        !isKingAttacked(board, color)
      ) {
        allMoves.push({ row: 0, col: 6 });
      }
  
      if (
        canCastle.blackQueenside &&
        board[0][3] == null &&
        board[0][2] == null &&
        board[0][1] == null &&
        isMoveLegal(board, row, col, 0, 3, color) &&
        isMoveLegal(board, row, col, 0, 2, color) &&
        !isKingAttacked(board, color)
      ) {
        allMoves.push({ row: 0, col: 2 });
      }
    }
  
    return allMoves;
  }
import "./index.css";
import { useState } from "react";
import Square from "./Square";
import GameOver from "./GameOver";

export default function Board(props) {
  const [boardSquares, setBoardSquares] = useState([
    [
      { color: "black", type: "r" },
      { color: "black", type: "n" },
      { color: "black", type: "b" },
      { color: "black", type: "q" },
      { color: "black", type: "k" },
      { color: "black", type: "b" },
      { color: "black", type: "n" },
      { color: "black", type: "r" },
    ],
    [
      { color: "black", type: "p" },
      { color: "black", type: "p" },
      { color: "black", type: "p" },
      { color: "black", type: "p" },
      { color: "black", type: "p" },
      { color: "black", type: "p" },
      { color: "black", type: "p" },
      { color: "black", type: "p" },
    ],
    [null, null, null, null, null, null, null, null],
    [null, null, null, null, null, null, null, null],
    [null, null, null, null, null, null, null, null],
    [null, null, null, null, null, null, null, null],
    [
      { color: "white", type: "p" },
      { color: "white", type: "p" },
      { color: "white", type: "p" },
      { color: "white", type: "p" },
      { color: "white", type: "p" },
      { color: "white", type: "p" },
      { color: "white", type: "p" },
      { color: "white", type: "p" },
    ],
    [
      { color: "white", type: "r" },
      { color: "white", type: "n" },
      { color: "white", type: "b" },
      { color: "white", type: "q" },
      { color: "white", type: "k" },
      { color: "white", type: "b" },
      { color: "white", type: "n" },
      { color: "white", type: "r" },
    ],
  ]);
  const [squareSelected, setSquareSelected] = useState(null);
  const [curTurn, setCurTurn] = useState("white");
  const [squarePossibleMoves, setSquarePossibleMoves] = useState([]);
  const [canCastle, setCanCastle] = useState({
    whiteQueenside: true,
    whiteKingside: true,
    blackQueenside: true,
    blackKingside: true,
  });
  const [canEnPassant, setCanEnPassant] = useState(null);
  const [boardPossibleMoves, setBoardPossibleMoves] = useState(
    findAllPossibleBoardMoves(boardSquares, "white", canEnPassant, canCastle)
  );
  const [result, setResult] = useState(null);

  function toggleSquareSelected(curRow, curCol) {
    const newSquare = {
      value: boardSquares[curRow][curCol],
      row: curRow,
      col: curCol,
    };
    if (
      squareSelected &&
      squarePossibleMoves.some((sq) => sq.row === curRow && sq.col === curCol)
    ) {
      setSquareSelected(null);
      setSquarePossibleMoves([]);
      setCurTurn((prevTurn) => getOppColor(prevTurn));
      setBoardSquares((prevBoard) => {
        const newBoard = getNewBoard(prevBoard, squareSelected, newSquare);
        setBoardPossibleMoves(() => {
          const moves = findAllPossibleBoardMoves(
            newBoard,
            getOppColor(curTurn),
            canEnPassant,
            canCastle
          );
          setResult(() => {
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
          });
          return moves;
        });

        return newBoard;
      });

      setCanCastle((prev) => {
        if (squareSelected.value.color == "white") {
          if (squareSelected.value.type == "k") {
            return { ...prev, whiteQueenside: false, whiteKingside: false };
          } else if (
            squareSelected.value.type == "r" &&
            squareSelected.row == 7 &&
            squareSelected.col == 0
          ) {
            return { ...prev, whiteQueenside: false };
          } else if (
            squareSelected.value.type == "r" &&
            squareSelected.row == 7 &&
            squareSelected.col == 7
          ) {
            return { ...prev, whiteKingside: false };
          }
        } else {
          if (squareSelected.value.type == "k") {
            return { ...prev, blackQueenside: false, blackKingside: false };
          } else if (
            squareSelected.value.type == "r" &&
            squareSelected.row == 0 &&
            squareSelected.col == 0
          ) {
            return { ...prev, blackQueenside: false };
          } else if (
            squareSelected.value.type == "r" &&
            squareSelected.row == 0 &&
            squareSelected.col == 7
          ) {
            return { ...prev, blackKingside: false };
          }
        }
        return prev;
      });

      setCanEnPassant(() => {
        if (
          (squareSelected.value.color == "white" &&
            squareSelected.row == 6 &&
            newSquare.row == 4) ||
          (squareSelected.value.color == "black" &&
            squareSelected.row == 1 &&
            newSquare.row == 3)
        ) {
          return { col: curCol };
        } else {
          return null;
        }
      });
    } else if (newSquare.value && newSquare.value.color == curTurn) {
      setSquareSelected(newSquare);
      setSquarePossibleMoves(boardPossibleMoves[curRow][curCol]);
    }
  }

  const squareElements = boardSquares.map((rank, rowNum) => {
    return rank.map((curSquare, colNum) => {
      return (
        <Square
          key={rowNum * 8 + colNum}
          board={boardSquares}
          value={curSquare}
          row={rowNum}
          col={colNum}
          isSelected={
            squareSelected &&
            squareSelected.row == rowNum &&
            squareSelected.col == colNum
          }
          handleBoardChange={toggleSquareSelected}
          isPossibleMove={
            squareSelected &&
            squarePossibleMoves.some(
              (sq) => sq.row == rowNum && sq.col == colNum
            )
          }
          isKingAttacked={isKingAttacked}
        />
      );
    });
  });

  return (
    <section className="boardElement">
      {squareElements}
      {result && <GameOver winner={result} restart={props.restart} />}
    </section>
  );
}

function getOppColor(color) {
  return color == "white" ? "black" : "white";
}

function getNewBoard(prevBoard, startSquare, endSquare) {
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
    startSquare.value.color == "white" &&
    startSquare.value.type == "p" &&
    endSquare.row == 0
  ) {
    newBoard[0][endSquare.col] = { color: "white", type: "q" };
  }

  if (
    startSquare.value.color == "black" &&
    startSquare.value.type == "p" &&
    endSquare.row == 7
  ) {
    newBoard[7][endSquare.col] = { color: "black", type: "q" };
  }

  return newBoard;
}

function findAllPossibleBoardMoves(board, color, canEnPassant, canCastle) {
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
        newPossible[row][col] = findAllPossibleSquareMoves(
          board,
          { value: board[row][col], row: row, col: col },
          canCastle,
          canEnPassant
        ).filter((move) => {
          return isMoveLegal(board, row, col, move.row, move.col, color);
        });
      }
    }
  }
  return newPossible;
}

function findAllPossibleSquareMoves(
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

    if (row > 0) {
      if (board[row - 1][col] == null) {
        allMoves.push({ row: row - 1, col: col });
      }

      if (
        col > 0 &&
        ((board[row - 1][col - 1] && board[row - 1][col - 1].color != color) ||
          (row == 3 && canEnPassant && canEnPassant.col == col - 1))
      ) {
        allMoves.push({ row: row - 1, col: col - 1 });
      }

      if (
        col < 7 &&
        ((board[row - 1][col + 1] && board[row - 1][col + 1].color != color) ||
          (row == 3 && canEnPassant && canEnPassant.col == col + 1))
      ) {
        allMoves.push({ row: row - 1, col: col + 1 });
      }
    }
  } else if (color == "black") {
    if (row == 1 && board[2][col] == null && board[3][col] == null) {
      allMoves.push({ row: 3, col: col });
    }

    if (row < 7) {
      if (board[row + 1][col] == null) {
        allMoves.push({ row: row + 1, col: col });
      }

      if (
        col > 0 &&
        ((board[row + 1][col - 1] && board[row + 1][col - 1].color != color) ||
          (row == 4 && canEnPassant && canEnPassant.col == col - 1))
      ) {
        allMoves.push({ row: row + 1, col: col - 1 });
      }

      if (
        col < 7 &&
        ((board[row + 1][col + 1] && board[row + 1][col + 1].color != color) ||
          (row == 4 && canEnPassant && canEnPassant.col == col + 1))
      ) {
        allMoves.push({ row: row + 1, col: col + 1 });
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

function isMoveLegal(board, fromRow, fromCol, toRow, toCol, color) {
  const oppColor = getOppColor(color);
  const simulated = board.map((row) => row.slice());
  const simulatedNewBoard = getNewBoard(
    simulated,
    { value: board[fromRow][fromCol], row: fromRow, col: fromCol },
    { value: board[toRow][toCol], row: toRow, col: toCol }
  );
  const kingPos = findKing(simulatedNewBoard, color);
  return !isSquareAttacked(
    simulatedNewBoard,
    kingPos.row,
    kingPos.col,
    oppColor
  );
}

function isKingAttacked(board, color) {
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
  let allMoves = findAllPossibleSquareMoves(
    board,
    startingPiece,
    canCastle,
    canEnPassant
  );
  return allMoves.some((sq) => sq.row === targetRow && sq.col === targetCol);
}

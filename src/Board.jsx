import "./index.css";
import { useState } from "react";
import Square from "./Square";
import GameOver from "./GameOver";
import PromoteModal from "./PromoteModal";
import FlipBoard from "./FlipBoard";
import {
  getNewBoard,
  getOppColor,
  findAllPossibleBoardMoves,
  isKingAttacked,
  checkIfOver,
  checkIfCanCastle,
  checkIfCanEnPassant,
} from "./updateBoard";
import StartGame from "./startGame";
import randomMoves from "./AI";

export default function Board(props) {
  const [board, setBoard] = useState([
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
  const [canCastle, setCanCastle] = useState({
    whiteQueenside: true,
    whiteKingside: true,
    blackQueenside: true,
    blackKingside: true,
  });
  const [canEnPassant, setCanEnPassant] = useState(null);
  const [boardPossibleMoves, setBoardPossibleMoves] = useState(
    findAllPossibleBoardMoves(board, "white", canEnPassant, canCastle)
  );
  const [result, setResult] = useState(null);
  const [pendingMove, setPendingMove] = useState(null);
  const [flipBoard, setFlipBoard] = useState(false);
  const [gameType, setGameType] = useState(null);
  const [AITurn, setAITurn] = useState(null);

  function toggleSquareSelected(curRow, curCol) {
    const newSquare = {
      value: board[curRow][curCol],
      row: curRow,
      col: curCol,
    };
    if (
      squareSelected &&
      boardPossibleMoves
        .find(
          (square) =>
            square.curRow == squareSelected.row &&
            square.curCol == squareSelected.col
        )
        .moves.some((sq) => sq.row === curRow && sq.col === curCol)
    ) {
      const promotionSquare =
        squareSelected.value.type == "p" &&
        (newSquare.row == 0 || newSquare.row == 7);

      if (promotionSquare) {
        setPendingMove({
          startSquare: squareSelected,
          endSquare: newSquare,
        });
        return;
      }

      makeMoveHelper(squareSelected, newSquare, null);
    } else if (boardPossibleMoves
      .some(
        (square) => square.curRow == curRow && square.curCol == curCol
      )) {
      setSquareSelected(newSquare);
    }
  }

  function updatePromote(type) {
    const { startSquare, endSquare } = pendingMove;
    setPendingMove(null);
    makeMoveHelper(startSquare, endSquare, type);
  }

  function makeAIMove(board, possibleMoves, color) {
    const { startSquare, endSquare } = randomMoves(board, possibleMoves, color);
    makeMoveHelper(startSquare, endSquare, endSquare.promoteTo);
    setAITurn(false);
  }

  if (AITurn){
    makeAIMove(board, boardPossibleMoves, curTurn);
  }

  function makeMoveHelper(startSquare, endSquare, promoteTo) {
    setSquareSelected(null);
    setCurTurn((prevTurn) => getOppColor(prevTurn));
    setBoard((prevBoard) => {
      const newBoard = getNewBoard(
        prevBoard,
        startSquare,
        endSquare,
        promoteTo
      );
      setBoardPossibleMoves(() => {
        const moves = findAllPossibleBoardMoves(
          newBoard,
          getOppColor(curTurn),
          canEnPassant,
          canCastle
        );
        setResult(() => {
          const updatedResult = checkIfOver(newBoard, moves, curTurn);
          if (!AITurn && gameType == "bot" && updatedResult == null) {
            setAITurn(true);
          }
          return updatedResult;
        });
        return moves;
      });

      return newBoard;
    });

    setCanCastle((prev) => checkIfCanCastle(prev, startSquare));
    setCanEnPassant(() => checkIfCanEnPassant(startSquare, endSquare));
  }

  const squareElements = board.map((rank, rowNum) => {
    return rank.map((curSquare, colNum) => {
      return (
        <Square
          key={rowNum * 8 + colNum}
          board={board}
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
            boardPossibleMoves
              .find(
                (square) =>
                  square.curRow == squareSelected.row &&
                  square.curCol == squareSelected.col
              )
              .moves.some((sq) => sq.row === rowNum && sq.col === colNum)
          }
          isKingAttacked={isKingAttacked}
        />
      );
    });
  });

  return (
    <main>
      <section className="boardElement">
        {flipBoard
          ? squareElements.toReversed().map((rank) => rank.toReversed())
          : squareElements}
        {pendingMove && (
          <PromoteModal
            color={curTurn}
            col={pendingMove.endSquare.col}
            promoteTo={updatePromote}
            moves={boardPossibleMoves
              .find(
                (square) =>
                  square.curRow == squareSelected.row &&
                  square.curCol == squareSelected.col
              )
              .moves.filter(
                (sq) =>
                  sq.row === pendingMove.endSquare.row &&
                  sq.col === pendingMove.endSquare.col
              )}
          />
        )}
      </section>

      {result && <GameOver winner={result} restart={props.restart} />}
      {gameType == null && (
        <StartGame setGameMode={(type) => setGameType(type)} />
      )}
      {gameType && result == null && (
        <FlipBoard flip={() => setFlipBoard(!flipBoard)} isOn={flipBoard} />
      )}
    </main>
  );
}

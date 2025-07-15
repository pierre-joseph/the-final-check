import "./index.css";
import { useState } from "react";
import Square from "./Square";
import GameOver from "./GameOver";
import PromoteModal from "./PromoteModal";
import FlipBoard from "./FlipBoard";
import {
  makeMove,
  getOppColor,
  findAllPossibleBoardMoves,
  isKingAttacked,
  checkIfOver,
  checkIfCanCastle,
  checkIfCanEnPassant,
  convertFENToBoard,
} from "./updateBoard";
import StartGame from "./startGame";
import { randomMoves } from "./AI";

export default function Board(props) {
  const [board, setBoard] = useState(
    convertFENToBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
  );
  const [squareSelected, setSquareSelected] = useState(null);
  const [curTurn, setCurTurn] = useState("white");
  const [canCastle, setCanCastle] = useState({
    whiteQueenside: true,
    whiteKingside: true,
    blackQueenside: true,
    blackKingside: true,
  });
  const [boardPossibleMoves, setBoardPossibleMoves] = useState(
    findAllPossibleBoardMoves(board, "white", canCastle, null)
  );
  const [result, setResult] = useState(null);
  const [pendingMove, setPendingMove] = useState(null);
  const [flipBoard, setFlipBoard] = useState(false);
  const [gameType, setGameType] = useState(null);
  const [AITurn, setAITurn] = useState(null);
  const [squaresAttacked, setSquaresAttacked] = useState({black});

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
    } else if (
      boardPossibleMoves.some(
        (square) => square.curRow == curRow && square.curCol == curCol
      )
    ) {
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

  if (AITurn) {
    makeAIMove(board, boardPossibleMoves, curTurn);
  }

  function makeMoveHelper(startSquare, endSquare, promoteTo) {
    setSquareSelected(null);
    setCurTurn((prevTurn) => getOppColor(prevTurn));
    setCanCastle((prev) => {
      const newCanCastle = checkIfCanCastle(prev, startSquare, endSquare);
      setBoard((prevBoard) => {
        makeMove(
          prevBoard,
          startSquare,
          endSquare,
          promoteTo
        );

        setBoardPossibleMoves(() => {
          const moves = findAllPossibleBoardMoves(
            prevBoard,
            getOppColor(curTurn),
            newCanCastle,
            checkIfCanEnPassant(startSquare, endSquare)
          );
          setResult(() => {
            const updatedResult = checkIfOver(prevBoard, moves, curTurn);
            if (!AITurn && gameType == "bot" && updatedResult == null) {
              setAITurn(true);
            }
            return updatedResult;
          });
          return moves;
        });

        return prevBoard;
      });
      
      return newCanCastle;
    });
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
